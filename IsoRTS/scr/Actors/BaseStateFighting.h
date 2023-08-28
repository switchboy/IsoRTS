#pragma once
#include "stateBase.h"
#include "actorStructs.h"

class BaseStateFighting : public StateBase {
public:
    using StateBase::StateBase;
    bool doAction(Actor* actor) override;
    ModesOfAttack getModeOfAttack();

private:
    targetTypes getTargettype();
    int getIdOfTarget();
    fightOrFlight();

private:
    ModesOfAttack _modeOfAttack;
    int _idOfTarget;
    targetTypes _targetType;

    friend class groundStateAttaking;
    friend class groundStateFleeing;
    friend class subStateResumePreviousTask;
    friend class subStateSearchNextTarget;
    friend class subStateInRangeMelee;
    friend class subSateInRangeRanged;
    friend class subStateSettingGoalToFleeTo;

};

