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

#include "CarApp.h"

Define_Module(CarApp);

void CarApp::initialize(int stage) {
	if (stage == 0) {
	    numReceived = 0;
	    numSent = 0;
	    objectType = par("objectType");
	    debug = par("debug").boolValue();
		infoLogging = par("infoLogging");
		toDecisionMaker = findGate("toDecisionMaker");
		fromDecisionMaker = findGate("fromDecisionMaker");

		cModule *tmpMobility = getParentModule()->getSubmodule("veinsmobility");
		Veins::TraCIMobility* mobility = dynamic_cast<Veins::TraCIMobility *>(tmpMobility);
		ASSERT(mobility);
		sumoId = mobility->getExternalId();

		serverToCarDelaySignal = registerSignal("serverToCarDelay");
		endToEndDelaySignal = registerSignal("endToEndDelay");
		numSentSignal = registerSignal("numSent");
		numReceivedSignal = registerSignal("numReceived");
	}
}

void CarApp::handleMessage(cMessage *msg) {
	if (msg->isSelfMessage()) {
	    /* No self message */
	} else {
	    numReceived++;
	    emit(numReceivedSignal, numReceived);
        /*
         * Get current position
         */
        cModule *tmpMobility = getParentModule()->getSubmodule("veinsmobility");
        Veins::TraCIMobility* mobility = dynamic_cast<Veins::TraCIMobility *>(tmpMobility);
        ASSERT(mobility);
        Coord currentPos = mobility->getPositionAt(simTime());

		HeterogeneousMessage *recvMsg = dynamic_cast<HeterogeneousMessage *>(msg);
		INFO_ID("Received message (length = " << recvMsg->getByteLength() << ") from " << recvMsg->getSourceAddress() << " at " << currentPos.info());
		emit(serverToCarDelaySignal, simTime() - recvMsg->getSendingTime());
		emit(endToEndDelaySignal, simTime() - recvMsg->getTimestamp());
	}
}

void CarApp::finish() {
    INFO_ID("Sent " << numSent << "; Received " << numReceived << " messages.");
}
