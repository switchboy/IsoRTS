#include "SubStateFindingNewTarget.h"
#include "actor.h"
#include "../buildings.h"
#include "../gamestate.h"
#include <iostream>

bool SubStateFindingNewTarget::doAction(Actor* actor) {
    std::list<cords> cordsInRange = getListOfCordsInCircle(actor->_actorCords.x, actor->_actorCords.y, 10);
    for (const cords& tile : cordsInRange)
    {
        if (!currentGame.occupiedByActorList[tile.x][tile.y].empty()) {
            for (const int& id : currentGame.occupiedByActorList[tile.x][tile.y]) {
                if (listOfActors[id].getTeam() != actor->_actorTeam) {
                    actor->_actorGoal = listOfActors[id].getActorCords();
                    actor->_baseState->setActionPreformedOn({ id, static_cast<int>(targetTypes::actor) });
                    actor->switchSubState(SubStateNames::SearchingAPath);
                    return false;
                }
            }
        }
        if (currentGame.occupiedByBuildingList[tile.x][tile.y] != -1) {
            if (listOfBuildings[currentGame.occupiedByBuildingList[tile.x][tile.y]].getTeam() != actor->_actorTeam) {
                actor->_actorGoal = tile;
                actor->_baseState->setActionPreformedOn({ currentGame.occupiedByBuildingList[tile.x][tile.y], static_cast<int>(targetTypes::building) });
                actor->switchSubState(SubStateNames::SearchingAPath);
                return false;
            }
        }
    }
    return true;
}