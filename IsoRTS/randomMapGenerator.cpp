#include <iostream>
#include "actors.h"
#include "humanReadableNames.h"
#include "objects.h"
#include "player.h"
#include "randomMapGenerator.h"
#include <bitset>

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
				case 0: currentGame.currentMap[x][y] = 3; break;
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
			listOfObjects.push_back(objects(objectTypes::cactus, { x, y }, static_cast<int>(listOfObjects.size())));
		}
	}

	bool neighbourHasTrees(int x, int y) {
		for (int i = x - 1; i < x + 1; i++) {
			for (int j = y - 1; j < y + 1; j++) {
				if (i >= 0 && i < MAP_WIDTH && j >= 0 && j < MAP_HEIGHT) {
					if (currentGame.objectLocationList[i][j] != -1) {
						if (
							listOfObjects[currentGame.objectLocationList[i][j]].getType() == objectTypes::cypress ||
							listOfObjects[currentGame.objectLocationList[i][j]].getType() == objectTypes::cactus ||
							listOfObjects[currentGame.objectLocationList[i][j]].getType() == objectTypes::maple ||
							listOfObjects[currentGame.objectLocationList[i][j]].getType() == objectTypes::pine
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

	bool spawnFoodStoneGold(int resource, int amountOfGroups) {
		//Devide the map into grids of 32x32
		struct gridTile{
			cords start;
			cords end;
			bool hasThisResourceGroup;
		};
		std::vector<gridTile> mapGrid;
		int gridMinX = 0;
		int gridMinY = 0;
		for (int gridMaxX = 32; gridMaxX < MAP_WIDTH; gridMaxX += 32) {
			for (int gridMaxY = 32; gridMaxY < MAP_HEIGHT; gridMaxY += 32) {
				mapGrid.push_back({ { gridMinX, gridMinY }, { gridMaxX, gridMaxY }, false });
				gridMinY = gridMaxY;
			}
			gridMinX = gridMaxX;
		}

		int amountOfGroupsPlaced = 0;
		int tries = 0;
		std::default_random_engine gen(roll(0,99));

		//While not all resources are placed shuffle the grids and try to place on the last one. If placed remove grid from the list
		while (amountOfGroupsPlaced < amountOfGroups && tries < 9000 && mapGrid.size() > 0 ) {
			std::shuffle(mapGrid.begin(), mapGrid.end(), gen);
			cords suggestedCords = { roll(mapGrid.back().start.x,mapGrid.back().end.x), roll(mapGrid.back().start.y,mapGrid.back().end.y) };
			if (suggestedCords.y - 1 >= 0 && suggestedCords.y + 1 < MAP_HEIGHT && suggestedCords.x + 1 < MAP_WIDTH) {
				if (currentGame.isPassable(suggestedCords) && currentGame.isPassable({ suggestedCords.x, suggestedCords.y + 1 }) && currentGame.isPassable({ suggestedCords.x + 1, suggestedCords.y }) && currentGame.isPassable({ suggestedCords.x + 1, suggestedCords.y - 1 }))
				{
					listOfObjects.push_back(objects(static_cast<objectTypes>(resource), suggestedCords, static_cast<int>(listOfObjects.size())));
					listOfObjects.push_back(objects(static_cast<objectTypes>(resource), { suggestedCords.x, suggestedCords.y + 1 }, static_cast<int>(listOfObjects.size())));
					listOfObjects.push_back(objects(static_cast<objectTypes>(resource), { suggestedCords.x + 1, suggestedCords.y }, static_cast<int>(listOfObjects.size())));
					listOfObjects.push_back(objects(static_cast<objectTypes>(resource), { suggestedCords.x + 1, suggestedCords.y - 1 }, static_cast<int>(listOfObjects.size())));
					amountOfGroupsPlaced++;
					mapGrid.pop_back();
				}
			}
			tries++;
		}

		//Check if resource placement was correct, otherwise raise the alarm.
		if (amountOfGroupsPlaced < amountOfGroups) {
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
				int possibleFood = roll(0, static_cast<int>(foodLocations.size()-1));
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
			return foodcords;
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
					if (currentGame.isPassable(suggestedCords) && currentGame.isPassable({ suggestedCords.x, suggestedCords.y + 1 }) && currentGame.isPassable({ suggestedCords.x + 1, suggestedCords.y }) && currentGame.isPassable({ suggestedCords.x + 1, suggestedCords.y + 1 }))
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
				viewOffsetX = worldSpace(listOfActors[i].getActorCords()).x;
				viewOffsetY = worldSpace(listOfActors[i].getActorCords()).y;
			}
		}
	}

	void generateTerrain() {
		srand(static_cast<int>(time(NULL)));
		//Moved noiseMap and noiseSeed from stack to heap because of size
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

void smoothSand(int x, int y) { //This function is really convoluted there is probably a better way

	std::bitset<8> tileByte;
		/* We have created the following square:
	*       x
	*     0 1 2      Where		0 = N
	*   y 3   4					1 = NE
	*     5 6 7                 2 = E
	*							3 = NW
	*							4 = W
	*	   0				    5 = SW
	* y	 3	1	x				6 = S
	*	5	 2					7 = SE
	*    6  4
	*     7
	*
	* There are 256 possible tiles. We should take action when sand is bordering a grass tile
	* Water bordering land is handeld by the water tile to keep it simple
	
	*/
	int tileByteIndex = 0;
	for (int yOffset = y - 1; yOffset < y + 2; yOffset++) {
		for (int xOffset = x - 1; xOffset < x + 2; xOffset++) {
			if (!(xOffset == x && yOffset == y)) {
				if (xOffset >= 0 && xOffset < MAP_WIDTH && yOffset >= 0 && yOffset < MAP_HEIGHT) {
					if (currentGame.currentMap[xOffset][yOffset] == 1) {
						tileByte[tileByteIndex] = 1;
					}
					else {
						tileByte[tileByteIndex] = 0;
					}
				}
				else {
					tileByte[tileByteIndex] = 0;
				}

				tileByteIndex++;
			}
		}
	}
	currentGame.tileBitmask[x][y] = tileByte.to_ulong();
}

void smoothWater(int x, int y) {
	std::bitset<8> tileByte;
	int tileByteIndex = 0;
	for (int yOffset = y - 1; yOffset < y + 2; yOffset++) {
		for (int xOffset = x - 1; xOffset < x + 2; xOffset++) {
			if (!(xOffset == x && yOffset == y)) {
				if (xOffset >= 0 && xOffset < MAP_WIDTH && yOffset >= 0 && yOffset < MAP_HEIGHT) {
					if (currentGame.currentMap[xOffset][yOffset] == 1 || currentGame.currentMap[xOffset][yOffset] == 2) {
						tileByte[tileByteIndex] = 1;
					}
					else {
						tileByte[tileByteIndex] = 0;
					}
				}
				else {
					tileByte[tileByteIndex] = 0;
				}

				tileByteIndex++;
			}
		}
	}

	currentGame.tileBitmask[x][y] = tileByte.to_ulong();
}

void createBeaches(int x, int y) {
	std::bitset<8> tileByte;
	int tileByteIndex = 0;
	for (int yOffset = y - 1; yOffset < y + 2; yOffset++) {
		for (int xOffset = x - 1; xOffset < x + 2; xOffset++) {
			if (!(xOffset == x && yOffset == y)) {
				if (xOffset >= 0 && xOffset < MAP_WIDTH && yOffset >= 0 && yOffset < MAP_HEIGHT) {
					if (currentGame.currentMap[xOffset][yOffset] == 3) {
						tileByte[tileByteIndex] = 1;
					}
					else {
						tileByte[tileByteIndex] = 0;
					}
				}
				else {
					tileByte[tileByteIndex] = 0;
				}

				tileByteIndex++;
			}
		}
	}
	currentGame.tileBitmask[x][y] = tileByte.to_ulong();
}


void smoothTerrain() {
	for (int i = 0; i < MAP_WIDTH; i++) {
		for (int j = 0; j < MAP_HEIGHT; j++) {
			if (currentGame.currentMap[i][j] == 1) {
				createBeaches(i, j);
			} else if (currentGame.currentMap[i][j] == 2) {
				smoothSand(i, j);
			}
			else if (currentGame.currentMap[i][j] == 3) {
				smoothWater(i, j);
			}
		}
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
	if (!mapGenerationSuccefull && tries < 1024) {
		tries++;
		generateRandomMap(players, amountOfFoodGroups, amountOfStoneGroups, amountOfGoldGroups, treeDensityLevel, tries);
	}
	else {
		smoothTerrain();
	}


}