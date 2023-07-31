#include "SubStateSearchingNextSimilairResource.h"
#include "actor.h"
#include <iostream>

bool SubStateSearchingNextSimilairResource::doAction(Actor* actor) {
    cords newResourceCords = findResource(actor->_resourceBeingGatherd, actor->_actorId);
    if (newResourceCords.x == -1) {
        _tries += 1;
        if (_tries > 10) {
            actor->switchBaseState(BaseStateNames::Idle);
            return false;
        }
        return false;
    }
    actor->_actorGoal = newResourceCords;
    return true;
}