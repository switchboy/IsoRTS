#include "randomMapGenerator.h"
#include "globalfunctions.h"
#include "gamestate.h"
#include <iostream>
#include "objects.h"
#include "actors.h"
#include "player.h"
#include "humanReadableNames.h"

void generatePerlinNoise(float scaleBias, int octaves, float* noiseSeed, float* output)
{
	for (int x = 0; x < MAP_WIDTH; x++) {
		for (int y = 0; y < MAP_HEIGHT; y++) {

			float noise = 0.0f;
			float totalScale = 0.0f;
			float scale = 1.0f;
			
			for (int o = 0; o < octaves; o++) {
				int pitch = MAP_WIDTH >> o;

				int sampleX1 = (x / pitch) * pitch;
				int sampleY1 = (y / pitch) * pitch;

				int sampleX2 = (sampleX1 / pitch) % MAP_WIDTH;
				int sampleY2 = (sampleY1 / pitch) % MAP_WIDTH;
				
				float blendX = static_cast<float>(x - sampleX1) / static_cast<float>(pitch);
				float blendY = static_cast<float>(y - sampleY1) / static_cast<float>(pitch);

				float sampleT = (1.0f - blendX) * noiseSeed[sampleY1 * MAP_WIDTH + sampleX1] + blendX * noiseSeed[sampleY1 * MAP_WIDTH + sampleX2];
				float sampleB = (1.0f - blendX) * noiseSeed[sampleY2 * MAP_WIDTH + sampleX2] + blendX * noiseSeed[sampleY2 * MAP_WIDTH + sampleX2];

				totalScale += scale;
				noise += (blendY * (sampleB - sampleT) + sampleT) * scale;
				scale = scale / scaleBias;
			}
			output[y * MAP_WIDTH + x] = noise / totalScale;
		}
	}
}

void convertPerlinNoiseToMap(float* noisemap) {
	for (int x = 0; x < MAP_WIDTH; x++)
	{
		for (int y = 0; y < MAP_HEIGHT; y++)
		{
			int depth = static_cast<int>(noisemap[y * MAP_WIDTH + x] * 3.0f);
			switch (depth)
			{
			case 0: currentGame.currentMap[x][y] = 7; break;
			case 1: currentGame.currentMap[x][y] = 1; break;
			case 2: currentGame.currentMap[x][y] = 2; break;
			default: currentGame.currentMap[x][y] = 1; break;
			}
		}
	}
}



void placeTrees() {
	srand(static_cast<int>(time(NULL)));
	for (int x = 0; x < MAP_WIDTH; x++)
	{
		for (int y = 0; y < MAP_HEIGHT; y++)
		{
			if (roll(0, 20) > 19) {
				if (currentGame.currentMap[x][y] == 1) {
					objects newObject(static_cast<objectTypes>(roll(1, 3)), x, y, static_cast<int>(listOfObjects.size()));
					listOfObjects.push_back(newObject);
				}
				else if (currentGame.currentMap[x][y] == 2)//Cactus
				{
					objects newObject(objectCactus, x, y, static_cast<int>(listOfObjects.size()));
					listOfObjects.push_back(newObject);
				}
			}
		}
	}
}

