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

#ifndef VEINS_MOBILITY_TRACI_TRACIVEHICLEMOBILITY_H
#define VEINS_MOBILITY_TRACI_TRACIVEHICLEMOBILITY_H

#define TRACI_SIGNAL_PARKING_CHANGE_NAME "parkingStateChanged"

#include <string>
#include <fstream>
#include <list>
#include <stdexcept>

#include "veins/modules/mobility/traci/TraCIMobility.h"
#include "veins/base/utils/FindModule.h"
#include "veins/modules/mobility/traci/TraCIScenarioManager.h"
#include "veins/modules/mobility/traci/TraCICommandInterface.h"

/**
 * @brief
 * Used in modules created by the TraCIScenarioManager.
 *
 * This module relies on the TraCIScenarioManager for state updates
 * and can not be used on its own.
 *
 * See the Veins website <a href="http://veins.car2x.org/"> for a tutorial, documentation, and publications </a>.
 *
 * @author Christoph Sommer, David Eckhoff, Luca Bedogni, Bastian Halmos, Stefan Joerer, Quang-Huy Nguyen
 *
 * @see TraCIScenarioManager
 * @see TraCIScenarioManagerLaunchd
 *
 * @ingroup mobility
 */
namespace Veins {
class TraCIVehicleMobility : public TraCIMobility
{
	public:
		TraCIVehicleMobility() : TraCIMobility(), vehicleCommandInterface(0) {}
		~TraCIVehicleMobility() {
			delete vehicleCommandInterface;
		}
		virtual void handleSelfMsg(cMessage *msg);
		virtual void nextPosition(const Coord& position, std::string road_id = "", double speed = -1, double angle = -1, TraCIScenarioManager::VehicleSignal signals = TraCIScenarioManager::VEH_SIGNAL_UNDEF);
		virtual void changeParkingState(bool);
        virtual bool getParkingState() const {
            return isParking;
        }

        virtual TraCIScenarioManager::VehicleSignal getSignals() const {
			if (signals == -1) throw cRuntimeError("TraCIVehicleMobility::getSignals called with no signals set yet");
			return signals;
		}

        virtual TraCICommandInterface::Vehicle* getVehicleCommandInterface() const {
			if (!vehicleCommandInterface) vehicleCommandInterface = new TraCICommandInterface::Vehicle(getCommandInterface()->vehicle(getExternalId()));
			return vehicleCommandInterface;
		}


	protected:
		TraCIScenarioManager::VehicleSignal signals; /**<updated by nextPosition() */

		mutable TraCICommandInterface::Vehicle* vehicleCommandInterface;

		const static simsignalwrap_t parkingStateChangedSignal;

		bool isParking;

		/**
		 * Returns the amount of CO2 emissions in grams/second, calculated for an average Car
		 * @param v speed in m/s
		 * @param a acceleration in m/s^2
		 * @returns emission in g/s
		 */
		virtual double calculateCO2emission(double v, double a) const;

		/**
		 * Calculates where the antenna of this car is, given its front bumper position
		 */
		virtual Coord calculateAntennaPosition(const Coord& vehiclePos) const;
};
}

namespace Veins {
class TraCIVehicleMobilityAccess
{
	public:
		TraCIVehicleMobility* get(cModule* host) {
			TraCIVehicleMobility* traci = FindModule<TraCIVehicleMobility*>::findSubModule(host);
			ASSERT(traci);
			return traci;
		};
};
}

#endif

