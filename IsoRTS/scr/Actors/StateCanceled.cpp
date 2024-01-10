#include "StateCanceled.h"
#include "actor.h"

#include <iostream>

bool StateCanceled::doAction(Actor* actor) {
    return true;
}