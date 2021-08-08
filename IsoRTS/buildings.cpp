#include <iostream>
#include "actors.h"
#include "buildings.h"
#include "gametext.h"
#include "player.h"
#include "projectile.h"

std::vector<actorOrBuildingPrice> priceOfBuilding;
std::vector<buildings>            listOfBuildings;
std::vector<footprintOfBuilding>  footprintOfBuildings;

namespace
{
    cords findCloseTile(const std::list<cords>& buidlingFootprint, const cords& target)
    {
        int lowestDist = 0;
        cords closeTile = { -1, -1 };
        for (const cords& footprintTile : buidlingFootprint) {
            if (distEuclidean(footprintTile.x, footprintTile.y, target.x, target.y) < lowestDist || lowestDist == 0) {
                closeTile = footprintTile;
                lowestDist = static_cast<int>(distEuclidean(footprintTile.x, footprintTile.y, target.x, target.y));
            }
        }
        return closeTile;
    }

    void addNeighboursOfImpassableNeighbours(int& i, std::vector<Cells>& cellsList, std::list<Cells*>& listToCheck)
    {
        if (cellsList[i].position.x > 0)
        {
            cellsList[i - MAP_HEIGHT].addNeighbours(cellsList);
            listToCheck.push_back(&cellsList[i - MAP_HEIGHT]);
        }
        if (cellsList[i].position.x < MAP_WIDTH - 1)
        {
            cellsList[i + MAP_HEIGHT].addNeighbours(cellsList);
            listToCheck.push_back(&cellsList[i + MAP_HEIGHT]);
        }
        if (cellsList[i].position.y > 0)
        {
            cellsList[i - 1].addNeighbours(cellsList);
            listToCheck.push_back(&cellsList[i - 1]);
        }
        if (cellsList[i].position.y != MAP_HEIGHT - 1)
        {
            cellsList[i + 1].addNeighbours(cellsList);
            listToCheck.push_back(&cellsList[i + 1]);
        }
        if (cellsList[i].position.y != MAP_HEIGHT - 1 && cellsList[i].position.x < MAP_WIDTH - 1)
        {
            cellsList[i + 1 + MAP_HEIGHT].addNeighbours(cellsList);
            listToCheck.push_back(&cellsList[i + 1 + MAP_HEIGHT]);
        }
        if (cellsList[i].position.y > 0 && cellsList[i].position.x < MAP_WIDTH - 1)
        {
            cellsList[i - 1 + MAP_HEIGHT].addNeighbours(cellsList);
            listToCheck.push_back(&cellsList[i - 1 + MAP_HEIGHT]);
        }
        if (cellsList[i].position.y != MAP_HEIGHT - 1 && cellsList[i].position.x > 0)
        {
            cellsList[i + 1 - MAP_HEIGHT].addNeighbours(cellsList);
            listToCheck.push_back(&cellsList[i + 1 - MAP_HEIGHT]);
        }
        if (cellsList[i].position.y > 0 && cellsList[i].position.x > 0)
        {
            cellsList[i - 1 - MAP_HEIGHT].addNeighbours(cellsList);
            listToCheck.push_back(&cellsList[i - 1 - MAP_HEIGHT]);
        }
    }

    cords findEmptySpot(cords startCords)
    {
        if (currentGame.isPassable(startCords))
        {
            return startCords;
        }
        else
        {
            cords foundCords;
            std::vector<Cells> cellsList;
            cellsList.reserve(MAP_HEIGHT * MAP_WIDTH);
            int startCell = (startCords.x * MAP_HEIGHT) + startCords.y;
            updateCells(-1, -1, cellsList);
            std::list<Cells*> listToCheck;
            cellsList[startCell].addNeighbours(cellsList);
            cellsList[startCell].visited = true;
            listToCheck.push_back(&cellsList[startCell]);
            bool freeCellFound = false;
            while (!freeCellFound && !listToCheck.empty())
            {
                for (std::vector<int>::const_iterator iterator = (*listToCheck.front()).neighbours.begin(), end = (*listToCheck.front()).neighbours.end(); iterator != end; ++iterator)
                {
                    freeCellFound = true;
                    foundCords = cellsList[*iterator].position;
                }
                if (!freeCellFound)
                {
                    addNeighboursOfImpassableNeighbours((*listToCheck.front()).cellId, cellsList, listToCheck);
                }
                listToCheck.pop_front();
            }
            if (freeCellFound) {
                return foundCords;
            }
            else {
                return { 0,0 };
            }
        }
    }

}


