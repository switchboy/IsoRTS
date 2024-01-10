#include "BaseStateIdle.h"
#include "actor.h"

#include <iostream>

bool BaseStateIdle::doAction(Actor* actor) {
	if (actor->doNextStackedCommand()) {
		return false;
   }
	return false;
}