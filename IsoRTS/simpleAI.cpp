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

void simpleAI::gatherCommandUnit(int& unitId, cords targetCords)
{


}

void simpleAI::attakCommandUnit(int& unitId, cords targetCords)
{
}

void simpleAI::produceCommandBuilding(int& buildingId, bool& isResearch, int& idOfUnitOrResearch)
{
}



void simpleAI::sandboxScript()
{
	if (hasBuildingType(1)) {
		//We hebben een TC 
		if (listOfPlayers[this->playerId].getIdleVillagerId(0) != -1) {

		}
		
	}
	else {
		//De eerste taak is een TC bouwen mits voldoende resources
		if (priceOfBuilding[1].food <= listOfPlayers[this->playerId].getStats().amountOfFood && priceOfBuilding[1].wood <= listOfPlayers[this->playerId].getStats().amountOfWood && priceOfBuilding[1].stone <= listOfPlayers[this->playerId].getStats().amountOfStone && priceOfBuilding[1].gold <= listOfPlayers[this->playerId].getStats().amountOfGold) {
			//zoek een vrije plek bij de villagers om een TC te bouwen
			int idleVillagerId = listOfPlayers[this->playerId].getIdleVillagerId(0);
			cords idleVillagerCords = listOfActors[idleVillagerId].getActorCords();
			cords buildingSlot = getFreeBuildingSlot(1, idleVillagerCords);
			buildBuilding(1, buildingSlot);
			buildCommandUnit(idleVillagerId, buildingSlot);
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

cords simpleAI::getFreeBuildingSlot(int buildingId, cords& closeToThis)
{
	std::cout << "trying to place building near " << closeToThis.x << " - " << closeToThis.y <<std::endl;
	int maximumDistance = 3;
	bool freeFootprintFound = false;
	cords suggestedCords;
	int tries = 0;
	while (!freeFootprintFound && tries < 60) {
		suggestedCords = { roll(closeToThis.x - maximumDistance, closeToThis.x + maximumDistance), roll(closeToThis.y - maximumDistance, closeToThis.y + maximumDistance) };
		std::cout << suggestedCords.x << " - " << suggestedCords.y << std::endl;
		bool buildingPlacable = true;
		if (suggestedCords.x < MAP_WIDTH && suggestedCords.y < MAP_HEIGHT) {
			for (int i = 0; i < footprintOfBuildings[buildingId].amountOfXFootprint; i++)
			{
				for (int j = 0; j < footprintOfBuildings[buildingId].amountOfYFootprint; j++)
				{
					if (suggestedCords.x - i >= 0 && suggestedCords.y - j >= 0) {
						if (currentGame.occupiedByBuildingList[suggestedCords.x - i][suggestedCords.y - j] != -1 || currentGame.objectLocationList[suggestedCords.x - i][suggestedCords.y - j] != -1 || currentGame.occupiedByActorList[suggestedCords.x - i][suggestedCords.y - j] != -1 || currentGame.currentMap[suggestedCords.x - i][suggestedCords.y - j] == 7) {
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
	std::cout << suggestedCords.x << " - " << suggestedCords.y;
	if (freeFootprintFound) {
		return suggestedCords;
	}
	else {
		return { -1, -1 };
	}
}

bool simpleAI::hasBuildingType(int id)
{
	return false;
}
