#include "simpleAI.h"
#include <iostream>

std::vector<simpleAI> listOfAI;

simpleAI::simpleAI(int playerId, int difficultyLevel)
{
	this->playerId = playerId;
	this->difficultyLevel = difficultyLevel;
	
}

void simpleAI::update()
{
	switch (difficultyLevel) {
	case 0:
		//sandbox
		sandboxScript();
		break;
	case 1:
		//easy
		break;
	case 2:
		//normal
		break;
	case 3:
		//hard
		break;
	case 4:
		//unfair
		break;
	default:
		//sandbox
		sandboxScript();
		break;
	}

}

void simpleAI::buildBuilding(int buildingId, cords buildingCords)
{
	buildings newBuilding(buildingId, buildingCords.x, buildingCords.y, listOfBuildings.size(), this->playerId);
	listOfBuildings.push_back(newBuilding);
	listOfPlayers[this->playerId].substractResources(0, priceOfBuilding[buildingId].wood);
	listOfPlayers[this->playerId].substractResources(1, priceOfBuilding[buildingId].food);
	listOfPlayers[this->playerId].substractResources(2, priceOfBuilding[buildingId].stone);
	listOfPlayers[this->playerId].substractResources(3, priceOfBuilding[buildingId].gold);
}

void simpleAI::moveCommandUnit(int& unitId, cords targetCords)
{
}

void simpleAI::buildCommandUnit(int& unitId, cords targetCords)
{
	nearestBuildingTile tempTile = findNearestBuildingTile(currentGame.occupiedByBuildingList[targetCords.x][targetCords.y], unitId);
	if (tempTile.isSet) {
		listOfActors[unitId].updateGoal(tempTile.locationX, tempTile.locationY, 0);
		listOfBuildings[currentGame.occupiedByBuildingList[targetCords.x][targetCords.y]].claimFreeBuiildingTile(tempTile.buildingId, unitId);
		listOfActors[unitId].setIsBuildingTrue(listOfBuildings[currentGame.occupiedByBuildingList[targetCords.x][targetCords.y]].getBuildingId(), tempTile.actionLocationX, tempTile.actionLocationY);
	}
}

void simpleAI::gatherCommandUnit(int unitId, cords targetCords)
{
	listOfActors[unitId].updateGoal(targetCords.x, targetCords.y, 0);
	listOfActors[unitId].setGatheringRecource(true);
}

void simpleAI::attakCommandUnit(int& unitId, cords targetCords)
{
}

void simpleAI::produceCommandBuilding(int buildingId, bool isResearch, int idOfUnitOrResearch)
{
	listOfBuildings[buildingId].getTask(isResearch, idOfUnitOrResearch);
}


int distanceToResource(int kind, cords from) {
	std::list <nearestBuildingTile> listOfResourceLocations;
	cords targetCords;
	int lowSearchLimitX = from.x - 60;
	if (lowSearchLimitX < 0)
	{
		lowSearchLimitX = 0;
	}
	int lowSearchLimitY = from.y - 60;
	if (lowSearchLimitY < 0)
	{
		lowSearchLimitY = 0;
	}
	int highSearchLimitX = from.x + 60;
	if (highSearchLimitX > MAP_WIDTH)
	{
		highSearchLimitX = MAP_WIDTH;
	}
	int highSearchLimitY = from.y + 60;
	if (highSearchLimitY > MAP_HEIGHT)
	{
		highSearchLimitY = MAP_HEIGHT;
	}
	for (int i = lowSearchLimitX; i < highSearchLimitX; i++)
	{
		for (int j = lowSearchLimitY; j < highSearchLimitY; j++)
		{
			if (currentGame.objectLocationList[i][j] != -1)
			{
				if (listOfObjects[currentGame.objectLocationList[i][j]].getTypeOfResource() == kind)
				{
					float tempDeltaDistance = distEuclidean(from.x, from.y, i, j);
					listOfResourceLocations.push_back({ tempDeltaDistance, i, j, currentGame.objectLocationList[i][j], true });
				}
			}
		}
	}
	if (!listOfResourceLocations.empty())
	{
		listOfResourceLocations.sort([](const nearestBuildingTile& f, const nearestBuildingTile& s)
			{
				return f.deltaDistance < s.deltaDistance;
			});
	}
	return listOfResourceLocations.front().deltaDistance;
}





