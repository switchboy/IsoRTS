
#pragma once

class StateBase;
enum class BaseStateNames;
enum class GroundStateNames;
enum class SubStateNames;

class Actor {
public:
    Actor(BaseStateNames requestState);
    void doAction();
    ~Actor();
    void switchBaseState(BaseStateNames desiredState);
    void switchGroundState(GroundStateNames desiredState);
    void switchSubState(SubStateNames desiredState);

private:
    StateBase* _baseState;
    StateBase* _groundState;
    StateBase* _subState;
};