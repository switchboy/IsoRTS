#ifndef PLAYER_H
#define PLAYER_H
#include <vector>

struct playerStats
{
    int amountOfWood;
    int amountOfFood;
    int amountOfGold;
    int amountOfStone;
    int currentPopulation;
    int populationRoom;
    int team;
};

class player
{
public:
    player();
    virtual ~player();
    void addResources(int resource, int amount);
    void substractResources(int resource, int amount);
    void addToCurrentPopulation(int amount);
    void substractFromCurrentPopulation(int amount);
    void addToPopulationRoom(int amount);
    void substractFromPopulationRoom(int amount);
    void setTeamToNeutral(int team);
    void setTeamToAlly(int team);
    void setTeamToEnemy(int team);
    int getFriendOrFoo(int team);
    int getTeam();
    void setTeam(int team);
    bool isPlayerDefeated();
    void clearIdleVillagerList();
    void insertIdIntoIdleVillagerList(int& id);
    int getIdleVillagerId(int it);
    playerStats getStats();

private:
    int team;
    int amountOfWood;
    int amountOfFood;
    int amountOfGold;
    int amountOfStone;
    int currentPopulation;
    int populationRoom;
    int friendOrFoo[8];
    bool isDefeated;
    bool isParticipating;
    std::vector<int> idleVillagersList;
};

extern player& currentPlayer;
extern player listOfPlayers[8];

#endif // PLAYER_H

