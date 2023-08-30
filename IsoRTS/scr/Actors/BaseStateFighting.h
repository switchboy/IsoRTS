#pragma once
#include "stateBase.h"
#include "actorStructs.h"

class BaseStateFighting : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
    cords getActionPreformedOn() override;
    void setActionPreformedOn(cords location) override;
    ModesOfAttack getModeOfAttack();

private:
    targetTypes getTargettype() const;
    int getIdOfTarget() const;
    FightOrFlight fightOrFlight(Actor* actor);

private:
    ModesOfAttack _modeOfAttack;
    int _idOfTarget;
    targetTypes _targetType;
    bool _wasAttakOrder = true;
    bool _isInitialised = false;


    friend class groundStateAttaking;
    friend class groundStateFleeing;
    friend class subStateResumePreviousTask;
    friend class subStateSearchNextTarget;
    friend class subStateInRangeMelee;
    friend class subSateInRangeRanged;
    friend class subStateSettingGoalToFleeTo;

};

