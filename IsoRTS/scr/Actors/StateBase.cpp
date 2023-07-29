#include "StateBase.h"
#include <iostream>

 bool StateBase::doAction(Actor* actor) {
    return true;
}

 ModesOfAttack StateBase::getModeOfAttack()
 {
     return ModesOfAttack();
 }

 bool StateBase::getMoved()
 {
     return false;
 }
