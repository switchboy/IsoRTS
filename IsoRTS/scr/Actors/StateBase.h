#pragma once

#include "StateNames.h"

class Actor;  // Forward declaration

class StateBase {
public:
    StateBase(BaseStateNames base, GroundStateNames ground, SubStateNames sub) {
        _base = base;
        _ground = ground;
        _sub = sub;
    }
    virtual bool doAction(Actor* actor);
    BaseStateNames _base;
    GroundStateNames _ground;
    SubStateNames _sub;
};