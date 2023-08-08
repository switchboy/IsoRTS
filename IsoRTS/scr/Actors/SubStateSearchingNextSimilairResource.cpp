#include "SubStateSearchingNextSimilairResource.h"
#include "actor.h"
#include <iostream>

bool SubStateSearchingNextSimilairResource::doAction(Actor* actor) {
    cords newResourceCords = findResource(actor->_resourceBeingGatherd, actor->_actorId);
    if (newResourceCords.x == -1) {
        std::cout << "Actor " << actor->_actorId << " could not find a reachable alternative resource! \n";
        _tries += 1;
        if (_tries > 10) {
            actor->switchBaseState(BaseStateNames::Idle);
            return false;
        }
        return false;
    }
    std::cout << "Actor " << actor->_actorId << " found an alternative resource @ x: "<< newResourceCords .x <<" y: "<< newResourceCords .y << "! \n";
    actor->_actorGoal = newResourceCords;
    actor->_cantPassActors = true;
    actor->calculateRoute();
    actor->_cantPassActors = false;
    actor->_baseState->setActionPreformedOn(newResourceCords);
    return true;
}