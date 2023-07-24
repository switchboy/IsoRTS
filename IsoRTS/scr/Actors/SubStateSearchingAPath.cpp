#include "SubStateSearchingAPath.h"
#include "actor.h"

#include <iostream>

bool SubStateSearchingAPath::doAction(Actor* actor) {
    // Implementation for SubStateSearchingAPath state
    actor->calculateRoute();
    if (actor->_route.pathFound) {
        return true;
    }
    return false;
}