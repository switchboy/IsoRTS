#include "SubStateSearchingAlternativeBuildingSpot.h"
#include "actor.h"
#include "../buildings.h"

#include <iostream>
#include "../cells.h"

/*
struct adjacentTile
{
    int tileId;
    cords tileCords;
    cords goal;
    bool occupied;
    int actorId;
};

*/

bool SubStateSearchingAlternativeBuildingSpot::doAction(Actor* actor) {

    std::vector<adjacentTile> freeBuildingSlots = listOfBuildings[actor->_buildingId].getFreeBuildingTile();

    std::sort(freeBuildingSlots.begin(), freeBuildingSlots.end(), [actor](const adjacentTile& a, const adjacentTile& b) {
        return dist(a.tileCords.x, a.tileCords.y, actor->_actorCords.x, actor->_actorCords.y) >
            dist(b.tileCords.x, b.tileCords.y, actor->_actorCords.x, actor->_actorCords.y);
        });

    while (!freeBuildingSlots.empty()) {
        if (canReachTarget(actor->_actorCords, freeBuildingSlots.back().tileCords, true) ){
            if (freeBuildingSlots.back().tileCords.x == 0 && freeBuildingSlots.back().tileCords.y == 0) {
                std::cout << "Actor " << actor->_actorId << ": Got fed an unlikly target! \n";
                freeBuildingSlots.pop_back();
                continue;
            }
            std::cout << "Actor " << actor->_actorId << ": new slot found @ X: "<< freeBuildingSlots.back().tileCords.x <<"Y: "<< freeBuildingSlots.back().tileCords.y <<  "! \n";
            listOfBuildings[actor->_buildingId].removeActorFromBuildingTile(actor->_actorId);
            listOfBuildings[actor->_buildingId].claimFreeBuiildingTile(freeBuildingSlots.back().tileId, actor->_actorId);
            actor->_actorGoal = freeBuildingSlots.back().tileCords;
            actor->switchGroundState(GroundStateNames::Walking);
            return false;
        }
       freeBuildingSlots.pop_back();
    } 
    return true;
}