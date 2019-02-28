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

#ifndef __VEINS_OFFLOAD_CARAPP_H_
#define __VEINS_OFFLOAD_CARAPP_H_

#include <omnetpp.h>
#include "veins/modules/application/heterogeneous/IHeterogeneousApp.h"
#include "veins/modules/application/heterogeneous/messages/HeterogeneousMessage_m.h"
#include "veins/modules/mobility/traci/TraCIScenarioManager.h"
#include "veins/modules/mobility/traci/TraCIPersonMobility.h"
#include "veins/modules/mobility/traci/TraCIVehicleMobility.h"
#include "veins/base/utils/SimpleLogger.h"

using Veins::TraCIScenarioManager;
using Veins::TraCIScenarioManagerAccess;

/**
 * @brief
 * A simple application that receive/send messages via the LTE channel
 *
 * @author Quang-Huy Nguyen
 */

class CarApp: public IHeterogeneousApp {

protected:
    /** Variables for statistic collection */
    simsignal_t endToEndDelaySignal;
    simsignal_t serverToCarDelaySignal;
    long numSentSignal;
    long numReceivedSignal;

protected:
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

#endif
