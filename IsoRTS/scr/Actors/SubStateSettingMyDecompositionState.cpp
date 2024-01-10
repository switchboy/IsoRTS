#include "SubStateSettingMyDecompositionState.h"
#include "actor.h"

#include <iostream>

bool SubStateSettingMyDecompositionState::doAction(Actor* actor) {
    // Implementation for SubStateSettingMyDecompositionState state
    std::cout << "State: SubStateSettingMyDecompositionState!\n";
    return false;
}