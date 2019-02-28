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

#include "OffloadServerApp.h"

Define_Module(OffloadServerApp);

OffloadServerApp::OffloadServerApp() {}

OffloadServerApp::~OffloadServerApp() {}

void OffloadServerApp::initialize(int stage) {
	ApplicationBase::initialize(stage);
	if(stage == 0){
		debug = par("debug").boolValue();
		infoLogging = par("infoLogging").boolValue();
		denmMsgLength = par("denmMsgLength");
		msgLength = par("msgLength");
		msgLengthSd = par("msgLengthSd");
		numReceived = 0;
		numSent = 0;
		manager = TraCIScenarioManagerAccess().get();
		ASSERT(manager);

		phoneToServerDelaySignal = registerSignal("phoneToServerDelay");
		numSentSignal = registerSignal("numSent");
		numReceivedSignal = registerSignal("numReceived");
	}
}

void OffloadServerApp::finish() {
	INFO_ID("Sent " << numSent << "; Received " << numReceived << " messages.");
}

void OffloadServerApp::handleMessageWhenUp(cMessage *msg) {
	HeterogeneousMessage* recvMsg = dynamic_cast<HeterogeneousMessage*>(msg);

	if (recvMsg) {
		std::string name = recvMsg->getName();
		if (name == "UE") {
			std::string destAddress = recvMsg->getSourceAddress();
			cModule* destination = simulation.getModuleByPath(destAddress.c_str());

			/* Message from UE */
			INFO_ID("Received message (length = " << recvMsg->getByteLength() << ") from " << destAddress );

			if (!destination){
				WARN_ID("Can't find a module with destination " << destAddress << ", not sending message!");
				delete msg;
				return;
			}
			IPv4Address address = IPvXAddressResolver().resolve(destAddress.c_str()).get4();
			if (address.isUnspecified()){
				WARN_ID("Address " << destAddress << " still unspecified!");
				delete msg;
				return;
			}

			HeterogeneousMessage* sendMsg = new HeterogeneousMessage("Reply Message");
			sendMsg->setByteLength((int)(msgLength + uniform(-msgLengthSd, msgLengthSd)));
			sendMsg->setSourceAddress("server");
			INFO_ID("Sending back message (length = " << sendMsg->getByteLength() << ") to " << destAddress);
			socket.sendTo(sendMsg, address, 4242);

		} else {
			/* Message from pedestrian */
			numReceived++;
			INFO_ID("Received message (length = " << recvMsg->getByteLength() << ") from " << recvMsg->getSourceAddress());

			/* phoneToServerDelay = currentTime - sendingTime */
			emit(phoneToServerDelaySignal, simTime() - recvMsg->getSendingTime());

			TraCIScenarioManager* manager = TraCIScenarioManagerAccess().get();
			std::map<std::string, cModule*> hosts = manager->getManagedHosts();

			/*
			 * Broadcast the results to all nodes (cars & pedestrians) currently in the context
			 */
			for (std::map<std::string, cModule*>::iterator it = hosts.begin(); it != hosts.end(); it++) {
				/*
				 * Server replies with a simple message. Note that no additional parameters (like exact
				 * message size) are set and therefore transmission will more likely succeed. If you use
				 * this function set it correctly to get realistic results.
				 */
				HeterogeneousMessage *denmMsg = new HeterogeneousMessage("DENM Message");
				denmMsg->setByteLength(denmMsgLength); // fixed length
				denmMsg->setSourceAddress("server");

				denmMsg->setTimestamp(recvMsg->getTimestamp()); // Forward the original startTime
				denmMsg->setSendingTime(simTime());  // sendingTime

				std::string destination(it->first);
				denmMsg->setDestinationAddress(destination.c_str());
				IPv4Address address = manager->getIPAddressForID(destination);

//				INFO_ID("Sending Broadcast message to " << destination);
				INFO_ID("Sending DENM message " << "(length = " << denmMsgLength << ") to " << (it->second)->getFullName());
				socket.sendTo(denmMsg, address, 4242);
				numSent++;
			}
		}
	}

	delete msg;
}

bool OffloadServerApp::handleNodeStart(IDoneCallback *doneCallback) {
	socket.setOutputGate(gate("udpOut"));
	int localPort = par("localPort");
	socket.bind(localPort);
	return true;
}

bool OffloadServerApp::handleNodeShutdown(IDoneCallback *doneCallback) {
	return true;
}

void OffloadServerApp::handleNodeCrash() {}
