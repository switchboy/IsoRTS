#pragma once
#include "stateBase.h"

class SubStateSearchingAlternativeBuildingToBuilt : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
};