cords findResource(int kind, int unitId ) {
	std::list <nearestBuildingTile> listOfResourceLocations;
	cords actorCords = listOfActors[unitId].getActorCords();
	cords targetCords;
	int lowSearchLimitX = actorCords.x - 60;
	if (lowSearchLimitX < 0)
	{
		lowSearchLimitX = 0;
	}
	int lowSearchLimitY = actorCords.y - 60;
	if (lowSearchLimitY < 0)
	{
		lowSearchLimitY = 0;
	}
	int highSearchLimitX = actorCords.x + 60;
	if (highSearchLimitX > MAP_WIDTH)
	{
		highSearchLimitX = MAP_WIDTH;
	}
	int highSearchLimitY = actorCords.y + 60;
	if (highSearchLimitY > MAP_HEIGHT)
	{
		highSearchLimitY = MAP_HEIGHT;
	}
	for (int i = lowSearchLimitX; i < highSearchLimitX; i++)
	{
		for (int j = lowSearchLimitY; j < highSearchLimitY; j++)
		{
			if (currentGame.objectLocationList[i][j] != -1)
			{
				if (listOfObjects[currentGame.objectLocationList[i][j]].getTypeOfResource() == kind)
				{
					float tempDeltaDistance =  distEuclidean(actorCords.x, actorCords.y, i, j);
					listOfResourceLocations.push_back({ tempDeltaDistance, i, j, currentGame.objectLocationList[i][j], true });
				}
			}
		}
	}
	if (!listOfResourceLocations.empty())
	{
		listOfResourceLocations.sort([](const nearestBuildingTile& f, const nearestBuildingTile& s)
			{
				return f.deltaDistance < s.deltaDistance;
			});
		
		targetCords.x = listOfResourceLocations.front().locationX;
		targetCords.y = listOfResourceLocations.front().locationY;
	}
	return targetCords;
}


void simpleAI::buildBuildingNearUnlessBuilding(int buildingId, int idleVillagerId, int nearResource) {
	int idOfUnfinishedHousing = isBuildingThereButIncomplete(buildingId);
	if (idOfUnfinishedHousing == -1) {
		if (priceOfBuilding[buildingId].food <= listOfPlayers[this->playerId].getStats().amountOfFood && priceOfBuilding[buildingId].wood <= listOfPlayers[this->playerId].getStats().amountOfWood && priceOfBuilding[buildingId].stone <= listOfPlayers[this->playerId].getStats().amountOfStone && priceOfBuilding[buildingId].gold <= listOfPlayers[this->playerId].getStats().amountOfGold) {
			cords buildingSlot;
			switch (nearResource) {
			case -1:
				buildingSlot = getOptimalFreeBuildingSlot(buildingId, listOfActors[idleVillagerId].getLocation(), false, false, false, false);
				break;
			case 0:
				buildingSlot = getOptimalFreeBuildingSlot(buildingId, listOfActors[idleVillagerId].getLocation(), true, false, false, false);
				break;
			case 1:
				buildingSlot = getOptimalFreeBuildingSlot(buildingId, listOfActors[idleVillagerId].getLocation(), false, true, false, false);
				break;
			case 2:
				buildingSlot = getOptimalFreeBuildingSlot(buildingId, listOfActors[idleVillagerId].getLocation(), false, false, true, false);
				break;
			case 3:
				buildingSlot = getOptimalFreeBuildingSlot(buildingId, listOfActors[idleVillagerId].getLocation(), false, false, false, true);
				break;
			}
			buildBuilding(buildingId, buildingSlot);
			buildCommandUnit(idleVillagerId, buildingSlot);
		}
	}else {
		//Suspisious... is there a builder assigned? If not assign builder
		bool hasBuilder = false;
		for (int i = 0; i < listOfPlayers[this->playerId].getTotalBuilding(); i++) {
			if (listOfActors[listOfPlayers[this->playerId].getBuilder(i)].getBuildingId() == idOfUnfinishedHousing) {
				hasBuilder = true;
			}
		}
		if (!hasBuilder) {
			std::cout << idOfUnfinishedHousing << std::endl;
			cords buildingCords = { listOfBuildings[idOfUnfinishedHousing].getLocationX() ,listOfBuildings[idOfUnfinishedHousing].getLocationY() };
			std::cout << "actor location" << listOfActors[idleVillagerId].getLocation().x << " - " << listOfActors[idleVillagerId].getLocation().y << std::endl;
			std::cout << "Building slot:" << buildingCords.x << " - " << buildingCords.y << std::endl;
			buildCommandUnit(idleVillagerId, buildingCords);
		}
	}
}

