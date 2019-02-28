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

#ifndef _SIMPLEUDPCLIENT_H_
#define _SIMPLEUDPCLIENT_H_

#include <omnetpp.h>
#include "UDPSocket.h"
#include "IPvXAddressResolver.h"
#include "veins/modules/mobility/traci/TraCIScenarioManager.h"
#include "veins/base/utils/SimpleLogger.h"

/*
 * @brief
 * A simple UDP client, sending random messages with random interval.
 *
 * @author Quang-Huy Nguyen
 */

class SimpleUDPClient: public cSimpleModule {

private:
	int localPort;  /** The port for the LTE messages. */

protected:
    bool debug;
    bool infoLogging;

    int udpIn;
    int udpOut;

    double msgLength;
    double msgLengthSd;
    double sendInterval;
    double sendIntervalSd;

    long numReceived;
    long numSent;

    Veins::TraCIScenarioManager* manager;  /** The scenario manager. */
    UDPSocket socket; /** The UDP socket for communication. */

public:
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

private:
    bool getDestinationAddress(const char* destAddress, IPv4Address& ipv4DestAddress);
};

#endif /* _SIMPLEUDPCLIENT_H_ */