void buildings::fillAdjacentTiles()
{
    //The row above and below the building
    for(int i = this->endLocation.x-1; i <= this->startLocation.x+1; i++)
    {
        if(i >= 0 && i <= MAP_WIDTH)
        {
            int goalX;
            if(i == this->endLocation.x-1)
            {
                goalX = i +1;
            }
            else if(i == this->startLocation.x+1)
            {
                goalX = i -1;
            }
            else
            {
                goalX = i;
            }

            //adjacent tile above
            this->adjacentTiles.push_back({ static_cast<int>(this->adjacentTiles.size()), i, this->endLocation.y - 1, goalX, this->endLocation.y, false, -1 });
            
            //adjacent tile below
            this->adjacentTiles.push_back({ static_cast<int>(this->adjacentTiles.size()), i, this->startLocation.y + 1, goalX, this->startLocation.y, false, -1 });
        }
    }
    //The row left and right of the building
    for(int i =  this->startLocation.y-footprintOfBuildings[this->buildingType].amountOfYFootprint+1; i <=  this->startLocation.y; i++)
    {
        if(i >= 0 && i <= MAP_HEIGHT)
        {
            //new left tile
            this->adjacentTiles.push_back({ static_cast<int>(this->adjacentTiles.size()), this->endLocation.x - 1, i, this->endLocation.x, i, false, -1 });

            //new right tile
            this->adjacentTiles.push_back({ static_cast<int>(this->adjacentTiles.size()), this->startLocation.x + 1, i, this->startLocation.x, i, false, -1 });
        }
    }
}

void buildings::removeActorFromBuildingTile(int actorId)
{
    for(int i = 0; i < this->adjacentTiles.size(); i++)
    {
        if(this->adjacentTiles[i].actorId == actorId)
        {
            this->adjacentTiles[i].occupied = false;
            this->adjacentTiles[i].actorId = -1;
        }
    }

}

std::vector<adjacentTile> buildings::getFreeBuildingTile() const
{
    std::vector<adjacentTile> tileList;
    for(int i = 0; i < this->adjacentTiles.size(); i++)
    {
        if(!this->adjacentTiles[i].occupied && this->adjacentTiles[i].actorId == -1)
        {
            if(currentGame.isPassable(this->adjacentTiles[i].tileCords))
            {
                tileList.push_back(this->adjacentTiles[i]);
            }
        }
    }
    return tileList;
}

std::vector<adjacentTile> buildings::getDropOffTiles() const
{
    std::vector<adjacentTile> tileList;
    for(int i = 0; i < this->adjacentTiles.size(); i++)
    {
        if(currentGame.isPassable(this->adjacentTiles[i].tileCords))
        {
            if (this->adjacentTiles[i].occupied == false) {
                tileList.push_back(this->adjacentTiles[i]);
            }
        }
    }
    return tileList;
}

bool buildings::claimFreeBuiildingTile(int id, int actorId)
{
    if(this->adjacentTiles[id].actorId == -1)
    {
        this->adjacentTiles[id].actorId = actorId;
        this->adjacentTiles[id].occupied = true;
        return true;
    }
    else {
        return false;
    }
}

bool buildings::getCompleted() const
{
    return this->buildingCompleted;
}

void buildings::setCompleted()
{
    this->buildingCompleted = true;
    if (this->ownedByPlayer == currentPlayer.getTeam()) {
        gameText.addNewMessage("- Building completed! -", 0);
    }
    listOfPlayers[this->ownedByPlayer].addToPopulationRoom(this->supportsPopulationOf);
}