void simpleAI::distributeIdleVillagers() {
	int villagersThatShouldBeGathering;
	int gatheringNow;
	int villagersAssigned = 0;
	for (int resourceId = 0; resourceId < 4; resourceId++) {
		switch (resourceId) {
		case 0:
			gatheringNow = listOfPlayers[this->playerId].getTotalGatheringWood();
			villagersThatShouldBeGathering = ceil((float)listOfPlayers[this->playerId].getVillagers() * 0.3f);
			break;
		case 1:
			gatheringNow = listOfPlayers[this->playerId].getTotalGatheringFood();
			villagersThatShouldBeGathering = ceil((float)listOfPlayers[this->playerId].getVillagers() * 0.4f);
			break;
		case 2:
			gatheringNow = listOfPlayers[this->playerId].getTotalGatheringStone();
			villagersThatShouldBeGathering = ceil((float)listOfPlayers[this->playerId].getVillagers() * 0.1f);
			break;
		case 3:
			gatheringNow = listOfPlayers[this->playerId].getTotalGatheringGold();
			villagersThatShouldBeGathering = ceil((float)listOfPlayers[this->playerId].getVillagers() * 0.2f);
			break;
		}
		while (gatheringNow < villagersThatShouldBeGathering && villagersAssigned < listOfPlayers[this->playerId].getIdleVillagers()) {
			//Get nearest food source
			cords targetCords = findResource(resourceId, listOfPlayers[this->playerId].getIdleVillagerId(villagersAssigned));
			//Order unit
			gatherCommandUnit(listOfPlayers[this->playerId].getIdleVillagerId(villagersAssigned), targetCords);
			villagersAssigned++;
			gatheringNow++;
		}
	}
}

