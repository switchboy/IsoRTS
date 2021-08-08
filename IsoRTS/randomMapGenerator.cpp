#include <iostream>
#include "actors.h"
#include "humanReadableNames.h"
#include "objects.h"
#include "player.h"
#include "randomMapGenerator.h"

std::list<int> occupiedFoodSources;

namespace
{
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

	void placeTree(int x, int y) {
		if (currentGame.currentMap[x][y] == 1) {
			listOfObjects.push_back(objects(static_cast<objectTypes>(roll(1, 3)), { x, y }, static_cast<int>(listOfObjects.size())));
		}
		else if (currentGame.currentMap[x][y] == 2)//Cactus
		{
			listOfObjects.push_back(objects(objectTypes::objectCactus, { x, y }, static_cast<int>(listOfObjects.size())));
		}
	}

	bool neighbourHasTrees(int x, int y) {
		for (int i = x - 1; i < x + 1; i++) {
			for (int j = y - 1; j < y + 1; j++) {
				if (i >= 0 && i < MAP_WIDTH && j >= 0 && j < MAP_HEIGHT) {
					if (currentGame.objectLocationList[i][j] != -1) {
						if (
							listOfObjects[currentGame.objectLocationList[i][j]].getType() == objectTypes::objectCypress ||
							listOfObjects[currentGame.objectLocationList[i][j]].getType() == objectTypes::objectCactus ||
							listOfObjects[currentGame.objectLocationList[i][j]].getType() == objectTypes::objectMaple ||
							listOfObjects[currentGame.objectLocationList[i][j]].getType() == objectTypes::objectPine
							)
						{
							return true;
						}
					}
				}
			}
		}
		return false;
	}

