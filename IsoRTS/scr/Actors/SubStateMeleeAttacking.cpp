#include "SubStateMeleeAttacking.h"
#include "actor.h"

#include <iostream>

bool SubStateMeleeAttacking::doAction(Actor* actor) {
    // Implementation for SubStateMeleeAttacking state
    std::cout << "State: SubStateMeleeAttacking!\n";
    return false;
}