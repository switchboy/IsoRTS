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
    void substractResources(resourceTypes resource, int amount);
    void addToCurrentPopulation(int amount);
    void substractFromCurrentPopulation(int amount);
    void addToPopulationRoom(int amount);
    void substractFromPopulationRoom(int amount);
    void setTeamToNeutral(int team);
    void setTeamToAlly(int team);
    void setTeamToEnemy(int team);
    int  getFriendOrFoo(int team);
    int  getTeam() const;
    void setTeam(int team);
    bool isPlayerDefeated() const;
    void clearLists();
    void insertIdIntoIdleVillagerList(int id);
    void insertVillagerList(int id);
    void insertGatheringWood(int id);
    void insertGatheringFood(int id);
    void insertGatheringStone(int id);
    void insertGatheringGold(int id);
    void insertBuilding(int id);
    int  getPopulationRoom() const;
    int  getIdleVillagerId(int it) const;
    int  getIdleVillagers() const;
    int  getVillagers() const;
    int  getTotalGatheringWood() const;
    int  getTotalGatheringFood() const;
    int  getTotalGatheringStone() const;
    int  getTotalGatheringGold() const;
    int  getGatheringWood(int id) const;
    int  getGatheringFood(int id) const;
    int  getGatheringStone(int id) const;
    int  getGatheringGold(int id) const;
    int  getTotalBuilding() const;
    int  getBuilder(int id) const;
    int  getVillager(int id) const;
    int  getSwordsman(int id) const;
    int  getTotalSwordsman() const;
    void insertSwordsman(int id);
    playerStats getStats() const;

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
    std::vector<int> building;
    std::vector<int> gatheringFood;
    std::vector<int> gatheringGold;
    std::vector<int> gatheringStone;
    std::vector<int> gatheringWood;
    std::vector<int> idleVillagersList;
    std::vector<int> listOfSwordsman;
    std::vector<int> villagersList;
};

extern player& currentPlayer;
extern player listOfPlayers[8];

#endif // PLAYER_H

