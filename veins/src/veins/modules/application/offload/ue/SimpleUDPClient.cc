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

#include "SimpleUDPClient.h"
#include "veins/modules/application/heterogeneous/messages/HeterogeneousMessage_m.h"

Define_Module(SimpleUDPClient);

void SimpleUDPClient::initialize(int stage) {
	if(stage == 0){
		debug = par("debug").boolValue();
		infoLogging = par("infoLogging").boolValue();
		msgLength = par("msgLength");
		msgLengthSd = par("msgLengthSd");
		sendInterval = par("sendInterval");
		sendIntervalSd = par("sendIntervalSd");

		udpIn = findGate("udpIn");
		udpOut = findGate("udpOut");

		numSent = 0;
		numReceived = 0;

		socket.setOutputGate(gate(udpOut));
		localPort = par("localPort");
		socket.bind(localPort);

		manager = Veins::TraCIScenarioManagerAccess().get();

		/* Don't schedule at the same time to prevent synchronization */
		scheduleAt(simTime() + uniform(0, 1), new cMessage("SelfMsg"));
	}
}

void SimpleUDPClient::handleMessage(cMessage *msg) {
	/*
	 * Send message with random length and random interval to server
	 */
	if (msg->isSelfMessage()) {
		/* Create a message with random length */
		HeterogeneousMessage* randomMsg = new HeterogeneousMessage("UE");
		randomMsg->setByteLength((int)(msgLength + uniform(-msgLengthSd, msgLengthSd)));

		cModule* source = getParentModule();
		randomMsg->setSourceAddress(source->getFullName());

		// Check the valid of destination address before sending
		IPv4Address destAddress;
		if (getDestinationAddress("server", destAddress)) {
			INFO_ID("Sending message (length = " << randomMsg->getByteLength() << ") to server...");
			socket.sendTo(randomMsg, destAddress, 4242);
			numSent++;
		} else {
			delete msg;
		}

		/* Repeat sending with random interval */
		scheduleAt(simTime() + sendInterval + uniform(-sendIntervalSd, sendIntervalSd), new cMessage("SelfMsg"));

	} else {
		/* Receive message from UDP module (from server) */
		HeterogeneousMessage* rcvMsg = dynamic_cast<HeterogeneousMessage*>(msg);
		INFO_ID("Received message (length = " << rcvMsg->getByteLength() << ") from " << rcvMsg->getSourceAddress());
		numReceived++;
		delete msg;
	}
}

void SimpleUDPClient::finish() {
	INFO_ID("Sent " << numSent << "; Received " << numReceived << " messages.");
}

bool SimpleUDPClient::getDestinationAddress(const char* destAddress, IPv4Address& ipv4DestAddress) {
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