void simpleAI::sandboxScript()
{
	if (this->hasBuildingType(1)) {
		//check if there are idle villagers and order them around
		if (listOfPlayers[this->playerId].getIdleVillagers() > 0) {
			//Er zijn idle villagers!
			distributeIdleVillagers();
		}
		//Alway expand villagers, if possible
		if (listOfPlayers[this->playerId].getVillagers() < 60) {
			//Check if there is population room
			if (listOfPlayers[this->playerId].getPopulationRoom() <= 0) {
				//We should build a house! Unless.. we are building a house of course...
				if (listOfPlayers[this->playerId].getTotalGatheringWood() > 0) {
					this->buildBuildingNearUnlessBuilding(0, listOfPlayers[this->playerId].getGatheringWood(0), -1);
				}
				else {
					if (listOfPlayers[this->playerId].getVillagers() > 0) {
						this->buildBuildingNearUnlessBuilding(0, listOfPlayers[this->playerId].getVillager(0), -1);
					}
				}
			}
			else {
				//Make a villager: first get the id of the town center
				std::vector<int> listOfTownCenterIds = getBuildingIdsOfType(1);
				for (int i = 0; i < listOfTownCenterIds.size(); i++) {
					//maakt een villager als de resources er zijn
					if (priceOfActor[0].food <= listOfPlayers[this->playerId].getStats().amountOfFood && priceOfActor[0].wood <= listOfPlayers[this->playerId].getStats().amountOfWood && priceOfActor[0].stone <= listOfPlayers[this->playerId].getStats().amountOfStone && priceOfActor[0].gold <= listOfPlayers[this->playerId].getStats().amountOfGold) {
						produceCommandBuilding(listOfTownCenterIds[i], false, 0);
					}
				}
			}
		}
		//build a mill near a food source when > 8 villagers are gathering food and we don't have it
		if (listOfPlayers[this->playerId].getTotalGatheringFood() > 8 && !this->hasBuildingType(2)) {
			buildBuildingNearUnlessBuilding(2, listOfPlayers[this->playerId].getGatheringFood(0), 1);
		}
		 
		//buiild a lumbercamp near a wood source when > 8 villagers are gathering wood and we don't have it
		if (listOfPlayers[this->playerId].getTotalGatheringWood() > 8 && !this->hasBuildingType(3)) {
			buildBuildingNearUnlessBuilding(3, listOfPlayers[this->playerId].getGatheringWood(0), 0);
		}

		//build a mining camp near a stone source when > 2 villagers are gathering stone and we don't have it
		if (listOfPlayers[this->playerId].getTotalGatheringStone() > 2 && !this->hasBuildingType(5)) {
			buildBuildingNearUnlessBuilding(5, listOfPlayers[this->playerId].getGatheringStone(0), 2);
		}
		//build a mining camp near a gold source when > 2 villagers ar mining gold and we don't have it
		if (listOfPlayers[this->playerId].getTotalGatheringGold() > 2 && !this->hasBuildingType(6)) {
			buildBuildingNearUnlessBuilding(6, listOfPlayers[this->playerId].getGatheringStone(0), 3);
		}
		//build barraks when there are more then 15 villagers
		if (listOfPlayers[this->playerId].getVillagers() >= 15 && !this->hasBuildingType(4)) {
			this->buildBuildingNearUnlessBuilding(4, listOfPlayers[this->playerId].getVillager(0), -1);
		}
		if(this->hasBuildingType(4) &&  true && listOfPlayers[this->playerId].getTotalSwordsman() < 30){
			std::vector<int> listOfTownCenterIds = getBuildingIdsOfType(4);
			for (int i = 0; i < listOfTownCenterIds.size(); i++) {
				//maakt een sworsman als er resources zijn
				if (priceOfActor[1].food <= listOfPlayers[this->playerId].getStats().amountOfFood && priceOfActor[1].wood <= listOfPlayers[this->playerId].getStats().amountOfWood && priceOfActor[1].stone <= listOfPlayers[this->playerId].getStats().amountOfStone && priceOfActor[1].gold <= listOfPlayers[this->playerId].getStats().amountOfGold) {
					produceCommandBuilding(listOfTownCenterIds[i], false, 1);
				}
			}
		}	
	}
	else {
		//De eerste taak is een TC bouwen mits voldoende resources
		if (priceOfBuilding[1].food <= listOfPlayers[this->playerId].getStats().amountOfFood && priceOfBuilding[1].wood <= listOfPlayers[this->playerId].getStats().amountOfWood && priceOfBuilding[1].stone <= listOfPlayers[this->playerId].getStats().amountOfStone && priceOfBuilding[1].gold <= listOfPlayers[this->playerId].getStats().amountOfGold) {
			//zoek een vrije plek bij de villagers om een TC te bouwen
			int idleVillagerId = listOfPlayers[this->playerId].getIdleVillagerId(0);
			cords idleVillagerCords = listOfActors[idleVillagerId].getActorCords();
			//cords buildingSlot = getFreeBuildingSlot(1, idleVillagerCords);
			cords buildingSlot = this->getOptimalFreeBuildingSlot(1, idleVillagerCords, true, true, true, true);
			buildBuilding(1, buildingSlot);
			for (int i = 0; i < listOfPlayers[this->playerId].getIdleVillagers(); i++) {
				int vilId = listOfPlayers[this->playerId].getIdleVillagerId(i);
				buildCommandUnit(vilId, buildingSlot);
			}
		}
		else {
			//Er zijn onvoldoende resources!
			// Heb ik voldoende hout?
			if (priceOfBuilding[1].wood <= listOfPlayers[this->playerId].getStats().amountOfWood) {
				//Probeer eerst hout te verzamelen: heb ik een lumbercamp? 
				if (!hasBuildingType(3)) {
					//Probeer lumber camp te bouwen
					if (priceOfBuilding[3].food <= listOfPlayers[this->playerId].getStats().amountOfFood && priceOfBuilding[3].wood <= listOfPlayers[this->playerId].getStats().amountOfWood && priceOfBuilding[3].stone <= listOfPlayers[this->playerId].getStats().amountOfStone && priceOfBuilding[3].gold <= listOfPlayers[this->playerId].getStats().amountOfGold) {
						int idleVillagerId = listOfPlayers[this->playerId].getIdleVillagerId(0);
						cords idleVillagerCords = listOfActors[idleVillagerId].getActorCords();
						cords buildingSlot = getFreeBuildingSlot(3, idleVillagerCords);
						buildBuilding(3, buildingSlot);
						buildCommandUnit(idleVillagerId, buildingSlot);
					}
					else {
						//oh dear er is geen weg naar een towncenter meer!
					}
				}
				//Kan ik een idle villager op hout zetten

			}
			else {

			}
		}
	}
}

