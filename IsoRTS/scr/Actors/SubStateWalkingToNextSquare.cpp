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
        std::cout << "Actor " << actor->_actorId << ": Route is empty! \n";
        return false;
    }

    if (actorHelper::actorOrientation(actor->_actorCords.x, actor->_actorCords.y, actor->_route.route.back().position.x, actor->_route.route.back().position.y) != actor->_orientation) {
        actor->_orientation = actorHelper::newOrientation(actor->_orientation, actorHelper::actorOrientation(actor->_actorCords.x, actor->_actorCords.y, actor->_route.route.back().position.x, actor->_route.route.back().position.y));
        return true;
    }

    if ((actor->_baseState->_base == BaseStateNames::Gathering || (actor->_baseState->_base == BaseStateNames::Fighting && actor->_baseState->getModeOfAttack() == ModesOfAttack::melee && !(actor->_groundState->_ground == GroundStateNames::Fleeing))) && actor->_route.route.size() == 1 ) {
            actor->_route.route.clear();
            std::cout << "Actor " << actor->_actorId << ": Route cleared! Reason at melee range! \n";
            return false;
    }

    if ((actor->_baseState->_base == BaseStateNames::Fighting && actor->_baseState->getModeOfAttack() == ModesOfAttack::ranged) && distEuclidean(actor->_actorCords.x, actor->_actorCords.y, actor->_actorGoal.x, actor->_actorGoal.y) <= actor->_range)
    {
        actor->_route.route.clear();
        std::cout << "Actor " << actor->_actorId << ": Route cleared! Reason at ranged range\n";
        return false;
    }

    if (  ( actor->_baseState->_base == BaseStateNames::Gathering || ( actor->_baseState->_base == BaseStateNames::Fighting && actor->_baseState->getModeOfAttack() == ModesOfAttack::melee ) ) &&
            actor->_route.route.size() == 2 &&
            !currentGame.occupiedByActorList[actor->_route.route[1].position.x][actor->_route.route[1].position.y].empty()
        )
    {
        std::cout << "Actor " << actor->_actorId << ": Last tile in walking te resource blocked! \n";
        _lastCellBlocked = true;
        if (!retryWalkingOrChangeGoal(actor)) {
            actor->_route.route.clear();
            actor->switchBaseState(BaseStateNames::Idle);
        }
        return true;
    }

    if (actor->_route.route.size() == 1 && !currentGame.occupiedByActorList[actor->_route.route.back().position.x][actor->_route.route.back().position.y].empty()) {
        _lastCellBlocked = true;
        if (!retryWalkingOrChangeGoal(actor)) {
            actor->_route.route.clear();
            actor->switchBaseState(BaseStateNames::Idle);
        }
        return true;
    }
    if (!currentGame.occupiedByActorList[actor->_route.route.back().position.x][actor->_route.route.back().position.y].empty() && listOfActors[currentGame.occupiedByActorList[actor->_route.route.back().position.x][actor->_route.route.back().position.y][0]].getTeam() != actor->_actorTeam) {
        if (!retryWalkingOrChangeGoal(actor)) {
            actor->_route.route.clear();
            actor->switchBaseState(BaseStateNames::Idle);
        }
        return true;
    }

    if (actor->_baseState->_base == BaseStateNames::Fighting && checkIfEnemyHasMoved(actor)) {
        //Intercept enemy
        actor->switchSubState(SubStateNames::SearchingAPath);
        actor->_route.route.clear();
        return true;
    }

    std::cout << "Actor " << actor->_actorId << ": moving one tile! \n";
    _retries = 0;
    actor->_listOfTargetsToRejectUntilSuccesfullMovement.clear();
    _moved = true;
    actor->_timeLastUpdate = currentGame.elapsedTimeMS;
    actor->_actorGoal = actor->_route.route.back().position;
    currentGame.occupiedByActorList[actor->_route.route.back().position.x][actor->_route.route.back().position.y].push_back(actor->_actorId);
    actor->_route.route.pop_back();
    return true;
}

bool SubStateWalkingToNextSquare::checkIfEnemyHasMoved(Actor* actor) {

    //Going to enemy targets avoids pathfinding for every square the enemy moves, but might feel a bit cheesy in the game

    int targetId = actor->_baseState->getActionPreformedOn().x;
    targetTypes targetType = static_cast<targetTypes>(actor->_baseState->getActionPreformedOn().y);

    if (targetType == targetTypes::actor && listOfActors[targetId].getHasRoute()) {
        if (actor->_actorRealGoal.x != listOfActors[targetId].getGoal().x || actor->_actorRealGoal.y != listOfActors[targetId].getGoal().y) {
            if (distEuclidean(actor->_actorCords.x, listOfActors[targetId].getActorCords().x, actor->_actorCords.y, listOfActors[targetId].getActorCords().y) < 15) {
                actor->_actorGoal = listOfActors[targetId].getGoal();
                actor->_actorRealGoal = listOfActors[targetId].getGoal();
                return true;
            }
            //Just continue walking the path totally oblivious to the movement
        }
    }
    return false;
}

bool SubStateWalkingToNextSquare::retryWalkingOrChangeGoal(Actor* actor) {
    if (_timeLastRetry + 500 < currentGame.getTime()) {
        _timeLastRetry = currentGame.getTime();
        if (_retries < 0)
        {
            std::cout << "Actor " << actor->_actorId << ": retry route! \n";
            if (_lastCellBlocked) {
                actor->_cantPassActors = true;
            }
            pathedRoute backupRoute = actor->_route;
            actor->calculateRoute();
            if (!actor->_route.realPath) {
                std::cout << "Actor " << actor->_actorId << ": I can't walk around the obstacle! \n";
                actor->_route = backupRoute;
            }
            _retries += 1;
            actor->_cantPassActors = false;
            _lastCellBlocked = false;
            return true;
        }

        std::cout << "Actor " << actor->_actorId << ": retries failed switching state! \n";

        if (actor->_groundState->_ground == GroundStateNames::ReturningTheResource)
        {
            actor->switchSubState(SubStateNames::SearchingDropOffPoint);
            return true;
        }
        if (actor->_baseState->_base == BaseStateNames::Gathering) {
            actor->switchGroundState(GroundStateNames::FindAlternativeSource);
            return true;
        }
        if (actor->_baseState->_base == BaseStateNames::Building) {
            actor->switchGroundState(GroundStateNames::SearchAlternativeBuildingSpot);
            return true;
        }
        return false;
    }
    return true;
}


