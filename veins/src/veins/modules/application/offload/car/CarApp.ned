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

package org.car2x.veins.modules.application.offload.car;

import org.car2x.veins.modules.application.heterogeneous.*;

//
// A simple application that just sends messages.
//
simple CarApp like IHeterogeneousApp {
	parameters:
	    int objectType = default(0);	// VEHICLE_TYPE
		bool debug = default(false);
		bool infoLogging = default(false);
		
		@signal[endToEndDelay](type="simtime_t");
		@statistic[endToEndDelay](title="endToEndDelay"; source="endToEndDelay"; record=vector, stats);
		
		@signal[serverToCarDelay](type="simtime_t");
		@statistic[serverToCarDelay](title="serverToCarDelay"; source="serverToCarDelay"; record=vector, stats);
		
		@signal[numSent](type="long");
		@statistic[numSent](title="numSent"; source="numSent"; record=stats);
		
		@signal[numSent](type="long");
		@statistic[numReceived](title="numReceived"; source="numReceived"; record=stats);
    gates:
        output toDecisionMaker;
        input  fromDecisionMaker;
}
