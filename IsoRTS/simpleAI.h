#ifndef simpleAI_H
#define simpleAI_H

#include "gamestate.h"
#include "player.h"
#include "globalfunctions.h"
#include "actors.h"
#include "buildings.h"

class simpleAI
{
public:
	simpleAI(int playerId, int difficultyLevel);
	void update();
private:
	void buildBuilding(int buildingId, cords buildingCords);
	void moveCommandUnit(int& unitId, cords targetCords);
	void buildCommandUnit(int& unitId, cords targetCords);
	void gatherCommandUnit(int& unitId, cords targetCords);
	void attakCommandUnit(int& unitId, cords targetCords);
	void produceCommandBuilding(int& buildingId, bool& isResearch, int& idOfUnitOrResearch);
	void sandboxScript();
	cords getFreeBuildingSlot(int buildingId, cords& closeToThis);
	int playerId;
	int difficultyLevel;
	bool hasBuildingType(int id);
};

extern std::vector<simpleAI> listOfAI;

#endif