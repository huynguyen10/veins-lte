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

#ifndef __VEINS_HETEROGENEOUSAPP_H_
#define __VEINS_HETEROGENEOUSAPP_H_

#include <omnetpp.h>
#include "veins/modules/application/heterogeneous/messages/HeterogeneousMessage_m.h"
#include "veins/modules/mobility/traci/TraCIScenarioManager.h"
#include "veins/modules/mobility/traci/TraCIPersonMobility.h"
#include "veins/modules/mobility/traci/TraCIVehicleMobility.h"
#include "veins/base/utils/SimpleLogger.h"

using Veins::TraCIScenarioManager;
using Veins::TraCIScenarioManagerAccess;

/**
 * @brief
 * A simple application that sends messages via the DONTCARE channel to other cars and sometimes a
 * message to the server via LTE.
 *
 * @author Quang-Huy Nguyen
 */

class IHeterogeneousApp: public cSimpleModule {

protected:
	int toDecisionMaker;
	int fromDecisionMaker;
	std::string sumoId;
	bool debug;
	bool infoLogging;
	int objectType;
	long numReceived;
	long numSent;

protected:
	virtual void initialize(int stage);
	virtual void handleMessage(cMessage *msg) {};
};

#endif
