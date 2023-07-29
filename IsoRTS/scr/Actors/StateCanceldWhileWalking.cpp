#include "StateCanceldWhileWalking.h"
#include "actor.h"

#include <iostream>

bool StateCanceldWhileWalking::doAction(Actor* actor) {
    // Implementation for StateCanceldWhileWalking state
    std::cout << "State: StateCanceldWhileWalking!\n";
    return false;
}