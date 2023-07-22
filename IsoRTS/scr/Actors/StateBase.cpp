#include "StateBase.h"
#include <iostream>

 bool StateBase::doAction(Actor* actor) {
    std::cout << "Base state!\n";
    return false;
}