void buildings::removeBuilding()
{
    this->exists = false;
    currentGame.buildingLocationList[this->startLocation.x][this->startLocation.y] = -1;
    for(int i = 0; i < footprintOfBuildings[this->buildingType].amountOfXFootprint; i++)
    {
        for(int j = 0; j < footprintOfBuildings[this->buildingType].amountOfYFootprint; j++)
        {
            currentGame.occupiedByBuildingList[startLocation.x-i][startLocation.y-j] = -1;
        }
    }
    if(currentGame.buildingSelectedId == this->buildingId)
    {
        currentGame.buildingSelectedId = -1;
    }
    noNewBuildings = false;
}

buildings::buildings(int type, cords startLocation, int buildingId, int team)
{
    this->buildingType = type;
    this->startLocation = startLocation;
    this->endLocation = { this->startLocation.x - footprintOfBuildings[this->buildingType].amountOfXFootprint + 1, this->startLocation.y - footprintOfBuildings[this->buildingType].amountOfYFootprint + 1 };
    this->buildingId = buildingId;
    this->ownedByPlayer = team;
    this->buildingCompleted = false;
    this->exists = true;
    this->lastShotFired = 0.0f;
    this->rallyPoint = { {0,0}, stackOrderTypes::stackActionMove, false }; //set dummy values for the rally point
    this->lastFrameUpdate = currentGame.getTime();
    this->hasDisplayedError = false;
    currentGame.buildingLocationList[startLocation.x][startLocation.y] = buildingId;
    for(int i = 0; i < footprintOfBuildings[type].amountOfXFootprint; i++)
    {
        for(int j = 0; j < footprintOfBuildings[type].amountOfYFootprint; j++)
        {
            currentGame.occupiedByBuildingList[startLocation.x-i][startLocation.y-j] = buildingId;
        }
    }
    noNewBuildings = false;
    switch(type)
    {
    case 0:
        //house
        hitPointsTotal = 450;
        hitPointsLeft = 450;
        canDoRangedDamage = false;
        amountOfRangedDamage = 0;
        range = 0;
        recievesWood = false;
        recievesStone= false;
        recievesGold = false;
        recievesFood = false;
        buildingPointsNeeded = 25;
        buildingPointsRecieved = 0;
        supportsPopulationOf = 5;
        this->offSetYStore = 1;
        this->amountOfAnimationSprites = 0;
        break;
    case 1:
        //towncenter
        hitPointsTotal = 5000;
        hitPointsLeft = 5000;
        canDoRangedDamage = true;
        amountOfRangedDamage = 10;
        range = 4;
        recievesWood = true;
        recievesStone= true;
        recievesGold = true;
        recievesFood = true;
        buildingPointsNeeded = 150;
        buildingPointsRecieved = 0;
        supportsPopulationOf = 5;
        this->offSetYStore = 1;
        this->amountOfAnimationSprites = 0;
        break;
    case 2:
        //Mill
        hitPointsTotal = 350;
        hitPointsLeft = 350;
        canDoRangedDamage = false;
        amountOfRangedDamage = 0;
        range = 0;
        recievesWood = false;
        recievesStone = false;
        recievesGold = false;
        recievesFood = true;
        buildingPointsNeeded = 25;
        buildingPointsRecieved = 0;
        supportsPopulationOf = 0;
        this->offSetYStore = 1;
        this->amountOfAnimationSprites = 8;
        break;
    case 3:
        //Lumbercamp
        hitPointsTotal = 250;
        hitPointsLeft = 250;
        canDoRangedDamage = false;
        amountOfRangedDamage = 0;
        range = 0;
        recievesWood = true;
        recievesStone = false;
        recievesGold = false;
        recievesFood = false;
        buildingPointsNeeded = 25;
        buildingPointsRecieved = 0;
        supportsPopulationOf = 0;
        this->offSetYStore = 1;
        this->amountOfAnimationSprites = 8;
        break;
    case 4:
        //Barracks
        hitPointsTotal = 500;
        hitPointsLeft = 500;
        canDoRangedDamage = false;
        amountOfRangedDamage = 0;
        range = 0;
        recievesWood = false;
        recievesStone = false;
        recievesGold = false;
        recievesFood = false;
        buildingPointsNeeded = 50;
        buildingPointsRecieved = 0;
        supportsPopulationOf = 0;
        this->offSetYStore = 1;
        this->amountOfAnimationSprites = 0;
        break;
    case 5:
        //MiningCampStone
        hitPointsTotal = 250;
        hitPointsLeft = 250;
        canDoRangedDamage = false;
        amountOfRangedDamage = 0;
        range = 0;
        recievesWood = false;
        recievesStone = true;
        recievesGold = true;
        recievesFood = false;
        buildingPointsNeeded = 25;
        buildingPointsRecieved = 0;
        supportsPopulationOf = 0;
        this->offSetYStore = 1;
        this->amountOfAnimationSprites = 8;
        break;
    case 6:
        //MiningCampGold
        hitPointsTotal = 250;
        hitPointsLeft = 250;
        canDoRangedDamage = false;
        amountOfRangedDamage = 0;
        range = 0;
        recievesWood = false;
        recievesStone = true;
        recievesGold = true;
        recievesFood = false;
        buildingPointsNeeded = 25;
        buildingPointsRecieved = 0;
        supportsPopulationOf = 0;
        this->offSetYStore = 1;
        this->amountOfAnimationSprites = 8;
        break;
    }
    fillAdjacentTiles();
}

