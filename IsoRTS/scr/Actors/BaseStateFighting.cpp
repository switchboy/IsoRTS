#include "BaseStateFighting.h"
#include "actor.h"

#include <iostream>

bool BaseStateFighting::doAction(Actor* actor) {
    // Implementation for BaseStateFighting state
    std::cout << "State: BaseStateFighting!\n";
    return false;
}

ModesOfAttack BaseStateFighting::getModeOfAttack()
{
    return _modeOfAttack;
}
