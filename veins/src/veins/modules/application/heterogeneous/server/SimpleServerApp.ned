package org.car2x.veins.modules.application.heterogeneous.server;

import inet.applications.IUDPApp;

simple SimpleServerApp like IUDPApp {
	parameters:
		int localPort = default(4242);
		bool debug = default(false);
		bool infoLogging = default(true);
	gates:
		input udpIn @labels(UDPControlInfo/up);
		output udpOut @labels(UDPControlInfo/down);
}