void spawmFoodStoneGold(int resource, int amountOfGroups)
{
	int gridMinX=0;
	int gridMinY=0;
	for (int gridMaxX = 64; gridMaxX < 256; gridMaxX += 64) {
		for (int gridMaxY = 64; gridMaxY < 256; gridMaxY += 64) {
			bool resourcePlaced = false;
			int maxTries = 0;
			int succesFullPlacements = 0;
			while (!resourcePlaced && maxTries < 9999 && succesFullPlacements <= amountOfGroups ) {
				cords suggestedCords = { roll(gridMinX,gridMaxX), roll(gridMinY,gridMaxY) };
				if (suggestedCords.y - 1 >= 0 && suggestedCords.y + 1 < MAP_HEIGHT && suggestedCords.x + 1 < MAP_WIDTH) {
					if (currentGame.isPassable(suggestedCords.x, suggestedCords.y) && currentGame.isPassable(suggestedCords.x, suggestedCords.y + 1) && currentGame.isPassable(suggestedCords.x + 1, suggestedCords.y) && currentGame.isPassable(suggestedCords.x + 1, suggestedCords.y - 1))
					{
						objects newObject(static_cast<objectTypes>(resource), suggestedCords.x, suggestedCords.y, static_cast<int>(listOfObjects.size()));
						listOfObjects.push_back(newObject);
						objects newObject1(static_cast<objectTypes>(resource), suggestedCords.x, suggestedCords.y+1, static_cast<int>(listOfObjects.size()));
						listOfObjects.push_back(newObject1);
						objects newObject2(static_cast<objectTypes>(resource), suggestedCords.x+1, suggestedCords.y, static_cast<int>(listOfObjects.size()));
						listOfObjects.push_back(newObject2);
						objects newObject3(static_cast<objectTypes>(resource), suggestedCords.x+1, suggestedCords.y-1, static_cast<int>(listOfObjects.size()));
						listOfObjects.push_back(newObject3);
						resourcePlaced = true;
						succesFullPlacements++;
					}
				}
				maxTries++;
			}
			gridMinY = gridMaxY;
		}
		gridMinX = gridMaxX;
	}
}

std::list<int> occupiedFoodSources;

cords findRandomFoodSource() {
	bool foodFound = false;
	std::vector<foodLocationData> foodLocations;
	int nextFoodGroupId = 0;
	for (int i = 0; i < listOfObjects.size(); i++) {
		if (listOfObjects[i].getTypeOfResource() == 1) {
			//check if this food source is next to another food source, if so it belongs to the same food group
			int foodGroup = -1;
			for (int j = 0; j < foodLocations.size(); j++) {
				if (
					listOfObjects[i].getLocation().x + 1 == listOfObjects[foodLocations[j].id].getLocation().x && listOfObjects[i].getLocation().y == listOfObjects[foodLocations[j].id].getLocation().y ||
					listOfObjects[i].getLocation().x - 1 == listOfObjects[foodLocations[j].id].getLocation().x && listOfObjects[i].getLocation().y == listOfObjects[foodLocations[j].id].getLocation().y ||
					listOfObjects[i].getLocation().x == listOfObjects[foodLocations[j].id].getLocation().x && listOfObjects[i].getLocation().y + 1 == listOfObjects[foodLocations[j].id].getLocation().y ||
					listOfObjects[i].getLocation().x == listOfObjects[foodLocations[j].id].getLocation().x && listOfObjects[i].getLocation().y - 1 == listOfObjects[foodLocations[j].id].getLocation().y ||
					listOfObjects[i].getLocation().x + 1 == listOfObjects[foodLocations[j].id].getLocation().x && listOfObjects[i].getLocation().y + 1 == listOfObjects[foodLocations[j].id].getLocation().y ||
					listOfObjects[i].getLocation().x + 1 == listOfObjects[foodLocations[j].id].getLocation().x && listOfObjects[i].getLocation().y - 1 == listOfObjects[foodLocations[j].id].getLocation().y ||
					listOfObjects[i].getLocation().x - 1 == listOfObjects[foodLocations[j].id].getLocation().x && listOfObjects[i].getLocation().y + 1 == listOfObjects[foodLocations[j].id].getLocation().y ||
					listOfObjects[i].getLocation().x - 1 == listOfObjects[foodLocations[j].id].getLocation().x && listOfObjects[i].getLocation().y - 1 == listOfObjects[foodLocations[j].id].getLocation().y
					)
				{
					foodGroup = foodLocations[j].foodGroupId;

				}
			}
			if (foodGroup == -1) {
				//create new food group
				foodGroup = nextFoodGroupId;
				nextFoodGroupId++;
			}
			foodLocations.push_back({ i , {listOfObjects[i].getLocation().x, listOfObjects[i].getLocation().y}, foodGroup });
		}
	}


	//Sort and remove food sources with the same foodGroupId to prevent players spawning on the same foodGroup
	std::sort(foodLocations.begin(), foodLocations.end(),[](const foodLocationData& lhs, const foodLocationData& rhs) {return lhs.foodGroupId < rhs.foodGroupId;});
	foodLocations.erase(std::unique(foodLocations.begin(), foodLocations.end(), [](const foodLocationData& lhs, const foodLocationData& rhs) {return lhs.foodGroupId == rhs.foodGroupId; }), foodLocations.end());
	
	if (foodLocations.size() >= currentGame.getPlayerCount()) {
		//Now the following while loop should always be able to finish
		while (!foodFound) {
			int possibleFood = roll(0, static_cast<int>(foodLocations.size()));
			bool foodSourceOccupied = false;
			for (const int& foodOwned : occupiedFoodSources)
				{
				if (foodOwned == possibleFood) {
						foodSourceOccupied = true;
				}
				}
			if (!foodSourceOccupied) {
				occupiedFoodSources.push_back(possibleFood);
				return  foodLocations[possibleFood].foodCords;
			}
		}
	}
	else {
		//There is not enough food on the map!
		return { -1,-1 };
	}
}

