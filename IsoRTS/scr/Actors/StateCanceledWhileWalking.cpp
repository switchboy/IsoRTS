#include "StateCanceledWhileWalking.h"
#include "actor.h"
#include "../gamestate.h"
#include <iostream>
#include "ActorHelper.h"
#include "../globalfunctions.h"


bool StateCanceledWhileWalking::doAction(Actor* actor) {
    if (!_moved && !_walkBackSet) {
        
        int deltaTime = currentGame.elapsedTimeMS - actor->_timeLastUpdate;
        if ( 
            (deltaTime * 2) < actor->_timeToCrossOneTile && 
            dist(actor->_actorCords.x, actor->_actorCords.y, actor->_listOfOrders.front().goal.x, actor->_listOfOrders.front().goal.y) > dist(actor->_actorGoal.x, actor->_actorGoal.y, actor->_listOfOrders.front().goal.x, actor->_listOfOrders.front().goal.y)
            ) {
            cords tempCords = actor->_actorCords;
            actor->_actorCords = actor->_actorGoal;
            actor->_actorGoal = tempCords;
            actor->_timeLastUpdate += actor->_timeToCrossOneTile - deltaTime;
            actor->_orientation = actorHelper::actorOrientation(actor->_actorCords.x, actor->_actorCords.y, actor->_actorGoal.x, actor->_actorGoal.y);
        }
        
        actor->_route.route.clear();
        _walkBackSet = true;
        _moved = true;
        return false;
    }

    if (currentGame.elapsedTimeMS - actor->_timeLastUpdate > actor->_timeToCrossOneTile && actor->_timeLastUpdate != 0 && _moved) {
        currentGame.occupiedByActorList[actor->_actorCords.x][actor->_actorCords.y].erase(
            std::remove(
                currentGame.occupiedByActorList[actor->_actorCords.x][actor->_actorCords.y].begin(),
                currentGame.occupiedByActorList[actor->_actorCords.x][actor->_actorCords.y].end(), actor->_actorId
            ),
            currentGame.occupiedByActorList[actor->_actorCords.x][actor->_actorCords.y].end()
        );
        actor->_actorCords = actor->_actorGoal;
        actor->makeSureActorIsOnTheMap();
        return true;
    }
    if (actor->_timeLastUpdate == 0 || !_moved) {
        return true;
    }
    return false;
}

bool StateCanceledWhileWalking::getMoved()
{
	return _moved;
}
