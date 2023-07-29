
#include <iostream>
#include "Actor.h"
#include "StateBase.h"
#include "../gamestate.h"
#include "../player.h"
#include "../cells.h"
#include "../gametext.h"

//Base states
#include "BaseStateIdle.h"
#include "BaseStateWalkingAToB.h"
#include "BaseStateBuilding.h"
#include "BaseStateGathering.h"
#include "BaseStateFighting.h"
#include "BaseStateDead.h"

//Ground states
#include "GroundStateWalking.h"
#include "GroundStateGatheringTheResource.h"
#include "GroundStateReturningTheResource.h"
#include "GroundStateBuildingTheBuilding.h"
#include "GroundStateAttacking.h"
#include "GroundStateFleeing.h"
#include "GroundStateDecomposing.h"

//Sub states
#include "SubStateWalkingToNextSquare.h"
#include "SubStateSearchingAPath.h"
#include "SubStateSearchingNextSimilairResource.h"
#include "SubStateSearchingDropOffPoint.h"
#include "SubStateSearchingAlternativeBuildingSpot.h"
#include "SubStateSearchingAlternativeBuildingToBuilt.h"
#include "SubStateMeleeAttacking.h"
#include "SubStateRangedAttacking.h"
#include "SubStateSearchNextTarget.h"
#include "SubStateSettingGoalToFleeTo.h"
#include "SubStateSettingMyDecompositionState.h"
#include "SubStateCountingDownToDestroySelf.h"


#include "StateNames.h"

std::vector<int> listOfActorsWhoNeedAPath;
std::vector <Actor> listOfActors;

Actor::Actor(int type, cords location, int actorTeam, int actorId)
{
    currentGame.occupiedByActorList[location.x][location.y].push_back(actorId);
    listOfPlayers[actorTeam].addToCurrentPopulation(1);

    _baseState = new BaseStateIdle(BaseStateNames::Idle, GroundStateNames::NONE, SubStateNames::NONE);
    _groundState = new StateBase(BaseStateNames::NONE, GroundStateNames::NONE, SubStateNames::NONE);
    _subState = new StateBase(BaseStateNames::NONE, GroundStateNames::NONE, SubStateNames::NONE);
   
    _actorType = type;
    _actorCords = location;
    _actorTeam = actorTeam;
    _actorId = actorId;

    _resourceBeingGatherd = resourceTypes::None;
    _amountOfFood = 0;
    _amountOfGold = 0;
    _amountOfStone = 0;
    _amountOfWood = 0;
    _actorGoal = { 0,0 };
    _actorRealGoal = { 0,0 };
    _buildingId = 0;

    _orientation = 0;
    _offSetX = 0;
    _offSetY = 0;
    _spriteYOffset = 0;
    _timeLastOffsetChange = currentGame.getTime();
    _timeLastUpdate = currentGame.getTime();

    _cantPassActors = false;
    _actorHealth = listOfActorTemplates[type].getHitPoints();
    _hitPoints = listOfActorTemplates[type].getHitPoints();
    _meleeDamage = listOfActorTemplates[type].getMeleeDamage();
    _range = listOfActorTemplates[type].getRange();
    _rangedDamage = listOfActorTemplates[type].getRangedDamage();
    _timeBetweenShots = listOfActorTemplates[type].getTimeBetweenShots();
    _splashDamage = listOfActorTemplates[type].getSplashDamage();
    _projectileType = listOfActorTemplates[type].getProjectileType();
    _doesRangedDamage = listOfActorTemplates[type].getDoesRangedDamage();
    _rateOfFire = listOfActorTemplates[type].getRateOfFire();
    _timeToCrossOneTile = listOfActorTemplates[type].getTimeToCrossOneTile();


}

Actor::~Actor() {
    // Check if _baseState is not nullptr before deleting it
    if (_baseState) {
        delete _baseState;
        _baseState = nullptr; // Set it to nullptr after deletion
    }

    // Check if _groundState is not nullptr before deleting it
    if (_groundState) {
        delete _groundState;
        _groundState = nullptr; // Set it to nullptr after deletion
    }

    // Check if _subState is not nullptr before deleting it
    if (_subState) {
        delete _subState;
        _subState = nullptr; // Set it to nullptr after deletion
    }
}


