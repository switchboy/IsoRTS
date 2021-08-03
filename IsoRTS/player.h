#ifndef PLAYER_H
#define PLAYER_H
#include <vector>
#include "humanReadableNames.h"

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
    void addResources(resourceTypes resource, int amount);
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
    void clearLists();
    void insertIdIntoIdleVillagerList(int& id);
    void insertVillagerList(int& id);
    void insertGatheringWood(int& id);
    void insertGatheringFood(int& id);
    void insertGatheringStone(int& id);
    void insertGatheringGold(int& id);
    void insertBuilding(int& id);
    int getPopulationRoom();
    int getIdleVillagerId(int it);
    int getIdleVillagers();
    int getVillagers();
    int getTotalGatheringWood();
    int getTotalGatheringFood();
    int getTotalGatheringStone();
    int getTotalGatheringGold();
    int getGatheringWood(int id);
    int getGatheringFood(int id);
    int getGatheringStone(int id);
    int getGatheringGold(int id);
    int getTotalBuilding();
    int getBuilder(int id);
    int getVillager(int id);
    int getSwordsman(int id);
    int getTotalSwordsman();
    void insertSwordsman(int& id);
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
    std::vector<int> villagersList;
    std::vector<int> idleVillagersList;
    std::vector<int> gatheringWood;
    std::vector<int> gatheringFood;
    std::vector<int> gatheringStone;
    std::vector<int> gatheringGold;
    std::vector<int> building;
    std::vector<int> listOfSwordsman;
};

extern player& currentPlayer;
extern player listOfPlayers[8];

#endif // PLAYER_H

