//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

import org.car2x.veins.base.connectionManager.ConnectionManager;
import org.car2x.veins.base.modules.BaseWorldUtility;
import org.car2x.veins.modules.mobility.traci.TraCIScenarioManagerForker;
import org.car2x.veins.modules.mobility.traci.TraCIScenarioManagerLaunchd;
import org.car2x.veins.modules.obstacle.ObstacleControl;
import org.car2x.veins.modules.world.annotations.AnnotationManager;

import inet.networklayer.autorouting.ipv4.IPv4NetworkConfigurator;
import inet.nodes.inet.AdhocHost;
import inet.world.radio.ChannelControl;
import inet.nodes.inet.Router;
import inet.nodes.inet.StandardHost;
import inet.nodes.ethernet.Eth10G;
import inet.networklayer.ipv4.RoutingTableRecorder;

import lte.corenetwork.binder.*;
import lte.corenetwork.nodes.*;

module BaseScenario
{
    parameters:
        double playgroundSizeX @unit(m); // x size of the area the nodes are in (in meters)
        double playgroundSizeY @unit(m); // y size of the area the nodes are in (in meters)
        double playgroundSizeZ @unit(m); // z size of the area the nodes are in (in meters)
        @display("bgb=595,396");
        int numUe = default(100);

    submodules:
        obstacles: ObstacleControl {
            @display("p=420,351");
        }
        annotations: AnnotationManager {
            @display("p=350,351");
        }
        connectionManager: ConnectionManager {
            parameters:
                @display("p=182,351;i=abstract/multicast");
        }
        world: BaseWorldUtility {
            parameters:
                playgroundSizeX = playgroundSizeX;
                playgroundSizeY = playgroundSizeY;
                playgroundSizeZ = playgroundSizeZ;
                @display("p=94,351;i=misc/globe");
        }
        manager: TraCIScenarioManagerForker {
            parameters:
                @display("p=276,351");
        }

        // needed for INET
        channelControl: ChannelControl {
            @display("p=507,350");
        }
        configurator: IPv4NetworkConfigurator {
            @display("p=507,256");
        }

        // LTE stuff
        binder: LteBinder {
            @display("p=507,205;is=s");
        }
        eNodeB1: eNodeB {
            @display("p=271,239;is=vl");
        }
        server: StandardHost {
            @display("p=324,250;is=n;i=device/server");
        }
        routingRecorder: RoutingTableRecorder {
            @display("p=507,303;is=s");
        }
        ue[numUe]: Ue {
            @display("p=507,149");
        }
    connections allowunconnected:
        server.pppg++ <--> Eth10G <--> eNodeB1.ppp;
}
