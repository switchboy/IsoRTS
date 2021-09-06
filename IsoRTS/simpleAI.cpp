#include <iostream>
#include "actors.h"
#include "buildings.h"
#include "objects.h"
#include "player.h"
#include "simpleAI.h"

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
	buildings newBuilding(buildingId, buildingCords, static_cast<int>(listOfBuildings.size()), this->playerId);
	listOfBuildings.push_back(newBuilding);
	listOfPlayers[this->playerId].substractResources(resourceTypes::resourceWood, listOfBuildingTemplates[buildingId].getPriceOfBuilding().wood);
	listOfPlayers[this->playerId].substractResources(resourceTypes::resourceFood, listOfBuildingTemplates[buildingId].getPriceOfBuilding().food);
	listOfPlayers[this->playerId].substractResources(resourceTypes::resourceStone, listOfBuildingTemplates[buildingId].getPriceOfBuilding().stone);
	listOfPlayers[this->playerId].substractResources(resourceTypes::resourceGold, listOfBuildingTemplates[buildingId].getPriceOfBuilding().gold);
}

void simpleAI::moveCommandUnit(int unitId, cords targetCords)
{
	listOfActors[unitId].updateGoal(targetCords, 0);
}

void simpleAI::buildCommandUnit(int unitId, cords targetCords)
{
	nearestBuildingTile tempTile = findNearestBuildingTile(currentGame.occupiedByBuildingList[targetCords.x][targetCords.y], unitId);
	if (tempTile.isSet) {
		listOfActors[unitId].updateGoal(tempTile.location, 0);
		listOfBuildings[currentGame.occupiedByBuildingList[targetCords.x][targetCords.y]].claimFreeBuiildingTile(tempTile.buildingId, unitId);
		listOfActors[unitId].setIsBuildingTrue(listOfBuildings[currentGame.occupiedByBuildingList[targetCords.x][targetCords.y]].getBuildingId(), tempTile.actionLocation);
	}
}

void simpleAI::gatherCommandUnit(int unitId, cords targetCords)
{
	listOfActors[unitId].updateGoal(targetCords, 0);
	listOfActors[unitId].setGatheringRecource(true);
}

void simpleAI::attakCommandUnit(int unitId, cords targetCords, bool first)
{
	if (first) {
		listOfActors[unitId].updateGoal(targetCords, 0);
		listOfActors[unitId].setIsDoingAttack();
	}
	else {
		listOfActors[unitId].stackOrder(targetCords, stackOrderTypes::stackActionAttack);
	}
}

void simpleAI::produceCommandBuilding(int buildingId, bool isResearch, int idOfUnitOrResearch)
{
	listOfBuildings[buildingId].getTask(isResearch, idOfUnitOrResearch);
}


int distanceToResource(resourceTypes kind, cords from) {
	std::list <nearestBuildingTile> listOfResourceLocations;
	if (!listOfObjects.empty()) {
		//more efficient search method; searching in a range 3600 squares have to be evaluated. Searching in the object list ~3100 items should be evaluated within one loop
		for (int i = 0; i < listOfObjects.size(); i++) {
			if (listOfObjects[i].getTypeOfResource() == kind) {
				float tempDeltaDistance = static_cast<float>(distEuclidean(from.x, from.y, listOfObjects[i].getLocation().x, listOfObjects[i].getLocation().y));
				listOfResourceLocations.push_back({ tempDeltaDistance, listOfObjects[i].getLocation().x, listOfObjects[i].getLocation().y, i, true });
			}
		}
	}
	if (!listOfResourceLocations.empty())
	{
		listOfResourceLocations.sort([](const nearestBuildingTile& f, const nearestBuildingTile& s)
			{
				return f.deltaDistance < s.deltaDistance;
			});
		return static_cast<int>(listOfResourceLocations.front().deltaDistance);
	}
	else {
		return 9999;
	}
}

