#include "SubStateSearchingDropOffPoint.h"
#include "actor.h"
#include "ActorHelper.h"
#include "../buildings.h"
#include <iostream>

bool SubStateSearchingDropOffPoint::doAction(Actor* actor) {

    std::list <nearestBuildingTile> listOfDropOffLocations;
    for (int i = 0; i < listOfBuildings.size(); i++)
    {
        if ((listOfBuildings[i].getRecievesWhichResources() == actor->_resourceBeingGatherd || listOfBuildings[i].getRecievesWhichResources() == resourceTypes::All) && listOfBuildings[i].getTeam() == actor->_actorTeam && listOfBuildings[i].getCompleted())
        {
            std::vector<adjacentTile> tileList = listOfBuildings[i].getDropOffTiles();
            for (int j = 0; j < tileList.size(); j++)
            {
                float tempDeltaDistance = static_cast<float>(dist(actor->_actorCords.x, actor->_actorCords.y, tileList[j].goal.x, tileList[j].goal.y));
                listOfDropOffLocations.push_back({ tempDeltaDistance, tileList[j].tileCords, tileList[j].goal.x, tileList[j].goal.y, i, true, tileList[j].tileId });
            }
        }
    }

    if (!listOfDropOffLocations.empty())
    {
        listOfDropOffLocations.sort([](const nearestBuildingTile& f, const nearestBuildingTile& s)
            {
                return f.deltaDistance < s.deltaDistance;
            });
    }
    actor->_route.realPath = false;
    while (actor->_route.realPath == false && !listOfDropOffLocations.empty()) {
        actor->_actorGoal = listOfDropOffLocations.front().actionLocation;
        actor->calculateRoute();
        actor->_buildingId = listOfDropOffLocations.front().buildingId;
        if (!actor->_route.realPath) {
            listOfDropOffLocations.pop_front();
        }
    }

    if (listOfDropOffLocations.empty())
    {
        _tries += 1;
        if (_tries < 10) {
            return false;
        }
        actor->switchBaseState(BaseStateNames::Idle);
    }
    return true;
 
}