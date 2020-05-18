#include "player.h"
player listOfPlayers[8];
player& currentPlayer = listOfPlayers[0];


player::player()
{
    //ctor
}

player::~player()
{
    //dtor
}


void player::addResources(int resource, int amount)
{

    switch(resource)
    {
    case 0:
        //wood
        this->amountOfWood += amount;
        break;
    case 1:
        //food
        this->amountOfFood += amount;
        break;
    case 2:
        //stone
        this->amountOfStone += amount;
        break;
    case 3:
        //gold
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

playerStats player::getStats()
{
    playerStats temp = {this->amountOfWood, this->amountOfFood, this->amountOfGold, this->amountOfStone,this->currentPopulation, this->populationRoom, this->getTeam()};
    return  temp;
}

