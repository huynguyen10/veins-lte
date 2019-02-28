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

#ifndef OFFLOADSERVER_H_
#define OFFLOADSERVER_H_

#include <omnetpp.h>
#include "ApplicationBase.h"
#include "INETDefs.h"
#include "UDPSocket.h"
#include "IPv4Address.h"
#include "veins/modules/application/heterogeneous/messages/HeterogeneousMessage_m.h"
#include "veins/modules/mobility/traci/TraCIScenarioManager.h"
#include "veins/base/utils/SimpleLogger.h"

/*
 * @brief
 * A simple server that just prints the received offload messages.
 *
 * @author Quang-Huy Nguyen
 */

using Veins::TraCIScenarioManager;
using Veins::TraCIScenarioManagerAccess;

class OffloadServerApp: public ApplicationBase {

protected:
    UDPSocket socket;
    TraCIScenarioManager* manager;
    long numReceived;
    long numSent;
    bool debug;
    bool infoLogging;
    int denmMsgLength;
    double msgLength;
    double msgLengthSd;

    /** Variables for statistic collection */
    simsignal_t phoneToServerDelaySignal;
    long numSentSignal;
    long numReceivedSignal;

public:
    OffloadServerApp();
    virtual ~OffloadServerApp();

    virtual int numInitStages() const {
        return 4;
    }
    virtual void initialize(int stage);
    virtual void finish();
    virtual void handleMessageWhenUp(cMessage *msg);

    virtual bool handleNodeStart(IDoneCallback *doneCallback);
    virtual bool handleNodeShutdown(IDoneCallback *doneCallback);
    virtual void handleNodeCrash();
};

#endif /* OFFLOADSERVER_H_ */
