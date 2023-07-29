#include "SubStateWalkingToNextSquare.h"
#include "actor.h"
#include "../gamestate.h"

#include <iostream>
#include "ActorHelper.h"

bool SubStateWalkingToNextSquare::doAction(Actor* actor) {
    // Implementation for SubStateWalkingToNextSquare state

    if (currentGame.elapsedTimeMS - actor->_timeLastUpdate > actor->_timeToCrossOneTile && actor->_timeLastUpdate != 0 && _moved) {

        currentGame.occupiedByActorList[actor->_actorCords.x][actor->_actorCords.y].erase(
            std::remove(
                currentGame.occupiedByActorList[actor->_actorCords.x][actor->_actorCords.y].begin(), 
                currentGame.occupiedByActorList[actor->_actorCords.x][actor->_actorCords.y].end(), actor->_actorId
            ), 
            currentGame.occupiedByActorList[actor->_actorCords.x][actor->_actorCords.y].end()
        );
        actor->_actorCords = actor->_actorGoal;
        return !walkToNextRoutePoint(actor);
    }
    if (actor->_timeLastUpdate == 0 || !_moved) {
        return !walkToNextRoutePoint(actor);
    }
    return false;
}

bool SubStateWalkingToNextSquare::getMoved()
{
    return _moved;
}

bool SubStateWalkingToNextSquare::walkToNextRoutePoint(Actor* actor)
{
    if (actor->_route.route.empty()) {
        return false;
    }

    if (actorHelper::actorOrientation(actor->_actorCords.x, actor->_actorCords.y, actor->_route.route.back().position.x, actor->_route.route.back().position.y) != actor->_orientation) {
        actor->_orientation = actorHelper::newOrientation(actor->_orientation, actorHelper::actorOrientation(actor->_actorCords.x, actor->_actorCords.y, actor->_route.route.back().position.x, actor->_route.route.back().position.y));
        return true;
    }

    if ((actor->_baseState->_base == BaseStateNames::Gathering || (actor->_baseState->_base == BaseStateNames::Fighting && actor->_baseState->getModeOfAttack() == ModesOfAttack::melee)) && actor->_route.route.size() == 1 ){
        //if (!actor->_isWalkingToUnloadingPoint) {
        actor->_route.route.clear();
        return false;
    }

    if ((actor->_baseState->_base == BaseStateNames::Fighting && actor->_baseState->getModeOfAttack() == ModesOfAttack::melee) && distEuclidean(actor->_actorCords.x, actor->_actorCords.y, actor->_actorGoal.x, actor->_actorGoal.y) <= actor->_range)
    {
        actor->_route.route.clear();
        return false;
    }

    if (    (   actor->_baseState->_base == BaseStateNames::Gathering || 
                (
                    actor->_baseState->_base == BaseStateNames::Fighting &&
                    actor->_baseState->getModeOfAttack() == ModesOfAttack::melee
                )
            ) &&
            actor->_route.route.size() == 2 &&
            !currentGame.occupiedByActorList[actor->_route.route.back().position.x][actor->_route.route.back().position.y].empty() &&
            !retryWalkingOrChangeGoal(actor)
        )
    {
        actor->switchBaseState(BaseStateNames::Idle);
        return true;       
    }

    if (actor->_route.route.size() == 1 && !currentGame.occupiedByActorList[actor->_route.route.back().position.x][actor->_route.route.back().position.y].empty() && !retryWalkingOrChangeGoal(actor)) {
            actor->switchBaseState(BaseStateNames::Idle);
            return true;      
    }
    _retries = 0;
    actor->_listOfTargetsToRejectUntilSuccesfullMovement.clear();
    startWalking(actor);
    return true;
}

void SubStateWalkingToNextSquare::startWalking(Actor* actor) {
    _moved = true;
    actor->_timeLastUpdate = currentGame.elapsedTimeMS;
    actor->_actorGoal = actor->_route.route.back().position;
    currentGame.occupiedByActorList[actor->_route.route.back().position.x][actor->_route.route.back().position.y].push_back(actor->_actorId);
    actor->_route.route.pop_back();
}

bool SubStateWalkingToNextSquare::retryWalkingOrChangeGoal(Actor* actor) {
    if (_timeLastRetry + 500 < currentGame.getTime()) {
        _timeLastRetry = currentGame.getTime();
        if (_retries < 30)
        {
            actor->calculateRoute();
            _retries += 1;
            return true;
        }
        if (actor->_groundState->_ground == GroundStateNames::ReturningTheResource || actor->_baseState->_base == BaseStateNames::Gathering || actor->_baseState->_base == BaseStateNames::Building)
        {
            //find alternative
            return true;
        }
        return false;
    }
    return true;
}