struct possibleBuildTile {
	cords startCordsOfTile;
	cords endCordsOfTile;
	int optimazationScore;


};

bool checkIfEmpty(cords tile) {
	if (currentGame.occupiedByBuildingList[tile.x][tile.y] == -1 && currentGame.objectLocationList[tile.x][tile.y] == -1) {
		return true;
	}
	else {
		return false;
	}
}

cords simpleAI::getOptimalFreeBuildingSlot(int buildingId, cords closeToVillager, bool closeToWood, bool closeToFood, bool closeToStone, bool closeToGold) {
	//Set the size of the square to search in
	std::vector <possibleBuildTile> listOfPossibilities;
	listOfPossibilities.clear();
	cords maxSearchArea = { 15, 15 }; //this will evaluate all start locations in a 30x30 square (900 induvidual evaluations)
	cords startCords = { closeToVillager.x - (maxSearchArea.x / 2), closeToVillager.y - (maxSearchArea.y / 2) };
	cords endCords = { closeToVillager.x + (maxSearchArea.x / 2), closeToVillager.y + (maxSearchArea.y / 2) };
	//Now trim to get only cords within the map limitations
	if (startCords.x - footprintOfBuildings[buildingId].amountOfXFootprint < 0) { startCords.x = footprintOfBuildings[buildingId].amountOfXFootprint; };
	if (startCords.y - footprintOfBuildings[buildingId].amountOfYFootprint < 0) { startCords.y = footprintOfBuildings[buildingId].amountOfYFootprint; };
	if (endCords.x >= MAP_WIDTH) { endCords.x = MAP_WIDTH; }
	if (endCords.y >= MAP_HEIGHT) { endCords.x = MAP_HEIGHT; }

	//Fill a temp array with zero's and make them 1 if obstructed on the real map
	int quickLookUpMap[MAP_WIDTH][MAP_HEIGHT] = { 0 };
	for (int x = startCords.x; x < endCords.x; x++) {
		for (int y = startCords.y; y < endCords.y; y++) {
			if (currentGame.occupiedByBuildingList[x][y] != -1 || currentGame.objectLocationList[x][y] != -1 || currentGame.occupiedByActorList[x][y] != -1 || currentGame.currentMap[x][y] >= 7 || currentGame.currentMap[x][y] == 0) {
				quickLookUpMap[x][y] = 1;
			}
		}
	}
	
	//Time to evaluate all the posibilities
	for (int x = startCords.x; x < endCords.x; x++) {
		for (int y = startCords.y; y < endCords.y; y++) {
			bool buildingPossible = true;
			for (int i = 0; i < footprintOfBuildings[buildingId].amountOfXFootprint; i++)
			{
				for (int j = 0; j < footprintOfBuildings[buildingId].amountOfYFootprint; j++)
				{
					if (x - i >= 0 && y - j >= 0) {
						if (quickLookUpMap[x - i][y - j] != 0){
							//Building slot blocked by terain, objects or actors
							buildingPossible = false;
						}
					}
					else {
						//Building slot out of bounds, this should not be possible because of earlier trimming but just in case..
						buildingPossible = false;
					}
				}
			}
			if (buildingPossible) {
				listOfPossibilities.push_back({ {x,y}, {x - footprintOfBuildings[buildingId].amountOfXFootprint, y - footprintOfBuildings[buildingId].amountOfYFootprint}, 0 });
				int thisOne = listOfPossibilities.size() - 1;

				//Lets calculate the score
				int tempScore = 0;
				int bonusFactorDistance = 1;
				int bonusFactorProximityToResource = 1;
				int penaltyForAdjacentTileBlocked = 10;

				//calculate distance to actor
				tempScore += (15 - distEuclidean(listOfPossibilities[thisOne].startCordsOfTile.x, listOfPossibilities[thisOne].startCordsOfTile.y, closeToVillager.x, closeToVillager.y)) * bonusFactorDistance;

				//Score for proximity to resource
				if (closeToWood) {
					//search for the closest source of a resource
					tempScore += 15 - distanceToResource(0, listOfPossibilities[thisOne].startCordsOfTile) * bonusFactorProximityToResource;
				}
				if (closeToFood) {
					//search for the closest source of a resource
					tempScore += 15 - distanceToResource(1, listOfPossibilities[thisOne].startCordsOfTile) * bonusFactorProximityToResource;
				}
				if (closeToStone) {
					//search for the closest source of a resource
					tempScore += 15 - distanceToResource(2, listOfPossibilities[thisOne].startCordsOfTile) * bonusFactorProximityToResource;
				}
				if (closeToGold) {
					//search for the closest source of a resource
					tempScore += 15 - distanceToResource(3, listOfPossibilities[thisOne].startCordsOfTile) * bonusFactorProximityToResource;
				}

				//try not to block resources or paths so substract points for any object or building directly adjacent to buildsite
				for (int g = listOfPossibilities[thisOne].endCordsOfTile.x - 1; g <= listOfPossibilities[thisOne].startCordsOfTile.x + 1; g++)
				{
					if (g >= 0 && g <= MAP_WIDTH)
					{
						int goalX;
						if (g == listOfPossibilities[thisOne].endCordsOfTile.x - 1)
						{
							goalX = g + 1;
						}
						else if (g == listOfPossibilities[thisOne].startCordsOfTile.x + 1)
						{
							goalX = g - 1;
						}
						else
						{
							goalX = g;
						}
						if (!checkIfEmpty({ g, listOfPossibilities[thisOne].endCordsOfTile.y - 1 })) // tegels erboven
						{
							tempScore -= penaltyForAdjacentTileBlocked;
						}
						if (!checkIfEmpty({ g, listOfPossibilities[thisOne].startCordsOfTile.y + 1 }))	 //tegels eronder
						{
							tempScore -= penaltyForAdjacentTileBlocked;
						}
					}
				}
				//The row left and right of the building
				for (int g = listOfPossibilities[thisOne].endCordsOfTile.y + 1; g <= listOfPossibilities[thisOne].startCordsOfTile.y; g++)
				{
					if (g >= 0 && g <= MAP_HEIGHT)
					{
						if (!checkIfEmpty({ listOfPossibilities[thisOne].endCordsOfTile.x - 1, g })) // tegels links
						{
							tempScore -= penaltyForAdjacentTileBlocked;
						}

						if (!checkIfEmpty({ listOfPossibilities[thisOne].startCordsOfTile.x + 1, g }))	 //tegels rechts
						{
							tempScore -= penaltyForAdjacentTileBlocked;
						}
					}
				}

				//Space for future evaluations
				listOfPossibilities[thisOne].optimazationScore = tempScore;
			}
		}
	}
	if (!listOfPossibilities.empty()) {
		//put the tile with the highest score at the front of the list
		std::sort(listOfPossibilities.begin(), listOfPossibilities.end(), [](const possibleBuildTile& a, const possibleBuildTile& b)
			{
				return (a.optimazationScore > b.optimazationScore);
			});
		//return the 'best' tile
		return listOfPossibilities[0].startCordsOfTile;
	}
	else {
		//No suitable tile found!
		return { -1, -1 };
	}
}


