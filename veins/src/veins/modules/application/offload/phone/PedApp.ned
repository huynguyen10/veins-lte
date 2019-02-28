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

package org.car2x.veins.modules.application.offload.phone;

//
// A simple application that just sends messages.
//
import inet.applications.IUDPApp;

simple PedApp like IUDPApp {
	parameters:
	    bool debug = default(false);
		bool infoLogging = default(false);
		double samplingFreq @unit(Hz) = default(50.0 Hz);
		double windowLength  @unit(s) = default(0.1 s);
		string mode = default("plusplus");	// Operation mode: local, window, or server
		string classifier = default("J48"); // Classifier used: J48, KNN, NaiveBayes
		
		int localPort = default(4242);
		
		int camMsgLength @unit(B) = default(500 B);
		xml localTimeModels;	// Model for local processing time

		@signal[localTime](type="simtime_t");
		@statistic[localTime](title="localTime"; source="localTime"; record=vector, stats);
		
		@signal[serverToPhoneDelay](type="simtime_t");
		@statistic[serverToPhoneDelay](title="serverToPhoneDelay"; source="serverToPhoneDelay"; record=vector, stats);
		
		@signal[offloadDelay](type="simtime_t");
		@statistic[offloadDelay](title="offloadDelay"; source="offloadDelay"; record=vector, stats);
		
		@signal[numSent](type="long");
		@statistic[numSent](title="numSent"; source="numSent"; record=stats);
		
		@signal[numSent](type="long");
		@statistic[numReceived](title="numReceived"; source="numReceived"; record=stats);
			
	gates:
		input udpIn @labels(UDPControlInfo/up);
		output udpOut @labels(UDPControlInfo/down);
}
