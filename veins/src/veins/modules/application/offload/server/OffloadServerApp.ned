package org.car2x.veins.modules.application.offload.server;

import inet.applications.IUDPApp;

simple OffloadServerApp like IUDPApp {
	parameters:
		int localPort = default(4242);
		bool debug = default(false);
		bool infoLogging = default(false);
		int denmMsgLength @unit(B) = default(500 B);
		double msgLength @unit(B) = default(400 B);
		double msgLengthSd @unit(B) = default(100 B);

		@signal[phoneToServerDelay](type="simtime_t");
		@statistic[phoneToServerDelay](title="phoneToServerDelay"; source="phoneToServerDelay"; record=vector, stats);
		
		@signal[numSent](type="long");
		@statistic[numSent](title="numSent"; source="numSent"; record=stats);
		
		@signal[numSent](type="long");
		@statistic[numReceived](title="numReceived"; source="numReceived"; record=stats);
	gates:
		input udpIn @labels(UDPControlInfo/up);
		output udpOut @labels(UDPControlInfo/down);
}
