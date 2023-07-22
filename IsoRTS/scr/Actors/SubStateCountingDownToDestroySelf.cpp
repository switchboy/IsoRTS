#include "SubStateCountingDownToDestroySelf.h"
#include "actor.h"

#include <iostream>

bool SubStateCountingDownToDestroySelf::doAction(Actor* actor) {
    // Implementation for SubStateCountingDownToDestroySelf state
    std::cout << "State: SubStateCountingDownToDestroySelf!\n";
    return false;
}