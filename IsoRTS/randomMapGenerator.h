
#pragma once
#include "globalfunctions.h"

struct foodLocationData
{
	int id;
	cords foodCords;
	int foodGroupId;

};

extern void generateRandomMap(int players, int amountOfFoodGroups, int amountOfStoneGroups, int amountOfGoldGroups, int treeDensityLevel, int tries);

