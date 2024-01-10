#pragma once
#include "stateBase.h"

class BaseStateIdle : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
};