int buildings::getTeam() const
{
    return this->ownedByPlayer;
}

int buildings::getType() const
{
    return this->buildingType;
}

cords buildings::getLocation() const
{
    return this->startLocation;
}

int buildings::getBuildingId() const
{
    return this->buildingId;
}

resourceTypes buildings::getRecievesWhichResources() const
{
    if(recievesFood && recievesGold && recievesStone && recievesWood)
    {
        return resourceTypes::All;
    }
    else if(recievesGold)
    {
        return resourceTypes::resourceGold;
    }
    else if(recievesStone)
    {
        return resourceTypes::resourceStone;
    }
    else if(recievesFood)
    {
        return resourceTypes::resourceFood;
    }
    else if(recievesWood)
    {
        return resourceTypes::resourceWood;
    }
    else
    {
        return resourceTypes::None;
    }
}

void buildings::addBuildingPoint()
{
    this->buildingPointsRecieved += 1;
}

void buildings::drawBuilding(int i, int j, int type, bool typeOverride)
{
    int transparant;
    int offsetY;
    if(!typeOverride)
    {
        type = this->buildingType;
        if(this->buildingCompleted)
        {
            offsetY = 1;
            if(this->amountOfAnimationSprites > 0)
            {
                offsetY = this->offSetYStore;
                if (this->lastFrameUpdate + 0.2 < currentGame.getTime()) {
                    this->lastFrameUpdate = currentGame.getTime();
                    offsetY++;
                }
                if(offsetY > amountOfAnimationSprites)
                {
                    offsetY = 1;
                }
                this->offSetYStore = offsetY;
            }
        }
        else
        {
            offsetY = 0;
        }
        transparant = 255;
    }
    else
    {
        offsetY = 1;
        transparant = 128;
    }
    switch(type)
    {
    case 0:
        currentGame.spriteBuildingHouse.setTextureRect(sf::IntRect(0, currentGame.spriteBuildingHouse.getTextureRect().height*offsetY, currentGame.spriteBuildingHouse.getTextureRect().width, currentGame.spriteBuildingHouse.getTextureRect().height));
        currentGame.spriteBuildingHouse.setPosition(static_cast<float>(worldSpace({ i,j }).x), static_cast<float>(worldSpace({ i, j }).y));
        currentGame.spriteBuildingHouse.setColor(sf::Color(255, 255, 255, transparant));
        window.draw(currentGame.spriteBuildingHouse);
        break;
    case 1:
        currentGame.spriteTownCenter.setTextureRect(sf::IntRect(0, currentGame.spriteTownCenter.getTextureRect().height*offsetY, currentGame.spriteTownCenter.getTextureRect().width, currentGame.spriteTownCenter.getTextureRect().height));
        currentGame.spriteTownCenter.setPosition(static_cast<float>(worldSpace({ i,j }).x), static_cast<float>(worldSpace({ i, j }).y));
        currentGame.spriteTownCenter.setColor(sf::Color(255, 255, 255, transparant));
        window.draw(currentGame.spriteTownCenter);
        break;
    case 2:
        currentGame.spriteBuildingMill.setTextureRect(sf::IntRect(0, currentGame.spriteBuildingMill.getTextureRect().height * offsetY, currentGame.spriteBuildingMill.getTextureRect().width, currentGame.spriteBuildingMill.getTextureRect().height));
        currentGame.spriteBuildingMill.setPosition(static_cast<float>(worldSpace({ i,j }).x), static_cast<float>(worldSpace({ i, j }).y));
        currentGame.spriteBuildingMill.setColor(sf::Color(255, 255, 255, transparant));
        window.draw(currentGame.spriteBuildingMill);
        break;
    case 3:
        currentGame.spriteBuildingLumberCamp.setTextureRect(sf::IntRect(0, currentGame.spriteBuildingLumberCamp.getTextureRect().height * offsetY, currentGame.spriteBuildingLumberCamp.getTextureRect().width, currentGame.spriteBuildingLumberCamp.getTextureRect().height));
        currentGame.spriteBuildingLumberCamp.setPosition(static_cast<float>(worldSpace({ i,j }).x), static_cast<float>(worldSpace({ i, j }).y));
        currentGame.spriteBuildingLumberCamp.setColor(sf::Color(255, 255, 255, transparant));
        window.draw(currentGame.spriteBuildingLumberCamp);
        break;
    case 4:
        currentGame.spriteBuildingBarracks.setTextureRect(sf::IntRect(0, currentGame.spriteBuildingBarracks.getTextureRect().height * offsetY, currentGame.spriteBuildingBarracks.getTextureRect().width, currentGame.spriteBuildingBarracks.getTextureRect().height));
        currentGame.spriteBuildingBarracks.setPosition(static_cast<float>(worldSpace({ i,j }).x), static_cast<float>(worldSpace({ i, j }).y));
        currentGame.spriteBuildingBarracks.setColor(sf::Color(255, 255, 255, transparant));
        window.draw(currentGame.spriteBuildingBarracks);
        break;
    case 5:
        currentGame.spriteBuildingMiningCamp.setTextureRect(sf::IntRect(0, currentGame.spriteBuildingMiningCamp.getTextureRect().height * offsetY, currentGame.spriteBuildingMiningCamp.getTextureRect().width, currentGame.spriteBuildingMiningCamp.getTextureRect().height));
        currentGame.spriteBuildingMiningCamp.setPosition(static_cast<float>(worldSpace({ i,j }).x), static_cast<float>(worldSpace({ i, j }).y));
        currentGame.spriteBuildingMiningCamp.setColor(sf::Color(255, 255, 255, transparant));
        window.draw(currentGame.spriteBuildingMiningCamp);
    case 6:
        currentGame.spriteBuildingMiningCamp.setTextureRect(sf::IntRect(0, currentGame.spriteBuildingMiningCamp.getTextureRect().height * offsetY, currentGame.spriteBuildingMiningCamp.getTextureRect().width, currentGame.spriteBuildingMiningCamp.getTextureRect().height));
        currentGame.spriteBuildingMiningCamp.setPosition(static_cast<float>(worldSpace({ i,j }).x), static_cast<float>(worldSpace({ i, j }).y));
        currentGame.spriteBuildingMiningCamp.setColor(sf::Color(255, 255, 255, transparant));
        window.draw(currentGame.spriteBuildingMiningCamp);
    }

    //Redraw possible overdrawn sprites
    if(!typeOverride)
    {
        for(int k = 1; k < 4; k++)
        {
            for(int y = 1; y < footprintOfBuildings[this->buildingType].amountOfYFootprint; y++)
            {
                if (currentGame.visability[i + k][j - y] > 0) {
                    currentGame.drawThingsOnTile(i + k, j - y);
                }
            }
        }
    }

    //Draw rally point if set
    if (currentGame.buildingIsSelected(this->buildingId)) {
        if (this->rallyPoint.isSet) {
            currentGame.spriteFlag.setPosition(static_cast<float>(worldSpace(this->rallyPoint.goal).x), static_cast<float>(worldSpace(this->rallyPoint.goal).y));
            window.draw(currentGame.spriteFlag);
        }
    }

    if (currentGame.showPaths) {
        //draw adjecent tiles
        for (adjacentTile const &tile : adjacentTiles) {
            if (tile.occupied) {
                currentGame.spriteTileObstructed.setPosition(static_cast<float>(worldSpace(tile.tileCords).x), static_cast<float>(worldSpace(tile.tileCords).y));
                window.draw(currentGame.spriteTileObstructed);
            }
            else {
                currentGame.spriteSelectedTileForPath.setPosition(static_cast<float>(worldSpace(tile.tileCords).x), static_cast<float>(worldSpace(tile.tileCords).y));
                window.draw(currentGame.spriteSelectedTileForPath);
            }
            currentGame.spriteUnitSelectedTile.setPosition(static_cast<float>(worldSpace(tile.goal).x), static_cast<float>(worldSpace(tile.goal).y));
            window.draw(currentGame.spriteUnitSelectedTile);
        }
    }
}

