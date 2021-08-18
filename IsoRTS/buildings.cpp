#include <iostream>
#include "actors.h"
#include "buildings.h"
#include "gametext.h"
#include "player.h"
#include "projectile.h"

std::vector<buildings>            listOfBuildings;

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
            updateCells(-1, -1, cellsList,true);
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
    for(int i =  this->startLocation.y- listOfBuildingTemplates[this->buildingType].getBuildingFootprint().amountOfYFootprint+1; i <=  this->startLocation.y; i++)
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
    for(int i = 0; i < listOfBuildingTemplates[this->buildingType].getBuildingFootprint().amountOfXFootprint; i++)
    {
        for(int j = 0; j < listOfBuildingTemplates[this->buildingType].getBuildingFootprint().amountOfYFootprint; j++)
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
    this->endLocation = { this->startLocation.x - listOfBuildingTemplates[this->buildingType].getBuildingFootprint().amountOfXFootprint + 1, this->startLocation.y - listOfBuildingTemplates[this->buildingType].getBuildingFootprint().amountOfYFootprint + 1 };
    this->buildingId = buildingId;
    this->ownedByPlayer = team;
    this->buildingCompleted = false;
    this->exists = true;
    this->lastShotFired = 0.0f;
    this->rallyPoint = { {0,0}, stackOrderTypes::stackActionMove, false }; //set dummy values for the rally point
    this->lastFrameUpdate = currentGame.getTime();
    this->hasDisplayedError = false;
    currentGame.buildingLocationList[startLocation.x][startLocation.y] = buildingId;
    for(int i = 0; i < listOfBuildingTemplates[type].getBuildingFootprint().amountOfXFootprint; i++)
    {
        for(int j = 0; j < listOfBuildingTemplates[type].getBuildingFootprint().amountOfYFootprint; j++)
        {
            currentGame.occupiedByBuildingList[startLocation.x-i][startLocation.y-j] = buildingId;
        }
    }
    this->buildingPointsRecieved = 0;
    this->hitPointsTotal = listOfBuildingTemplates[type].getHitPointsTotal();
    this->hitPointsLeft = listOfBuildingTemplates[type].getHitPointsTotal();
    this->canDoRangedDamage = listOfBuildingTemplates[type].getCanDoRangedDamage();
    this->amountOfRangedDamage = listOfBuildingTemplates[type].getAmountOfRangedDamage();
    this->range = listOfBuildingTemplates[type].getRange();
    this->recievesWood = listOfBuildingTemplates[type].getRecievesWood();
    this->recievesStone = listOfBuildingTemplates[type].getRecievesStone();
    this->recievesGold = listOfBuildingTemplates[type].getRecievesGold();
    this->recievesFood = listOfBuildingTemplates[type].getRecievesFood();
    this->buildingPointsNeeded = listOfBuildingTemplates[type].getBuildingPointsNeeded();
    this->supportsPopulationOf = listOfBuildingTemplates[type].getSupportsPopulationOf();
    this->offSetYStore = listOfBuildingTemplates[type].getOffSetYStore();
    this->amountOfAnimationSprites = listOfBuildingTemplates[type].getAmountOfAnimationSprites();
    this->buildingName = listOfBuildingTemplates[type].getBuildingName();
    fillAdjacentTiles();
    noNewBuildings = false;
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

    listOfBuildingTemplates[type].getBuildingSprite().setTextureRect(sf::IntRect(0, listOfBuildingTemplates[type].getBuildingSprite().getTextureRect().height * offsetY, listOfBuildingTemplates[type].getBuildingSprite().getTextureRect().width, listOfBuildingTemplates[type].getBuildingSprite().getTextureRect().height));
    listOfBuildingTemplates[type].setSpritePosition(worldSpace({ i,j }));
    listOfBuildingTemplates[type].getBuildingSprite().setColor(sf::Color(255, 255, 255, transparant));
    window.draw(listOfBuildingTemplates[type].getBuildingSprite());

    //Redraw possible overdrawn sprites
    if(!typeOverride)
    {
        for(int k = 1; k < 4; k++)
        {
            for(int y = 1; y < listOfBuildingTemplates[this->buildingType].getBuildingFootprint().amountOfYFootprint; y++)
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
    return this->buildingName;
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
    if (!listOfBuildingTemplates[type].getIsWall() || currentGame.getFirstWallClick().x == -1) {
        if (!(mouseWorld.x - listOfBuildingTemplates[type].getBuildingFootprint().amountOfXFootprint < -1) && !(mouseWorld.y - listOfBuildingTemplates[type].getBuildingFootprint().amountOfYFootprint < -1) && !(mouseWorld.x >= MAP_WIDTH) && !(mouseWorld.y >= MAP_HEIGHT))
        {
            drawBuilding(mouseWorld.x, mouseWorld.y, type, true);
            for (int i = 0; i < listOfBuildingTemplates[type].getBuildingFootprint().amountOfXFootprint; i++)
            {
                for (int j = 0; j < listOfBuildingTemplates[type].getBuildingFootprint().amountOfYFootprint; j++)
                {
                    if (
                        currentGame.occupiedByBuildingList[mouseWorld.x - i][mouseWorld.y - j] != -1 ||
                        currentGame.objectLocationList[mouseWorld.x - i][mouseWorld.y - j] != -1 ||
                        (!currentGame.occupiedByActorList[mouseWorld.x - i][mouseWorld.y - j].empty()) ||
                        currentGame.currentMap[mouseWorld.x - i][mouseWorld.y - j] == 7
                        )
                    {
                        currentGame.drawMousePosition(mouseWorld.x - i, mouseWorld.y - j, false);
                    }
                    else
                    {
                        currentGame.drawMousePosition(mouseWorld.x - i, mouseWorld.y - j, true);
                    }
                }
            }
        }
    }
    else {
        std::list<cords> listOfWallTilesToDraw = bresenham(mouseWorld, currentGame.getFirstWallClick());
        for (cords& wallTile : listOfWallTilesToDraw) {
            drawBuilding(wallTile.x, wallTile.y, type, true);
            if (wallTile.x > 0 && wallTile.x < MAP_WIDTH && wallTile.y >0 && wallTile.y < MAP_HEIGHT) {
                if (
                    currentGame.occupiedByBuildingList[wallTile.x][wallTile.y] != -1 ||
                    currentGame.objectLocationList[wallTile.x][wallTile.y] != -1 ||
                    (!currentGame.occupiedByActorList[wallTile.x][wallTile.y].empty()) ||
                    currentGame.currentMap[wallTile.x][wallTile.y] == 7
                    ) {
                    currentGame.drawMousePosition(wallTile.x, wallTile.y, false);
                }
                else {
                    currentGame.drawMousePosition(wallTile.x, wallTile.y, true);
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
            this->productionQueue.push_back({isResearch, idOfUnitOrResearch, 0, listOfActorTemplates[idOfUnitOrResearch].getPriceOfActor().productionPoints, 0});
            listOfPlayers[ownedByPlayer].substractResources(resourceTypes::resourceFood, listOfActorTemplates[idOfUnitOrResearch].getPriceOfActor().food);
            listOfPlayers[ownedByPlayer].substractResources(resourceTypes::resourceWood, listOfActorTemplates[idOfUnitOrResearch].getPriceOfActor().wood);
            listOfPlayers[ownedByPlayer].substractResources(resourceTypes::resourceStone, listOfActorTemplates[idOfUnitOrResearch].getPriceOfActor().stone);
            listOfPlayers[ownedByPlayer].substractResources(resourceTypes::resourceGold, listOfActorTemplates[idOfUnitOrResearch].getPriceOfActor().gold);
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
    for (int x = this->startLocation.x; x > this->startLocation.x - listOfBuildingTemplates[this->buildingType].getBuildingFootprint().amountOfXFootprint; x--) {
        for (int y = this->startLocation.y; y > this->startLocation.y - listOfBuildingTemplates[this->buildingType].getBuildingFootprint().amountOfYFootprint; y--) {
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
                if (!currentGame.occupiedByActorList[cord.x][cord.y].empty())
                {
                    for (auto& id : currentGame.occupiedByActorList[cord.x][cord.y]) {
                        if (listOfActors[id].getTeam() != this->ownedByPlayer)
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
