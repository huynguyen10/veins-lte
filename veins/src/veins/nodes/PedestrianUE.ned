// 
//                           SimuLTE
// Copyright (C) 2012 Antonio Virdis, Daniele Migliorini, Giovanni
// Accongiagioco, Generoso Pagano, Vincenzo Pii.
// 
// This file is part of a software released under the license included in file
// "license.pdf". This license can be also found at http://www.ltesimulator.com/
// The above file and the present reference are part of the software itself, 
// and cannot be removed from it.
// 

// @author: Quang-Huy Nguyen
// @date: 25.06.2018
// Pedestrian
//
// This Pedestrian module is based on UE module.

package org.car2x.veins.nodes;

import lte.corenetwork.nodes.Ue;
import inet.networklayer.autorouting.ipv4.HostAutoConfigurator;

// 
// User Equipment Module carryed by Pedestrian
//
module PedestrianUE extends Ue
{
    parameters:
        @node();
        @display("i=device/pocketpc;bgb=400,518");

        string veinsmobilityType;  //type of the mobility module

    submodules:
	    // Other configurations
        veinsmobility: <veinsmobilityType> like org.car2x.veins.base.modules.IMobility {
            parameters:
                @display("p=50,283;i=block/cogwheel");
        }

        configurator: HostAutoConfigurator {
            @display("p=50,227");
        }
}
