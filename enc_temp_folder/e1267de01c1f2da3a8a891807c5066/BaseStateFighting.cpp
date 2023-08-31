#include "BaseStateFighting.h"
#include "actor.h"
#include "../buildings.h"
#include "../gamestate.h"
#include <iostream>

bool BaseStateFighting::doAction(Actor* actor) {

    if (!_isInitialised) {
        if (actor->_listOfOrders.front().orderType != stackOrderTypes::stackActionAttack) {
            _wasAttakOrder = false;
        }
        if (!currentGame.occupiedByActorList[actor->_actorGoal.x][actor->_actorGoal.y].empty()) {
            _idOfTarget = currentGame.occupiedByActorList[actor->_actorGoal.x][actor->_actorGoal.y].back();
            _targetType = targetTypes::actor;
        }
        else if(currentGame.buildingLocationList[actor->_actorGoal.x][actor->_actorGoal.y] != -1 ){
            _idOfTarget = currentGame.buildingLocationList[actor->_actorGoal.x][actor->_actorGoal.y];
            _targetType = targetTypes::building;
        }
        else if (currentGame.objectLocationList[actor->_actorGoal.x][actor->_actorGoal.y] != -1) {
            _idOfTarget = currentGame.objectLocationList[actor->_actorGoal.x][actor->_actorGoal.y];
            _targetType = targetTypes::object;
        }
        else {
            _targetType = targetTypes::groundTile;
            _idOfTarget = actor->_actorRealGoal.y * MAP_HEIGHT + actor->_actorRealGoal.x;
        }
        _isInitialised = true;
    }

    switch (actor->_groundState->_ground) {
    case GroundStateNames::NONE:
        if (fightOrFlight(actor) == FightOrFlight::fight) {
            actor->switchGroundState(GroundStateNames::Attacking);
            return false;
        }
        actor->switchGroundState(GroundStateNames::Fleeing);
        return false;
    default:
        return true;
    }
}



cords BaseStateFighting::getActionPreformedOn()
{
    return { _idOfTarget, static_cast<int>(_targetType) };
}

void BaseStateFighting::setActionPreformedOn(cords location)
{
    _targetType = static_cast<targetTypes>(location.y);
    _idOfTarget = location.x;
}

bool canActorWinThisFight(int enemyHitpoints, int enemyDamage, int actorHitpoints, int actorDamage) {
    if (actorDamage <= 0 || enemyDamage <= 0) {
        return false;
    }
    if (enemyHitpoints / actorDamage <= actorHitpoints / enemyDamage) {
        return true;
    }
    return false;
}

ModesOfAttack BaseStateFighting::getModeOfAttack()
{
    return _modeOfAttack;
}

targetTypes BaseStateFighting::getTargettype() const
{
    return _targetType;
}

int BaseStateFighting::getIdOfTarget() const
{
    return _idOfTarget;
}

FightOrFlight BaseStateFighting::fightOrFlight(Actor* actor)
{
    if (_wasAttakOrder) {
        return FightOrFlight::fight;
    }
    int enemyDMG = 0;
    int myDMG = (_modeOfAttack == ModesOfAttack::ranged) ? actor->_rangedDamage : actor->_meleeDamage;
    switch (_targetType) {
    case targetTypes::actor:
        enemyDMG = std::max(listOfActors[_idOfTarget].getMeleeDMG(), listOfActors[_idOfTarget].getRangedDMG());
        if (canActorWinThisFight(listOfActors[_idOfTarget].getHealth().first, enemyDMG, actor->_actorHealth, myDMG)) {
            return FightOrFlight::fight;
        }
        return FightOrFlight::flight;
    case targetTypes::building:
        enemyDMG = listOfBuildings[_idOfTarget].getRangedDMG();
        if (canActorWinThisFight(listOfBuildings[_idOfTarget].getHealth().first, enemyDMG, actor->_actorHealth, myDMG)) {
            return FightOrFlight::fight;
        }
        return FightOrFlight::flight;
    default:
        return FightOrFlight::fight;
  }

}
