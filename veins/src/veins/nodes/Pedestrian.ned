// @author: Quang-Huy Nguyen
// @date: 25.06.2018
// Pedestrian
//
// This Pedestrian module provides two interfaces for sending and receiving messages via LTE and/or DSRC.

package org.car2x.veins.nodes;

import org.car2x.veins.base.modules.*;
import org.car2x.veins.modules.nic.Nic80211p;

import org.car2x.veins.modules.lte.*;
import org.car2x.veins.modules.application.heterogeneous.*;
import lte.corenetwork.nodes.*;

import inet.nodes.inet.NetworkLayerNoConfig;
import lte.stack.phy.LteNicUe;
import inet.transport.udp.UDP;
import inet.transport.tcp.TCP;
import inet.applications.ITCPApp;

import inet.networklayer.ipv4.RoutingTable;
import inet.mobility.contract.IMobility;
import inet.base.NotificationBoard;
import inet.networklayer.common.InterfaceTable;
import inet.networklayer.autorouting.ipv4.HostAutoConfigurator;

module Pedestrian
{
    parameters:
        @node();

        string applType;           //type of the application layer
        string veinsmobilityType;  //type of the mobility module
        string mobilityType;       // INET mobility

        // Following Parameters taken from Ue.ned to make LTE work
        string nodeType = "UE";  // DO NOT CHANGE
        int masterId;
        int macNodeId = default(0); // TODO: this is not a real parameter
        int macCellId = default(0); // TODO: this is not a real parameter

        @display("bgb=383,573");

    gates:
        input radioIn @directIn; //LTE input gate

    submodules:
        interfaceTable: InterfaceTable {
            @display("p=307,190;is=s");
        }

        application: <applType> like IHeterogeneousApp {
            @display("p=144,64;i=block/app2");
        }

        decisionMaker: ExampleDecisionMaker {
            parameters:
                @display("p=144,148;i=block/fork");
        }

        nic80211p: Nic80211p {
            parameters:
                @display("p=53,509");
        }

        heterogeneousToLTE: HeterogeneousToLTE {
            parameters:
                @display("p=183,271;i=abstract/accesspoint");
        }

        UDP: UDP {
            @display("p=183,348");
        }

        networkLayer: NetworkLayerNoConfig {
            @display("p=183,428");
        }

        // NOTE: instance must be named "nic", therefore the other nic is called nic80211p
        nic: LteNicUe {
            nodeType = nodeType;
            @display("p=183,509");
        }

		// Other configurations
        veinsmobility: <veinsmobilityType> like org.car2x.veins.base.modules.IMobility {
            parameters:
                @display("p=307,64;i=block/cogwheel");
        }

        notificationBoard: NotificationBoard {
            @display("p=307,140;is=s");
        }

        routingTable: RoutingTable {
            @display("p=307,240;is=s");
        }

        configurator: HostAutoConfigurator {
            @display("p=307,299");
        }
        
        mobility: <mobilityType> like IMobility {
            @display("p=307,356;is=s");
        }
    connections allowunconnected:

        application.toDecisionMaker --> decisionMaker.fromApplication;
        decisionMaker.toApplication --> application.fromDecisionMaker;

        nic80211p.upperLayerOut --> decisionMaker.lowerLayerIn;
        nic80211p.upperLayerIn <-- decisionMaker.lowerLayerOut;

        nic80211p.upperControlOut --> decisionMaker.lowerControlIn;
        nic80211p.upperControlIn <-- decisionMaker.lowerControlOut;

        heterogeneousToLTE.toApplication --> decisionMaker.fromLte;
        decisionMaker.toLte --> heterogeneousToLTE.fromApplication;

        heterogeneousToLTE.toLTE --> UDP.appIn++;
        UDP.ipOut --> networkLayer.transportIn++;

        heterogeneousToLTE.fromLTE <-- UDP.appOut++;
        UDP.ipIn <-- networkLayer.transportOut++;

        networkLayer.ifOut++ --> nic.upperLayerIn;
        networkLayer.ifIn++ <-- nic.upperLayerOut;

        radioIn --> nic.radioIn;
}
