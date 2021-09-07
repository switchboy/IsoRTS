#ifndef simpleAI_H
#define simpleAI_H
#include "globalfunctions.h"

class simpleAI
{
public:
						simpleAI(int playerId, int difficultyLevel);
	void				update();

private:
	bool				hasBuildingType(int id);
	void				attakCommandUnit(int unitId, cords targetCords, bool first);
	void				buildBuilding(int buildingId, cords buildingCords);
	void				buildBuildingNearUnlessBuilding(int buildingId, int idleVillagerId, resourceTypes nearResource);
	void				excecuteAttackPlan();
	void				buildCommandUnit(int unitId, cords targetCords);
	void				distributeIdleVillagers();
	void				gatherCommandUnit(int unitId, cords targetCords);
	void				moveCommandUnit(int unitId, cords targetCords);
	void				produceCommandBuilding(int buildingId, bool isResearch, int idOfUnitOrResearch);
	void				sandboxScript();
	void				orderBuildingToProduceUnitIfItHadNoTask(int buildingTypeId, int unitId);
	bool				buildDropOffPointWhen(int buildingId, resourceTypes resource, int first, int second, int third);
	void				tryBuilding();
	int					isBuildingThereButIncomplete(int type);
	std::vector<int>	getBuildingIdsOfType(int type);
	cords				getFreeBuildingSlot(int buildingId, cords closeToThis); //Just a random slot
	cords				getOptimalFreeBuildingSlot(int buildingId, cords closeToVillager, bool closeToWood, bool closeToFood, bool closeToStone, bool closeToGold); //Best possible slot

	int					attackWaveSent = 0;
	int					stoneMiningCampOwned;
	int					playerId;
	int					difficultyLevel;
};

extern std::vector<simpleAI> listOfAI;
extern cords findResource(resourceTypes kind, int unitId);

#endif