// Copy constructor
Actor::Actor(const Actor& other) {
    // Implement the copy constructor code here...
    _baseState = new StateBase(*other._baseState);
    _groundState = new StateBase(*other._groundState);
    _subState = new StateBase(*other._subState);

    // Copy other member variables...
    _actorType = other._actorType;
    _actorHealth = other._actorHealth;
    _hitPoints = other._hitPoints;
    _actorId = other._actorId;
    _actorTeam = other._actorTeam;
    _meleeDamage = other._meleeDamage;
    _rangedDamage = other._rangedDamage;
    _amountOfWood = other._amountOfWood;
    _amountOfFood = other._amountOfFood;
    _amountOfStone = other._amountOfStone;
    _amountOfGold = other._amountOfGold;
    _buildingId = other._buildingId;
    _range = other._range;
    _timeBetweenShots = other._timeBetweenShots;
    _splashDamage = other._splashDamage;
    _projectileType = other._projectileType;
    _doesRangedDamage = other._doesRangedDamage;
    _rateOfFire = other._rateOfFire;
    _timeToCrossOneTile = other._timeToCrossOneTile;
    _actorGoal = other._actorGoal;
    _actorCords = other._actorCords;
    _lastIntRect = other._lastIntRect;
    _listOfTargetsToRejectUntilSuccesfullMovement = other._listOfTargetsToRejectUntilSuccesfullMovement;
    _resourceBeingGatherd = other._resourceBeingGatherd;
    _spriteYOffset = other._spriteYOffset;
    _timeLastOffsetChange = other._timeLastOffsetChange;
    _orientation = other._orientation;
    _offSetX = other._offSetX;
    _offSetY = other._offSetY;
    _timeLastUpdate = other._timeLastUpdate;
    _listOfOrders = other._listOfOrders;
    _cantPassActors = other._cantPassActors;
    _route = other._route;
    _actorRealGoal = other._actorRealGoal;
}

// Copy assignment operator
Actor& Actor::operator=(const Actor& other) {
    if (this == &other) {
        return *this; // Check for self-assignment
    }

    // Implement the copy assignment operator code here...

    // Clean up existing resources (avoid memory leak)
    delete _baseState;
    delete _groundState;
    delete _subState;

    // Perform member variable copy here
    _baseState = new StateBase(*other._baseState);
    _groundState = new StateBase(*other._groundState);
    _subState = new StateBase(*other._subState);

    // Copy other member variables...
    _actorType = other._actorType;
    _actorHealth = other._actorHealth;
    _hitPoints = other._hitPoints;
    _actorId = other._actorId;
    _actorTeam = other._actorTeam;
    _meleeDamage = other._meleeDamage;
    _rangedDamage = other._rangedDamage;
    _amountOfWood = other._amountOfWood;
    _amountOfFood = other._amountOfFood;
    _amountOfStone = other._amountOfStone;
    _amountOfGold = other._amountOfGold;
    _buildingId = other._buildingId;
    _range = other._range;
    _timeBetweenShots = other._timeBetweenShots;
    _splashDamage = other._splashDamage;
    _projectileType = other._projectileType;
    _doesRangedDamage = other._doesRangedDamage;
    _rateOfFire = other._rateOfFire;
    _timeToCrossOneTile = other._timeToCrossOneTile;
    _actorGoal = other._actorGoal;
    _actorCords = other._actorCords;
    _lastIntRect = other._lastIntRect;
    _listOfTargetsToRejectUntilSuccesfullMovement = other._listOfTargetsToRejectUntilSuccesfullMovement;
    _resourceBeingGatherd = other._resourceBeingGatherd;
    _spriteYOffset = other._spriteYOffset;
    _timeLastOffsetChange = other._timeLastOffsetChange;
    _orientation = other._orientation;
    _offSetX = other._offSetX;
    _offSetY = other._offSetY;
    _timeLastUpdate = other._timeLastUpdate;
    _listOfOrders = other._listOfOrders;
    _cantPassActors = other._cantPassActors;
    _route = other._route;
    _actorRealGoal = other._actorRealGoal;

    return *this;
}

