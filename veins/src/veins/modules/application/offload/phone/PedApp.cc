//
// Copyright (C) 2018 Quang-Huy Nguyen <nguyen@ccs-labs.org>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "PedApp.h"

Define_Module(PedApp);

void PedApp::initialize(int stage) {
	if (stage == 0) {
		debug = par("debug").boolValue();
		infoLogging = par("infoLogging").boolValue();
		udpIn = findGate("udpIn");
		udpOut = findGate("udpOut");

		// Application parameters
		mode = par("mode").stdstringValue();
		classifier = par("classifier").stdstringValue();
		camMsgLength = par("camMsgLength");
		localTimeModel = 0;

		socket.setOutputGate(gate(udpOut));
		localPort = par("localPort");
		socket.bind(localPort);

		manager = Veins::TraCIScenarioManagerAccess().get();

		if (mode == "plusplus") {
			initializeLocalTimeModel(par("localTimeModels").xmlValue());
			localTimeModel = getLocalTimeModelFromName(classifier);
			if (localTimeModel == 0) {
				throw cRuntimeError("Local time model not found.");
			}
		}

		samplingFreq = par("samplingFreq");
		windowLength = par("windowLength");

		numReceived = 0;
		numSent = 0;

		cModule *tmpMobility = getParentModule()->getSubmodule("veinsmobility");
		Veins::TraCIMobility* mobility = dynamic_cast<Veins::TraCIMobility *>(tmpMobility);
		ASSERT(mobility);
		sumoId = mobility->getExternalId();

		localTimeSignal = registerSignal("localTime");
		serverToPhoneDelaySignal = registerSignal("serverToPhoneDelay");
		offloadDelaySignal = registerSignal("offloadDelay");
		numSentSignal = registerSignal("numSent");
		numReceivedSignal = registerSignal("numReceived");

		dataReadyEvent = new cMessage("data");
		sendAfterEvent = new cMessage("send");

		/* Don't schedule at the same time to prevent synchronization */
		scheduleAt(simTime() + uniform(0, 1), dataReadyEvent);
	}
}

void PedApp::handleMessage(cMessage *msg) {
	cModule* source = getParentModule();

	/*
	 * Get current position
	 */
	cModule *tmpMobility = getParentModule()->getSubmodule("veinsmobility");
	Veins::TraCIMobility* mobility = dynamic_cast<Veins::TraCIMobility *>(tmpMobility);
	ASSERT(mobility);
	Coord currentPos = mobility->getPositionAt(simTime());

	/* Data ready for local processing or offload to server */
	if (msg == dataReadyEvent) {
		if (mode == "plusplus") {
			/* Message containing classification result */
			camMsg = new HeterogeneousMessage("CAM Message");
//			camMsg->setSourceAddress(sumoId.c_str());
			camMsg->setSourceAddress(source->getFullName());
			camMsg->setByteLength(camMsgLength);	// fix length
			camMsg->setTimestamp(simTime());   // startTime
			camMsg->setSenderPos(currentPos);

			double localTime = localTimeModel->getLocalTime(samplingFreq, windowLength);

			/* Sending result message after local processing */
			scheduleAt(simTime() + localTime, sendAfterEvent);

			scheduleAt(simTime() + windowLength, dataReadyEvent);

		} else {
			int messageLength;
			double interval;
			if (mode == "offload") {
				/*
				 * 3 sensors (accelerometer, gyroscope & GNSS)
				 * 3 values for each sensor, 4 bytes for each value (float type)
				 * nSamples = freq x wl
				 */
				messageLength = (int)(samplingFreq * windowLength * 4 * 3 * 3);
				interval = windowLength;

			} else if (mode == "stream") {
				/*
				 * nSamples = 1
				 */
				messageLength = 4 * 3 * 3;
				interval = 1.0 / samplingFreq;
			}

			/*
			 * Sending messages to server via LTE, sending interval & message length
			 * depending on mode, window-length & sampling frequency
			 */
			HeterogeneousMessage* dataMsg = new HeterogeneousMessage("Data Message");
//			dataMsg->setSourceAddress(sumoId.c_str());
			dataMsg->setSourceAddress(source->getFullName());
			dataMsg->setByteLength(messageLength);
			dataMsg->setTimestamp(simTime());   // startTime
			dataMsg->setSendingTime(simTime());	//
			dataMsg->setSenderPos(currentPos);
			INFO_ID("Sending DATA message (length = " << dataMsg->getByteLength() << ") to server...");

			// Check the valid of destination address
			IPv4Address destAddress;
			if (getDestinationAddress("server", destAddress)) {
				socket.sendTo(dataMsg, destAddress, 4242);
				numSent++;
				emit(numSentSignal, numSent);
			}

			/* Schedule for next data processing */
			scheduleAt(simTime() + interval, dataReadyEvent);
		}

	} else if (msg == sendAfterEvent) {
		INFO_ID("Sending CAM message (length = " << camMsg->getByteLength() << ") to server...");
		emit(localTimeSignal, simTime() - camMsg->getTimestamp()); // classTime - startTime

		camMsg->setSendingTime(simTime());	// classTime (after local processing)

		// Check the valid of destination address before sending
		IPv4Address destAddress;
		if (getDestinationAddress("server", destAddress)) {
			socket.sendTo(camMsg, destAddress, 4242);
			numSent++;
			emit(numSentSignal, numSent);
		}

	} else {
		numReceived++;
		emit(numReceivedSignal, numReceived);
		HeterogeneousMessage* recvMsg = dynamic_cast<HeterogeneousMessage *>(msg);
		INFO_ID("Received message (length = " << recvMsg->getByteLength() << ") from "<< recvMsg->getSourceAddress() << " at " << currentPos.info());
		emit(serverToPhoneDelaySignal, simTime() - recvMsg->getSendingTime());
		emit(offloadDelaySignal, simTime() - recvMsg->getTimestamp());
		delete msg;
	}
}

