#include "randomMapGenerator.h"
#include "globalfunctions.h"
#include "gamestate.h"
#include <iostream>
#include "objects.h"
#include "actors.h"
#include "player.h"

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
				
				float blendX = (float)(x - sampleX1) / (float)pitch;
				float blendY = (float)(y - sampleY1) / (float)pitch;

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
			int depth = (int)(noisemap[y * MAP_WIDTH + x] * 3.0f);
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

int roll(int min, int max)
{
	return  min + (rand() % static_cast<int>(max - min + 1));
}

void placeTrees() {
	srand(time(NULL));
	for (int x = 0; x < MAP_WIDTH; x++)
	{
		for (int y = 0; y < MAP_HEIGHT; y++)
		{
			if (roll(0, 20) > 19) {
				if (currentGame.currentMap[x][y] == 1) {
					objects newObject(roll(1, 3), x, y, listOfObjects.size());
					listOfObjects.push_back(newObject);
				}
				else if (currentGame.currentMap[x][y] == 2)
				{
					objects newObject(0, x, y, listOfObjects.size());
					listOfObjects.push_back(newObject);
				}
			}
		}
	}
}

void spawmFoodStoneGold(int resource)
{
	int gridMinX=0;
	int gridMinY=0;
	for (int gridMaxX = 64; gridMaxX < 256; gridMaxX += 64) {
		for (int gridMaxY = 64; gridMaxY < 256; gridMaxY += 64) {
			bool resourcePlaced = false;
			int maxTries = 0;
			while (!resourcePlaced && maxTries < 100) {
				mouseWorldCord suggestedCords = { roll(gridMinX,gridMaxX), roll(gridMinY,gridMaxY) };
				if (suggestedCords.y - 1 >= 0 && suggestedCords.y + 1 < MAP_HEIGHT && suggestedCords.x + 1 < MAP_WIDTH) {
					if (currentGame.isPassable(suggestedCords.x, suggestedCords.y) && currentGame.isPassable(suggestedCords.x, suggestedCords.y + 1) && currentGame.isPassable(suggestedCords.x + 1, suggestedCords.y) && currentGame.isPassable(suggestedCords.x + 1, suggestedCords.y - 1))
					{
						objects newObject(resource, suggestedCords.x, suggestedCords.y, listOfObjects.size());
						listOfObjects.push_back(newObject);
						objects newObject1(resource, suggestedCords.x, suggestedCords.y+1, listOfObjects.size());
						listOfObjects.push_back(newObject1);
						objects newObject2(resource, suggestedCords.x+1, suggestedCords.y, listOfObjects.size());
						listOfObjects.push_back(newObject2);
						objects newObject3(resource, suggestedCords.x+1, suggestedCords.y-1, listOfObjects.size());
						listOfObjects.push_back(newObject3);
						resourcePlaced = true;
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

mouseWorldCord findRandomFoodSource() {
	bool foodFound = false;

	while (!foodFound) {
		int possibleFood = roll(0, listOfObjects.size());
		if (listOfObjects[possibleFood].getTypeOfResource() == 1) {
			bool foodSourceOccupied = false;
			for (const int& foodOwned : occupiedFoodSources)
			{
				if (foodOwned == possibleFood) {
					foodSourceOccupied = true;
				}
			}
			if (!foodSourceOccupied) {
				return  listOfObjects[possibleFood].getLocation();
			}
		}
	}
	return { 0,0 };
}

void spawmFirstVillager(int distanceFromFood, int teamId) {
	bool villagerIsPlaced = false;
	mouseWorldCord randomFoodSource = findRandomFoodSource();

	while (!villagerIsPlaced) {
		mouseWorldCord suggestedCords = { roll(randomFoodSource.x - distanceFromFood,randomFoodSource.x + distanceFromFood), roll(randomFoodSource.y - distanceFromFood, randomFoodSource.y + distanceFromFood) };
		if (suggestedCords.y + 1 < MAP_HEIGHT && suggestedCords.x + 1 < MAP_WIDTH) {
			if (currentGame.isPassable(suggestedCords.x, suggestedCords.y) && currentGame.isPassable(suggestedCords.x, suggestedCords.y + 1) && currentGame.isPassable(suggestedCords.x + 1, suggestedCords.y) && currentGame.isPassable(suggestedCords.x + 1, suggestedCords.y + 1))
			{
				listOfActorsMutex.lock();
				actors newActor(0, suggestedCords.x, suggestedCords.y, teamId, listOfActors.size());
				listOfActors.push_back(newActor);
				actors newActor1(0, suggestedCords.x, suggestedCords.y+1, teamId, listOfActors.size());
				listOfActors.push_back(newActor1);
				actors newActor2(0, suggestedCords.x+1, suggestedCords.y, teamId, listOfActors.size());
				listOfActors.push_back(newActor2);
				actors newActor3(0, suggestedCords.x+1, suggestedCords.y, teamId+1, listOfActors.size());
				listOfActors.push_back(newActor3);
				listOfActorsMutex.unlock();
				villagerIsPlaced = true;
			}
		}
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
	srand(time(NULL));
	float* noiseMap = nullptr;
	float* noiseSeed = nullptr;
	noiseSeed = new float[MAP_WIDTH * MAP_HEIGHT];
	noiseMap = new float[MAP_WIDTH * MAP_HEIGHT];
	for (int i = 0; i < MAP_WIDTH * MAP_HEIGHT; i++) {
		noiseSeed[i] = (float)rand() / (float)RAND_MAX;
	};
	generatePerlinNoise(1.4f, 5, noiseSeed, noiseMap);
	convertPerlinNoiseToMap(noiseMap);
	delete []noiseMap;
	delete []noiseSeed;
}

void generateRandomMap(int players) {
	generateTerrain();
	placeTrees();
	spawmFoodStoneGold(6);
	for (int i = 0; i < players; i++) {
		spawmFirstVillager(8, i);
	}
	spawmFoodStoneGold(4);
	spawmFoodStoneGold(5);
	centerViewOnVillager();
}