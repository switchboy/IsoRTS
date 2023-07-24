
#pragma once
#include <SFML/System.hpp>
#include "actorStructs.h"
#include "../globalfunctions.h"
#include "../actorTemplates.h"

class StateBase;
enum class BaseStateNames;
enum class GroundStateNames;
enum class SubStateNames;

class Actor {
public:
    Actor(int type, cords location, int actorTeam, int actorId);
    ~Actor();
    // Copy constructor
    Actor(const Actor& other);
    // Copy assignment operator
    Actor& operator=(const Actor& other);

    //State machine
    void switchBaseState(BaseStateNames desiredState);
    void switchGroundState(GroundStateNames desiredState);
    void switchSubState(SubStateNames desiredState);

    //Interact
    void takeDamage(int amountOfDamage, int idOfAttacker);
    void stackOrder(cords Goal, stackOrderTypes orderType);
    void clearCommandStack();

    //Updateing
    void update();
    void calculateRoute(); //marked for deletion

    //Drawing & animation
    void drawActor();
    void renderPath();
    void printActorDebugText();

    //Getters
    int getActorId() const;
    bool getIsAlive() const;
    bool getIsGathering() const;
    bool getIsIdle() const;
    bool getIsBuilding() const;
    resourceTypes getResourceGathered() const;

    void pathAStar();

    bool getHasRoute();
    std::pair<int, int> getHealth() const;
    cords getGoal() const;
    cords getActorCords() const;
    int getTeam() const;
    int getMeleeDMG() const;
    int getRangedDMG() const;
    sf::IntRect getLastIntRect() const;
    const std::list<cords>& getRejectedTargetsList() const;
    std::string getNameOfActor() const;
    int getType() const;
    std::string getResources() const;
    int getBuildingId() const;
   

private:
    bool doNextStackedCommand();
    StateBase* _baseState;
    StateBase* _groundState;
    StateBase* _subState;
    int _actorType, _actorHealth, _hitPoints, _actorId, _actorTeam, _meleeDamage, _rangedDamage, _amountOfWood, _amountOfFood, _amountOfStone,
        _amountOfGold, _buildingId, _range, _timeBetweenShots, _splashDamage, _projectileType, _doesRangedDamage, _rateOfFire, _timeToCrossOneTile,
        _spriteYOffset, _timeLastOffsetChange, _orientation, _offSetX, _offSetY, _timeLastUpdate;
    bool _cantPassActors;
    cords _actorGoal, _actorCords ;
    sf::IntRect _lastIntRect;
    std::list<cords> _listOfTargetsToRejectUntilSuccesfullMovement;
    pathedRoute _route;
    std::list<orderStack> _listOfOrders;
    resourceTypes _resourceBeingGatherd;

    //Actor modifiers
    void updateGoal(cords goal, int waitTime);

    friend class StateBase;
    //Base states
    friend class BaseStateIdle;
    friend class BaseStateWalkingAToB;
    friend class BaseStateBuilding;
    friend class BaseStateGathering;
    friend class BaseStateFighting;
    friend class BaseStateDead;

    //Ground states
    friend class GroundStateWalking;
    friend class GroundStateGatheringTheResource;
    friend class GroundStateReturningTheResource;
    friend class GroundStateBuildingTheBuilding;
    friend class GroundStateAttacking;
    friend class GroundStateFleeing;
    friend class GroundStateDecomposing;

    //Sub states
    friend class SubStateWalkingToNextSquare;
    friend class SubStateSearchingAPath;
    friend class SubStateSearchingNextSimilairResource;
    friend class SubStateSearchingDropOffPoint;
    friend class SubStateSearchingAlternativeBuildingSpot;
    friend class SubStateSearchingAlternativeBuildingToBuilt;
    friend class SubStateMeleeAttacking;
    friend class SubStateRangedAttacking;
    friend class SubStateSearchNextTarget;
    friend class SubStateSettingGoalToFleeTo;
    friend class SubStateSettingMyDecompositionState;
    friend class SubStateCountingDownToDestroySelf;
};

extern std::vector <Actor> listOfActors;
extern std::vector<int>    listOfActorsWhoNeedAPath;
extern nearestBuildingTile findNearestBuildingTile(int buildingId, int actorId);