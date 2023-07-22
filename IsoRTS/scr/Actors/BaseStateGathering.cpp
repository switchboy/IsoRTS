#include "BaseStateGathering.h"
#include "actor.h"

#include <iostream>

bool BaseStateGathering::doAction(Actor* actor) {
    // Implementation for BaseStateGathering state
    std::cout << "State: BaseStateGathering!\n";
    return false;
}