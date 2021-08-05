#include "player.h"
player listOfPlayers[8];
player& currentPlayer = listOfPlayers[0];


player::player()
{
    this->team;
    this->amountOfWood = 300;
    this->amountOfFood = 100;
    this->amountOfGold = 100;
    this->amountOfStone = 100;
    this->currentPopulation = 0;
    this->populationRoom = 0;
    for (int i = 0; i < 8; i++) {
        this->friendOrFoo[i] = 0;
    }
    this->isDefeated = false;
    this->isParticipating = true;
}

player::~player()
{
    //dtor
}


void player::addResources(resourceTypes resource, int amount)
{

    switch(resource)
    {
    case resourceTypes::resourceWood:
        this->amountOfWood += amount;
        break;
    case resourceTypes::resourceFood:
        this->amountOfFood += amount;
        break;
    case resourceTypes::resourceStone:
        this->amountOfStone += amount;
        break;
    case resourceTypes::resourceGold:
        this->amountOfGold += amount;
        break;
    }

}

void player::setTeam(int team)
{
    this->team = team;
}

void player::substractResources(int resource, int amount)
{
    switch(resource)
    {
    case 0:
        //wood
        this->amountOfWood -= amount;
        break;
    case 1:
        //food
        this->amountOfFood -= amount;
        break;
    case 2:
        //stone
        this->amountOfStone -= amount;
        break;
    case 3:
        //gold
        this->amountOfGold -= amount;
        break;
    }
}

void player::addToCurrentPopulation(int amount)
{
    this->currentPopulation += amount;
}

void player::substractFromCurrentPopulation(int amount)
{
    this->currentPopulation -= amount;
}

void player::addToPopulationRoom(int amount)
{
    this->populationRoom += amount;
}

void player::substractFromPopulationRoom(int amount)
{
    this->populationRoom -= amount;
}

void player::setTeamToNeutral(int team)
{

}

void player::setTeamToAlly(int team)
{

}

void player::setTeamToEnemy(int team)
{

}

int player::getFriendOrFoo(int team)
{
    //tbd
    return 0;
}

int player::getTeam()
{
    return this->team;
}

bool player::isPlayerDefeated()
{
    return this->isDefeated;
}

void player::clearLists()
{
    this->idleVillagersList.clear();
    this->villagersList.clear();
    this->gatheringWood.clear();
    this->gatheringFood.clear();
    this->gatheringStone.clear();
    this->gatheringGold.clear();
    this->building.clear();

}


int player::getVillagers() 
{
    return static_cast<int>(this->villagersList.size());
}

int player::getTotalGatheringWood() 
{
    return static_cast<int>(this->gatheringWood.size());
}

int player::getTotalGatheringFood() 
{
    return static_cast<int>(this->gatheringFood.size());
}
int player::getTotalGatheringStone() 
{
    return static_cast<int>(this->gatheringStone.size());
}
int player::getTotalGatheringGold() 
{
    return static_cast<int>(this->gatheringGold.size());
}
int player::getGatheringWood(int id)
{
    return this->gatheringWood[id];
}

int player::getGatheringFood(int id) 
{
    return this->gatheringFood[id];
}

int player::getGatheringStone(int id)
{
    return this->gatheringStone[id];
}

int player::getGatheringGold(int id) 
{
    return this->gatheringGold[id];
}

int player::getTotalBuilding()
{
    return static_cast<int>(this->building.size());
}

int player::getBuilder(int id)
{
    return this->building[id];
}

int player::getVillager(int id)
{
    return this->villagersList[id];
}

int player::getSwordsman(int id)
{
    return this->listOfSwordsman[id];
}

int player::getTotalSwordsman()
{
    return static_cast<int>(this->listOfSwordsman.size());
}

void player::insertSwordsman(int& id)
{
    this->listOfSwordsman.push_back(id);
}


void player::insertIdIntoIdleVillagerList(int& id)
{
    this->idleVillagersList.push_back(id);
}

void player::insertVillagerList(int& id)
{
    this->villagersList.push_back(id);
}

void player::insertGatheringWood(int& id)
{
    this->gatheringWood.push_back(id);
}

void player::insertGatheringFood(int& id)
{
    this->gatheringFood.push_back(id);
}

void player::insertGatheringStone(int& id)
{
    this->gatheringStone.push_back(id);
}

void player::insertGatheringGold(int& id)
{
    this->gatheringGold.push_back(id);
}

void player::insertBuilding(int& id)
{
    this->building.push_back(id);
}

int player::getPopulationRoom()
{
    return this->populationRoom - this->currentPopulation;
}

int player::getIdleVillagerId(int it)
{
    if (it < idleVillagersList.size()) {
        return idleVillagersList[it];
    }
    else {
        return -1;
    }
}

int player::getIdleVillagers()
{
    return static_cast<int>(this->idleVillagersList.size());
}

playerStats player::getStats()
{
    playerStats temp = {this->amountOfWood, this->amountOfFood, this->amountOfGold, this->amountOfStone,this->currentPopulation, this->populationRoom, this->getTeam()};
    return  temp;
}

