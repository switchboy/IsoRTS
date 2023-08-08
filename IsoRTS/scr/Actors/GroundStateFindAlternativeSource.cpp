#include "GroundStateFIndAlternativeSource.h"
#include "actor.h"
#include "../gamestate.h"
#include <iostream>

bool GroundStateFindAlternativeSource::doAction(Actor* actor) {
    switch (actor->_subState->_sub) {
    case SubStateNames::NONE:
        actor->switchSubState(SubStateNames::SearchingNextSimilairResource);
        return false;
    case SubStateNames::SearchingNextSimilairResource:
         return true;
    default:
        actor->switchSubState(SubStateNames::SearchingNextSimilairResource);
        return false;
    }
}