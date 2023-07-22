#include "BaseStateBuilding.h"
#include "actor.h"

#include <iostream>

bool BaseStateBuilding::doAction(Actor* actor) {
    // Implementation for BaseStateBuilding state
    std::cout << "State: BaseStateBuilding!\n";
    return false;
}