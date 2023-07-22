#include "GroundStateReturningTheResource.h"
#include "actor.h"

#include <iostream>

bool GroundStateReturningTheResource::doAction(Actor* actor) {
    // Implementation for GroundStateReturningTheResource state
    std::cout << "State: GroundStateReturningTheResource!\n";
    return false;
}