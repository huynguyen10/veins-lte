package org.car2x.veins.modules.application.offload.ue;

import inet.applications.IUDPApp;

simple SimpleUDPClient like IUDPApp {
	parameters:
		int localPort = default(4242);
		bool debug = default(false);
		bool infoLogging = default(false);
		double msgLength @unit(B) = default(400 B);
		double msgLengthSd @unit(B) = default(100 B);
		double sendInterval @unit(s) = default(1.0 s);
		double sendIntervalSd @unit(s) = default(0.5 s);
				
	gates:
		input udpIn @labels(UDPControlInfo/up);
		output udpOut @labels(UDPControlInfo/down);
}