void Actor::switchBaseState(BaseStateNames desiredState) {
    delete _baseState;
    switch (desiredState) {
    case BaseStateNames::NONE:
        _baseState = new StateBase(BaseStateNames::NONE, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    case BaseStateNames::Idle:
        _baseState =  new BaseStateIdle(BaseStateNames::Idle, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    case BaseStateNames::WalikngAToB:
        _baseState = new BaseStateWalkingAToB(BaseStateNames::WalikngAToB, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    case BaseStateNames::Building:
        _baseState = new BaseStateBuilding(BaseStateNames::Building, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    case BaseStateNames::Gathering:
        _baseState = new BaseStateGathering(BaseStateNames::Gathering, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    case BaseStateNames::Fighting:
        _baseState = new BaseStateFighting(BaseStateNames::Fighting, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    case BaseStateNames::Dead:
        _baseState = new BaseStateDead(BaseStateNames::Dead, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    default:
        _baseState = new StateBase(BaseStateNames::NONE, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    }
    switchGroundState(GroundStateNames::NONE);
}

void Actor::switchGroundState(GroundStateNames desiredState) {
    delete _groundState;

    switch (desiredState) {
    case GroundStateNames::Walking:
        _groundState = new GroundStateWalking(BaseStateNames::Idle, GroundStateNames::Walking, SubStateNames::NONE);
        break;
    case GroundStateNames::AtTheResource:
        _groundState = new GroundStateGatheringTheResource(BaseStateNames::Idle, GroundStateNames::Walking, SubStateNames::NONE);
        break;
    case GroundStateNames::ReturningTheResource:
        _groundState = new GroundStateReturningTheResource(BaseStateNames::Idle, GroundStateNames::Walking, SubStateNames::NONE);
        break;
    case GroundStateNames::AtTheBuilding:
        _groundState = new GroundStateBuildingTheBuilding(BaseStateNames::Idle, GroundStateNames::Walking, SubStateNames::NONE);
        break;
    case GroundStateNames::Attacking:
        _groundState = new GroundStateAttacking(BaseStateNames::Idle, GroundStateNames::Walking, SubStateNames::NONE);
        break;
    case GroundStateNames::Fleeing:
        _groundState = new GroundStateFleeing(BaseStateNames::Idle, GroundStateNames::Walking, SubStateNames::NONE);
        break;
    case GroundStateNames::Decomposing:
        _groundState = new GroundStateDecomposing(BaseStateNames::Idle, GroundStateNames::Walking, SubStateNames::NONE);
        break;
    default:
        _groundState = new StateBase(BaseStateNames::NONE, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    }
    switchSubState(SubStateNames::NONE);
}

void Actor::switchSubState(SubStateNames desiredState) {
    delete _subState;
    switch (desiredState) {
    case SubStateNames::WalkingToNextSquare:
        _subState = new SubStateWalkingToNextSquare(BaseStateNames::NONE, GroundStateNames::NONE, SubStateNames::WalkingToNextSquare);
        break;
    case SubStateNames::SearchingAPath:
        _subState = new SubStateSearchingAPath(BaseStateNames::NONE, GroundStateNames::NONE, SubStateNames::SearchingAPath);
        break;
    case SubStateNames::SearchingNextSimilairResource:
        _subState = new SubStateSearchingNextSimilairResource(BaseStateNames::NONE, GroundStateNames::NONE, SubStateNames::SearchingNextSimilairResource);
        break;
    case SubStateNames::SearchingDropOffPoint:
        _subState = new SubStateSearchingDropOffPoint(BaseStateNames::NONE, GroundStateNames::NONE, SubStateNames::SearchingDropOffPoint);
        break;
    case SubStateNames::SearchingAlternativeBuildingSpot:
        _subState = new SubStateSearchingAlternativeBuildingSpot(BaseStateNames::NONE, GroundStateNames::NONE, SubStateNames::SearchingAlternativeBuildingSpot);
        break;
    case SubStateNames::SearchingAlternativeBuildingToBuilt:
        _subState = new SubStateSearchingAlternativeBuildingToBuilt(BaseStateNames::NONE, GroundStateNames::NONE, SubStateNames::SearchingAlternativeBuildingToBuilt);
        break;
    case SubStateNames::MeleeAttacking:
        _subState = new SubStateMeleeAttacking(BaseStateNames::NONE, GroundStateNames::NONE, SubStateNames::MeleeAttacking);
        break;
    case SubStateNames::RangedAttacking:
        _subState = new SubStateRangedAttacking(BaseStateNames::NONE, GroundStateNames::NONE, SubStateNames::RangedAttacking);
        break;
    case SubStateNames::SearchNextTarget:
        _subState = new SubStateSearchNextTarget(BaseStateNames::NONE, GroundStateNames::NONE, SubStateNames::SearchNextTarget);
        break;
    case SubStateNames::SettingGoalToFleeTo:
        _subState = new SubStateSettingGoalToFleeTo(BaseStateNames::NONE, GroundStateNames::NONE, SubStateNames::SettingGoalToFleeTo);
        break;
    case SubStateNames::SettingMyDecompositionState:
        _subState = new SubStateSettingMyDecompositionState(BaseStateNames::NONE, GroundStateNames::NONE, SubStateNames::SettingMyDecompositionState);
        break;
    case SubStateNames::CountingDownToDestroySelf:
        _subState = new SubStateCountingDownToDestroySelf(BaseStateNames::NONE, GroundStateNames::NONE, SubStateNames::CountingDownToDestroySelf);
        break;
    default:
        _subState = new StateBase(BaseStateNames::NONE, GroundStateNames::NONE, SubStateNames::NONE);
        break;
    }
}

void Actor::takeDamage(int amountOfDamage, int idOfAttacker)
{
}

void Actor::stackOrder(cords Goal, stackOrderTypes orderType)
{
    _listOfOrders.push_back({ Goal, orderType });
}

void Actor::updateGoal(cords goal, int waitTime)
{

}


void Actor::clearCommandStack()
{
    _listOfOrders.clear();
    
}

void Actor::update()
{
    if (_subState == nullptr || _subState->doAction(this)) {
        if (_groundState == nullptr || _groundState->doAction(this)) {
            if (_baseState->doAction(this)) {
                switchBaseState(BaseStateNames::Idle);
            }
        }
    }
}

void Actor::calculateRoute()
{
    _route.route.clear();
    _route.pathFound = false;
    _route.realPath = false;
    _actorRealGoal = _actorGoal;
    pathAStar();
    if (_route.realPath && _route.pathFound) {
        _route.timeLastPathTry = currentGame.elapsedTimeMS;
    }
}

void Actor::drawActor()
{
    int i = _actorCords.x;
    int j = _actorCords.y;
    int x = _actorGoal.x;
    int y = _actorGoal.y;
    cords position = worldSpace({ i,j });
    int spriteXoffset = 0;
    int spriteYoffset = 0;

    /*To Do:
    if (_subState->_sub == SubStateNames::){
        this->animateWalkingToResource();
    }
    if(_subState->_sub == SubStateNames::){
        this->walkBackToOwnSquare();
    }
    */

    if (_subState->_sub == SubStateNames::WalkingToNextSquare || _subState->_sub == SubStateNames::WalkingToAction)// || _timeStartedWalkingToRecource > 0)
    {
        if (currentGame.elapsedTimeMS - _timeLastOffsetChange > 200)
        {
            _spriteYOffset = _spriteYOffset + 32;
            if (_spriteYOffset > 128)
            {
                _spriteYOffset = 32;
            }
            _timeLastOffsetChange = currentGame.elapsedTimeMS;
        }
        if (_subState->_sub == SubStateNames::WalkingToNextSquare && _subState->getMoved())
        {
            cords nPosition = worldSpace({ x, y });
            int deltaX = position.x - nPosition.x;
            int deltaY = position.y - nPosition.y;
            float deltaTime = static_cast<float>(currentGame.elapsedTimeMS) - static_cast<float>(_timeLastUpdate);
            if (deltaTime > _timeToCrossOneTile) {
                deltaTime = _timeToCrossOneTile;
            }
            float speedMultiplier = 1.f / _timeToCrossOneTile;
            float deltaXCompleted = deltaX * (deltaTime * speedMultiplier);
            float deltaYCompleted = deltaY * (deltaTime * speedMultiplier);
            
            position = { position.x - static_cast<int>(deltaXCompleted),  position.y - static_cast<int>(deltaYCompleted) };
        }
    }
    else if (_subState->_sub == SubStateNames::GatheringTheResource || _subState->_sub == SubStateNames::BuildingTheBuilding)
    {
        if (currentGame.elapsedTimeMS - _timeLastOffsetChange > 200)
        {
            _spriteYOffset = _spriteYOffset + 32;
            if (_spriteYOffset > 128)
            {
                _spriteYOffset = 0;
            }
            _timeLastOffsetChange = currentGame.elapsedTimeMS;
        }
    }
    else
    {
        _spriteYOffset = 0;
    }
    switch (_spriteYOffset)
    {
    case 0:
        spriteYoffset = 0;
        break;
    case 32:
        spriteYoffset = 32;
        break;
    case 64:
        spriteYoffset = 64;
        break;
    case 96:
        spriteYoffset = 96;
        break;
    case 128:
        spriteYoffset = 128;
        break;
    }
    int spriteOffset = 0;
    if (_subState->_sub == SubStateNames::GatheringTheResource)
    {
        if (_resourceBeingGatherd == resourceTypes::resourceWood)
        {
            spriteOffset = 128;
        }
        else if (_resourceBeingGatherd == resourceTypes::resourceFood || _resourceBeingGatherd == resourceTypes::resourceStone)
        {
            spriteOffset = 256;
        }
        else if (_resourceBeingGatherd == resourceTypes::resourceGold)
        {
            spriteOffset = 384;
        }
    }
    switch (_orientation)
    {
    case 0:
        spriteXoffset = 80 + spriteOffset;
        break;
    case 1:
        spriteXoffset = 0 + spriteOffset;
        break;
    case 2:
        spriteXoffset = 96 + spriteOffset;
        break;
    case 3:
        spriteXoffset = 48 + spriteOffset;
        break;
    case 4:
        spriteXoffset = 64 + spriteOffset;
        break;
    case 5:
        spriteXoffset = 32 + spriteOffset;
        break;
    case 6:
        spriteXoffset = 112 + spriteOffset;
        break;
    case 7:
        spriteXoffset = 16 + spriteOffset;
        break;
    default:
        spriteXoffset = 0 + spriteOffset;
        break;
    }
    position = { position.x + _offSetX, position.y + _offSetY };

    /* To do:
    if (this->isWalkingBackXYDrawOverride.isActive) {
        anitmateWalkingBackAfterAbortedCommand();
        position = this->isWalkingBackXYDrawOverride.newXY;
    }
    */

    bool drawHealth = false;

    //determine if actor is the only actor on the tile
    if (currentGame.occupiedByActorList[_actorCords.x][_actorCords.y].size() > 1) {
        //Tile is occupied by more then one actor!
        int id = 0;
        for (auto& actorId : currentGame.occupiedByActorList[_actorCords.x][_actorCords.y]) {
            if (actorId == _actorId) {
                break;
            }
            id++;
        }
        //If the actor was occuping the cell before the other one let it step asside to let the other pass
        if (id == 0) {
            position.x = position.x - 7;
        }
    }

    if (currentGame.isInSelectedActors(_actorId))
    {
        currentGame.spriteUnitSelectedTile.setPosition(static_cast<float>(position.x), static_cast<float>(position.y));
        window.draw(currentGame.spriteUnitSelectedTile);
        drawHealth = true;
        if (!_listOfOrders.empty() && _actorTeam == listOfPlayers[currentPlayerI].getTeam()) {
            for (const orderStack order : _listOfOrders) {
                currentGame.listOfFlagsToDraw.push_back(order.goal);
            }
        }
    }
    else if (_actorHealth < _hitPoints) {
        drawHealth = true;
    }


    _actorDeclaringString = "Actor " + std::to_string(_actorId) + "\n" + "Postion x : " + std::to_string(_actorCords.x) + " y : " + std::to_string(_actorCords.y) + "\n" + "Draw pos x : " + std::to_string(position.x) + " y : " + std::to_string(position.y) + "\n" + "Sprite x : " + std::to_string(spriteXoffset) + " y : " + std::to_string(spriteYoffset) + "\n\n";
   


    listOfActorTemplates[_actorType].setSpritePosition(position);
    listOfActorTemplates[_actorType].getActorSprite().setTextureRect(sf::IntRect(spriteXoffset, spriteYoffset, 16, 32));
    window.draw(listOfActorTemplates[_actorType].getActorSprite());
    _lastIntRect = static_cast<sf::IntRect>(listOfActorTemplates[_actorType].getActorSprite().getGlobalBounds());


    if (drawHealth) {
        currentGame.healthBarBackground.setFillColor(sf::Color(255, 0, 0));
        currentGame.healthBarBackground.setSize(sf::Vector2f(32, 3));
        currentGame.healthBarGreenBar.setFillColor(sf::Color(0, 255, 0));
        currentGame.healthBarGreenBar.setSize(sf::Vector2f((static_cast<float>(_actorHealth) / static_cast<float>(_hitPoints)) * 32, 3));
        currentGame.healthBarBackground.setPosition(static_cast<float>(position.x + 16), static_cast<float>(position.y - 28));
        currentGame.healthBarGreenBar.setPosition(static_cast<float>(position.x + 16), static_cast<float>(position.y - 28));
        window.draw(currentGame.healthBarBackground);
        window.draw(currentGame.healthBarGreenBar);

    }
}

void Actor::renderPath()
{
    std::list<routeCell>::iterator it;
    for (it = _route.route.begin(); it != _route.route.end(); it++)
    {
        currentGame.spriteSelectedTileForPath.setPosition(static_cast<float>(worldSpace(it->position).x), static_cast<float>(worldSpace(it->position).y));
        window.draw(currentGame.spriteSelectedTileForPath);
    }
}

void Actor::printActorDebugText()
{
    gameText.addDebugMessage(_actorDeclaringString, 0);
}

bool Actor::getIsAlive() const
{
    if (_baseState->_base != BaseStateNames::Dead) {
        return true;
    }
    return false;
}

bool Actor::getIsGathering() const
{
    if (_baseState->_base == BaseStateNames::Gathering) {
        return true;
    }
    return false;
}

bool Actor::getHasRoute()
{
    if (_subState->_sub != SubStateNames::SearchingAPath) {
        return true;
    }
    return false;
}

int Actor::getActorId() const
{
    return _actorId;
}

std::pair<int, int> Actor::getHealth() const
{
    return { _actorHealth, _hitPoints };
}

cords Actor::getGoal() const
{
    return _actorGoal;
}

cords Actor::getActorCords() const
{
    return _actorCords;
}

int Actor::getTeam() const
{
    return _actorTeam;
}

int Actor::getMeleeDMG() const
{
    return _meleeDamage;
}

int Actor::getRangedDMG() const
{
    return _rangedDamage;
}

sf::IntRect Actor::getLastIntRect() const
{
    return _lastIntRect;
}

const std::list<cords>& Actor::getRejectedTargetsList() const
{
    return _listOfTargetsToRejectUntilSuccesfullMovement;
}

std::string Actor::getNameOfActor() const
{
    switch (_actorType)
    {
    case 0:
        return  "Villager";
        break;
    case 1:
        return "Swordsman";
        break;
    default:
        return "NameOfActorNotSet";
        break;
    }
}

int Actor::getType() const
{
    return _actorType;
}

std::string Actor::getResources() const
{
    return  "Wood: "+ std::to_string(_amountOfWood) + " Food: " + std::to_string(_amountOfFood) + " Stone: " + std::to_string(_amountOfStone) + " Gold: " + std::to_string(_amountOfGold);

}

int Actor::getBuildingId() const
{
    return _buildingId;
}

bool Actor::doNextStackedCommand()
{
    if (_listOfOrders.empty()) {
        return false;
    }
    switch (_listOfOrders.front().orderType) {
    case stackOrderTypes::stackActionMove:
        switchBaseState(BaseStateNames::WalikngAToB);
        _actorGoal = _listOfOrders.front().goal;
        _actorRealGoal = _actorGoal;
        //_actorCommandGoal = location;
        break;
    /*
    case stackOrderTypes::stackActionGather:
        this->updateGoal(this->listOfOrders.front().goal, 0);
        this->setGatheringRecource(true);
        break;
    case stackOrderTypes::stackActionBuild:
        if (currentGame.occupiedByBuildingList[this->listOfOrders.front().goal.x][this->listOfOrders.front().goal.y] != -1) {
            if (!listOfBuildings[currentGame.occupiedByBuildingList[this->listOfOrders.front().goal.x][this->listOfOrders.front().goal.y]].getCompleted()
                && listOfBuildings[currentGame.occupiedByBuildingList[this->listOfOrders.front().goal.x][this->listOfOrders.front().goal.y]].getTeam() == this->actorTeam) {
                nearestBuildingTile tempTile = findNearestBuildingTile(currentGame.occupiedByBuildingList[this->listOfOrders.front().goal.x][this->listOfOrders.front().goal.y], this->actorId);
                this->updateGoal(tempTile.location, 0);
                listOfBuildings[currentGame.occupiedByBuildingList[this->listOfOrders.front().goal.x][this->listOfOrders.front().goal.y]].claimFreeBuiildingTile(tempTile.buildingId, this->actorId);
                this->setIsBuildingTrue(currentGame.occupiedByBuildingList[this->listOfOrders.front().goal.x][this->listOfOrders.front().goal.y], this->listOfOrders.front().goal);
            }
        }
        break;
    case stackOrderTypes::stackActionAttack:
        this->updateGoal(this->listOfOrders.front().goal, 0);
        this->setIsDoingAttack(false);
        break;
    */
    }
    _listOfOrders.pop_front();
    return true;
}

bool Actor::getIsIdle() const
{
    if (_baseState->_base == BaseStateNames::Idle) {
        return true;
    }
    return false;
}

bool Actor::getIsBuilding() const
{
    if (_baseState->_base == BaseStateNames::Building) {
        return true;
    }
    return false;
}

resourceTypes Actor::getResourceGathered() const
{
    return _resourceBeingGatherd;
}



void Actor::pathAStar()
{
    std::vector<Cells> cellsList = baseCellList;
    int startCell = (_actorCords.x * MAP_HEIGHT) + _actorCords.y; //eigen positie
    int endCell = (_actorGoal.x * MAP_HEIGHT) + _actorGoal.y; //doel positie
    updateCells(endCell, startCell, cellsList, _cantPassActors);
    std::vector<Cells*> listToCheck;
    std::vector<Cells*> checkedList;
    bool endReached = false;

    //check of de doelcel niet 1 hokje weg is 
    if (((_actorCords.x - _actorGoal.x == 0) || (_actorCords.x - _actorGoal.x == -1) || (_actorCords.x - _actorGoal.x == 1)) && ((_actorCords.y - _actorGoal.y == 0) || (_actorCords.y - _actorGoal.y == -1) || (_actorCords.y - _actorGoal.y == 1)))
    {
        //en geen obstakel is of het hokje van de actor zelf is
        if (!cellsList[endCell].obstacle || (_actorCords.x == _actorGoal.x && _actorCords.y == _actorGoal.y))
        {
            endReached = true;
        }
        else
        {
            _route.pathFound = false;
        }
    }
    else
    {
       //De startcell initialiseren
        _route.pathFound = false;
        cellsList[startCell].addNeighbours(cellsList);
        cellsList[startCell].totalCostGuess = dist(cellsList[startCell].position.x, cellsList[startCell].position.y, cellsList[endCell].position.x, cellsList[endCell].position.y);
        cellsList[startCell].visited = true;
        cellsList[startCell].parentCellId = -1;
        cellsList[startCell].costToGoal = dist(cellsList[startCell].position.x, cellsList[startCell].position.y, cellsList[endCell].position.x, cellsList[endCell].position.y);
        //De startcel in de lijst van te checken cellen zetten om te kunnen beginnen
        listToCheck.push_back(&cellsList[startCell]);
    }

    while (!listToCheck.empty() && startCell != endCell)
    {
        int cellId = 0;
        double lowestCostGuess = (*listToCheck.front()).totalCostGuess;
        int iterator = 0;
        for (Cells* cell : listToCheck) {
            if (cell->totalCostGuess < lowestCostGuess) {
                lowestCostGuess = cell->totalCostGuess;
                cellId = iterator;
            }
            iterator++;
        }

        //Check of de te checken cell het doel is. Als dat zo is zijn we klaar
        if ((*listToCheck[cellId]).cellId == endCell)
        {
            listToCheck.clear();
            _route.pathFound = true;
        }
        else if (!listToCheck.empty())
        {
            //Loop door de lijst van "buurcellen van de te checken cell"
            for (std::vector<int>::const_iterator iterator = (*listToCheck[cellId]).neighbours.begin(), end = (*listToCheck[cellId]).neighbours.end(); iterator != end; ++iterator)
            {
                //Kijk of deze cell eerder bezocht is
                if (!cellsList[*iterator].visited)
                {
                    //Deze cell heeft geen parent is is dus nooit eerder gevonden! De buren moeten dus toegevoegd worden!
                    cellsList[*iterator].addNeighbours(cellsList);
                    //De cell waarvan we de neighbours onderzoeken is uiteraard tot nu toe de kortste route hiernaartoe
                    cellsList[*iterator].parentCellId = (*listToCheck[cellId]).cellId;
                    //Nu moeten de kosten voor de route hiernatoe uitgerekend worden (Dit zijn de kosten van naar de parent gaan +1)
                    cellsList[*iterator].cummulativeCost = (*listToCheck[cellId]).cummulativeCost + 1;
                    //Als laatste zetten we de cell in de lijst met cellen die gecheckt moet worden
                    listToCheck.push_back(&cellsList[*iterator]);
                    //En we schatten vanaf deze cell de kosten naar het doel
                    cellsList[*iterator].costToGoal = dist(cellsList[*iterator].position.x, cellsList[*iterator].position.y, cellsList[endCell].position.x, cellsList[endCell].position.y);
                    //Waardoor we dus de totale kosten kunnen schatten
                    cellsList[*iterator].totalCostGuess = cellsList[*iterator].costToGoal + cellsList[*iterator].cummulativeCost;
                    //We zijn voor nu klaar met deze tegel en laten een vlag achter dat de tegel al eens bekeken is
                    cellsList[*iterator].visited = true;
                }
                else
                {
                    //Deze tegel is al eerder gevonden, de buren staan dus al in de te checken cell lijst en hoeft niet opnieuw gechecked te worden
                    //We moeten wel weten of de route waarmee deze tegel nu is aangedaan niet korter is dan een eerder gevonden route
                    if ((*listToCheck[cellId]).cummulativeCost + 1 < cellsList[*iterator].cummulativeCost)
                    {
                        //Er is een kortere route naar deze cell! Pas de parent cell dus aan en geef een nieuwe cummulative Cost;
                        cellsList[*iterator].parentCellId = (*listToCheck[cellId]).cellId;
                        cellsList[*iterator].cummulativeCost = (*listToCheck[cellId]).cummulativeCost + 1;
                        //Uiteraard verranderd dit dan ook de geschatte totale kosten vanaf deze tegel
                        cellsList[*iterator].totalCostGuess = cellsList[*iterator].costToGoal + cellsList[*iterator].cummulativeCost;
                    }
                }
            }
            //zet de tegel op de bezochte tegellijst
            checkedList.push_back(&cellsList[(*listToCheck[cellId]).cellId]);
            //en haal hem van de te bezoeken tegellijst
            listToCheck.erase(std::next(listToCheck.begin(), cellId));
        }
    }
    //Alle tegels die te bereiken zijn zijn bekeken of er is een route gevonden.
    //Maak een route door de parent cells achter elkaar te zetten tot de begin cell
    _route.route.clear();
    if (_route.pathFound)
    {
        //Er is een pad naar het doel gevonden! Stippel de route maar uit!
        _route.route.push_back({ cellsList[endCell].position, cellsList[endCell].visited, cellsList[endCell].parentCellId });
        while (!endReached)
        {
            if (_route.route.back().visited == true)
            {
                //Voeg voor iedere tegel de parent toe aan de lijst (dit was de tegel met de korste weg naar deze tegel)
                _route.route.push_back({ cellsList[_route.route.back().parentCellId].position, cellsList[_route.route.back().parentCellId].visited, cellsList[_route.route.back().parentCellId].parentCellId });
                if (_route.route.back().parentCellId == startCell) {
                    //Als de nu volgende parentCell de tegel is waar de actor op staat zijn we klaar.
                    endReached = true;
                }
                if (_route.route.back().parentCellId == -1) {
                    endReached = true;
                }
                //Fix voor een bug waarin op een of andere manier géén parent cell ingevult staat en dus null zou zijn. (al zou dat niet moeten kunnen)
                if (_route.route.back().parentCellId == 0) {
                    //We gaan kijken of het een logische buur kan zijn, gelukkig hoeven we maar drie posities te controleren:
                    if (_route.route.back().position.x == 0 && _route.route.back().position.x == 1) {
                        //Logische parent; doe niets!
                    }
                    else if (_route.route.back().position.x == 0 && _route.route.back().position.x == 1) {
                        //Logische parent; doe niets!
                    }
                    else if (_route.route.back().position.x == 1 && _route.route.back().position.x == 1) {
                        //Logische parent; doe niets!
                    }
                    else {
                        //Onlogisch! breek de route af zonder de parent cell toe te voegen
                        endReached = true;
                    }
                }
            }
            else
            {
                //Blijkbaar is deze tegel nooit geevalueerd of het is de starttegel; onlogisch! breek het route maken af!
                endReached = true;
            }
        }
        _route.realPath = true; //Vlaggetje dat de route naar het eindpunt gaat en er in princiepe geen herbereking nodig is
    }
    else {
        //Check of de actor niet naar een opdracht moet lopen. Is dat het geval wil ik niet naar de dichtsbijzijnde tegel lopen maar de actor een nieuw doel laten kiezen
        if (_baseState->_base == BaseStateNames::Building) {
            switchSubState(SubStateNames::SearchingAlternativeBuildingSpot);
        } else if (_baseState->_base == BaseStateNames::Gathering) {
            switchSubState(SubStateNames::SearchingNextSimilairResource);
        } else{
            //We kunnen niet bij het doel komen en hebben geen bouw of verzamel opdracht, dan gaan we maar naar de geschatte dichtsbijzijnde cell waar we naartoe kunnen!
            if (!checkedList.empty()) {
                std::sort(checkedList.begin(), checkedList.end(), ([](const Cells* f, const Cells* s)
                    {
                        return f->costToGoal < s->costToGoal;
                    }));
                //Een bereikbare tegel met de laagst geschatte totale kosten staat nu vooraan in de rij van de bezochte tegels
                _actorGoal = cellsList[(*checkedList.front()).cellId].position;
                if ((*checkedList.front()).parentCellId != -1) {
                    //Actor staat niet op de dichtsbijzijnde tegel: Stippel de route hiernaartoe uit
                            //Er is een pad naar het doel gevonden! Stippel de route maar uit!
                    _route.route.push_back({ cellsList[endCell].position, cellsList[endCell].visited, cellsList[endCell].parentCellId });
                    while (!endReached)
                    {
                        if (_route.route.back().visited == true)
                        {
                            //Voeg voor iedere tegel de parent toe aan de lijst (dit was de tegel met de korste weg naar deze tegel)
                            _route.route.push_back({ cellsList[_route.route.back().parentCellId].position, cellsList[_route.route.back().parentCellId].visited, cellsList[_route.route.back().parentCellId].parentCellId });
                            if (_route.route.back().parentCellId == startCell) {
                                //Als de nu volgende parentCell de tegel is waar de actor op staat zijn we klaar.
                                endReached = true;
                            }
                            if (_route.route.back().parentCellId == -1) {
                                endReached = true;
                            }
                            //Fix voor een bug waarin op een of andere manier géén parent cell ingevult staat en dus null zou zijn. (al zou dat niet moeten kunnen)
                            if (_route.route.back().parentCellId == 0) {
                                //We gaan kijken of het een logische buur kan zijn, gelukkig hoeven we maar drie posities te controleren:
                                if (_route.route.back().position.x == 0 && _route.route.back().position.x == 1) {
                                    //Logische parent; doe niets!
                                }
                                else if (_route.route.back().position.x == 0 && _route.route.back().position.x == 1) {
                                    //Logische parent; doe niets!
                                }
                                else if (_route.route.back().position.x == 1 && _route.route.back().position.x == 1) {
                                    //Logische parent; doe niets!
                                }
                                else {
                                    //Onlogisch! breek de route af zonder de parent cell toe te voegen
                                    endReached = true;
                                }
                            }
                        }
                        else
                        {
                            //Blijkbaar is deze tegel nooit geevalueerd of het is de starttegel; onlogisch! breek het route maken af!
                            endReached = true;
                        }
                    }
                }
                else {
                    //Actor staat al op de dichtsbijzijnde tegel er is dus geen route nodig
                }
               _route.pathFound = true; //Er is geen pad naar het doel maar wel een pad naar de dichtsbijzijnde plek, de actor moet dus wel gaan lopen
               _route.realPath = false; //Vlaggetje op false laten staan zodat op een later tijdstip er opnieuw geprobeerd kan worden of er nu wél een pad naar het doel is
            }
        }
    }
}