cords findResource(resourceTypes kind, int unitId ) {
	std::list <nearestBuildingTile> listOfResourceLocations;
	cords actorCords = listOfActors[unitId].getActorCords();
	cords targetCords{ -1, -1 };

	for (int i = 0; i < listOfObjects.size(); i++) {
		if (listOfObjects[i].getTypeOfResource() == kind && listOfObjects[i].amountOfResourcesLeft() > 0 ) {
			float tempDeltaDistance = static_cast<float>(distEuclidean(listOfActors[unitId].getActorCords().x, listOfActors[unitId].getActorCords().y, listOfObjects[i].getLocation().x, listOfObjects[i].getLocation().y));
			listOfResourceLocations.push_back({ tempDeltaDistance, listOfObjects[i].getLocation().x, listOfObjects[i].getLocation().y, i, true });
		}
	}

	if (!listOfResourceLocations.empty())
	{
		listOfResourceLocations.sort([](const nearestBuildingTile& f, const nearestBuildingTile& s)
			{
				return f.deltaDistance < s.deltaDistance;
			});
		
		bool firstLocationIsNotRejected = false;
		while (!firstLocationIsNotRejected && !listOfResourceLocations.empty() && !listOfActors[unitId].getRejectedTargetsList().empty()) {
			bool rejectionsDuringLoop = false;
			for (const auto& reject : listOfActors[unitId].getRejectedTargetsList())
			{
				if (reject.x == listOfResourceLocations.front().location.x && reject.y == listOfResourceLocations.front().location.y) {
					//target rejected!
					listOfResourceLocations.pop_front();
					rejectionsDuringLoop = true;
				}
			}
			if (!rejectionsDuringLoop) {
				firstLocationIsNotRejected = true; //break the loop 
			}
		}

		if (!listOfResourceLocations.empty()) {
			targetCords = listOfResourceLocations.front().location;
		}

	}
	return targetCords;
}

