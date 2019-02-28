//
// Copyright (C) 2006-2016 Florian Hagenauer <hagenauer@ccs-labs.org>
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

#include "SimpleServerApp.h"

Define_Module(SimpleServerApp);

SimpleServerApp::SimpleServerApp() {}

SimpleServerApp::~SimpleServerApp() {}

void SimpleServerApp::initialize(int stage){
	ApplicationBase::initialize(stage);
	if(stage == 0){
		debug = par("debug").boolValue();
		infoLogging = par("infoLogging").boolValue();
		receivedMessages = 0;
		manager = TraCIScenarioManagerAccess().get();
		ASSERT(manager);
	}
}

void SimpleServerApp::finish(){
	INFO_ID("Received " << receivedMessages << " messages via LTE.");
}

void SimpleServerApp::handleMessageWhenUp(cMessage *msg){
	HeterogeneousMessage* heterogeneousMessage = dynamic_cast<HeterogeneousMessage*>(msg);
	if(heterogeneousMessage){
		receivedMessages++;
		std::string sourceAddress = heterogeneousMessage->getSourceAddress();
		INFO_ID("Received Heterogeneous Message from " << sourceAddress);

		/*
		 * Server replies with a simple message. Note that no additional parameters (like exact
		 * message size) are set and therefore transmission will more likely succeed. If you use
		 * this function set it correctly to get realistic results.
		 */
		HeterogeneousMessage *reply = new HeterogeneousMessage("Server Reply");
		IPv4Address address = manager->getIPAddressForID(sourceAddress);
		reply->setSourceAddress("server");
		INFO_ID("Sending Message back to " << address);
		socket.sendTo(reply, address, 4242);
	}
	delete msg;
}

bool SimpleServerApp::handleNodeStart(IDoneCallback *doneCallback){
	socket.setOutputGate(gate("udpOut"));
	int localPort = par("localPort");
	socket.bind(localPort);
	return true;
}

bool SimpleServerApp::handleNodeShutdown(IDoneCallback *doneCallback){
	return true;
}

void SimpleServerApp::handleNodeCrash(){}
