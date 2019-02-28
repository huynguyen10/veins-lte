package org.car2x.veins.modules.application.heterogeneous.car;

//
// A simple application that just sends messages.
//
import org.car2x.veins.modules.application.heterogeneous.*;

simple SimpleApp like IHeterogeneousApp {
	parameters:
	    int objectType = default(0);	// VEHILCE_TYPE
		bool debug = default(false);
		bool infoLogging = default(true);
    gates:
        output toDecisionMaker;
        input  fromDecisionMaker;
}
