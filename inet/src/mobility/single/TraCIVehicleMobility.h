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

#ifndef MOBILITY_TRACI_TRACIVEHICLEMOBILITY_H
#define MOBILITY_TRACI_TRACIVEHICLEMOBILITY_H
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
class INET_API TraCIVehicleMobility : public TraCIMobility
{
    public:
        TraCIVehicleMobility() : TraCIMobility() {}
        virtual void handleSelfMessage(cMessage *msg);
        virtual void nextPosition(const Coord& position, std::string road_id = "", double speed = -1, double angle = -1, TraCIScenarioManager::VehicleSignal signals = TraCIScenarioManager::VEH_SIGNAL_UNDEF);

        virtual TraCIScenarioManager::VehicleSignal getSignals() const {
            if (signals == -1) throw cRuntimeError("TraCIVehicleMobility::getSignals called with no signals set yet");
            return signals;
        }
        void commandParkVehicle() {
            getManager()->commandSetVehicleParking(getExternalId());
        }
        bool commandAddVehicle(std::string vehicleId, std::string vehicleTypeId, std::string routeId, std::string laneId, double emitPosition, double emitSpeed) {
            return getManager()->commandAddVehicle(vehicleId, vehicleTypeId, routeId, laneId, emitPosition, emitSpeed);
        }

    protected:
        TraCIScenarioManager::VehicleSignal signals; /**<updated by nextPosition() */
        /**
         * Returns the amount of CO2 emissions in grams/second, calculated for an average Car
         * @param v speed in m/s
         * @param a acceleration in m/s^2
         * @returns emission in g/s
         */
        virtual double calculateCO2emission(double v, double a) const;
};

class TraCIVehicleMobilityAccess : public ModuleAccess<TraCIVehicleMobility>
{
    public:
        TraCIVehicleMobilityAccess() : ModuleAccess<TraCIVehicleMobility>("mobility") {};
};


#endif
#endif