std::string buildings::getName() const
{
    switch(this->buildingType)
    {
    case 0:
        return "House";
    case 1:
        return "Town Center";
    case 2:
        return "Mill";
    case 3:
        return "Lumber Camp";
    case 4:
        return "Barracks";
    case 5:
        return "Mining Camp";
    default: 
        return "Building Name Label";
    }

}

std::pair<int, int> buildings::getHealth() const
{
    return {this->hitPointsLeft, this->hitPointsTotal};
}

std::pair<int, int> buildings::getBuildingPoints() const
{
    return {this->buildingPointsRecieved, this->buildingPointsNeeded};
}

int buildings::getRangedDMG() const
{
    return this->amountOfRangedDamage;
}

void buildings::setRallyPoint(cords goal, stackOrderTypes orderType)
{
    this->rallyPoint = { goal, orderType, true };
}

void buildings::drawBuildingFootprint(int type, cords mouseWorld)
{
    if(!(mouseWorld.x-footprintOfBuildings[0].amountOfXFootprint < -1) && !(mouseWorld.y-footprintOfBuildings[0].amountOfYFootprint < -1) && !(mouseWorld.x >= MAP_WIDTH) && !(mouseWorld.y >= MAP_HEIGHT))
    {
        drawBuilding(mouseWorld.x, mouseWorld.y, type, true);
        for(int i = 0; i < footprintOfBuildings[type].amountOfXFootprint; i++)
        {
            for(int j = 0; j < footprintOfBuildings[type].amountOfYFootprint; j++)
            {
                if(
                    currentGame.occupiedByBuildingList[mouseWorld.x-i][mouseWorld.y-j] != -1 ||
                    currentGame.objectLocationList[mouseWorld.x - i][mouseWorld.y - j] != -1 ||
                    currentGame.occupiedByActorList[mouseWorld.x - i][mouseWorld.y - j] != -1 ||
                    currentGame.currentMap[mouseWorld.x - i][mouseWorld.y - j] == 7
                    )
                {
                    currentGame.drawMousePosition(mouseWorld.x-i, mouseWorld.y-j, false);
                }
                else
                {
                    currentGame.drawMousePosition(mouseWorld.x-i, mouseWorld.y-j, true);
                }
            }
        }
    }
}

