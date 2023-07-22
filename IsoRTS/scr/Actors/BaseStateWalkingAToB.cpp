#include "BaseStateWalkingAToB.h"
#include "actor.h"

#include <iostream>

bool BaseStateWalkingAToB::doAction(Actor* actor) {
    // Implementation for BaseStateWalkingAToB state
    std::cout << "State: BaseStateWalkingAToB!\n";
    return false;
}