cords simpleAI::getFreeBuildingSlot(int buildingId, cords closeToThis)
{
	//depricated function use getOptimalFreeBuildingSlot instead
	int maximumDistance = 3;
	bool freeFootprintFound = false;
	cords suggestedCords;
	int tries = 0;
	while (!freeFootprintFound && tries < 120) {
		suggestedCords = { roll(closeToThis.x - maximumDistance, closeToThis.x + maximumDistance), roll(closeToThis.y - maximumDistance, closeToThis.y + maximumDistance) };
		bool buildingPlacable = true;
		if (suggestedCords.x < MAP_WIDTH && suggestedCords.y < MAP_HEIGHT) {
			for (int i = 0; i < footprintOfBuildings[buildingId].amountOfXFootprint; i++)
			{
				for (int j = 0; j < footprintOfBuildings[buildingId].amountOfYFootprint; j++)
				{
					if (suggestedCords.x - i >= 0 && suggestedCords.y - j >= 0) {
						if (currentGame.occupiedByBuildingList[suggestedCords.x - i][suggestedCords.y - j] != -1 || currentGame.objectLocationList[suggestedCords.x - i][suggestedCords.y - j] != -1 || currentGame.occupiedByActorList[suggestedCords.x - i][suggestedCords.y - j] != -1 || currentGame.currentMap[suggestedCords.x - i][suggestedCords.y - j] >= 7|| currentGame.currentMap[suggestedCords.x - i][suggestedCords.y - j] == 0) {
							buildingPlacable = false;
						}
					}
					else {
						buildingPlacable = false;
					}
				}
			}
		}
		freeFootprintFound = buildingPlacable;
		tries++;
	}
	if (freeFootprintFound) {
		return suggestedCords;
	}
	else {
		return { -1, -1 };
	}
}


int simpleAI::isBuildingThereButIncomplete(int type) {
	for (int i = 0; i < listOfBuildings.size(); i++) {
		if (listOfBuildings[i].getType() == type && listOfBuildings[i].getTeam() == this->playerId) {
			if (!listOfBuildings[i].getCompleted()) {
				return i;
			}
		}
	}
	return -1;
}

std::vector<int> simpleAI::getBuildingIdsOfType(int type) {
	std::vector<int> buildingList;
	for (int i = 0; i < listOfBuildings.size(); i++) {
		if (listOfBuildings[i].getType() == type && listOfBuildings[i].getTeam() == this->playerId) {
			buildingList.push_back(listOfBuildings[i].getBuildingId());
		}
	}
	return buildingList;
}

bool simpleAI::hasBuildingType(int id)
{
	for (int i = 0; i < listOfBuildings.size(); i++) {
		if (listOfBuildings[i].getType() == id && listOfBuildings[i].getTeam() == this->playerId && listOfBuildings[i].getCompleted()) {
			return true;
		}
	}
	return false;
}
