## Veins LTE

Veins LTE adds LTE support to the vehicular network simulation framework [Veins](http://veins.car2x.org/).

### Prerequisites

* Install [OMNeT++](http://www.omnetpp.org/) (recommended version: 4.6).
* Install [SUMO](https://sumo.dlr.de/wiki/Simulation_of_Urban_MObility_-_Wiki) (recommended version: 0.30.0).
* Clone this repository (or download the zip file).
* If you are on Linux and use the command line:
  * switch to the root of the repository and type
  * `make makefiles`
  * `make`
  * go to the folder `veins/examples/car2p` and run the example.
* If you are on Linux and use the OMNeT++ IDE:
  * File -> Import -> General/Existing Projects into Workspace -> Next
  * select the Veins LTE directory as root and tick all three projects (inet, lte and veins) -> Finish
  * rightclick on veins -> Properties -> OMNeT++ -> tick With INET Framework
  * Project -> Build All (or `CTRL+B`)
  * go to the folder `veins/examples/car2p` -> rightclick on `omnetpp.ini` -> Run As -> OMNeT++ Simulation
  * run with Config: "plusplus", "offload", or "stream"
* If you are on Windows:
  * `TODO` *(Currently there seem to be several issues with Veins LTE on Windows, we are working on them)*

Further information can be found on the [Veins LTE Website](http://veins-lte.car2x.org/).