void simpleAI::buildBuildingNearUnlessBuilding(int buildingId, int idleVillagerId, resourceTypes nearResource) {
	int idOfUnfinishedHousing = isBuildingThereButIncomplete(buildingId);
	if (idOfUnfinishedHousing == -1) {
		if (listOfBuildingTemplates[buildingId].getPriceOfBuilding().food <= listOfPlayers[this->playerId].getStats().amountOfFood && listOfBuildingTemplates[buildingId].getPriceOfBuilding().wood <= listOfPlayers[this->playerId].getStats().amountOfWood && listOfBuildingTemplates[buildingId].getPriceOfBuilding().stone <= listOfPlayers[this->playerId].getStats().amountOfStone && listOfBuildingTemplates[buildingId].getPriceOfBuilding().gold <= listOfPlayers[this->playerId].getStats().amountOfGold) {
			cords buildingSlot = {0,0};
			switch (nearResource) {
			case resourceTypes::None:
				buildingSlot = getOptimalFreeBuildingSlot(buildingId, listOfActors[idleVillagerId].getActorCords(), false, false, false, false);
				break;
			case resourceTypes::resourceWood:
				buildingSlot = getOptimalFreeBuildingSlot(buildingId, listOfActors[idleVillagerId].getActorCords(), true, false, false, false);
				break;
			case resourceTypes::resourceFood:
				buildingSlot = getOptimalFreeBuildingSlot(buildingId, listOfActors[idleVillagerId].getActorCords(), false, true, false, false);
				break;
			case resourceTypes::resourceStone:
				buildingSlot = getOptimalFreeBuildingSlot(buildingId, listOfActors[idleVillagerId].getActorCords(), false, false, true, false);
				break;
			case resourceTypes::resourceGold:
				buildingSlot = getOptimalFreeBuildingSlot(buildingId, listOfActors[idleVillagerId].getActorCords(), false, false, false, true);
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
			cords buildingCords = listOfBuildings[idOfUnfinishedHousing].getLocation() ;
			buildCommandUnit(idleVillagerId, buildingCords);
		}
	}
}

void simpleAI::distributeIdleVillagers() {
	int villagersThatShouldBeGathering;
	int gatheringNow;
	int villagersAssigned = 0;
	for (int resourceId = 0; resourceId < 4; resourceId++) {
		resourceTypes resourceType;
		switch (resourceId) {
		case 0:
			resourceType = resourceTypes::resourceWood;
			gatheringNow = listOfPlayers[this->playerId].getTotalGatheringWood();
			villagersThatShouldBeGathering = static_cast<int>(ceil(listOfPlayers[this->playerId].getVillagers() * 0.3f));
			break;
		case 1:
			resourceType = resourceTypes::resourceFood;
			gatheringNow = listOfPlayers[this->playerId].getTotalGatheringFood();
			villagersThatShouldBeGathering = static_cast<int>(ceil(listOfPlayers[this->playerId].getVillagers() * 0.4f));
			break;
		case 2:
			resourceType = resourceTypes::resourceStone;
			gatheringNow = listOfPlayers[this->playerId].getTotalGatheringStone();
			villagersThatShouldBeGathering = static_cast<int>(ceil(listOfPlayers[this->playerId].getVillagers() * 0.1f));
			break;
		case 3:
			resourceType = resourceTypes::resourceGold;
			gatheringNow = listOfPlayers[this->playerId].getTotalGatheringGold();
			villagersThatShouldBeGathering = static_cast<int>(ceil(listOfPlayers[this->playerId].getVillagers() * 0.2f));
			break;
		}
		while (gatheringNow < villagersThatShouldBeGathering && villagersAssigned < listOfPlayers[this->playerId].getIdleVillagers()) {
			//Get nearest food source
			cords targetCords = findResource(resourceType, listOfPlayers[this->playerId].getIdleVillagerId(villagersAssigned));
			//Order unit
			if (targetCords.x != -1) {
				gatherCommandUnit(listOfPlayers[this->playerId].getIdleVillagerId(villagersAssigned), targetCords);
				villagersAssigned++;
				gatheringNow++;
			}
			else {
				//This  resource is not available for this unit; Whut do?
				villagersAssigned++;
				gatheringNow++;
			}
		}
	}
}

void simpleAI::orderBuildingToProduceUnitIfItHadNoTask(int buildingTypeId, int unitId) {
	std::vector<int> listOfTownCenterIds = getBuildingIdsOfType(buildingTypeId);
	for (int i = 0; i < listOfTownCenterIds.size(); i++) {
		//check of er iets gemaakt wordt
		if (!listOfBuildings[listOfTownCenterIds[i]].hasTask()) {
			//maakt een villager als de resources er zijn
			if (
				listOfActorTemplates[unitId].getPriceOfActor().food <= listOfPlayers[this->playerId].getStats().amountOfFood &&
				listOfActorTemplates[unitId].getPriceOfActor().wood <= listOfPlayers[this->playerId].getStats().amountOfWood &&
				listOfActorTemplates[unitId].getPriceOfActor().stone <= listOfPlayers[this->playerId].getStats().amountOfStone &&
				listOfActorTemplates[unitId].getPriceOfActor().gold <= listOfPlayers[this->playerId].getStats().amountOfGold)
			{
				produceCommandBuilding(listOfTownCenterIds[i], false, unitId);
			}
		}
	}
}

bool simpleAI::buildDropOffPointWhen(int buildingId, resourceTypes resource, int first, int second, int third) {
	if (getBuildingIdsOfType(buildingId).size() == 0) {
		if (listOfPlayers[this->playerId].getTotalGathering(resource) > first) {
			buildBuildingNearUnlessBuilding(buildingId, listOfPlayers[this->playerId].getGatheringFood(0), resource);
			return true;
		}
	}
	else if (getBuildingIdsOfType(buildingId).size() == 1) {
		if (listOfPlayers[this->playerId].getTotalGathering(resource) > second) {
			buildBuildingNearUnlessBuilding(buildingId, listOfPlayers[this->playerId].getGatheringFood(0), resource);
			return true;
		}
	}
	else if (getBuildingIdsOfType(buildingId).size() == 1) {
		if (listOfPlayers[this->playerId].getTotalGathering(resource) > third) {
			buildBuildingNearUnlessBuilding(buildingId, listOfPlayers[this->playerId].getGatheringFood(0), resource);
			return true;
		}
	}
	return false;
}

void simpleAI::tryBuilding()
{
	if (buildDropOffPointWhen(2, resourceTypes::resourceFood, 8, 16, 32)) {

	}
	else if (buildDropOffPointWhen(3, resourceTypes::resourceWood, 8, 16, 32)) {

	}
	else if (buildDropOffPointWhen(5, resourceTypes::resourceStone, 8, 16, 32)) {

	}
	else if (buildDropOffPointWhen(6, resourceTypes::resourceGold, 8, 16, 32)) {
	}
	else if (listOfPlayers[this->playerId].getVillagers() >= 15 && !this->hasBuildingType(4)) {
		this->buildBuildingNearUnlessBuilding(4, listOfPlayers[this->playerId].getVillager(0), resourceTypes::None);
	}
	else if (listOfPlayers[this->playerId].getVillagers() >= 30 && !this->hasBuildingType(4)) {
		this->buildBuildingNearUnlessBuilding(4, listOfPlayers[this->playerId].getVillager(0), resourceTypes::None);
	}
}

cords getRandomCords(cords currentCords) {
	bool validCordsFound = false;
	cords suggestedCords;
	while (!validCordsFound) {
		suggestedCords = { roll(0, MAP_WIDTH), roll(0 , MAP_HEIGHT) };
		validCordsFound = currentGame.isPassableButMightHaveActor(suggestedCords);
	}
	return suggestedCords;
}

void simpleAI::sandboxScript()
{
	if (this->hasBuildingType(1)) {
		//check if there are idle villagers and order them around
		if (listOfPlayers[this->playerId].getIdleVillagers() > 0) {
			//Er zijn idle villagers!
			distributeIdleVillagers();
		}
		else {
			//Alway expand villagers, if possible
			if (listOfPlayers[this->playerId].getVillagers() < 60) {
				//Check if there is population room
				if (listOfPlayers[this->playerId].getPopulationRoom() <= 0) {
					//We should build a house! Unless.. we are building a house of course...
					if (listOfPlayers[this->playerId].getTotalGatheringWood() > 0) {
						this->buildBuildingNearUnlessBuilding(0, listOfPlayers[this->playerId].getGatheringWood(0), resourceTypes::None);
					}
					else {
						if (listOfPlayers[this->playerId].getVillagers() > 0) {
							this->buildBuildingNearUnlessBuilding(0, listOfPlayers[this->playerId].getVillager(0), resourceTypes::None);
						}
					}
				}
				else {
					//Make a villager: first get the id of the town center
					orderBuildingToProduceUnitIfItHadNoTask(1, 0);
				}
			}

			if (this->hasBuildingType(4) && true && listOfPlayers[this->playerId].getTotalSwordsman() < 30) {
				orderBuildingToProduceUnitIfItHadNoTask(4, 1);
			}

		}
		
		tryBuilding(); //Try to build something. Only build one thing per tick
	}
	else {
		//De eerste taak is een TC bouwen mits voldoende resources
		if (listOfBuildingTemplates[1].getPriceOfBuilding().food <= listOfPlayers[this->playerId].getStats().amountOfFood && listOfBuildingTemplates[1].getPriceOfBuilding().wood <= listOfPlayers[this->playerId].getStats().amountOfWood && listOfBuildingTemplates[1].getPriceOfBuilding().stone <= listOfPlayers[this->playerId].getStats().amountOfStone && listOfBuildingTemplates[1].getPriceOfBuilding().gold <= listOfPlayers[this->playerId].getStats().amountOfGold) {
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
			if (listOfBuildingTemplates[1].getPriceOfBuilding().wood <= listOfPlayers[this->playerId].getStats().amountOfWood) {
				if(isBuildingThereButIncomplete(3) == -1){
					//Probeer eerst hout te verzamelen: heb ik een lumbercamp? 
					if (!hasBuildingType(3)) {
						//Probeer lumber camp te bouwen
						if (listOfBuildingTemplates[3].getPriceOfBuilding().food <= listOfPlayers[this->playerId].getStats().amountOfFood && listOfBuildingTemplates[3].getPriceOfBuilding().wood <= listOfPlayers[this->playerId].getStats().amountOfWood && listOfBuildingTemplates[3].getPriceOfBuilding().stone <= listOfPlayers[this->playerId].getStats().amountOfStone && listOfBuildingTemplates[3].getPriceOfBuilding().gold <= listOfPlayers[this->playerId].getStats().amountOfGold) {
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
					if (listOfPlayers[this->playerId].getVillagers() > 0) {
						//Zet alle villagers op hout!

						for (int i = 0; i < listOfPlayers[this->playerId].getVillagers(); i++) {
							cords targetCords = findResource(resourceTypes::resourceWood, listOfPlayers[this->playerId].getVillager(i));
							//Order unit
							if (targetCords.x != -1) {
								gatherCommandUnit(listOfPlayers[this->playerId].getVillager(i), targetCords);;
							}
							else {
								//No wood we are screwed!
							}
						}

					}
					else {
						//no villagers we are screwed!

					}
				}
				else {
					//we are building a TC!  TODO check if villagers are asigned

				}
			}
			else {
				// We could check other resources; but we don't need to
			}
		}
	}

	//Explore with a swordsman for now
	if (listOfPlayers[this->playerId].getTotalSwordsman() > 0) {
		bool commandGiven = false;
		int swordsmanId = 0;
		while (!commandGiven && swordsmanId < listOfPlayers[this->playerId].getTotalSwordsman()) {
			if (listOfActors[listOfPlayers[this->playerId].getSwordsman(swordsmanId)].isAlive()) {
				if (listOfActors[listOfPlayers[this->playerId].getSwordsman(swordsmanId)].idle()) {
					this->moveCommandUnit(listOfPlayers[this->playerId].getSwordsman(swordsmanId), getRandomCords(listOfActors[listOfPlayers[this->playerId].getSwordsman(swordsmanId)].getActorCords()));
				}
				commandGiven = true;
			}
			swordsmanId++;
		}
	}


	if( attackWaveSent < 1) {
		if (listOfPlayers[this->playerId].getTotalSwordsman() > 6) {
			excecuteAttackPlan();
			attackWaveSent++;
		}
	}
	else if (attackWaveSent < 2) {
		if (listOfPlayers[this->playerId].getTotalSwordsman() > 12) {
			excecuteAttackPlan();
			attackWaveSent++;
		}
	}
}

struct possibleBuildTile {
	cords startCordsOfTile;
	cords endCordsOfTile;
	int optimazationScore;
};

void simpleAI::excecuteAttackPlan() {
	int playerToAttakId;
	bool enemyFound = false;
	while (!enemyFound) {
		playerToAttakId = roll(0, currentGame.getPlayerCount() - 1);
		if (playerToAttakId != this->playerId) { enemyFound = true; }
	}
	for (int i = 0; i < listOfPlayers[this->playerId].getTotalSwordsman(); i++) {
		attakCommandUnit(listOfPlayers[this->playerId].getSwordsman(i), listOfActors[listOfPlayers[playerToAttakId].getUnit(0)].getActorCords(), true);
	}
}

bool checkIfEmpty(cords tile) {
	if (tile.x >= 0 && tile.x < MAP_WIDTH && tile.y >= 0 && tile.y < MAP_HEIGHT) {
		if (currentGame.occupiedByBuildingList[tile.x][tile.y] == -1 && currentGame.objectLocationList[tile.x][tile.y] == -1) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}


cords simpleAI::getOptimalFreeBuildingSlot(int buildingId, cords closeToVillager, bool closeToWood, bool closeToFood, bool closeToStone, bool closeToGold) {
	//Set the size of the square to search in
	std::vector <possibleBuildTile> listOfPossibilities;
	listOfPossibilities.clear();
	cords maxSearchArea = { 15, 15 }; //this will evaluate all start locations in a 15x15 square (900 induvidual evaluations)
	cords startCords = { closeToVillager.x - (maxSearchArea.x / 2), closeToVillager.y - (maxSearchArea.y / 2) };
	cords endCords = { closeToVillager.x + (maxSearchArea.x / 2), closeToVillager.y + (maxSearchArea.y / 2) };
	//Now trim to get only cords within the map limitations
	if (startCords.x - listOfBuildingTemplates[buildingId].getBuildingFootprint().amountOfXFootprint < 0) { startCords.x = listOfBuildingTemplates[buildingId].getBuildingFootprint().amountOfXFootprint; };
	if (startCords.y - listOfBuildingTemplates[buildingId].getBuildingFootprint().amountOfYFootprint < 0) { startCords.y = listOfBuildingTemplates[buildingId].getBuildingFootprint().amountOfYFootprint; };
	if (endCords.x >= MAP_WIDTH) { endCords.x = MAP_WIDTH; }
	if (endCords.y >= MAP_HEIGHT) { endCords.x = MAP_HEIGHT; }

	//Time to evaluate all the posibilities
	for (int x = startCords.x; x < endCords.x; x++) {
		for (int y = startCords.y; y < endCords.y; y++) {
			bool buildingPossible = true;
			for (int i = 0; i < listOfBuildingTemplates[buildingId].getBuildingFootprint().amountOfXFootprint; i++)
			{
				for (int j = 0; j < listOfBuildingTemplates[buildingId].getBuildingFootprint().amountOfYFootprint; j++)
				{
					if (x - i >= 0 && y - j >= 0) {
						if (!currentGame.isPassable({ x - i, y - j })) {
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
				listOfPossibilities.push_back({ {x,y}, {x - listOfBuildingTemplates[buildingId].getBuildingFootprint().amountOfXFootprint, y - listOfBuildingTemplates[buildingId].getBuildingFootprint().amountOfYFootprint}, 0 });
				int thisOne = static_cast<int>(listOfPossibilities.size()) - 1;

				//Lets calculate the score
				int tempScore = 0;
				int bonusFactorDistance = 1;
				int bonusFactorProximityToResource = 1;
				int penaltyForAdjacentTileBlocked = 10;

				//calculate distance to actor
				tempScore += static_cast<int>(15 - distEuclidean(listOfPossibilities[thisOne].startCordsOfTile.x, listOfPossibilities[thisOne].startCordsOfTile.y, closeToVillager.x, closeToVillager.y)) * bonusFactorDistance;

				//Score for proximity to resource
				if (closeToWood) {
					//search for the closest source of a resource
					tempScore += 15 - distanceToResource(resourceTypes::resourceWood, listOfPossibilities[thisOne].startCordsOfTile) * bonusFactorProximityToResource;
				}
				if (closeToFood) {
					//search for the closest source of a resource
					tempScore += 15 - distanceToResource(resourceTypes::resourceFood, listOfPossibilities[thisOne].startCordsOfTile) * bonusFactorProximityToResource;
				}
				if (closeToStone) {
					//search for the closest source of a resource
					tempScore += 15 - distanceToResource(resourceTypes::resourceStone, listOfPossibilities[thisOne].startCordsOfTile) * bonusFactorProximityToResource;
				}
				if (closeToGold) {
					//search for the closest source of a resource
					tempScore += 15 - distanceToResource(resourceTypes::resourceGold, listOfPossibilities[thisOne].startCordsOfTile) * bonusFactorProximityToResource;
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
			for (int i = 0; i < listOfBuildingTemplates[buildingId].getBuildingFootprint().amountOfXFootprint; i++)
			{
				for (int j = 0; j < listOfBuildingTemplates[buildingId].getBuildingFootprint().amountOfYFootprint; j++)
				{
					if (suggestedCords.x - i >= 0 && suggestedCords.y - j >= 0) {
						if (currentGame.occupiedByBuildingList[suggestedCords.x - i][suggestedCords.y - j] != -1 || currentGame.objectLocationList[suggestedCords.x - i][suggestedCords.y - j] != -1 || (!currentGame.occupiedByActorList[suggestedCords.x - i][suggestedCords.y - j].empty()) || currentGame.currentMap[suggestedCords.x - i][suggestedCords.y - j] >= 3|| currentGame.currentMap[suggestedCords.x - i][suggestedCords.y - j] == 0) {
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