void spawmFirstVillager(int distanceFromFood, int teamId) {
	bool villagerIsPlaced = false;
	cords randomFoodSource = findRandomFoodSource();

	if (randomFoodSource.x != -1) {
		while (!villagerIsPlaced) {
			cords suggestedCords = { roll(randomFoodSource.x - distanceFromFood,randomFoodSource.x + distanceFromFood), roll(randomFoodSource.y - distanceFromFood, randomFoodSource.y + distanceFromFood) };
			if (suggestedCords.y + 1 < MAP_HEIGHT && suggestedCords.x + 1 < MAP_WIDTH) {
				if (currentGame.isPassable(suggestedCords.x, suggestedCords.y) && currentGame.isPassable(suggestedCords.x, suggestedCords.y + 1) && currentGame.isPassable(suggestedCords.x + 1, suggestedCords.y) && currentGame.isPassable(suggestedCords.x + 1, suggestedCords.y + 1))
				{
					actors newActor(0, suggestedCords.x, suggestedCords.y, teamId, static_cast<int>(listOfActors.size()));
					listOfActors.push_back(newActor);
					actors newActor1(0, suggestedCords.x, suggestedCords.y + 1, teamId, static_cast<int>(listOfActors.size()));
					listOfActors.push_back(newActor1);
					actors newActor2(0, suggestedCords.x + 1, suggestedCords.y, teamId, static_cast<int>(listOfActors.size()));
					listOfActors.push_back(newActor2);
					actors newActor3(0, suggestedCords.x + 1, suggestedCords.y + 1, teamId, static_cast<int>(listOfActors.size()));
					listOfActors.push_back(newActor3);
					villagerIsPlaced = true;
				}
			}
		}
	}
	else {
		//there is not enough food on the map!

	}
}

void centerViewOnVillager() {
	for (int i = 0; i < listOfActors.size(); i++) {
		if (listOfActors[i].getTeam() == currentPlayer.getTeam()) {
			viewOffsetX = worldSpace(listOfActors[i].getLocation().x, listOfActors[i].getLocation().y, true);
			viewOffsetY = worldSpace(listOfActors[i].getLocation().x, listOfActors[i].getLocation().y, false);
		}
	}
}

void generateTerrain() {
	srand(static_cast<int>(time(NULL)));
	float* noiseMap = nullptr;
	float* noiseSeed = nullptr;
	noiseSeed = new float[MAP_WIDTH * MAP_HEIGHT];
	noiseMap = new float[MAP_WIDTH * MAP_HEIGHT];
	for (int i = 0; i < MAP_WIDTH * MAP_HEIGHT; i++) {
		noiseSeed[i] = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	};
	generatePerlinNoise(1.4f, 5, noiseSeed, noiseMap);
	convertPerlinNoiseToMap(noiseMap);
	delete []noiseMap;
	delete []noiseSeed;
}

void generateRandomMap(int players, int amountOfFoodGroups, int amountOfStoneGroups, int amountOfGoldGroups) {
	generateTerrain();
	placeTrees();
	spawmFoodStoneGold(6, amountOfFoodGroups);
	for (int i = 0; i < players; i++) {
		spawmFirstVillager(8, i);
	}
	spawmFoodStoneGold(4, amountOfStoneGroups);
	spawmFoodStoneGold(5, amountOfGoldGroups);
	centerViewOnVillager();
}