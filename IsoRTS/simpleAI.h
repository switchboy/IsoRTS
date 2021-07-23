#ifndef simpleAI_H
#define simpleAI_H

#include "gamestate.h";
#include "player.h";

class simpleAI
{
public:
	simpleAI(int& playerId);
	void update();
private:
	void buildBuilding(int& buildingId, int& x, int& y);
	void moveCommandUnit(int& unitId, int& x, int& y);
	void buildCommandUnit(int& unitId, int& x, int& y);
	void gatherCommandUnit(int& unitId, int& x, int& y);
	void attakCommandUnit(int& unitId, int& x, int& y);
	void produceCommandBuilding(int& buildingId, bool& isResearch, int& idOfUnitOrResearch);
	int getIdOfIdleVillager();

	int playerId;
	int difficultyLevel;
};

#endif