void PedApp::finish() {
	INFO_ID("Sent " << numSent << "; Received " << numReceived << " messages.");
	cancelAndDelete(dataReadyEvent);
	cancelAndDelete(sendAfterEvent);
}

bool PedApp::getDestinationAddress(const char* destAddress, IPv4Address& ipv4DestAddress) {
	cModule* destination = simulation.getModuleByPath(destAddress);
	if (!destination){
		WARN_ID("Can't find a module with destination " << destAddress);
		return false;
	}
	IPv4Address address = IPvXAddressResolver().resolve(destAddress).get4();
	if (address.isUnspecified()) {
		address = manager->getIPAddressForID(destAddress);
	}
	if (address.isUnspecified()){
		WARN_ID("Address " << destAddress << " still unspecified!");
		return false;
	}
	ipv4DestAddress = address;
	return true;
}

LocalTimeModel* PedApp::getLocalTimeModelFromName(std::string name) {

	for (LocalTimeModelList::const_iterator it = localTimeModels.begin();
			it != localTimeModels.end(); it++) {
		if ((*it)->getName() == name) {
			return (*it);
		}
	}
	return 0;
}

void PedApp::initializeLocalTimeModel(cXMLElement* xmlConfig) {

	if (xmlConfig == 0)
		throw cRuntimeError("No local time models configuration file specified");

	// Get channel Model field which contains parameters fields
	cXMLElementList localTimeModelList = xmlConfig->getElementsByTagName("LocalTimeModel");

	if (localTimeModelList.empty())
		throw cRuntimeError("No local time models configuration found in configuration file");

	// iterate over all LocalTimeModel-entries, get a new LocalTimeModel instance and add
	// it to LocalTimeModels
	for (cXMLElementList::const_iterator it = localTimeModelList.begin();
			it != localTimeModelList.end(); it++) {

		cXMLElement* localTimeModelData = *it;

		const char* name = localTimeModelData->getAttribute("type");

		if (name == 0) {
			throw cRuntimeError("Could not read name of local time model.");
		}

		ConfigMap configs;
		getConfigsFromXML(localTimeModelData, configs);

		LocalTimeModel* newLocalTimeModel = new LocalTimeModel(name, configs);

		// attach the new localTimeModel to the localTimeModelList
		localTimeModels.push_back(newLocalTimeModel);

		std::cout << "LocalTimeModel \"" << name << "\" loaded." << endl;

	} // end iterator loop
}

void PedApp::getConfigsFromXML(cXMLElement* xmlData, ConfigMap& outputMap) {

	cXMLElementList configList = xmlData->getElementsByTagName("ClassificationConfig");

	if (configList.empty())
		throw cRuntimeError("No classification configuration found for local time model.");

	for (cXMLElementList::const_iterator it = configList.begin();
			it != configList.end(); it++) {

		cXMLElement* configData = *it;

		const char* name = configData->getAttribute("name");

		if (name == 0) {
			throw cRuntimeError("Could not read name of classification configuration.");
		}

		ParameterMap params;
		getParametersFromXML(configData, params);

		// Add configuration to output map
		outputMap[name] = params;
	}
}

void PedApp::getParametersFromXML(cXMLElement* xmlData, ParameterMap& outputMap) {

	cXMLElementList parameters = xmlData->getElementsByTagName("Parameter");

	for(cXMLElementList::const_iterator it = parameters.begin();
			it != parameters.end(); it++) {

		const char* name = (*it)->getAttribute("name");
		const char* type = (*it)->getAttribute("type");
		const char* value = (*it)->getAttribute("value");
		if(name == 0 || type == 0 || value == 0)
			throw cRuntimeError("Invalid parameter, could not find name, type or value");

		std::string sType = type;   //needed for easier comparison
		std::string sValue = value; //needed for easier comparison

		cMsgPar param(name);

		//parse type of parameter and set value
		if (sType == "bool") {
			param.setBoolValue(sValue == "true" || sValue == "1");

		} else if (sType == "double") {
			param.setDoubleValue(strtod(value, 0));

		} else if (sType == "string") {
			param.setStringValue(value);

		} else if (sType == "long") {
			param.setLongValue(strtol(value, 0, 0));

		} else {
			throw cRuntimeError("Unknown parameter type: '%s'", sType.c_str());
		}

		//add parameter to output map
		outputMap[name] = param;
	}
}