	void placeTrees(int chanceOfTreeNextToTree, int chanceOfNewTree) {
		srand(static_cast<int>(time(NULL)));
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			for (int y = 0; y < MAP_HEIGHT; y++)
			{
				//check if neigbnoring tile is a tree if so the chance of a tree is lager
				if (neighbourHasTrees(x, y)) {
					if (roll(0, chanceOfTreeNextToTree) > chanceOfTreeNextToTree - 1) {
						placeTree(x, y);
					}
				}
				else {
					if (roll(0, chanceOfNewTree) > chanceOfNewTree - 1) {
						placeTree(x, y);
					}
				}

			}
		}
	}

	bool spawnFoodStoneGold(int resource, int amountOfGroups)
	{
		int gridMinX = 0;//TODO: these variables are never used
		int gridMinY = 0;//TODO: these variables are never used
		int succesFullPlacements = 0;
		for (int gridMaxX = 32; gridMaxX < MAP_WIDTH; gridMaxX += 32) {
			for (int gridMaxY = 32; gridMaxY < MAP_HEIGHT; gridMaxY += 32) {
				bool resourcePlaced = false;
				int maxTries = 0;
				while (!resourcePlaced && maxTries < 999999999999 && succesFullPlacements <= amountOfGroups) {
					cords suggestedCords = { roll(0,MAP_WIDTH), roll(0,MAP_HEIGHT) };
					if (suggestedCords.y - 1 >= 0 && suggestedCords.y + 1 < MAP_HEIGHT && suggestedCords.x + 1 < MAP_WIDTH) {
						if (currentGame.isPassable(suggestedCords.x, suggestedCords.y) && currentGame.isPassable(suggestedCords.x, suggestedCords.y + 1) && currentGame.isPassable(suggestedCords.x + 1, suggestedCords.y) && currentGame.isPassable(suggestedCords.x + 1, suggestedCords.y - 1))
						{
							listOfObjects.push_back(objects(static_cast<objectTypes>(resource), suggestedCords, static_cast<int>(listOfObjects.size())));
							listOfObjects.push_back(objects(static_cast<objectTypes>(resource), { suggestedCords.x, suggestedCords.y + 1 }, static_cast<int>(listOfObjects.size())));
							listOfObjects.push_back(objects(static_cast<objectTypes>(resource), { suggestedCords.x + 1, suggestedCords.y }, static_cast<int>(listOfObjects.size())));
							listOfObjects.push_back(objects(static_cast<objectTypes>(resource), { suggestedCords.x + 1, suggestedCords.y - 1 }, static_cast<int>(listOfObjects.size())));
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
		if (succesFullPlacements <= amountOfGroups) {
			return false;
		}
		else {
			return true;
		}
	}

	cords findRandomFoodSource() {
		bool foodFound = false;
		std::vector<foodLocationData> foodLocations;
		int nextFoodGroupId = 0;
		for (int i = 0; i < listOfObjects.size(); i++) {
			if (listOfObjects[i].getTypeOfResource() == resourceTypes::resourceFood) {
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
				foodLocations.push_back({ i , foodGroup, {listOfObjects[i].getLocation().x, listOfObjects[i].getLocation().y} });
			}
		}


		//Sort and remove food sources with the same foodGroupId to prevent players spawning on the same foodGroup
		std::sort(foodLocations.begin(), foodLocations.end(), [](const foodLocationData& lhs, const foodLocationData& rhs) {return lhs.foodGroupId < rhs.foodGroupId; });
		foodLocations.erase(std::unique(foodLocations.begin(), foodLocations.end(), [](const foodLocationData& lhs, const foodLocationData& rhs) {return lhs.foodGroupId == rhs.foodGroupId; }), foodLocations.end());

		if (foodLocations.size() >= currentGame.getPlayerCount()) {
			cords foodcords;
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
					foodFound = true;
					occupiedFoodSources.push_back(possibleFood);
					foodcords = foodLocations[possibleFood].foodCords;
				}
			}
			if (foodFound) {//TODO: this is always true
				return foodcords;
			}
			else {
				return { -1,-1 };
			}
		}
		else {
			//There is not enough food on the map!
			return { -1,-1 };
		}
	}

	bool spawnFirstVillager(int distanceFromFood, int teamId) {
		bool villagerIsPlaced = false;
		cords randomFoodSource = findRandomFoodSource();

		if (randomFoodSource.x != -1) {
			int tries = 0;
			while (!villagerIsPlaced && tries < 9000) {
				cords suggestedCords = { roll(randomFoodSource.x - distanceFromFood,randomFoodSource.x + distanceFromFood), roll(randomFoodSource.y - distanceFromFood, randomFoodSource.y + distanceFromFood) };
				if (suggestedCords.y + 1 < MAP_HEIGHT && suggestedCords.x + 1 < MAP_WIDTH) {
					if (currentGame.isPassable(suggestedCords.x, suggestedCords.y) && currentGame.isPassable(suggestedCords.x, suggestedCords.y + 1) && currentGame.isPassable(suggestedCords.x + 1, suggestedCords.y) && currentGame.isPassable(suggestedCords.x + 1, suggestedCords.y + 1))
					{
						listOfActors.push_back(actors(0, suggestedCords, teamId, static_cast<int>(listOfActors.size())));
						listOfActors.push_back(actors(0, { suggestedCords.x, suggestedCords.y + 1 }, teamId, static_cast<int>(listOfActors.size())));
						listOfActors.push_back(actors(0, { suggestedCords.x + 1, suggestedCords.y }, teamId, static_cast<int>(listOfActors.size())));
						listOfActors.push_back(actors(0, { suggestedCords.x + 1, suggestedCords.y + 1 }, teamId, static_cast<int>(listOfActors.size())));
						villagerIsPlaced = true;
					}
				}
				tries++;
			}
			if (villagerIsPlaced) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			//there is not enough food on the map!
			return false;
		}
	}

	void centerViewOnVillager() {
		for (int i = 0; i < listOfActors.size(); i++) {
			if (listOfActors[i].getTeam() == currentPlayer.getTeam()) {
				viewOffsetX = worldSpace(listOfActors[i].getActorCords().x, listOfActors[i].getActorCords().y, true);
				viewOffsetY = worldSpace(listOfActors[i].getActorCords().x, listOfActors[i].getActorCords().y, false);
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
		delete[]noiseMap;
		delete[]noiseSeed;
	}

}

void generateRandomMap(int players, int amountOfFoodGroups, int amountOfStoneGroups, int amountOfGoldGroups, int treeDensityLevel, int tries) {
	bool mapGenerationSuccefull = true;
	generateTerrain();
	switch (treeDensityLevel) {
	case 0:
		placeTrees(4, 200);
		break;
	case 1:
		placeTrees(3, 200);
		break;
	case 2:
		placeTrees(3, 150);
		break;
	case 3:
		placeTrees(2, 150);
		break;
	case 4:
		placeTrees(2, 100);
		break;
	case 5:
		placeTrees(1, 100);
		break;
	case 6:
		placeTrees(1, 75);
		break;
	case 7:
		placeTrees(1, 50);
		break;
	case 8:
		placeTrees(1, 25);
		break;
	case 9:
		placeTrees(1, 10);
		break;
	}
	
	if (!spawnFoodStoneGold(6, amountOfFoodGroups)) { mapGenerationSuccefull = false; std::cout << "map generation failed on food sources!" << std::endl; }
	for (int i = 0; i < players; i++) {
		if (!spawnFirstVillager(8, i)) { mapGenerationSuccefull = false; std::cout << "map generation failed not enough food groups for players" << std::endl;}
	}
	if (!spawnFoodStoneGold(4, amountOfStoneGroups)) { mapGenerationSuccefull = false; std::cout << "map generation failed on stone sources!" << std::endl;	}
	if (!spawnFoodStoneGold(5, amountOfGoldGroups)) { mapGenerationSuccefull = false; std::cout << "map generation failed on gold sources!" << std::endl; }
	centerViewOnVillager();
	if (!mapGenerationSuccefull && tries < 99) {
		tries++;
		generateRandomMap(players, amountOfFoodGroups, amountOfStoneGroups, amountOfGoldGroups, treeDensityLevel, tries);
	}
}