void buildings::getTask(bool isResearch, int idOfUnitOrResearch)
{
    if(this->productionQueue.size() < 5 )
    {
        if (!isResearch) {
            this->productionQueue.push_back({isResearch, idOfUnitOrResearch, 0, priceOfActor[idOfUnitOrResearch].productionPoints, 0});
            listOfPlayers[ownedByPlayer].substractResources(resourceTypes::resourceFood, priceOfActor[idOfUnitOrResearch].food);
            listOfPlayers[ownedByPlayer].substractResources(resourceTypes::resourceWood, priceOfActor[idOfUnitOrResearch].wood);
            listOfPlayers[ownedByPlayer].substractResources(resourceTypes::resourceStone, priceOfActor[idOfUnitOrResearch].stone);
            listOfPlayers[ownedByPlayer].substractResources(resourceTypes::resourceGold, priceOfActor[idOfUnitOrResearch].gold);
        }
    }
    else
    {
        if (currentPlayer.getTeam() == ownedByPlayer) {
            gameText.addNewMessage("No room in building queue for production...", 1);
        }
    }
}

bool buildings::hasTask() const
{
    return !this->productionQueue.empty();
}

std::list<cords> buildings::getFootprintOfBuilding() const {
    std::list<cords> tempList;
    for (int x = this->startLocation.x; x > this->startLocation.x - footprintOfBuildings[this->buildingType].amountOfXFootprint; x--) {
        for (int y = this->startLocation.y; y > this->startLocation.y - footprintOfBuildings[this->buildingType].amountOfYFootprint; y--) {
            tempList.push_back({ x, y });
        }
    }
    return tempList;
}

