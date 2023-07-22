#include "BaseStateDead.h"
#include "actor.h"

#include <iostream>

bool BaseStateDead::doAction(Actor* actor) {
    // Implementation for BaseStateDead state
    std::cout << "State: BaseStateDead!\n";
    return false;
}