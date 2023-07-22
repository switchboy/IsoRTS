
#include <iostream>
#include "Actor.h"
#include "StateBase.h"

//Base states
#include "BaseStateIdle.h"
#include "BaseStateWalkingAToB.h"
#include "BaseStateBuilding.h"
#include "BaseStateGathering.h"
#include "BaseStateFighting.h"
#include "BaseStateDead.h"

//Ground states
#include "GroundStateWalking.h"
#include "GroundStateGatheringTheResource.h"
#include "GroundStateReturningTheResource.h"
#include "GroundStateBuildingTheBuilding.h"
#include "GroundStateAttacking.h"
#include "GroundStateFleeing.h"
#include "GroundStateDecomposing.h"

//Sub states
#include "SubStateWalkingToNextSquare.h"
#include "SubStateSearchingAPath.h"
#include "SubStateSearchingNextSimilairResource.h"
#include "SubStateSearchingDropOffPoint.h"
#include "SubStateSearchingAlternativeBuildingSpot.h"
#include "SubStateSearchingAlternativeBuildingToBuilt.h"
#include "SubStateMeleeAttacking.h"
#include "SubStateRangedAttacking.h"
#include "SubStateSearchNextTarget.h"
#include "SubStateSettingGoalToFleeTo.h"
#include "SubStateSettingMyDecompositionState.h"
#include "SubStateCountingDownToDestroySelf.h"


#include "StateNames.h"

Actor::Actor(BaseStateNames requestState) {
    _baseState = nullptr;
    _groundState = nullptr;
    _subState = nullptr;
    switchBaseState(requestState);
}

void Actor::doAction() {
    if (_subState == nullptr || _subState->doAction(this)) {
        if (_groundState == nullptr || _groundState->doAction(this)) {
            if (_baseState->doAction(this)) {
                switchBaseState(BaseStateNames::Idle);
            }
        }
    }
}

Actor::~Actor() {
    delete _baseState;
    delete _groundState;
    delete _subState;
}

void Actor::switchBaseState(BaseStateNames desiredState) {
    delete _baseState;
    switch (desiredState) {
    case BaseStateNames::NONE:
        _baseState = new StateBase(BaseStateNames::NONE, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    case BaseStateNames::Idle:
        _baseState =  new BaseStateIdle(BaseStateNames::Idle, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    case BaseStateNames::WalikngAToB:
        _baseState = new BaseStateWalkingAToB(BaseStateNames::WalikngAToB, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    case BaseStateNames::Building:
        _baseState = new BaseStateBuilding(BaseStateNames::Building, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    case BaseStateNames::Gathering:
        _baseState = new BaseStateGathering(BaseStateNames::Gathering, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    case BaseStateNames::Fighting:
        _baseState = new BaseStateFighting(BaseStateNames::Fighting, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    case BaseStateNames::Dead:
        _baseState = new BaseStateDead(BaseStateNames::Dead, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    default:
        _baseState = new StateBase(BaseStateNames::NONE, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    }
    switchGroundState(GroundStateNames::NONE);
    switchSubState(SubStateNames::NONE);
}

void Actor::switchGroundState(GroundStateNames desiredState) {
    delete _groundState;

    switch (desiredState) {
    case GroundStateNames::Walking:
        _groundState = new GroundStateWalking(BaseStateNames::Idle, GroundStateNames::Walking, SubStateNames::NONE);
        break;
    case GroundStateNames::GatheringTheResource:
        _groundState = new GroundStateGatheringTheResource(BaseStateNames::Idle, GroundStateNames::Walking, SubStateNames::NONE);
        break;
    case GroundStateNames::ReturningTheResource:
        _groundState = new GroundStateReturningTheResource(BaseStateNames::Idle, GroundStateNames::Walking, SubStateNames::NONE);
        break;
    case GroundStateNames::BuildingTheBuilding:
        _groundState = new GroundStateBuildingTheBuilding(BaseStateNames::Idle, GroundStateNames::Walking, SubStateNames::NONE);
        break;
    case GroundStateNames::Attacking:
        _groundState = new GroundStateAttacking(BaseStateNames::Idle, GroundStateNames::Walking, SubStateNames::NONE);
        break;
    case GroundStateNames::Fleeing:
        _groundState = new GroundStateFleeing(BaseStateNames::Idle, GroundStateNames::Walking, SubStateNames::NONE);
        break;
    case GroundStateNames::Decomposing:
        _groundState = new GroundStateDecomposing(BaseStateNames::Idle, GroundStateNames::Walking, SubStateNames::NONE);
        break;
    default:
        _groundState = new StateBase(BaseStateNames::NONE, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    }
    switchSubState(SubStateNames::NONE);
}

void Actor::switchSubState(SubStateNames desiredState) {
    delete _subState;
    switch (desiredState) {
    case SubStateNames::WalkingToNextSquare:
        _subState = new SubStateWalkingToNextSquare(BaseStateNames::Idle, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    case SubStateNames::SearchingAPath:
        _subState = new SubStateSearchingAPath(BaseStateNames::Idle, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    case SubStateNames::SearchingNextSimilairResource:
        _subState = new SubStateSearchingNextSimilairResource(BaseStateNames::Idle, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    case SubStateNames::SearchingDropOffPoint:
        _subState = new SubStateSearchingDropOffPoint(BaseStateNames::Idle, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    case SubStateNames::SearchingAlternativeBuildingSpot:
        _subState = new SubStateSearchingAlternativeBuildingSpot(BaseStateNames::Idle, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    case SubStateNames::SearchingAlternativeBuildingToBuilt:
        _subState = new SubStateSearchingAlternativeBuildingToBuilt(BaseStateNames::Idle, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    case SubStateNames::MeleeAttacking:
        _subState = new SubStateMeleeAttacking(BaseStateNames::Idle, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    case SubStateNames::RangedAttacking:
        _subState = new SubStateRangedAttacking(BaseStateNames::Idle, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    case SubStateNames::SearchNextTarget:
        _subState = new SubStateSearchNextTarget(BaseStateNames::Idle, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    case SubStateNames::SettingGoalToFleeTo:
        _subState = new SubStateSettingGoalToFleeTo(BaseStateNames::Idle, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    case SubStateNames::SettingMyDecompositionState:
        _subState = new SubStateSettingMyDecompositionState(BaseStateNames::Idle, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    case SubStateNames::CountingDownToDestroySelf:
        _subState = new SubStateCountingDownToDestroySelf(BaseStateNames::Idle, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    default:
        _subState = new StateBase(BaseStateNames::NONE, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    }
}