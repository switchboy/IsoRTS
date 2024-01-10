#pragma once

#include "StateNames.h"
#include "../globalfunctions.h"

class Actor;  // Forward declaration

class StateBase {
public:
    StateBase(BaseStateNames base, GroundStateNames ground, SubStateNames sub) {
        _base = base;
        _ground = ground;
        _sub = sub;
    }
    virtual bool doAction(Actor* actor);
    virtual ModesOfAttack getModeOfAttack();
    virtual bool getMoved();
    virtual cords getActionPreformedOn();
    virtual void setActionPreformedOn(cords location);
    BaseStateNames _base;
    GroundStateNames _ground;
    SubStateNames _sub;
};