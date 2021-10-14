#pragma once
#include "globalfunctions.h"

struct foodLocationData
{
	int   id;
	int   foodGroupId;
	cords foodCords;
};

extern void generateRandomMap(int players, int amountOfFoodGroups, int amountOfStoneGroups, int amountOfGoldGroups, int treeDensityLevel, int tries);
extern void centerViewOnVillager();
extern void smoothTerrain();