#include "veins/base/utils/Coord.h"

#ifdef WITH_INET

#define WITH_TRACI
#include "mobility/single/TraCIVehicleMobility.h"
#include "mobility/single/TraCIPersonMobility.h"

void ifInetTraCIMobilityCallPreInitialize(int objectType, cModule* mod, const std::string& nodeId, const Coord& position, const std::string& road_id, double speed, double angle) {
    if (objectType == TraCIScenarioManager::VEHICLE_TYPE) {
        ::TraCIVehicleMobility* inetmm = dynamic_cast< ::TraCIVehicleMobility*>(mod);
        if (!inetmm) return;
        inetmm->preInitialize(nodeId, ::Coord(position.x, position.y), road_id, speed, angle);
    } else {
        ::TraCIPersonMobility* inetmm = dynamic_cast< ::TraCIPersonMobility*>(mod);
        if (!inetmm) return;
        inetmm->preInitialize(nodeId, ::Coord(position.x, position.y), road_id, speed, angle);
    }
}


void ifInetTraCIMobilityCallNextPosition(int objectType, cModule* mod, const Coord& p, const std::string& edge, double speed, double angle) {
    if (objectType == TraCIScenarioManager::VEHICLE_TYPE) {
        ::TraCIVehicleMobility *inetmm = dynamic_cast< ::TraCIVehicleMobility*>(mod);
        if (!inetmm) return;
        inetmm->nextPosition(::Coord(p.x, p.y), edge, speed, angle);
    } else {
        ::TraCIPersonMobility *inetmm = dynamic_cast< ::TraCIPersonMobility*>(mod);
        if (!inetmm) return;
        inetmm->nextPosition(::Coord(p.x, p.y), edge, speed, angle);
    }
}

#else // not WITH_INET

void ifInetTraCIMobilityCallPreInitialize(int objectType, cModule* mod, const std::string& nodeId, const Coord& position, const std::string& road_id, double speed, double angle) {
	return;
}


void ifInetTraCIMobilityCallNextPosition(int objectType, cModule* mod, const Coord& p, const std::string& edge, double speed, double angle) {
	return;
}

#endif // WITH_INET
