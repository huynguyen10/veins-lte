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

#ifdef WITH_TRACI

#include <limits>
#include <iostream>
#include <sstream>

#include "FWMath.h"  // for M_PI
#include "mobility/single/TraCIPersonMobility.h"

Define_Module(TraCIPersonMobility);

void TraCIPersonMobility::handleSelfMessage(cMessage *msg)
{
    if (msg == startAccidentMsg) {
        commandSetSpeed(TraCIScenarioManager::PERSON_TYPE, 0);
        simtime_t accidentDuration = par("accidentDuration");
        scheduleAt(simTime() + accidentDuration, stopAccidentMsg);
        accidentCount--;
    }
    else if (msg == stopAccidentMsg) {
        commandSetSpeed(TraCIScenarioManager::PERSON_TYPE, -1);
        if (accidentCount > 0) {
            simtime_t accidentInterval = par("accidentInterval");
            scheduleAt(simTime() + accidentInterval, startAccidentMsg);
        }
    }
}

void TraCIPersonMobility::nextPosition(const Coord& position, std::string road_id, double speed, double angle)
{
    EV_DEBUG << "next position = " << position << " " << road_id << " " << speed << " " << angle << std::endl;
    isPreInitialized = false;
    nextPos = position;
    this->road_id = road_id;
    this->speed = speed;
    this->angle = angle;
    move();
}

#endif
