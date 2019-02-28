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

#include <limits>
#include <iostream>
#include <sstream>

#include "veins/modules/mobility/traci/TraCIVehicleMobility.h"

using Veins::TraCIVehicleMobility;

Define_Module(Veins::TraCIVehicleMobility);

const simsignalwrap_t TraCIVehicleMobility::parkingStateChangedSignal = simsignalwrap_t(TRACI_SIGNAL_PARKING_CHANGE_NAME);

namespace {
	const double MY_INFINITY = (std::numeric_limits<double>::has_infinity ? std::numeric_limits<double>::infinity() : std::numeric_limits<double>::max());
}

void TraCIVehicleMobility::handleSelfMsg(cMessage *msg)
{
	if (msg == startAccidentMsg) {
		getVehicleCommandInterface()->setSpeed(0);
		simtime_t accidentDuration = par("accidentDuration");
		scheduleAt(simTime() + accidentDuration, stopAccidentMsg);
		accidentCount--;
	}
	else if (msg == stopAccidentMsg) {
		getVehicleCommandInterface()->setSpeed(-1);
		if (accidentCount > 0) {
			simtime_t accidentInterval = par("accidentInterval");
			scheduleAt(simTime() + accidentInterval, startAccidentMsg);
		}
	}
}

void TraCIVehicleMobility::nextPosition(const Coord& position, std::string road_id, double speed, double angle, TraCIScenarioManager::VehicleSignal signals)
{
	if (debug) EV << "nextPosition " << position.x << " " << position.y << " " << road_id << " " << speed << " " << angle << std::endl;
	isPreInitialized = false;
	this->roadPosition = position;
	this->road_id = road_id;
	this->speed = speed;
	this->angle = angle;
	this->signals = signals;

	changePosition();
}

void TraCIVehicleMobility::changeParkingState(bool newState) {
	isParking = newState;
	emit(parkingStateChangedSignal, this);
}

double TraCIVehicleMobility::calculateCO2emission(double v, double a) const {
	// Calculate CO2 emission parameters according to:
	// Cappiello, A. and Chabini, I. and Nam, E.K. and Lue, A. and Abou Zeid, M., "A statistical model of vehicle emissions and fuel consumption," IEEE 5th International Conference on Intelligent Transportation Systems (IEEE ITSC), pp. 801-809, 2002

	double A = 1000 * 0.1326; // W/m/s
	double B = 1000 * 2.7384e-03; // W/(m/s)^2
	double C = 1000 * 1.0843e-03; // W/(m/s)^3
	double M = 1325.0; // kg

	// power in W
	double P_tract = A*v + B*v*v + C*v*v*v + M*a*v; // for sloped roads: +M*g*sin_theta*v

	/*
	// "Category 7 vehicle" (e.g. a '92 Suzuki Swift)
	double alpha = 1.01;
	double beta = 0.0162;
	double delta = 1.90e-06;
	double zeta = 0.252;
	double alpha1 = 0.985;
	*/

	// "Category 9 vehicle" (e.g. a '94 Dodge Spirit)
	double alpha = 1.11;
	double beta = 0.0134;
	double delta = 1.98e-06;
	double zeta = 0.241;
	double alpha1 = 0.973;

	if (P_tract <= 0) return alpha1;
	return alpha + beta*v*3.6 + delta*v*v*v*(3.6*3.6*3.6) + zeta*a*v;
}

Coord TraCIVehicleMobility::calculateAntennaPosition(const Coord& vehiclePos) const {
	Coord corPos;
	if (antennaPositionOffset >= 0.001) {
		//calculate antenna position of vehicle according to antenna offset
		corPos = Coord(vehiclePos.x - antennaPositionOffset*cos(angle), vehiclePos.y + antennaPositionOffset*sin(angle), vehiclePos.z);
	} else {
		corPos = Coord(vehiclePos.x, vehiclePos.y, vehiclePos.z);
	}
	return corPos;
}
