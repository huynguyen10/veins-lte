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

#include "IHeterogeneousApp.h"

Define_Module(IHeterogeneousApp);

void IHeterogeneousApp::initialize(int stage) {
	if (stage == 0) {
	    objectType = par("objectType");
		debug = par("debug").boolValue();
		infoLogging = par("infoLogging");
		toDecisionMaker = findGate("toDecisionMaker");
		fromDecisionMaker = findGate("fromDecisionMaker");
		numReceived = 0;
		numSent = 0;

		cModule *tmpMobility = getParentModule()->getSubmodule("veinsmobility");
		if (objectType == TraCIScenarioManager::PERSON_TYPE) {
		    Veins::TraCIPersonMobility* mobility = dynamic_cast<Veins::TraCIPersonMobility *>(tmpMobility);
		    ASSERT(mobility);
		    sumoId = mobility->getExternalId();
		} else {
		    Veins::TraCIVehicleMobility* mobility = dynamic_cast<Veins::TraCIVehicleMobility *>(tmpMobility);
		    ASSERT(mobility);
		    sumoId = mobility->getExternalId();
		}
	}
}