void buildings::takeDamage(int amountOfDamage)
{
    this->hitPointsLeft -= amountOfDamage;
    if (this->hitPointsLeft <= 0) {
        this->removeBuilding();
    }
}

void buildings::spawnProduce()
{
    cords spawnCords = findEmptySpot({ this->startLocation.x + 1, this->startLocation.y + 1 });
    if (currentPlayer.getStats().currentPopulation < currentPlayer.getStats().populationRoom)
    {
        actors newActor(this->productionQueue.front().idOfUnitOrResearch, spawnCords, this->ownedByPlayer, static_cast<int>(listOfActors.size()));
        listOfActors.push_back(newActor);
        if (this->rallyPoint.isSet) {
            listOfActors[newActor.getActorId()].stackOrder(this->rallyPoint.goal, this->rallyPoint.orderType); //Puts rally point order in command stackList of new unit
        }
        if (this->ownedByPlayer == currentPlayer.getTeam()) {
            gameText.addNewMessage("-  " + newActor.nameOfActor() + " completed! -", 0);
        }
        this->productionQueue.erase(productionQueue.begin());
        this->hasDisplayedError = false;
    }
    else
    {
        if (!this->hasDisplayedError)
        {
            if (this->ownedByPlayer == currentPlayer.getTeam()) {
                gameText.addNewMessage("Not enough population room to add more units, build more houses!", 1);
                this->hasDisplayedError = true;
            }
        }
    }
}

void::buildings::doProduction()
{
    if (this->productionQueue.front().lastTimeUpdate + 1 < currentGame.elapsedTime)
    {
        if (this->productionQueue.front().productionPointsGained >= this->productionQueue.front().productionPointsNeeded)
        {
            if (!this->productionQueue.front().isResearch)
            {
                spawnProduce();
            }
            else
            {
                //research things do ehh TBI
                this->productionQueue.erase(productionQueue.begin());
            }
        }
        else
        {
            this->productionQueue.front().productionPointsGained += 1;
            this->productionQueue.front().lastTimeUpdate = currentGame.elapsedTime;
            this->hasDisplayedError = false;
        }
    }

}

void buildings::checkOnEnemyAndShoot()
{
    if (lastShotFired + 2.f < currentGame.getTime()) {
        lastShotFired = currentGame.getTime();
        std::list<cords> buidlingFootprint = this->getFootprintOfBuilding();
        for (const cords& footprintTile : buidlingFootprint) {
            std::list<cords> tempList = getListOfCordsInCircle(footprintTile.x, footprintTile.y, this->range);
            for (const cords& cord : tempList)
            {
                if (currentGame.occupiedByActorList[cord.x][cord.y] != -1)
                {
                    if (listOfActors[currentGame.occupiedByActorList[cord.x][cord.y]].getTeam() != this->ownedByPlayer)
                    {
                        cords sourceTile = findCloseTile(buidlingFootprint, cord);
                        listOfProjectiles.push_back(projectile(sourceTile.x, sourceTile.y, cord.x, cord.y, 0, this->amountOfRangedDamage, 0, -this->buildingId));
                        return;
                    }
                }
            }
        }
    }
}

void buildings::update()
{
    if(this->exists)
    {
        if(this->buildingCompleted)
        {
            if(!this->productionQueue.empty())
            {
                doProduction();
            }
            if (this->canDoRangedDamage) {
                checkOnEnemyAndShoot();
            }
        }
        else if(this->buildingPointsNeeded <= this->buildingPointsRecieved)
        {
            this->setCompleted();
        }
    }
}
