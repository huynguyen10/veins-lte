//
// Copyright (C) 2006-2012 Christoph Sommer <christoph.sommer@uibk.ac.at>
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

#ifndef MOBILITY_TRACI_TRACIPERSONMOBILITY_H
#define MOBILITY_TRACI_TRACIPERSONMOBILITY_H
#ifdef WITH_TRACI

#include <string>
#include <fstream>
#include <list>
#include <stdexcept>

#include <omnetpp.h>

#include "INETDefs.h"
#include "TraCIMobility.h"
#include "ModuleAccess.h"
#include "world/traci/TraCIScenarioManager.h"

/**
 * @brief
 * Used in modules created by the TraCIScenarioManager.
 *
 * This module relies on the TraCIScenarioManager for state updates
 * and can not be used on its own.
 *
 * See the Veins website <a href="http://veins.car2x.org/"> for a tutorial, documentation, and publications </a>.
 *
 * @author Quang-Huy Nguyen
 *
 * @see TraCIScenarioManager
 * @see TraCIScenarioManagerLaunchd
 *
 * @ingroup mobility
 */
class INET_API TraCIPersonMobility : public TraCIMobility
{
    public:
        TraCIPersonMobility() : TraCIMobility() {}
        virtual void handleSelfMessage(cMessage *msg);
        virtual void nextPosition(const Coord& position, std::string road_id = "", double speed = -1, double angle = -1);
};

class TraCIPersonMobilityAccess : public ModuleAccess<TraCIPersonMobility>
{
    public:
        TraCIPersonMobilityAccess() : ModuleAccess<TraCIPersonMobility>("mobility") {};
};


#endif
#endif

