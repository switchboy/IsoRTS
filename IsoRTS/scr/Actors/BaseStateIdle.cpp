#include "BaseStateIdle.h"
#include "actor.h"

#include <iostream>

bool BaseStateIdle::doAction(Actor* actor) {
    // Implementation for BaseStateIdle state
    std::cout << "State: BaseStateIdle!\n";
    return false;
}