#include "SubStateSearchingAPath.h"
#include "actor.h"

#include <iostream>

bool SubStateSearchingAPath::doAction(Actor* actor) {
    // Implementation for SubStateSearchingAPath state
    actor->calculateRoute();
    if (actor->_route.pathFound) {
        return true;
    }
    _tries += 1;
    if (_tries > 10) {
        switch (actor->_baseState->_base) {
        case BaseStateNames::Gathering:
            actor->switchGroundState(GroundStateNames::FindAlternativeSource);
            return false;
        case BaseStateNames::Building:
            actor->switchGroundState(GroundStateNames::SearchAlternativeBuildingSpot);
            return false;
        default:
            actor->switchBaseState(BaseStateNames::Idle);
            return false;
        }
    }
    return false;
}