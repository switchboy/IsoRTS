#include "gamestate.h"
#include "buildings.h"
#include "objects.h"
#include "actors.h"
#include "player.h"
#include <iostream>
#include <sstream>
#include <algorithm> 
#include <future>
#include <vector>
#include "gametext.h"
#include "buildings.h"
#include "randomMapGenerator.h"
#include "projectile.h"
#include "orderCursor.h"

sf::RenderTexture minimapBuildingsTexture;
sf::RenderTexture minimapActorsTexture;
sf::RenderTexture minimapMistTexture;
sf::RenderTexture minimapObjectsTexture;

bool noNewBuildings;

cords toWorldMousePosition(int mouseX, int mouseY)
{
    if (!(mouseX < 0 || mouseY < 0))
    {
        auto cheatTile = currentGame.textureCheatTile.copyToImage();
        int cellX = mouseX / 64;
        int cellY = mouseY / 32;
        int offSetX = mouseX % 64;
        int offSetY = mouseY % 32;
        int worldX = (cellY - mapOffsetY) + (cellX - mapOffsetX);
        int worldY = (cellY - mapOffsetY) - (cellX - mapOffsetX);
        auto color = cheatTile.getPixel(offSetX, offSetY);
        if (color == sf::Color::Red)
        {
            worldX += -1;
        }
        else if (color == sf::Color::Green)
        {
            worldY += +1;
        }
        else if (color == sf::Color::Blue)
        {
            worldY += -1;
        }
        else if (color == sf::Color::Yellow)
        {
            worldX += +1;
        }
        return { worldX, worldY };
    }
    else
    {
        return { 0,0 };
    }
}

void gameState::drawMousePosition(int x,int y, bool noProblem)
{
    if(noProblem)
    {
        spriteSelectedTile.setPosition(static_cast<float>(worldSpace(x, y,true)), static_cast<float>(worldSpace(x, y,false)));
        window.draw(spriteSelectedTile);
    }
    else
    {
        spriteTileObstructed.setPosition(static_cast<float>(worldSpace(x, y,true)), static_cast<float>(worldSpace(x, y,false)));
        window.draw(spriteTileObstructed);
    }
}

bool gameState::isPassable(int x, int y)
{
    //check if the terrain is passable 1-6 and within map bounds
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
        if ((this->currentMap[x][y] > 0 && this->currentMap[x][y] < 7) && this->objectLocationList[x][y] == -1 && this->occupiedByBuildingList[x][y] == -1 && this->occupiedByActorList[x][y] == -1)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else {
        return false;
    }
}

void gameState::drawGround(int i, int j)
{
    switch(currentGame.currentMap[i][j])
    {
    case 0:
        spriteEmptyTile.setPosition(static_cast<float>(worldSpace(i,j,true)), static_cast<float>(worldSpace(i,j,false)));
        window.draw(spriteEmptyTile);
        break;
    case 1:
        spriteGrassTile.setPosition(static_cast<float>(worldSpace(i,j,true)), static_cast<float>(worldSpace(i,j,false)));
        window.draw(spriteGrassTile);
        break;
    case 2:
        spriteSandTile.setPosition(static_cast<float>(worldSpace(i,j,true)), static_cast<float>(worldSpace(i,j,false)));
        window.draw(spriteSandTile);
        break;
    case 3:
        spriteSandTileNE.setPosition(static_cast<float>(worldSpace(i,j,true)), static_cast<float>(worldSpace(i,j,false)));
        window.draw(spriteSandTileNE);
        break;
    case 4:
        spriteSandTileNW.setPosition(static_cast<float>(worldSpace(i,j,true)), static_cast<float>(worldSpace(i,j,false)));
        window.draw(spriteSandTileNW);
        break;
    case 5:
        spriteSandTileSE.setPosition(static_cast<float>(worldSpace(i,j,true)), static_cast<float>(worldSpace(i,j,false)));
        window.draw(spriteSandTileSE);
        break;
    case 6:
        spriteSandTileSW.setPosition(static_cast<float>(worldSpace(i,j,true)), static_cast<float>(worldSpace(i,j,false)));
        window.draw(spriteSandTileSW);
        break;
    case 7:
        spriteWaterTile.setPosition(static_cast<float>(worldSpace(i,j,true)), static_cast<float>(worldSpace(i,j,false)));
        window.draw(spriteWaterTile);
        break;
    case 8:
        spriteBeachTileNE.setPosition(static_cast<float>(worldSpace(i,j,true)), static_cast<float>(worldSpace(i,j,false)));
        window.draw(spriteBeachTileNE);
        break;
    case 9:
        spriteBeachTileNW.setPosition(static_cast<float>(worldSpace(i,j,true)), static_cast<float>(worldSpace(i,j,false)));
        window.draw(spriteBeachTileNW);
        break;
    case 10:
        spriteBeachTileSE.setPosition(static_cast<float>(worldSpace(i,j,true)), static_cast<float>(worldSpace(i,j,false)));
        window.draw(spriteBeachTileSE);
        break;
    case 11:
        spriteBeachTileSW.setPosition(static_cast<float>(worldSpace(i,j,true)), static_cast<float>(worldSpace(i,j,false)));
        window.draw(spriteBeachTileSW);
        break;
    }
    if (this->showPaths) {
        if (!isPassable(i, j)) {
            spriteTileObstructed.setPosition(static_cast<float>(worldSpace(i, j, true)), static_cast<float>(worldSpace(i, j, false)));
            window.draw(spriteTileObstructed);
        }
    }
}

void gameState::setIsPlacingBuilding()
{
    this->isPlacingBuilding = true;
}

void gameState::drawThingsOnTile(int i, int j)
{
    if(this->buildingLocationList[i][j] != -1)
    {
        listOfBuildings[this->buildingLocationList[i][j]].drawBuilding(i, j, 0, false);
    }
    else if(this->objectLocationList[i][j]!= -1)
    {
        listOfObjects[this->objectLocationList[i][j]].drawObject(i, j);
    }
    else if(this->occupiedByActorList[i][j] != -1 && this->visability[(i * MAP_HEIGHT) + j] > 1)
    {
        listOfActors[this->occupiedByActorList[i][j]].drawActor();
    }
}

bool gameState::isInSelectedActors(int id)
{
    if(!this->selectedUnits.empty())
    {
        for(int i = 0; i < this->selectedUnits.size(); i++)
        {
            if(selectedUnits[i] == id)
            {
                return true;
            }
        }
    }
    return false;
}

bool gameState::buildingIsSelected(int& id)
{
    if (this->buildingSelectedId == id) {
        return true;
    }
    else {
        return false;
    }
}

void gameState::drawMap()
{
    int lowX = toWorldMousePosition(viewOffsetX-(mainWindowWidth/2), static_cast<int>(viewOffsetY-((mainWindowWidth*0.8)/2))).x;

    int highX = toWorldMousePosition(viewOffsetX + (mainWindowWidth / 2), static_cast<int>(viewOffsetY + ((mainWindowWidth * 0.8) / 2))).x;

    int lowY = toWorldMousePosition(viewOffsetX, viewOffsetY-(mainWindowWidth/2)).y;
    int highY = toWorldMousePosition(viewOffsetX, viewOffsetY+(mainWindowWidth/2)).y;
    if (lowX < 0) { lowX = 0; }
    if (highX > MAP_WIDTH) { highX = MAP_WIDTH; }
    if (lowY < 0) { lowY = 0; }
    if (highY > MAP_HEIGHT) { highX = MAP_HEIGHT; }

    for(int j = lowY; j < highY; j++)
    {
        for(int i = lowX; i < highX; i++ )
        {
            if (this->visability[(i * MAP_HEIGHT) + j] > 0) {
                drawGround(i, j);
            }
            else {
                 spriteBlackTile.setPosition(static_cast<float>(worldSpace(i, j, true)), static_cast<float>(worldSpace(i, j, false)));
                 window.draw(spriteBlackTile);
            }
        }
    }
    for (int j = lowY; j < highY; j++)
    {
        for (int i = lowX; i < highX; i++)
        {
            if (this->visability[(i * MAP_HEIGHT) + j] > 0) {
                drawThingsOnTile(i, j);
            }
           
        }
    }
    for (int j = lowY; j < highY; j++)
    {
        for (int i = lowX; i < highX; i++)
        {
            if (this->visability[(i * MAP_HEIGHT) + j] <= 1) {
                spriteMistTile.setPosition(static_cast<float>(worldSpace(i, j, true)), static_cast<float>(worldSpace(i, j, false)));
                window.draw(spriteMistTile);
            }
        }
    }
}



void gameState::loadTextures()
{
    this->selectionRectangle.setOutlineThickness(1);
    this->selectionRectangle.setOutlineColor(sf::Color(255, 255, 255));
    this->selectionRectangle.setFillColor(sf::Color(255, 255, 255, 40));
    if (textureArrow.loadFromFile("textures/arrow.png"))
    {
        spriteArrow.setTexture(textureArrow);
    }
    else
    {
        std::cout << "Error loading texture: largeIconGrid.png \n" << std::endl;
    }
    spriteArrow.setOrigin(spriteArrow.getLocalBounds().width/2, spriteArrow.getLocalBounds().height/2);
    if(textureBigSelectedIcon.loadFromFile("textures/largeIconGrid.png"))
    {
        spriteBigSelectedIcon.setTexture(textureBigSelectedIcon);
    }
    else
    {
        std::cout << "Error loading texture: largeIconGrid.png \n" << std::endl;
    }
    if(textureUIButton.loadFromFile("textures/icons.png"))
    {
        spriteUIButton.setTexture(textureUIButton);
    }
    else
    {
        std::cout << "Error loading texture: icons.png \n" << std::endl;
    }
    if(textureUnitSelectedTile.loadFromFile("textures/unitSelectedTile.png"))
    {
        spriteUnitSelectedTile.setTexture(textureUnitSelectedTile);
    }
    else
    {
        std::cout << "Error loading texture: unitSelectedTile.png \n" << std::endl;
    }
    if(textureStone.loadFromFile("textures/stone.png"))
    {
        spriteStone.setTexture(textureStone);
        spriteStone.setOrigin(0,2);
    }
    else
    {
        std::cout << "Error loading texture: stone.png \n" << std::endl;
    }
    if(textureGold.loadFromFile("textures/gold.png"))
    {
        spriteGold.setTexture(textureGold);
        spriteGold.setOrigin(0,2);
    }
    else
    {
        std::cout << "Error loading texture: gold.png \n" << std::endl;
    }
    if(textureBerryBush.loadFromFile("textures/berrybush.png"))
    {
        spriteBerryBush.setTexture(textureBerryBush);
        spriteBerryBush.setOrigin(0,2);
    }
    else
    {
        std::cout << "Error loading texture: berrybush.png \n" << std::endl;
    }

    if (textureCommandCursor.loadFromFile("textures/giveOrder.png"))
    {
        spriteCommandCursor.setTexture(textureCommandCursor);
        spriteCommandCursor.setTextureRect(sf::IntRect(0, 0, 16, 16));
        spriteCommandCursor.setOrigin(8, 8);
    }
    else
    {
        std::cout << "Error loading texture: giveOrder.png \n" << std::endl;
    }

    if(textureTileObstructed.loadFromFile("textures/tileObstructed.png"))
    {
        spriteTileObstructed.setTexture(textureTileObstructed);
    }
    else
    {
        std::cout << "Error loading texture: tileObstructed.png \n" << std::endl;
    }
    if(textureBuildingHouse.loadFromFile("textures/house.png"))
    {
        spriteBuildingHouse.setTexture(textureBuildingHouse);
        spriteBuildingHouse.setTextureRect(sf::IntRect(0,0,128,128));
        spriteBuildingHouse.setOrigin(32,96);
    }
    else
    {
        std::cout << "Error loading texture: house.png \n" << std::endl;
    }


    if (textureBuildingMill.loadFromFile("textures/mill.png"))
    {
        spriteBuildingMill.setTexture(textureBuildingMill);
        spriteBuildingMill.setTextureRect(sf::IntRect(0, 0, 192, 192));
        spriteBuildingMill.setOrigin(64, 160);
    }
    else
    {
        std::cout << "Error loading texture: mill.png \n" << std::endl;
    }


    if (textureBuildingLumberCamp.loadFromFile("textures/lumbercamp.png"))
    {
        spriteBuildingLumberCamp.setTexture(textureBuildingLumberCamp);
        spriteBuildingLumberCamp.setTextureRect(sf::IntRect(0, 0, 192, 192));
        spriteBuildingLumberCamp.setOrigin(64, 160);
    }
    else
    {
        std::cout << "Error loading texture: lumbercamp.png \n" << std::endl;
    }

    if (textureBuildingMiningCamp.loadFromFile("textures/miningCamp.png"))
    {
        spriteBuildingMiningCamp.setTexture(textureBuildingMiningCamp);
        spriteBuildingMiningCamp.setTextureRect(sf::IntRect(0, 0, 192, 192));
        spriteBuildingMiningCamp.setOrigin(64, 160);
    }
    else
    {
        std::cout << "Error loading texture: lumbercamp.png \n" << std::endl;
    }



    if (textureBuildingBarracks.loadFromFile("textures/barracks.png"))
    {
        spriteBuildingBarracks.setTexture(textureBuildingBarracks);
        spriteBuildingBarracks.setTextureRect(sf::IntRect(0, 0, 192, 192));
        spriteBuildingBarracks.setOrigin(64, 160);
    }
    else
    {
        std::cout << "Error loading texture: lumbercamp.png \n" << std::endl;
    }


    if(texturePineTreeTile.loadFromFile("textures/pineTree.png"))
    {
        spritePineTreeTile.setTexture(texturePineTreeTile);
        spritePineTreeTile.setOrigin(0,32);
    }
    else
    {
        std::cout << "Error loading texture: normalTree.png \n" << std::endl;
    }
    if(textureNormalTreeTile.loadFromFile("textures/normalTree.png"))
    {
        spriteNormalTreeTile.setTexture(textureNormalTreeTile);
        spriteNormalTreeTile.setOrigin(0,32);
    }
    else
    {
        std::cout << "Error loading texture: normalTree.png \n" << std::endl;
    }
    if(textureCypressTrileTile.loadFromFile("textures/cypressTree.png"))
    {
        spriteCypressTrileTile.setTexture(textureCypressTrileTile);
        spriteCypressTrileTile.setOrigin(0,32);
    }
    if(textureTotalBackground.loadFromFile("textures/totalBackground.png"))
    {
        spriteTotalBackground.setTexture(textureTotalBackground);
    }
    else
    {
        std::cout << "Error loading texture: cypressTree.png \n" << std::endl;
    }
    if(textureTownCenter.loadFromFile("textures/townCenter.png"))
    {
        spriteTownCenter.setTexture(textureTownCenter);
        spriteTownCenter.setTextureRect(sf::IntRect(0,0,256,256));
        spriteTownCenter.setOrigin(96,224);
    }
    else
    {
        std::cout << "Error loading texture: townCenter.png \n" << std::endl;
    }
    if(textureCactusTile.loadFromFile("textures/cactus.png"))
    {
        spriteCactusTile.setTexture(textureCactusTile);
        spriteCactusTile.setOrigin(0,32);
    }
    else
    {
        std::cout << "Error loading texture: cactus.png \n" << std::endl;
    }
    if(textureSelectedTile.loadFromFile("textures/tileSelected.png"))
    {
        spriteSelectedTile.setTexture(textureSelectedTile);
    }
    else
    {
        std::cout << "Error loading texture: tileSelected.png \n" << std::endl;
    }
    if (textureMistTile.loadFromFile("textures/mistTile.png"))
    {
        spriteMistTile.setTexture(textureMistTile);
    }
    else
    {
        std::cout << "Error loading texture: mistTile.png \n" << std::endl;
    }
    if(textureSelectedTileForPath.loadFromFile("textures/tileSelectedForPath.png"))
    {
        spriteSelectedTileForPath.setTexture(textureSelectedTileForPath);
    }
    else
    {
        std::cout << "Error loading texture: tileSelected.png \n" << std::endl;
    }
    if(textureEmptyTile.loadFromFile("textures/emptyTile.png"))
    {
        spriteEmptyTile.setTexture(textureEmptyTile);
    }
    else
    {
        std::cout << "Error loading texture: emptyTile.png \n" << std::endl;
    }
    if (textureBlackTile.loadFromFile("textures/blackTile.png"))
    {
        spriteBlackTile.setTexture(textureBlackTile);
    }
    else
    {
        std::cout << "Error loading texture: blackTile.png \n" << std::endl;
    }
    if(textureGrassTile.loadFromFile("textures/grassTile.png"))
    {
        spriteGrassTile.setTexture(textureGrassTile);
    }
    else
    {
        std::cout << "Error loading texture: grassTile.png \n" << std::endl;
    }
    if(textureSandTile.loadFromFile("textures/sand.png"))
    {
        spriteSandTile.setTexture(textureSandTile);
    }
    else
    {
        std::cout << "Error loading texture: sand.png \n" << std::endl;
    }
    if(textureSandTileNE.loadFromFile("textures/sandNE.png"))
    {
        spriteSandTileNE.setTexture(textureSandTileNE);
    }
    else
    {
        std::cout << "Error loading texture: sandNE.png \n" << std::endl;
    }
    if(textureSandTileNW.loadFromFile("textures/sandNW.png"))
    {
        spriteSandTileNW.setTexture(textureSandTileNW);
    }
    else
    {
        std::cout << "Error loading texture: sandNW.png \n" << std::endl;
    }
    if(textureSandTileSE.loadFromFile("textures/sandSE.png"))
    {
        spriteSandTileSE.setTexture(textureSandTileSE);
    }
    else
    {
        std::cout << "Error loading texture: sandSE.png \n" << std::endl;
    }
    if(textureSandTileSW.loadFromFile("textures/sandSW.png"))
    {
        spriteSandTileSW.setTexture(textureSandTileSW);
    }
    else
    {
        std::cout << "Error loading texture: sandSW.png \n" << std::endl;
    }
    if(textureWaterTile.loadFromFile("textures/waterTile.png"))
    {
        spriteWaterTile.setTexture(textureWaterTile);
    }
    else
    {
        std::cout << "Error loading texture: waterTile.png \n" << std::endl;
    }
    if(textureBeachTileNE.loadFromFile("textures/beachTileNE.png"))
    {
        spriteBeachTileNE.setTexture(textureBeachTileNE);
    }
    else
    {
        std::cout << "Error loading texture: beachTileNE.png \n" << std::endl;
    }
    if(textureBeachTileSW.loadFromFile("textures/beachTileSW.png"))
    {
        spriteBeachTileSW.setTexture(textureBeachTileSW);
    }
    else
    {
        std::cout << "Error loading texture: beachTileSW.png \n" << std::endl;
    }
    if(textureBeachTileNW.loadFromFile("textures/beachTileNW.png"))
    {
        spriteBeachTileNW.setTexture(textureBeachTileNW);
    }
    else
    {
        std::cout << "Error loading texture: beachTileNW.png \n" << std::endl;
    }
    if(textureBeachTileSE.loadFromFile("textures/beachTileSE.png"))
    {
        spriteBeachTileSE.setTexture(textureBeachTileSE);
    }
    else
    {
        std::cout << "Error loading texture: beachTileSE.png \n" << std::endl;
    }
    if(!textureCheatTile.loadFromFile("textures/cheatTile.png"))
    {
        std::cout << "Error loading texture: cheatTile.png \n" << std::endl;
    }

    //actors
    if(this->textureVillager.loadFromFile("textures/testVillagerSprite.png"))
    {
        this->spriteVillager.setTexture(textureVillager);
        this->spriteVillager.setTextureRect(sf::IntRect(0,0,16,32));
        this->spriteVillager.setOrigin(-24,12);

    }
    else
    {
        std::cout << "Error loading texture: testVillagerSprite.png \n" << std::endl;
    }
    if (this->textureSwordsman.loadFromFile("textures/swordsman.png"))
    {
        this->spriteSwordsman.setTexture(textureSwordsman);
        this->spriteSwordsman.setTextureRect(sf::IntRect(0, 0, 16, 32));
        this->spriteSwordsman.setOrigin(-24, 12);

    }
    else
    {
        std::cout << "Error loading texture: swordsman.png \n" << std::endl;
    }

    if (this->textureFlag.loadFromFile("textures/flag.png"))
    {
        this->spriteFlag.setTexture(textureFlag);
        this->spriteFlag.setTextureRect(sf::IntRect(0, 0, 16, 32));
        this->spriteFlag.setOrigin(-24, 12);

    }
    else
    {
        std::cout << "Error loading texture: flag.png \n" << std::endl;
    }
}

void gameState::setBuildingType(int id)
{
    this->buildingTypeSelected = id;
}

void gameState::calculateRectangle()
{
    int endLocation[2]= {0,0};
    int highYLocation[2]= {0,0};
    int lowYLocation[2]= {0,0};
    //Endlocation = current world mouse location
    endLocation[0] = this->mouseWorldPosition.x;
    endLocation[1] = this->mouseWorldPosition.y;

    //calculate highY = same screen Y but with start X
    highYLocation[0] = toWorldMousePosition(this->startMouseCords[0], static_cast<int>(this->mousePosition.y)).x;
    highYLocation[1] = toWorldMousePosition(this->startMouseCords[0], static_cast<int>(this->mousePosition.y)).y;

    //calculate lowY = same screen X but with start Y
    lowYLocation[0] = toWorldMousePosition(static_cast<int>(this->mousePosition.x), this->startMouseCords[1]).x;
    lowYLocation[1] = toWorldMousePosition(static_cast<int>(this->mousePosition.x), this->startMouseCords[1]).y;

    //Fix for edge-case: mouse Y is < start mouse Y
    int startLocation2[2] = {this->startLocation[0], this->startLocation[1]};

    if(this->mousePosition.y < this->startMouseCords[1])
    {
        int tempStorage[2] = {endLocation[0], endLocation[1]};
        int tempStorage2[2] = {highYLocation[0], highYLocation[1]};
        endLocation[0] = lowYLocation[0];
        endLocation[1] = lowYLocation[1];
        lowYLocation[0] = tempStorage[0];
        lowYLocation[1] = tempStorage[1];
        highYLocation[0] = startLocation2[0];
        highYLocation[1] = startLocation2[1];
        startLocation2[0] = tempStorage2[0];
        startLocation2[1] = tempStorage2[1];
    }

    //Fix for edge-case: mouse X is < start mouse X
    if(this->mousePosition.x < this->startMouseCords[0])
    {
        int tempStorage[2] = {endLocation[0], endLocation[1]};
        int tempStorage2[2] = {startLocation2[0], startLocation2[1]};
        endLocation[0] = highYLocation[0];
        endLocation[1] = highYLocation[1];
        highYLocation[0] = tempStorage[0];
        highYLocation[1] = tempStorage[1];
        startLocation2[0] = lowYLocation[0];
        startLocation2[1] = lowYLocation[1];
        lowYLocation[0] = tempStorage2[0];
        lowYLocation[1] = tempStorage2[1];
    }

    //Start filling the list!
    this->rectangleCords.clear();
    this->rectangleCords.push_back({startLocation2[0],startLocation2[1]});
    this->rectangleCords.push_back({endLocation[0],endLocation[1]});
    this->rectangleCords.push_back({highYLocation[0],highYLocation[1]});
    this->rectangleCords.push_back({lowYLocation[0], lowYLocation[1]});

    //calculate the inbetweens form startLocation to lowYLocation
    int i = startLocation2[0]+1;
    int j = startLocation2[1]-1;
    while(i <= lowYLocation[0] && j >= lowYLocation[1])
    {
        //add new coördanate {i, j} to a list
        if(i != lowYLocation[0] && j != lowYLocation[1])
        {
            this->rectangleCords.push_back({i, j});
        }
        i++;
        j--;
    }

    //calculate the inbetweens from lowYlocation to endLocation
    i = lowYLocation[0]+1;
    j = lowYLocation[1]+1;
    while( i <= endLocation[0] && j <= endLocation[1])
    {
        //add new coördenates {i, j} to a list
        if(i != endLocation[0] && j != endLocation[1])
        {
            this->rectangleCords.push_back({i, j});
        }
        else
        {
            if(this->rectangleCords.back().y+1 != endLocation[1])
            {
                this->rectangleCords.push_back({this->rectangleCords.back().x+1, this->rectangleCords.back().y+1 });
            }
        }

        i++;
        j++;
    }

    //calculate the inbetweens from highYLocation to endLocation
    i = highYLocation[0]+1;
    j = highYLocation[1]-1;
    while(i <= endLocation[0] && j >= endLocation[1])
    {
        //add new coördenates {i, j} to a list
        if(i != endLocation[0] && j != endLocation[1])
        {
            this->rectangleCords.push_back({i, j});

        }
        else
        {
            if(this->rectangleCords.back().y-1 != endLocation[1])
            {
                this->rectangleCords.push_back({this->rectangleCords.back().x+1, this->rectangleCords.back().y-1 });
            }
        }
        i++;
        j--;
    }

    //calculate the inbetweens from startLocation to highYLocation
    i = startLocation2[0]+1;
    j = startLocation2[1]+1;
    while( i <= highYLocation[0] && j <= highYLocation[1])
    {
        //add new coördanates {i, j}to a list
        if( i != highYLocation[0] && j != highYLocation[1])
        {
            this->rectangleCords.push_back({i, j});
        }
        i++;
        j++;
    }

    //sort the list on the Y coördanates
    if(!this->rectangleCords.empty())
    {
        std::sort(this->rectangleCords.begin(),this->rectangleCords.end(), rectCord);
    }

    //add cells on the list between the two cells with the same Y coördanets repeat until list is empty
    if(!this->rectangleCords.empty())
    {
        int stopAt = static_cast<int>(this->rectangleCords.size());
        for(int i = 0; i < stopAt; i++)
        {
            if (i < stopAt - 1) 
            {
                if (this->rectangleCords[i].y == this->rectangleCords[i + 1].y)
                {
                    if (this->rectangleCords[i].x < this->rectangleCords[i + 1].x)
                    {
                        for (int j = this->rectangleCords[i].x + 1; j < this->rectangleCords[i + 1].x; j++)
                        {
                            this->rectangleCords.push_back({ j, this->rectangleCords[i].y });

                        }
                    }
                    else
                    {
                        for (int j = this->rectangleCords[i + 1].x + 1; j < this->rectangleCords[i].x; j++)
                        {
                            this->rectangleCords.push_back({ j, this->rectangleCords[i].y });
                        }
                    }
                }
            }
        }
    }

    //Throw out any cells which are out of bounds
    if(!this->rectangleCords.empty())
    {
        for(int i = 0; i< this->rectangleCords.size();)
        {
            if(this->rectangleCords[i].x < 0 || this->rectangleCords[i].x >= MAP_WIDTH || this->rectangleCords[i].y < 0 || this->rectangleCords[i].y >= MAP_HEIGHT)
            {
                //Deze cell is out of bounds gooi hem weg
                this->rectangleCords.erase(this->rectangleCords.begin()+ i);
            }
            else
            {
                i++;
            }
        }
    }

    //Throw out duplicates
    if(!this->rectangleCords.empty())
    {
       std::sort(this->rectangleCords.begin(), this->rectangleCords.end(), sortCordByX);
       this->rectangleCords.erase(std::unique(this->rectangleCords.begin(), this->rectangleCords.end(), compareCord), this->rectangleCords.end());
    }
}

void gameState::changeViewFromMiniMap() {
    viewOffsetX = static_cast<int>(((mouseFakePosition.x - (mainWindowWidth * 0.8f)) / (0.2f * mainWindowWidth)) * (MAP_WIDTH * 64));
    viewOffsetY = static_cast<int>(((mouseFakePosition.y - (mainWindowHeigth * 0.8f)) / (0.2f * mainWindowHeigth)) * (MAP_HEIGHT * 32));
}

void gameState::clickUIButton() {
    //check if a UI button was pressed
    for (auto& Button : listOfButtons)
    {
        Button.isClicked(this->mouseFakePosition);
    }
}

void gameState::clickToPlaceBuilding() {
    if (!(this->mouseWorldPosition.x - footprintOfBuildings[this->buildingTypeSelected].amountOfXFootprint < -1) && !(this->mouseWorldPosition.y - footprintOfBuildings[this->buildingTypeSelected].amountOfYFootprint < -1) && !(this->mouseWorldPosition.x >= MAP_WIDTH) && !(this->mouseWorldPosition.y >= MAP_HEIGHT))
    {
        //check of het gebouw hier kan staan:
        bool buildingPlacable = true;

        for (int i = 0; i < footprintOfBuildings[this->buildingTypeSelected].amountOfXFootprint; i++)
        {
            for (int j = 0; j < footprintOfBuildings[this->buildingTypeSelected].amountOfYFootprint; j++)
            {
                if (
                    this->occupiedByBuildingList[this->mouseWorldPosition.x - i][this->mouseWorldPosition.y - j] != -1 || 
                    this->objectLocationList[mouseWorldPosition.x - i][mouseWorldPosition.y - j] != -1 || 
                    this->occupiedByActorList[this->mouseWorldPosition.x - i][this->mouseWorldPosition.y - j] != -1 ||
                    this->currentMap[this->mouseWorldPosition.x - i][this->mouseWorldPosition.y - j] == 7
                    )
                {
                    buildingPlacable = false;
                }
            }
        }
        if (buildingPlacable)
        {
            //Zet het gebouw neer
            buildings newBuilding(this->buildingTypeSelected, this->mouseWorldPosition.x, this->mouseWorldPosition.y, static_cast<int>(listOfBuildings.size()), currentPlayer.getTeam());
            listOfBuildings.push_back(newBuilding);
            if (this->isPlacingBuilding)
            {
                currentPlayer.substractResources(0, priceOfBuilding[this->buildingTypeSelected].wood);
                currentPlayer.substractResources(1, priceOfBuilding[this->buildingTypeSelected].food);
                currentPlayer.substractResources(2, priceOfBuilding[this->buildingTypeSelected].stone);
                currentPlayer.substractResources(3, priceOfBuilding[this->buildingTypeSelected].gold);
                for (int i = 0; i < this->selectedUnits.size(); i++)
                {
                    nearestBuildingTile tempTile = findNearestBuildingTile(this->occupiedByBuildingList[this->mouseWorldPosition.x][this->mouseWorldPosition.y], this->selectedUnits[i]);
                    if (tempTile.isSet)
                    {
                        if (this->selectedUnits.size() > 1)
                        {
                            if (listOfActors[this->selectedUnits[i]].getType() == 0 && listOfActors[this->selectedUnits[i]].getTeam() == currentPlayer.getTeam())
                            {
                                listOfActors[this->selectedUnits[i]].updateGoal(tempTile.locationX, tempTile.locationY, i / 5);
                                listOfActors[this->selectedUnits[i]].setCommonGoalTrue();
                                listOfBuildings[this->occupiedByBuildingList[this->mouseWorldPosition.x][this->mouseWorldPosition.y]].claimFreeBuiildingTile(tempTile.buildingId, listOfActors[this->selectedUnits[i]].getActorId());
                            }
                        }
                        else
                        {
                            if (listOfActors[this->selectedUnits[i]].getType() == 0 && listOfActors[this->selectedUnits[i]].getTeam() == currentPlayer.getTeam())
                            {
                                listOfActors[this->selectedUnits[i]].updateGoal(tempTile.locationX, tempTile.locationY, i / 5);
                                listOfBuildings[this->occupiedByBuildingList[this->mouseWorldPosition.x][this->mouseWorldPosition.y]].claimFreeBuiildingTile(tempTile.buildingId, listOfActors[this->selectedUnits[i]].getActorId());
                            }
                        }
                        if (listOfActors[this->selectedUnits[i]].getType() == 0 && listOfActors[this->selectedUnits[i]].getTeam() == currentPlayer.getTeam())
                        {
                            listOfActors[this->selectedUnits[i]].setIsBuildingTrue(listOfBuildings[this->occupiedByBuildingList[this->mouseWorldPosition.x][this->mouseWorldPosition.y]].getBuildingId(), tempTile.actionLocationX, tempTile.actionLocationY);
                        }
                    }
                }
            }
            this->isPlacingBuilding = false;
            this->mousePressOutofWorld = true;
        }
    }
}

void gameState::clickToPlaceObject()
{
    if (!(this->mouseWorldPosition.x < 0) && !(this->mouseWorldPosition.y < 0) && !(this->mouseWorldPosition.x >= MAP_WIDTH) && !(this->mouseWorldPosition.y >= MAP_HEIGHT))
    {
        //check of het object hier kan staan:
        if (this->objectLocationList[mouseWorldPosition.x][mouseWorldPosition.y] == -1 && this->occupiedByBuildingList[this->mouseWorldPosition.x][this->mouseWorldPosition.y] == -1)
        {
            //Zet het object neer
            objects newObject(static_cast<objectTypes>(this->objectTypeSelected), this->mouseWorldPosition.x, this->mouseWorldPosition.y, static_cast<int>(listOfObjects.size()));
            listOfObjects.push_back(newObject);
        }
    }
}

void gameState::clickToPlaceActor()
{
    if (!(this->mouseWorldPosition.x < 0) && !(this->mouseWorldPosition.y < 0) && !(this->mouseWorldPosition.x >= MAP_WIDTH) && !(this->mouseWorldPosition.y >= MAP_HEIGHT))
    {
        //check of de actor hier kan staan:
        if (this->objectLocationList[mouseWorldPosition.x][mouseWorldPosition.y] == -1 && this->occupiedByBuildingList[this->mouseWorldPosition.x][this->mouseWorldPosition.y] == -1 && this->occupiedByActorList[mouseWorldPosition.x][mouseWorldPosition.y] == -1)
        {
            //Zet de actor neer
            listOfActorsMutex.lock();
            actors newActor(0, this->mouseWorldPosition.x, this->mouseWorldPosition.y, currentPlayer.getTeam(), static_cast<int>(listOfActors.size()));
            listOfActors.push_back(newActor);
            listOfActorsMutex.unlock();
        }
    }
}

void gameState::clickToSelectObjectOrBuilding()
{
    if (this->occupiedByBuildingList[this->mouseWorldPosition.x][this->mouseWorldPosition.y] != -1)
    {
        this->buildingSelectedId = this->occupiedByBuildingList[this->mouseWorldPosition.x][this->mouseWorldPosition.y];
    }
    else
    {
        this->buildingSelectedId = -1;
    }
    if (this->objectLocationList[this->mouseWorldPosition.x][this->mouseWorldPosition.y] != -1)
    {
        this->objectSelectedId = this->objectLocationList[this->mouseWorldPosition.x][this->mouseWorldPosition.y];
    }
    else
    {
        this->objectSelectedId = -1;
    }
}

void gameState::clickToSelect()
{
    if (!(this->mouseWorldPosition.x >= MAP_WIDTH || this->mouseWorldPosition.x < 0 || this->mouseWorldPosition.y >= MAP_HEIGHT || this->mouseWorldPosition.y < 0))
    {
        this->startLocation[0] = this->mouseWorldPosition.x;
        this->startLocation[1] = this->mouseWorldPosition.y;
        this->startMouseCords[0] = static_cast<int>(this->mousePosition.x);
        this->startMouseCords[1] = static_cast<int>(this->mousePosition.y);
        clickToSelectObjectOrBuilding();
    }
}

void gameState::drawMouseBox() {
    if (!(this->startLocation[0] == this->mouseWorldPosition.x && this->startLocation[1] == this->mouseWorldPosition.y)) {
        int diffX = this->startMouseCords[0] - static_cast<int>(this->mousePosition.x);
        int diffY = this->startMouseCords[1] - static_cast<int>(this->mousePosition.y);
        this->selectionRectangle.setSize(sf::Vector2f(static_cast<float>(diffX), static_cast<float>(diffY)));
        this->selectionRectangle.setPosition(mousePosition.x, this->mousePosition.y);
        window.draw(this->selectionRectangle);
    }
}

void gameState::mouseLeftClick()
{
    if(!this->mousePressedLeft)
    {
        if (mouseFakePosition.y > mainWindowHeigth * 0.8f)
        {
            this->mousePressOutofWorld = true;
            if (mouseFakePosition.x > mainWindowWidth * 0.8f)
            {
                changeViewFromMiniMap();
            }
            else
            {
                clickUIButton();
            }
        }
        else
        {
            this->mousePressOutofWorld = false;
            if (this->isPressedB || this->isPlacingBuilding)
            {
                clickToPlaceBuilding();
            }
            else if (this->isPressedO)
            {
                clickToPlaceObject();
            }
            else if (this->isPressedA)
            {
                clickToPlaceActor();
            }
            else
            {
                clickToSelect();
            }
        }
        this->mousePressedLeft = true;
    }
    if (!this->isPressedB && !this->isPressedO && !this->isPressedA && !this->mousePressOutofWorld)
    {
        calculateRectangle();
    }
}



void gameState::getDefinitiveSelection()
{
    if (this->mousePressedLeft && !(mouseFakePosition.y > mainWindowHeigth * 0.8f))
    {
        this->selectedUnits.clear();
        if (!this->rectangleCords.empty())
        {
            for (int i = 0; i < this->rectangleCords.size(); i++)
            {
                if (this->occupiedByActorList[this->rectangleCords[i].x][this->rectangleCords[i].y] != -1)
                {
                    selectedUnits.push_back({ this->occupiedByActorList[this->rectangleCords[i].x][this->rectangleCords[i].y] });
                }
            }
            if (selectedUnits.size() > 1) {
                //Haal duplicaten eruit
                sort(selectedUnits.begin(), selectedUnits.end());
                selectedUnits.erase(unique(selectedUnits.begin(), selectedUnits.end()), selectedUnits.end());
                //Haal id's eruit die niet in de actor list zitten
                selectedUnits.erase(std::remove_if(
                    selectedUnits.begin(), selectedUnits.end(),
                    [&](const int id) -> bool {
                        {return (id < 0 || id > listOfActors.size()); }
                    }),
                    selectedUnits.end());

                //haal id's eruit die niet 
                selectedUnits.erase(std::remove_if(selectedUnits.begin(),
                    selectedUnits.end(),
                    [&](const int id) -> bool
                    {
                        return listOfActors[id].getTeam() != currentPlayer.getTeam();
                    }), 
                    selectedUnits.end());
            }

        }
    }
}

bool gameState::clickToMove(int posX, int posY, bool minimap)
{
    bool actionDone = false;
    for (int i = 0; i < this->selectedUnits.size(); i++)
    {
        if (this->selectedUnits.size() > 1)
        {
            if (listOfActors[this->selectedUnits[i]].getTeam() == currentPlayer.getTeam())
            {
                cords tempCords = { 0,0 };
                if (minimap) {
                    if ((posX <= MAP_WIDTH && posY <= MAP_HEIGHT)) {
                        tempCords = currentGame.getNextCord(posX, posY);
                    }
                }
                else {
                    tempCords = currentGame.getNextCord(this->mouseWorldPosition.x, this->mouseWorldPosition.y);
                }
                if (this->isPressedShift) {
                    listOfActors[this->selectedUnits[i]].stackOrder({ tempCords.x, tempCords.y }, stackOrderTypes::stackActionMove);
                }
                else {
                    listOfActors[this->selectedUnits[i]].clearCommandStack();
                    listOfActors[this->selectedUnits[i]].updateGoal(tempCords.x, tempCords.y, i / 5);
                    listOfActors[this->selectedUnits[i]].setCommonGoalTrue();//might be depricated
                }
                actionDone = true;
            }
        }
        else
        {
            if (listOfActors[this->selectedUnits[i]].getTeam() == currentPlayer.getTeam())
            {
                if (minimap) {
                    if ((posX <= MAP_WIDTH && posY <= MAP_HEIGHT)) {
                        if (this->isPressedShift) {
                            listOfActors[this->selectedUnits[i]].stackOrder({ posX, posY }, stackOrderTypes::stackActionMove);
                        }
                        else {
                            listOfActors[this->selectedUnits[i]].clearCommandStack();
                            listOfActors[this->selectedUnits[i]].updateGoal(posX, posY, 0);
                        }
                    }
                }
                else {
                    if (this->isPressedShift) {
                        listOfActors[this->selectedUnits[i]].stackOrder({ this->mouseWorldPosition.x, this->mouseWorldPosition.y }, stackOrderTypes::stackActionMove);
                    }
                    else {
                        listOfActors[this->selectedUnits[i]].clearCommandStack();
                        listOfActors[this->selectedUnits[i]].updateGoal(this->mouseWorldPosition.x, this->mouseWorldPosition.y, 0);
                    }
                    actionDone = true;
                }
            }
        }
        if (listOfActors[this->selectedUnits[i]].getTeam() == currentPlayer.getTeam())
        {
            listOfActors[this->selectedUnits[i]].setGatheringRecource(false);
        }
    }
    return actionDone;
}

bool gameState::clickToGatherResource()
{
    bool actionDone = false;
    for (int i = 0; i < this->selectedUnits.size(); i++)
    {
        if (this->selectedUnits.size() > 1)
        {
            if (listOfActors[this->selectedUnits[i]].getType() == 0 && listOfActors[this->selectedUnits[i]].getTeam() == currentPlayer.getTeam())
            {
                if (this->isPressedShift) {
                    listOfActors[this->selectedUnits[i]].stackOrder({ this->mouseWorldPosition.x, this->mouseWorldPosition.y }, stackOrderTypes::stackActionGather);
                }
                else {
                    listOfActors[this->selectedUnits[i]].clearCommandStack();
                    listOfActors[this->selectedUnits[i]].updateGoal(this->mouseWorldPosition.x, this->mouseWorldPosition.y, i / 5);
                    listOfActors[this->selectedUnits[i]].setCommonGoalTrue();
                }
                actionDone = true;
            }
        }
        else
        {
            if (listOfActors[this->selectedUnits[i]].getType() == 0 && listOfActors[this->selectedUnits[i]].getTeam() == currentPlayer.getTeam())
            {
                if (this->isPressedShift) {
                    listOfActors[this->selectedUnits[i]].stackOrder({ this->mouseWorldPosition.x, this->mouseWorldPosition.y }, stackOrderTypes::stackActionGather);
                }
                else {
                    listOfActors[this->selectedUnits[i]].clearCommandStack();
                    listOfActors[this->selectedUnits[i]].updateGoal(this->mouseWorldPosition.x, this->mouseWorldPosition.y, 0);
                }
                actionDone = true;
            }
        }
        if (listOfActors[this->selectedUnits[i]].getType() == 0 && listOfActors[this->selectedUnits[i]].getTeam() == currentPlayer.getTeam())
        {
            if (!this->isPressedShift) {
                listOfActors[this->selectedUnits[i]].setGatheringRecource(true);
            }
        }
    }
    return actionDone;
}

nearestBuildingTile findNearestBuildingTile(int buildingId, int actorId)
{
    std::list <nearestBuildingTile> listOfBuildLocations;
    std::vector<adjacentTile> tileList = listOfBuildings[buildingId].getFreeBuildingTile();
    if (!tileList.empty()) {
        for (int j = 0; j < tileList.size(); j++)
        {
            float tempDeltaDistance = static_cast<float>(dist(listOfActors[actorId].getLocation().x, listOfActors[actorId].getLocation().y, tileList[j].tileX, tileList[j].tileY));
            listOfBuildLocations.push_back({ tempDeltaDistance, tileList[j].tileX, tileList[j].tileY, tileList[j].goalX , tileList[j].goalY, tileList[j].tileId, true });
        }
        if (!listOfBuildLocations.empty())
        {
            listOfBuildLocations.sort([](const nearestBuildingTile& f, const nearestBuildingTile& s)
                {
                    return f.deltaDistance < s.deltaDistance;
                });
        }
        bool firstLocationIsNotRejected = false;
        while (!firstLocationIsNotRejected && !listOfBuildLocations.empty() && !listOfActors[actorId].getRejectedTargetsList().empty()) {
            bool rejectionsDuringLoop = false;
            for (const auto& reject : listOfActors[actorId].getRejectedTargetsList())
            {
                if (reject.x == listOfBuildLocations.front().locationX && reject.y == listOfBuildLocations.front().locationY) {
                    //target rejected!
                    listOfBuildLocations.pop_front();
                    rejectionsDuringLoop = true;
                }
            }
            if (!rejectionsDuringLoop) {
                firstLocationIsNotRejected = true; //break the loop 
            }
        }
        //Return the location if one is accepted
        if (!listOfBuildLocations.empty()) {
            return  listOfBuildLocations.front();
        }
        else {
            return { 0, 0, 0, 0, false };
        }
    }
    else {
        return { 0, 0, 0, 0, false };
    }
}

bool gameState::clickToBuildOrRepairBuilding()
{
    bool actionDone = false;
    if (!listOfBuildings[this->occupiedByBuildingList[this->mouseWorldPosition.x][this->mouseWorldPosition.y]].getCompleted())
    {
        for (int i = 0; i < this->selectedUnits.size(); i++)
        {
            nearestBuildingTile tempTile = findNearestBuildingTile(this->occupiedByBuildingList[this->mouseWorldPosition.x][this->mouseWorldPosition.y], this->selectedUnits[i]);
            if (tempTile.isSet)
            {
                if (this->selectedUnits.size() > 1)
                {
                    if (listOfActors[this->selectedUnits[i]].getType() == 0 && listOfActors[this->selectedUnits[i]].getTeam() == currentPlayer.getTeam())
                    {
                        if (this->isPressedShift) {
                            listOfActors[this->selectedUnits[i]].stackOrder({ this->mouseWorldPosition.x, this->mouseWorldPosition.y }, stackOrderTypes::stackActionBuild);
                        }
                        else {
                            listOfActors[this->selectedUnits[i]].clearCommandStack();
                            listOfActors[this->selectedUnits[i]].updateGoal(tempTile.locationX, tempTile.locationY, i / 5);
                            listOfActors[this->selectedUnits[i]].setCommonGoalTrue();
                            listOfBuildings[this->occupiedByBuildingList[this->mouseWorldPosition.x][this->mouseWorldPosition.y]].claimFreeBuiildingTile(tempTile.buildingId, listOfActors[this->selectedUnits[i]].getActorId());
                        }
                        actionDone = true;
                    }
                }
                else
                {
                    if (listOfActors[this->selectedUnits[i]].getType() == 0 && listOfActors[this->selectedUnits[i]].getTeam() == currentPlayer.getTeam())
                    {
                        if (this->isPressedShift) {
                            listOfActors[this->selectedUnits[i]].stackOrder({ this->mouseWorldPosition.x, this->mouseWorldPosition.y }, stackOrderTypes::stackActionBuild);
                        }
                        else {
                            listOfActors[this->selectedUnits[i]].clearCommandStack();
                            listOfActors[this->selectedUnits[i]].updateGoal(tempTile.locationX, tempTile.locationY, i / 5);
                            listOfBuildings[this->occupiedByBuildingList[this->mouseWorldPosition.x][this->mouseWorldPosition.y]].claimFreeBuiildingTile(tempTile.buildingId, listOfActors[this->selectedUnits[i]].getActorId());
                            actionDone = true;
                        }
                    }
                }
                if (listOfActors[this->selectedUnits[i]].getType() == 0 && listOfActors[this->selectedUnits[i]].getTeam() == currentPlayer.getTeam())
                {
                    if (!this->isPressedShift) {
                        listOfActors[this->selectedUnits[i]].setIsBuildingTrue(listOfBuildings[this->occupiedByBuildingList[this->mouseWorldPosition.x][this->mouseWorldPosition.y]].getBuildingId(), tempTile.actionLocationX, tempTile.actionLocationY);
                    }
                }
            }
        }
    }
    return actionDone;
}

void gameState::changeBuildingType()
{
    this->buildingTypeSelected += 1;
    if (this->buildingTypeSelected > 5)
    {
        this->buildingTypeSelected = 0;
    }
}

void gameState::changeObjectType()
{
    this->objectTypeSelected += 1;
    if (this->objectTypeSelected > 6)
    {
        this->objectTypeSelected = 0;
    }
}

void testfunctiuon(int testVar)
{
    if (testVar == 32) {
        std::cout << "You have answerd the universe, life and everything!";
    }
}

bool gameState::clickToAttack() {
    bool actionDone = false;
    for (int i = 0; i < this->selectedUnits.size(); i++)
    {
        if (this->selectedUnits.size() > 1)
        {
            if (listOfActors[this->selectedUnits[i]].getTeam() == currentPlayer.getTeam())
            {
                if (this->isPressedShift) {
                    listOfActors[this->selectedUnits[i]].stackOrder({ this->mouseWorldPosition.x, this->mouseWorldPosition.y }, stackOrderTypes::stackActionAttack);
                }
                else {
                    listOfActors[this->selectedUnits[i]].clearCommandStack();
                    listOfActors[this->selectedUnits[i]].updateGoal(this->mouseWorldPosition.x, this->mouseWorldPosition.y, i / 5);
                    listOfActors[this->selectedUnits[i]].setCommonGoalTrue();
                    listOfActors[this->selectedUnits[i]].setIsDoingAttack();
                }
                actionDone = true;
            }
        }
        else
        {
            if (listOfActors[this->selectedUnits[i]].getTeam() == currentPlayer.getTeam())
            {
                if (this->isPressedShift) {
                    listOfActors[this->selectedUnits[i]].stackOrder({ this->mouseWorldPosition.x, this->mouseWorldPosition.y }, stackOrderTypes::stackActionAttack);
                }
                else {
                    listOfActors[this->selectedUnits[i]].clearCommandStack();
                    listOfActors[this->selectedUnits[i]].updateGoal(this->mouseWorldPosition.x, this->mouseWorldPosition.y, 0);
                    listOfActors[this->selectedUnits[i]].setIsDoingAttack();
                }
                actionDone = true;
            }
        }
    }
    return actionDone;
}

void gameState::clickToGiveCommand()
{
    this->firstRound = true;
    this->lastIandJ[0] = 0;
    this->lastIandJ[1] = 0;
    bool actionPreformed = false;
    if (this->isPassable(this->mouseWorldPosition.x, this->mouseWorldPosition.y))
    {
        actionPreformed = clickToMove(0,0,false);
    }
    else if (this->objectLocationList[this->mouseWorldPosition.x][this->mouseWorldPosition.y] != -1)
    {
        actionPreformed = clickToGatherResource();
    }
    else if (this->occupiedByBuildingList[this->mouseWorldPosition.x][this->mouseWorldPosition.y] != -1)
    {
        if (listOfBuildings[this->occupiedByBuildingList[this->mouseWorldPosition.x][this->mouseWorldPosition.y]].getTeam() == currentPlayer.getTeam()) 
        {
            actionPreformed = clickToBuildOrRepairBuilding();
        }
        else 
        {
            actionPreformed = clickToAttack();
        }
        
    }
    else if (this->occupiedByActorList[this->mouseWorldPosition.x][this->mouseWorldPosition.y] != -1) {
        if (listOfActors[this->occupiedByActorList[this->mouseWorldPosition.x][this->mouseWorldPosition.y]].getTeam() != currentPlayer.getTeam()) 
        {
            clickToAttack();
        }
    }

    if (actionPreformed) {
        orderCursor newOrderCursor(this->mousePosition);
        listOfOrderCursors.push_back(newOrderCursor);
    }

}

void gameState::clickToGiveMinimapCommand()
{
    this->firstRound = true;
    this->lastIandJ[0] = 0;
    this->lastIandJ[1] = 0;

    cords minimapToWorldPosition;
    minimapToWorldPosition = toWorldMousePosition(static_cast<int>(((mouseFakePosition.x - (mainWindowWidth * 0.8f)) / (0.2f * mainWindowWidth)) * (MAP_WIDTH * 64)), static_cast<int>(((mouseFakePosition.y - (mainWindowHeigth * 0.8f)) / (0.2f * mainWindowHeigth)) * (MAP_HEIGHT * 32)));

    for (int posX = minimapToWorldPosition.x - 1; posX < minimapToWorldPosition.x + 2; posX++) {
        for (int posY = minimapToWorldPosition.y - 1; posY < minimapToWorldPosition.y + 2; posY++) {
            clickToMove(posX, posY, true);
            orderCursor newOrderCursor(this->mousePosition);
            listOfOrderCursors.push_back(newOrderCursor);
            return;
        }
    }
 }


void gameState::orderRallyPoint() {
    if (this->isPassable(this->mouseWorldPosition.x, this->mouseWorldPosition.y))
    {
        listOfBuildings[this->buildingSelectedId].setRallyPoint({ this->mouseWorldPosition.x, this->mouseWorldPosition.y }, stackOrderTypes::stackActionMove);
    }
    else if (this->objectLocationList[this->mouseWorldPosition.x][this->mouseWorldPosition.y] != -1)
    {
        listOfBuildings[this->buildingSelectedId].setRallyPoint({ this->mouseWorldPosition.x, this->mouseWorldPosition.y }, stackOrderTypes::stackActionGather);
    }
    else if (this->occupiedByBuildingList[this->mouseWorldPosition.x][this->mouseWorldPosition.y] != -1)
    {
        if (listOfBuildings[this->occupiedByBuildingList[this->mouseWorldPosition.x][this->mouseWorldPosition.y]].getTeam() == currentPlayer.getTeam())
        {
            listOfBuildings[this->buildingSelectedId].setRallyPoint({ this->mouseWorldPosition.x, this->mouseWorldPosition.y }, stackOrderTypes::stackActionBuild);
        }
        else
        {
            listOfBuildings[this->buildingSelectedId].setRallyPoint({ this->mouseWorldPosition.x, this->mouseWorldPosition.y }, stackOrderTypes::stackActionAttack);
        }

    }
    else if (this->occupiedByActorList[this->mouseWorldPosition.x][this->mouseWorldPosition.y] != -1) {
        if (listOfActors[this->occupiedByActorList[this->mouseWorldPosition.x][this->mouseWorldPosition.y]].getTeam() != currentPlayer.getTeam())
        {
            listOfBuildings[this->buildingSelectedId].setRallyPoint({ this->mouseWorldPosition.x, this->mouseWorldPosition.y }, stackOrderTypes::stackActionAttack);
        }
    }
    orderCursor newOrderCursor(this->mousePosition);
    listOfOrderCursors.push_back(newOrderCursor);
}

void gameState::mouseRightClick()
{
    this->rectangleCords.clear();
    this->mousePressedRight = true;
    if (mouseFakePosition.y > mainWindowHeigth * 0.8f)
    {
        this->clickToGiveMinimapCommand();
    }
    else {
        if (this->isPressedB)
        {
            this->changeBuildingType();
        }
        else if (this->isPressedO)
        {
            this->changeObjectType();
        }
        else if (this->isPlacingBuilding)
        {
            this->isPlacingBuilding = false;
        }
        else if (!this->selectedUnits.empty())
        {
            this->clickToGiveCommand();
        }
        else if (this->buildingSelectedId != -1) {
            this->orderRallyPoint();
        }
    }
}

void gameState::changeTiles()
{
    for (int i = 0; i < this->rectangleCords.size(); i++)
    {
        this->currentMap[this->rectangleCords[i].x][this->rectangleCords[i].y] += +1;
        if (this->currentMap[this->rectangleCords[i].x][this->rectangleCords[i].y] > 11)
        {
            this->currentMap[this->rectangleCords[i].x][this->rectangleCords[i].y] = 1;
        }
    }
    minimapTextureExist = false;
    this->equalIsPressed = true;
}

void gameState::edgeScrolling()
{
    if (mouseFakePosition.x < 0) {
        viewOffsetX += -10;
    }
    else if (mouseFakePosition.x > mainWindowWidth)
    {
        viewOffsetX += 10;
    }
    if (mouseFakePosition.y < 0) {
        viewOffsetY += -10;
    }
    else if (mouseFakePosition.y > mainWindowHeigth)
    {
        viewOffsetY += 10;
    }
}

void gameState::interact()
{
    this->mouseFakePosition = sf::Mouse::getPosition(window);
    this->mousePosition = window.mapPixelToCoords(mouseFakePosition);

    while (window.pollEvent(this->event))
    {
        switch(this->event.type)
        {
        case sf::Event::Closed:
            window.close();
            break;
        case sf::Event::LostFocus:
            this->focus = false;
            break;
        case sf::Event::GainedFocus:
            this->focus = true;
            break;
        }
    }

    if (this->focus)
    {
        this->mouseWorldPosition = toWorldMousePosition(static_cast<int>(this->mousePosition.x), static_cast<int>(this->mousePosition.y));
        edgeScrolling();
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && this->focus)
    {
        viewOffsetY += -5;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && this->focus)
    {
        viewOffsetY += +5;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && this->focus)
    {
        viewOffsetX += -5;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && this->focus)
    {
        viewOffsetX += +5;
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A) && this->focus)
    {
        this->isPressedA = true;
        this->rectangleCords.clear();
    }
    else if(!sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        this->isPressedA = false;
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::B) && this->focus)
    {
        this->isPressedB = true;
        this->rectangleCords.clear();
    }
    else if(!sf::Keyboard::isKeyPressed(sf::Keyboard::B))
    {
        this->isPressedB = false;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && this->focus && !this->isPressedS)
    {
        this->isPressedS = true;
        if (this->showPaths) {
            this->showPaths = false;
        }
        else {
            this->showPaths = true;
        }
    }
    else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::S) && this->focus)
    {
        this->isPressedS = false;
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::O) && this->focus)
    {
        this->isPressedO = true;
        this->rectangleCords.clear();
    }
    else if(!sf::Keyboard::isKeyPressed(sf::Keyboard::B))
    {
        this->isPressedO = false;
    }

    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) && this->focus)
    {
        this->isPressedShift = true;

    }
    else 
    {
        this->isPressedShift = false;
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Tab) && this->focus && !this->isPressedTab)
    {
        this->isPressedTab = true;
        int tempTeam = currentPlayer.getTeam()+1;
        if(tempTeam > 7)
        {
            currentPlayer = listOfPlayers[1];
        }
        else
        {
            currentPlayer = listOfPlayers[tempTeam];
        }

    }
    else if(!sf::Keyboard::isKeyPressed(sf::Keyboard::Tab))
    {
        this->isPressedTab = false;
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && this->focus)
    {
        this->mouseLeftClick();
    }
    else if(!sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        getDefinitiveSelection();
        this->mousePressedLeft = false;
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && !this->mousePressedRight && this->focus)
    {
        this->mouseRightClick();
    }
    else if(!sf::Mouse::isButtonPressed(sf::Mouse::Right))
    {
        this->mousePressedRight = false;
    }

    if(!this->rectangleCords.empty() && sf::Keyboard::isKeyPressed(sf::Keyboard::Equal) &&!this->equalIsPressed && this->focus)
    {
        this->changeTiles();
    }
    else if(!sf::Keyboard::isKeyPressed(sf::Keyboard::Equal))
    {
        this->equalIsPressed = false;
    }
}

void gameState::selectUnit(int id)
{
    this->selectedUnits.clear();
    this->selectedUnits.push_back(id);
}

void gameState::drawMouseInteraction()
{
    if(this->mousePressedLeft && !this->mousePressOutofWorld)
    {
        drawMouseBox();
        if(!rectangleCords.empty())
        {
            for(int i = 0; i < rectangleCords.size(); i++)
            {
                if (showPaths) {
                    drawMousePosition(rectangleCords[i].x, rectangleCords[i].y, true);
                }
            }
        }
    }
    if(isPressedB || this->isPlacingBuilding)
    {
        listOfBuildings[0].drawBuildingFootprint(this->buildingTypeSelected, mouseWorldPosition.x, mouseWorldPosition.y);
    }
    if(isPressedO)
    {
        listOfObjects[0].drawObjectFootprint(static_cast<objectTypes>(this->objectTypeSelected), mouseWorldPosition.x, mouseWorldPosition.y);
    }
}

cords gameState::getNextCord(int x, int y)
{
    if(!this->firstRound && this->roundDone)
    {
        if(this->lastIandJ[0] == this->lastIandJ[1])
        {
            this->lastIandJ[0] += 1;
            this->lastIandJ[1]  = 0;
        }
        else
        {
            this->lastIandJ[1] += 1;
        }
        this->addSubstractX = true;
        this->addSubstractY = true;

    }
    else
    {
        this->firstRound = false;
    }

    if(this->addSubstractX)
    {
        if(this->addSubstractY)
        {
            x = x + this->lastIandJ[0];
            y = y + this->lastIandJ[1];
            this->addSubstractY = false;
        }
        else
        {
            x = x + this->lastIandJ[0];
            y = y - this->lastIandJ[1] ;
            this->addSubstractX = false;
            this->addSubstractY = true;
        }
    }
    else
    {
        if(this->addSubstractY)
        {
            x = x - this->lastIandJ[0];
            y = y + this->lastIandJ[1];
            this->addSubstractY = false;
        }
        else
        {
            x = x - this->lastIandJ[0];
            y = y - this->lastIandJ[1] ;
            this->roundDone = true;
        }

    }

    if(currentGame.isPassable(x, y))
    {
        return {x, y};
    }
    else
    {
        return currentGame.getNextCord(x, y);
    }
}

int gameState::getPlayerCount()
{
    return players;
}

void drawMiniMapBackground(sf::RectangleShape& miniMapPixel)
{
    if(!minimapTextureExist)
    {
        minimapTexture.clear(sf::Color(0,0,0,0));
        for(int j = 0; j < MAP_HEIGHT; j++)
        {
            for(int i = 0; i < MAP_WIDTH; i++ )
            {
                switch(currentGame.currentMap[i][j])
                {
                case 0:
                    miniMapPixel.setFillColor(sf::Color(0, 0, 0));
                    miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i,j,true)), static_cast<float>(miniMapSpace(i,j,false)));
                    minimapTexture.draw(miniMapPixel);
                    break;
                case 1:
                    miniMapPixel.setFillColor(sf::Color(152, 205, 115));
                    miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i,j,true)), static_cast<float>(miniMapSpace(i,j,false)));
                    minimapTexture.draw(miniMapPixel);
                    break;
                case 2:
                    miniMapPixel.setFillColor(sf::Color(200, 160, 80));
                    miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i,j,true)), static_cast<float>(miniMapSpace(i,j,false)));
                    minimapTexture.draw(miniMapPixel);
                    break;
                case 3:
                    miniMapPixel.setFillColor(sf::Color(200, 160, 80));
                    miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i,j,true)), static_cast<float>(miniMapSpace(i,j,false)));
                    minimapTexture.draw(miniMapPixel);
                    break;
                case 4:
                    miniMapPixel.setFillColor(sf::Color(200, 160, 80));
                    miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i,j,true)), static_cast<float>(miniMapSpace(i,j,false)));
                    minimapTexture.draw(miniMapPixel);
                    break;
                case 5:
                    miniMapPixel.setFillColor(sf::Color(200, 160, 80));
                    miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i,j,true)), static_cast<float>(miniMapSpace(i,j,false)));
                    minimapTexture.draw(miniMapPixel);
                    break;
                case 6:
                    miniMapPixel.setFillColor(sf::Color(200, 160, 80));
                    miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i,j,true)), static_cast<float>(miniMapSpace(i,j,false)));
                    minimapTexture.draw(miniMapPixel);
                    break;
                case 7:
                    miniMapPixel.setFillColor(sf::Color(69, 164, 208));
                    miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i,j,true)), static_cast<float>(miniMapSpace(i,j,false)));
                    minimapTexture.draw(miniMapPixel);
                    break;
                case 8:
                    miniMapPixel.setFillColor(sf::Color(69, 164, 208));
                    miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i,j,true)), static_cast<float>(miniMapSpace(i,j,false)));
                    minimapTexture.draw(miniMapPixel);
                    break;
                case 9:
                    miniMapPixel.setFillColor(sf::Color(69, 164, 208));
                    miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i,j,true)), static_cast<float>(miniMapSpace(i,j,false)));
                    minimapTexture.draw(miniMapPixel);
                    break;
                case 10:
                    miniMapPixel.setFillColor(sf::Color(69, 164, 208));
                    miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i,j,true)), static_cast<float>(miniMapSpace(i,j,false)));
                    minimapTexture.draw(miniMapPixel);
                    break;
                case 11:
                    miniMapPixel.setFillColor(sf::Color(69, 164, 208));
                    miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i,j,true)), static_cast<float>(miniMapSpace(i,j,false)));
                    minimapTexture.draw(miniMapPixel);
                    break;
                }
            }

        }
        minimapTexture.display();
        minimapTextureExist = true;
    }
}

void drawMiniMapBuildings(sf::RectangleShape& miniMapPixel)
{
    if(!noNewBuildings)
    {
        minimapBuildingsTexture.clear(sf::Color(0,0,0,0));
        for(int j = 0; j < MAP_HEIGHT; j++)
        {
            for(int i = 0; i < MAP_WIDTH; i++ )
            {
                if(currentGame.occupiedByBuildingList[i][j] != -1)
                {
                    switch(listOfBuildings[currentGame.occupiedByBuildingList[i][j]].getTeam())
                    {
                    case 0:
                        miniMapPixel.setFillColor(sf::Color(0, 0, 255));
                        miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i,j,true)), static_cast<float>(miniMapSpace(i,j,false)));
                        minimapBuildingsTexture.draw(miniMapPixel);
                        break;
                    case 1:
                        miniMapPixel.setFillColor(sf::Color(0, 255, 0));
                        miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i,j,true)), static_cast<float>(miniMapSpace(i,j,false)));
                        minimapBuildingsTexture.draw(miniMapPixel);
                        break;
                    case 2:
                        miniMapPixel.setFillColor(sf::Color(255, 0, 0));
                        miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i,j,true)), static_cast<float>(miniMapSpace(i,j,false)));
                        minimapBuildingsTexture.draw(miniMapPixel);
                        break;
                    case 3:
                        miniMapPixel.setFillColor(sf::Color(255, 255, 0 ));
                        miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i,j,true)), static_cast<float>(miniMapSpace(i,j,false)));
                        minimapBuildingsTexture.draw(miniMapPixel);
                        break;
                    case 4:
                        miniMapPixel.setFillColor(sf::Color(0, 255, 255));
                        miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i,j,true)), static_cast<float>(miniMapSpace(i,j,false)));
                        minimapBuildingsTexture.draw(miniMapPixel);
                        break;
                    case 5:
                        miniMapPixel.setFillColor(sf::Color(255, 0, 255));
                        miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i,j,true)), static_cast<float>(miniMapSpace(i,j,false)));
                        minimapBuildingsTexture.draw(miniMapPixel);
                        break;
                    case 6:
                        miniMapPixel.setFillColor(sf::Color(255, 127, 0));
                        miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i,j,true)), static_cast<float>(miniMapSpace(i,j,false)));
                        minimapBuildingsTexture.draw(miniMapPixel);
                        break;
                    case 7:
                        miniMapPixel.setFillColor(sf::Color(127, 127, 127));
                        miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i,j,true)), static_cast<float>(miniMapSpace(i,j,false)));
                        minimapBuildingsTexture.draw(miniMapPixel);
                        break;
                    }
                }
            }
        }
        minimapBuildingsTexture.display();
        noNewBuildings = true;
    }
}

void gameState::drawMiniMapActors(sf::RectangleShape& miniMapPixel)
{
    minimapActorsTexture.clear(sf::Color(0,0,0,0));
    for(int j = 0; j < MAP_HEIGHT; j++)
    {
        for(int i = 0; i < MAP_WIDTH; i++ )
        {
            if (this->visability[(i * MAP_HEIGHT) + j] > 1) {
                if (currentGame.occupiedByActorList[i][j] != -1)
                {
                    switch (listOfActors[currentGame.occupiedByActorList[i][j]].getTeam())
                    {
                    case 0:
                        miniMapPixel.setFillColor(sf::Color(0, 0, 255));
                        miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i, j, true)), static_cast<float>(miniMapSpace(i, j, false)));
                        minimapActorsTexture.draw(miniMapPixel);
                        break;
                    case 1:
                        miniMapPixel.setFillColor(sf::Color(0, 255, 0));
                        miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i, j, true)), static_cast<float>(miniMapSpace(i, j, false)));
                        minimapActorsTexture.draw(miniMapPixel);
                        break;
                    case 2:
                        miniMapPixel.setFillColor(sf::Color(255, 0, 0));
                        miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i, j, true)), static_cast<float>(miniMapSpace(i, j, false)));
                        minimapActorsTexture.draw(miniMapPixel);
                        break;
                    case 3:
                        miniMapPixel.setFillColor(sf::Color(255, 255, 0));
                        miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i, j, true)), static_cast<float>(miniMapSpace(i, j, false)));
                        minimapActorsTexture.draw(miniMapPixel);
                        break;
                    case 4:
                        miniMapPixel.setFillColor(sf::Color(0, 255, 255));
                        miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i, j, true)), static_cast<float>(miniMapSpace(i, j, false)));
                        minimapActorsTexture.draw(miniMapPixel);
                        break;
                    case 5:
                        miniMapPixel.setFillColor(sf::Color(255, 0, 255));
                        miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i, j, true)), static_cast<float>(miniMapSpace(i, j, false)));
                        minimapActorsTexture.draw(miniMapPixel);
                        break;
                    case 6:
                        miniMapPixel.setFillColor(sf::Color(255, 127, 0));
                        miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i, j, true)), static_cast<float>(miniMapSpace(i, j, false)));
                        minimapActorsTexture.draw(miniMapPixel);
                        break;
                    case 7:
                        miniMapPixel.setFillColor(sf::Color(127, 127, 127));
                        miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i, j, true)), static_cast<float>(miniMapSpace(i, j, false)));
                        minimapActorsTexture.draw(miniMapPixel);
                        break;
                    }
                }
            }
        }
    }
    minimapActorsTexture.display();
}

void gameState::drawMiniMapMist(sf::RectangleShape& miniMapPixel)
{
    if (this->lastMistDraw + 1 < this->elapsedTime) {
        sf::RectangleShape miniMapPixelTL(sf::Vector2f(20.f, 10.f));
        miniMapPixel.setFillColor(sf::Color(0, 0, 0));
        miniMapPixelTL.setFillColor(sf::Color(0, 0, 0, 75));
        minimapMistTexture.clear(sf::Color(0, 0, 0, 0));
        this->lastMistDraw = this->elapsedTime;
        for (int j = 0; j < MAP_HEIGHT; j++)
        {
            for (int i = 0; i < MAP_WIDTH; i++)
            {
                if (this->visability[(i * MAP_HEIGHT) + j] == 0) {
                    miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i, j, true)), static_cast<float>(miniMapSpace(i, j, false)));
                    minimapMistTexture.draw(miniMapPixel);
                }
                else if (this->visability[(i * MAP_HEIGHT) + j] == 1) {
                    miniMapPixelTL.setPosition(static_cast<float>(miniMapSpace(i, j, true)), static_cast<float>(miniMapSpace(i, j, false)));
                    minimapMistTexture.draw(miniMapPixelTL);
                }
            }
        }
    }
    minimapMistTexture.display();
}

void drawMiniMapObjects(sf::RectangleShape& miniMapPixel)
{

    minimapObjectsTexture.clear(sf::Color(0,0,0,0));
    for(int j = 0; j < MAP_HEIGHT; j++)
    {
        for(int i = 0; i < MAP_WIDTH; i++ )
        {
            if(currentGame.objectLocationList[i][j] != -1)
            {
                switch(listOfObjects[currentGame.objectLocationList[i][j]].getTypeOfResource())
                {
                case resourceTypes::resourceWood:
                    miniMapPixel.setFillColor(sf::Color(33, 77, 33));
                    miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i,j,true)), static_cast<float>(miniMapSpace(i,j,false)));
                    minimapObjectsTexture.draw(miniMapPixel);
                    break;
                case resourceTypes::resourceFood:
                    miniMapPixel.setFillColor(sf::Color(150, 88, 88));
                    miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i,j,true)), static_cast<float>(miniMapSpace(i,j,false)));
                    minimapObjectsTexture.draw(miniMapPixel);
                    break;
                case resourceTypes::resourceStone:
                    miniMapPixel.setFillColor(sf::Color(65, 65, 65));
                    miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i,j,true)), static_cast<float>(miniMapSpace(i,j,false)));
                    minimapObjectsTexture.draw(miniMapPixel);
                    break;
                case resourceTypes::resourceGold:
                    miniMapPixel.setFillColor(sf::Color(110, 90, 0 ));
                    miniMapPixel.setPosition(static_cast<float>(miniMapSpace(i,j,true)), static_cast<float>(miniMapSpace(i,j,false)));
                    minimapObjectsTexture.draw(miniMapPixel);
                    break;
                }
            }
        }
    }
    minimapObjectsTexture.display();
}

void gameState::drawMiniMap()
{
    window.setView(miniMap);
    sf::RectangleShape miniMapPixel(sf::Vector2f(20.f,10.f));
    sf::Sprite miniMapBackground;

    drawMiniMapBackground(miniMapPixel);
    miniMapBackground.setTexture(minimapTexture.getTexture());
    miniMapBackground.setScale(this->miniMapWidth/(20*MAP_WIDTH), this->miniMapHeigth/(10*MAP_HEIGHT));
    window.draw(miniMapBackground);

    drawMiniMapObjects(miniMapPixel);
    miniMapBackground.setTexture(minimapObjectsTexture.getTexture());
    miniMapBackground.setScale(this->miniMapWidth/(20*MAP_WIDTH), this->miniMapHeigth/(10*MAP_HEIGHT));
    window.draw(miniMapBackground);

    drawMiniMapBuildings(miniMapPixel);
    miniMapBackground.setTexture(minimapBuildingsTexture.getTexture());
    miniMapBackground.setScale(this->miniMapWidth/(20*MAP_WIDTH), this->miniMapHeigth/(10*MAP_HEIGHT));
    window.draw(miniMapBackground);

    drawMiniMapActors(miniMapPixel);
    miniMapBackground.setTexture(minimapActorsTexture.getTexture());
    miniMapBackground.setScale(this->miniMapWidth / (20 * MAP_WIDTH), this->miniMapHeigth / (10 * MAP_HEIGHT));
    window.draw(miniMapBackground);

    drawMiniMapMist(miniMapPixel);
    miniMapBackground.setTexture(minimapMistTexture.getTexture());
    miniMapBackground.setScale(this->miniMapWidth / (20 * MAP_WIDTH), this->miniMapHeigth / (10 * MAP_HEIGHT));
    window.draw(miniMapBackground);
  

    sf::RectangleShape viewBox(sf::Vector2f(this->viewBoxX, this->viewBoxY));
    viewBox.setOrigin(sf::Vector2f(viewBoxX/2.f, viewBoxY/2.f));
    viewBox.setFillColor(sf::Color(0,0,0,0));
    viewBox.setOutlineThickness(3.f);
    viewBox.setOutlineColor(sf::Color(255,255,255));
    float xBoxLocation = (viewOffsetX/ static_cast<float>(this->mapPixelWidth)*this->miniMapWidth);
    float yBoxLocation = (viewOffsetY/ static_cast<float>(this->mapPixelHeigth)*this->miniMapHeigth);
    viewBox.setPosition(xBoxLocation, yBoxLocation);
    window.draw(viewBox);
    window.setView(worldView);
}

void createVillagerButtons(int& startX, int& startY, int& incrementalXOffset,  bool& villagerButtonsAreThere)
{
    if (!villagerButtonsAreThere) {
        int startXOr = startX;
        button newButton = { startX, startY, spriteTypes::spriteTownCenter, actionTypes::actionBuildTownCenter, 0, static_cast<int>(listOfButtons.size()),0 };
        listOfButtons.push_back(newButton);
        startX += incrementalXOffset;
        button newButton1 = { startX, startY, spriteTypes::spriteHouse, actionTypes::actionBuildHouse, 0, static_cast<int>(listOfButtons.size()),0 };
        listOfButtons.push_back(newButton1);
        startX += incrementalXOffset;
        button newButton2 = { startX, startY, spriteTypes::spriteMill, actionTypes::actionBuildMill, 0, static_cast<int>(listOfButtons.size()),0 };
        listOfButtons.push_back(newButton2);
        startX += incrementalXOffset;
        button newButton3 = { startX, startY, spriteTypes::spriteLumberCamp, actionTypes::actionBuildLumberCamp, 0, static_cast<int>(listOfButtons.size()),0 };
        listOfButtons.push_back(newButton3);
        startX += incrementalXOffset;
        button newButton4 = { startX, startY, spriteTypes::spriteBarracks, actionTypes::actionBuildBarracks, 0, static_cast<int>(listOfButtons.size()),0 };
        listOfButtons.push_back(newButton4);
        villagerButtonsAreThere = true;
        startX = startXOr;
        startY += incrementalXOffset;
        button newButton5 = { startX, startY, spriteTypes::spriteMiningCamp, actionTypes::actionBuildMiningCamp, 0, static_cast<int>(listOfButtons.size()),0 };
        listOfButtons.push_back(newButton5);
        villagerButtonsAreThere = true;
    }
}

void addActorSelectorButton(int& actorId, int& startDeck, int& tempY, int& startY, int& incrementalYOffset, int& offSetToNextCard)
{
    int buttonType = 0;
    //Speelruimte is 730 pixels = 1920/2.63 = cardDecksize
    switch (listOfActors[actorId].getType())
    {
    case 0:
        buttonType = 2;
        break;
    case 1:
        buttonType = 7;
        break;
    }
    button newButton = { startDeck, tempY, static_cast<spriteTypes>(buttonType), actionTypes::actionUnitSelect, actorId, static_cast<int>(listOfButtons.size()),0 };
    listOfButtons.push_back(newButton);
    if (tempY == startY)
    {
        tempY += incrementalYOffset;
    }
    else
    {
        startDeck += offSetToNextCard;
        tempY = startY;
    }
}

int getActorSpriteOffSet(int& actorId)
{
    switch (listOfActors[actorId].getType())
    {
    case 0:
        return  0;
    case 1:
        return 128;
    }
    return -1;
}

void gameState::drawActorTitle(int& actorId, int& textStartX, int& textStartY)
{
    text.setString(listOfActors[actorId].nameOfActor());
    text.setCharacterSize(26);
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(2.f);
    text.setFillColor(sf::Color::White);
    text.setPosition(static_cast<float>(textStartX), static_cast<float>(textStartY));
    window.draw(text);
}

void gameState::drawActorBigSprite(int& actorId)
{
    this->spriteBigSelectedIcon.setTextureRect(sf::IntRect(128, getActorSpriteOffSet(actorId), 128, 128));
    this->spriteBigSelectedIcon.setPosition(static_cast<float>(mainWindowWidth / 4.08), static_cast<float>(mainWindowHeigth / 30));
    window.draw(this->spriteBigSelectedIcon);
}

void gameState::drawActorStats(int& actorId, int& textStartX, int& textStartY)
{
    text.setCharacterSize(18);
    std::stringstream healthText;
    healthText << "Hitpoints: " << listOfActors[actorId].getHealth().first << "/" << listOfActors[actorId].getHealth().second;
    text.setString(healthText.str());
    textStartY += 50;
    text.setPosition(static_cast<float>(textStartX), static_cast<float>(textStartY));
    window.draw(text);
    textStartY += 20;
    std::stringstream attakPoints;
    attakPoints << "Melee damage: " << listOfActors[actorId].getMeleeDMG();
    text.setString(attakPoints.str());
    text.setPosition(static_cast<float>(textStartX), static_cast<float>(textStartY));
    window.draw(text);
    std::stringstream rangedDamage;
    rangedDamage << "Ranged damage: " << listOfActors[actorId].getRangedDMG();
    textStartY += 20;
    text.setString(rangedDamage.str());
    text.setPosition(static_cast<float>(textStartX), static_cast<float>(textStartY));
    window.draw(text);
    std::stringstream teamId;
    teamId << "Team: " << listOfActors[actorId].getTeam();
    textStartY += 20;
    text.setString(teamId.str());
    text.setPosition(static_cast<float>(textStartX), static_cast<float>(textStartY));
    window.draw(text);
    if (listOfActors[actorId].getType() == 0) {
        textStartY += 20;
        text.setString(listOfActors[actorId].getRecources());
        text.setPosition(static_cast<float>(textStartX), static_cast<float>(textStartY));
        window.draw(text);
    }
}

void gameState::drawActorToolbar(int &startX, int &startY, int &incrementalXOffset, int &spriteYOffset, int &startDeck, int &tempY, int &incrementalYOffset, int &offSetTonextCard)
{
    bool villagerButtonsAreThere = false;
    int textStartX = static_cast<int>((mainWindowWidth / 4.08) + (128 + (mainWindowWidth / 160)));
    int textStartY = static_cast<int>(mainWindowHeigth / 30);
    for (int i = 0; i < this->selectedUnits.size(); i++)
    {
        if (listOfActors[this->selectedUnits[i]].getType() == 0 && listOfActors[this->selectedUnits[i]].getTeam() == currentPlayer.getTeam())
        {
            createVillagerButtons(startX, startY, incrementalXOffset, villagerButtonsAreThere);
        }
        if (this->selectedUnits.size() > 1)
        {
            startY = mainWindowHeigth / 30;
            addActorSelectorButton(this->selectedUnits[i], startDeck, tempY, startY, incrementalYOffset, offSetTonextCard);
        }
        else if (i == 0) {
            drawActorBigSprite(this->selectedUnits[i]);
            drawActorTitle(this->selectedUnits[i], textStartX, textStartY);
            drawActorStats(this->selectedUnits[i], textStartX, textStartY);
        }
    }
}

int getBuildingSpriteOffset(int& buildingId)
{
    switch (listOfBuildings[buildingId].getType())
    {
    case 0:
        return  128;
        break;
    case 1:
        return  0;
        break;
    case 2:
        return  384;
        break;
    case 3:
        return  256;
        break;
    case 4:
        return  512;
        break;
    case 5:
        return  640;
        break;
    case 6:
        return  640;
        break;

    }
    return -1;
}

void createBuildingButtons(int& buildingId, int& startX, int& startY)
{
    switch (listOfBuildings[buildingId].getType())
    {
    case 0:
        break;
    case 1:
        //town center
        if (listOfBuildings[buildingId].getCompleted())
        {
            button makeVillager = { startX, startY, spriteTypes::spriteVillager, actionTypes::actionMakeVillager, buildingId, static_cast<int>(listOfButtons.size()),0 };
            listOfButtons.push_back(makeVillager);
            //research will also go here
        }
        break;
    case 2:
        //mill
        break;
    case 3:
        //lumbercamp
        break;
    case 4:
        //Barracks
        if (listOfBuildings[buildingId].getCompleted())
        {
            button makeSwordsman = { startX, startY, spriteTypes::spriteSwordsman, actionTypes::actionMakeSwordsman, buildingId, static_cast<int>(listOfButtons.size()),0 };
            listOfButtons.push_back(makeSwordsman);
            //research will also go here
        }
        break;
    case 5:
        //mingincamp
        break;
    case 6:
        //mingincamp
        break;
    default:
        break;
    }
}

void gameState::drawBuildingBigSprite(int& buildingId)
{
    this->spriteBigSelectedIcon.setTextureRect(sf::IntRect(0, getBuildingSpriteOffset(buildingId), 128, 128));
    this->spriteBigSelectedIcon.setPosition(static_cast<float>(mainWindowWidth / 4.08), static_cast<float>(mainWindowHeigth / 30));
    window.draw(this->spriteBigSelectedIcon);
}

void gameState::drawBuildingToolbarTitle(int& textStartX, int& textStartY)
{
    text.setString(listOfBuildings[this->buildingSelectedId].getName());
    text.setCharacterSize(26);
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(2.f);
    text.setFillColor(sf::Color::White);
    text.setPosition(static_cast<float>(textStartX), static_cast<float>(textStartY));
    window.draw(text);
}

void gameState::drawBuildingToolbarStats(int& textStartX, int& textStartY)
{
    text.setCharacterSize(18);
    std::stringstream healthText;
    healthText << "Hitpoints: " << listOfBuildings[this->buildingSelectedId].getHealth().first << "/" << listOfBuildings[this->buildingSelectedId].getHealth().second;
    text.setString(healthText.str());
    textStartY += 50;
    text.setPosition(static_cast<float>(textStartX), static_cast<float>(textStartY));
    window.draw(text);
    textStartY += 20;
    std::stringstream attakPoints;
    attakPoints << "Occupants: TBI";
    text.setString(attakPoints.str());
    text.setPosition(static_cast<float>(textStartX), static_cast<float>(textStartY));
    window.draw(text);
    std::stringstream rangedDamage;
    rangedDamage << "Ranged damage: " << listOfBuildings[this->buildingSelectedId].getRangedDMG();
    textStartY += 20;
    text.setString(rangedDamage.str());
    text.setPosition(static_cast<float>(textStartX), static_cast<float>(textStartY));
    window.draw(text);
    std::stringstream teamId;
    teamId << "Team: " << listOfBuildings[this->buildingSelectedId].getTeam();
    textStartY += 20;
    text.setString(teamId.str());
    text.setPosition(static_cast<float>(textStartX), static_cast<float>(textStartY));
    window.draw(text);
}

void gameState::drawProgressBar(float pointsGained, float pointsRequired, int& totalBarLength, int& startBarX, int& startBarY)
{
    sf::RectangleShape totalBar(sf::Vector2f(static_cast<float>(totalBarLength), 18.f));
    sf::RectangleShape completeBar(sf::Vector2f((pointsGained /pointsRequired ) * totalBarLength, 18.f));
    totalBar.setFillColor(sf::Color(255, 0, 0));
    totalBar.setOutlineThickness(1.f);
    totalBar.setOutlineColor(sf::Color(0, 0, 0));
    totalBar.setPosition(static_cast<float>(startBarX), static_cast<float>(startBarY));
    completeBar.setFillColor(sf::Color(0, 255, 0));
    completeBar.setOutlineThickness(1.f);
    completeBar.setOutlineColor(sf::Color(0, 0, 0));
    completeBar.setPosition(static_cast<float>(startBarX), static_cast<float>(startBarY));
    window.draw(totalBar);
    window.draw(completeBar);
}

std::string getBuildingIsProducingName(int& buildingId)
{
    if (listOfBuildings[buildingId].productionQueue.front().isResearch)
    {
        switch (listOfBuildings[buildingId].productionQueue.front().idOfUnitOrResearch)
        {
        case 0:
            return "placeholder name";
            break;
        default:
            return "Name of research not set";
        }
    }
    else
    {
        switch (listOfBuildings[buildingId].productionQueue.front().idOfUnitOrResearch)
        {
        case 0:
            return "Villager";
        case 1:
            return "Swordsman";
        default:
            return "Name of production not set";
        }
    }
}

cords getSpriteOffSetTask(int& buildingId)
{
    if (listOfBuildings[buildingId].productionQueue.front().isResearch)
    {
        switch (listOfBuildings[buildingId].productionQueue.front().idOfUnitOrResearch)
        {
        case 0:
            return { 64, 0 };//placeholder change to icon of research 
        default:
            return { 64, 0 };//placeholder change to icon of research 
            break;
        }
    }
    else
    {
        switch (listOfBuildings[buildingId].productionQueue.front().idOfUnitOrResearch)
        {
        case 0:
            return { 64, 0 };
        case 1:
            return { 64, 64 };
        default:
            return { 64, 0 };//placeholder change to icon of research 
            break;
        }
    }
}

int getCardForButtonByTask(int& buildingId, int& taskId)
{
    if (listOfBuildings[buildingId].productionQueue[taskId].isResearch)
    {
        switch (listOfBuildings[buildingId].productionQueue[taskId].idOfUnitOrResearch)
        {
        case 0:
            return 0;
            break;
        default:
            return 0;
            break;
        }
    }
    else
    {
        switch (listOfBuildings[buildingId].productionQueue[taskId].idOfUnitOrResearch)
        {
        case 0:
            return 2;
        case 1:
            return 7;
        default:
            return 0;
            break;
        }
    }
}

void gameState::drawBuildingTaskToolbar(int& startDeck, int& startY)
{
    int iconStartX = startDeck + static_cast<int>(mainWindowWidth / 30);
    int iconStartY = startY + static_cast<int>(mainWindowHeigth / 27);
    int startBarX = iconStartX + static_cast<int>(mainWindowWidth / 25.6);
    int startBarY = iconStartY + static_cast<int>(mainWindowHeigth / 46.9);
    int totalBarLength = static_cast<int>(mainWindowWidth / 6.4f);
    float percentageCompleted = static_cast<float>(static_cast<float>(listOfBuildings[this->buildingSelectedId].productionQueue.front().productionPointsGained) / static_cast<float>(listOfBuildings[this->buildingSelectedId].productionQueue.front().productionPointsNeeded)) * 100;
    int tempXOffset = 0;
    int tempYOffset = 0;
    drawProgressBar(static_cast<float>(listOfBuildings[this->buildingSelectedId].productionQueue.front().productionPointsGained), static_cast<float>(listOfBuildings[this->buildingSelectedId].productionQueue.front().productionPointsNeeded), totalBarLength, startBarX, startBarY);
    text.setString("Producing: " + getBuildingIsProducingName(this->buildingSelectedId) + " " + std::to_string(static_cast<int>(percentageCompleted)) + "%...");
    text.setPosition(static_cast<float>(startBarX), static_cast<float>(iconStartY));
    window.draw(text);
    this->spriteUIButton.setTextureRect(sf::IntRect(getSpriteOffSetTask(this->buildingSelectedId).x, getSpriteOffSetTask(this->buildingSelectedId).y, 64, 64));
    this->spriteUIButton.setPosition(static_cast<float>(iconStartX), static_cast<float>(iconStartY));
    window.draw(this->spriteUIButton);
    button cancelTask = { static_cast<int>(startBarX + totalBarLength + static_cast<int>(mainWindowWidth / 174.54)), iconStartY, spriteTypes::spriteCancel, actionTypes::actionCancelProduction, this->buildingSelectedId, static_cast<int>(listOfButtons.size()), 0 };
    listOfButtons.push_back(cancelTask);
    if (listOfBuildings[this->buildingSelectedId].productionQueue.size() > 1)
    {
        tempXOffset = iconStartX + static_cast<int>(mainWindowWidth / 24.93);
        tempYOffset = iconStartY + static_cast<int>(mainWindowHeigth / 22.97);
        for (int i = 1; i < listOfBuildings[this->buildingSelectedId].productionQueue.size(); i++)
        {
            button tempButton = { tempXOffset, tempYOffset, static_cast<spriteTypes>(getCardForButtonByTask(this->buildingSelectedId, i)), actionTypes::actionCancelProduction, this->buildingSelectedId, static_cast<int>(listOfButtons.size()), i };
            listOfButtons.push_back(tempButton);
            tempXOffset += 64 + static_cast<int>(mainWindowWidth / 160);
        }
    }
}

void gameState::drawBuildingConstructionToolbar(int& startDeck, int& startY)
{
    int iconStartX = startDeck + static_cast<int>(mainWindowWidth / 30);
    int iconStartY = startY + static_cast<int>(mainWindowHeigth / 27);
    int startBarX = iconStartX + static_cast<int>(mainWindowWidth / 25.6);
    int startBarY = iconStartY + static_cast<int>(mainWindowHeigth / 46.9);
    int totalBarLength = static_cast<int>(mainWindowWidth / 6.4f);
    drawProgressBar(static_cast<float>(listOfBuildings[this->buildingSelectedId].getBuildingPoints().first), static_cast<float>(listOfBuildings[this->buildingSelectedId].getBuildingPoints().second), totalBarLength, startBarX, startBarY);
    float percentageCompleted = static_cast<float>(static_cast<float>(listOfBuildings[this->buildingSelectedId].getBuildingPoints().first) / static_cast<float>(listOfBuildings[this->buildingSelectedId].getBuildingPoints().second)) * 100;
    text.setString("Building: " + listOfBuildings[this->buildingSelectedId].getName() + " " + std::to_string(static_cast<int>(percentageCompleted)) + "%...");
    text.setPosition(static_cast<float>(startBarX), static_cast<float>(iconStartY));
    window.draw(text);
    this->spriteUIButton.setTextureRect(sf::IntRect(0, (getBuildingSpriteOffset(this->buildingSelectedId) / 2), 64, 64));
    this->spriteUIButton.setPosition(static_cast<float>(iconStartX), static_cast<float>(iconStartY));
    window.draw(this->spriteUIButton);
    button cancelBuilding = { static_cast<int>(startBarX + totalBarLength + (mainWindowWidth / 174.54)), iconStartY, spriteTypes::spriteCancel, actionTypes::actionCancelBuilding, this->buildingSelectedId, static_cast<int>(listOfButtons.size()),0 };
    listOfButtons.push_back(cancelBuilding);
}

void gameState::drawBuildingToolbar(int& startX, int& startY, int& incrementalXOffset, int& spriteYOffset, int& startDeck, int& tempY, int& incrementalYOffset, int& offSetTonextCard)
{
    int textStartX = static_cast<int>((mainWindowWidth / 4.08) + (128 + (mainWindowWidth / 160)));
    int textStartY = static_cast<int>(mainWindowHeigth / 30);
    createBuildingButtons(this->buildingSelectedId, startX, startY);
    drawBuildingBigSprite(this->buildingSelectedId);
    drawBuildingToolbarTitle(textStartX, textStartY);
    drawBuildingToolbarStats(textStartX, textStartY);

    //Show what the building is doing ATM
    if (!listOfBuildings[this->buildingSelectedId].getCompleted())
    {
        drawBuildingConstructionToolbar(startDeck, startY);
    }
    else if (listOfBuildings[this->buildingSelectedId].hasTask())
    {
        drawBuildingTaskToolbar(startDeck, startY);
    }
}

int getObjectBigSpriteYOffset(int& objectId)
{
    switch (listOfObjects[objectId].getType())
    {
    case objectTypes::objectCactus:
        return 0;
        break;
    case objectTypes::objectCypress:
        return 128;
        break;
    case objectTypes::objectMaple:
        return 256;
        break;
    case objectTypes::objectPine:
        return 384;
        break;
    case objectTypes::objectStone:
        return 512;
        break;
    case objectTypes::objectGold:
        return 640;
        break;
    case objectTypes::objectBerry:
        return 768;
        break;
    default:
        return 0;
        break;
    }
}

void gameState::drawObjectToolbar(int& startX, int& startY, int& incrementalXOffset, int& spriteYOffset, int& startDeck, int& tempY, int& incrementalYOffset, int& offSetTonextCard)
{
    this->spriteBigSelectedIcon.setTextureRect(sf::IntRect(256, getObjectBigSpriteYOffset(this->objectSelectedId), 128, 128));
    this->spriteBigSelectedIcon.setPosition(static_cast<float>(mainWindowWidth / 4.08), static_cast<float>(mainWindowHeigth / 30));
    window.draw(this->spriteBigSelectedIcon);
    text.setString(listOfObjects[this->objectSelectedId].getName());
    text.setCharacterSize(26);
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(2.f);
    text.setFillColor(sf::Color::White);
    int textStartX = static_cast<int>((mainWindowWidth / 4.08) + (128 + (mainWindowWidth / 160)));
    int textStartY = static_cast<int>(mainWindowHeigth / 30);
    text.setPosition(static_cast<float>(textStartX), static_cast<float>(textStartY));
    window.draw(text);
    text.setCharacterSize(18);
    std::stringstream resourcesLeftText;
    resourcesLeftText << listOfObjects[this->objectSelectedId].nameOfResource() << " left: " << listOfObjects[this->objectSelectedId].amountOfResourcesLeft();
    text.setString(resourcesLeftText.str());
    textStartY += 50;
    text.setPosition(static_cast<float>(textStartX), static_cast<float>(textStartY));
    window.draw(text);
}

void drawButtons()
{
    for (auto& Button : listOfButtons)
    {
        Button.drawButton();
    }
}

void gameState::drawToolbar()
{
    listOfButtons.clear();
    window.setView(toolBar);
    int startX = mainWindowWidth/60;
    int startY = mainWindowHeigth/30;
    int tempY = startY;
    int incrementalXOffset = 64+(mainWindowWidth/160);
    int incrementalYOffset = 64+(mainWindowHeigth/90);
    int spriteYOffset;
    int cardDeckSize = static_cast<int>(mainWindowWidth / 1.82);
    int amountOfCardsPerRow = static_cast<int>((this->selectedUnits.size()+1)/2);
    int requiredSize = amountOfCardsPerRow*64;
    int devider = static_cast<int>((this->selectedUnits.size()+1)/2);
    if(devider == 0) devider = 1;
    int spaceBetweenCards = (cardDeckSize - requiredSize)/devider;
    int offSetTonextCard = 64 + spaceBetweenCards;
    int startDeck = static_cast<int>(mainWindowWidth / 4.2);
    int startProgress = static_cast<int>(mainWindowWidth / 2.48);

    if(!this->selectedUnits.empty())
    {
        drawActorToolbar(startX, startY, incrementalXOffset, spriteYOffset, startDeck, tempY, incrementalYOffset, offSetTonextCard);
    }
    else if(this->buildingSelectedId != -1)
    {
        drawBuildingToolbar(startX, startY, incrementalXOffset, spriteYOffset, startProgress, tempY, incrementalYOffset, offSetTonextCard);
    }
    else if(this->objectSelectedId != -1)
    {
        drawObjectToolbar(startX, startY, incrementalXOffset, spriteYOffset, startProgress, tempY, incrementalYOffset, offSetTonextCard);
    }
    drawButtons();
    window.setView(worldView);
}

void gameState::drawTopBar()
{
    window.setView(topBar);
    playerStats tempStats = currentPlayer.getStats();
    std::stringstream resourcesText;
    int seconds = static_cast<int>(currentGame.elapsedTime); //Tijd in seconden
    int minutes = 0;
    int hours = 0;
    if (seconds >= 60) {
        minutes = seconds / 60;
        seconds = seconds - (minutes*60);
    }
    if (minutes >= 60) {
        hours = minutes/60;
        minutes = minutes - (hours*60);
    }
    //vanwege esthetiek
    std::stringstream  time;
    if (hours < 10) {
        time << "0";
    }
    time << hours << ":";
    if (minutes < 10) {
        time << "0";
    }
    time << minutes << ":";
    if (seconds < 10) {
        time << "0";
    }
    time << seconds;

    resourcesText << "|  Wood: " << tempStats.amountOfWood << " (" << currentPlayer.getTotalGatheringWood() <<
        ")  |  Food: " << tempStats.amountOfFood << " (" << currentPlayer.getTotalGatheringFood() <<
        ")  |  Stone: " << tempStats.amountOfStone << " (" << currentPlayer.getTotalGatheringStone() <<
        ")  |  Gold: " << tempStats.amountOfGold << " (" << currentPlayer.getTotalGatheringGold() <<
        ")  |                |  Population: " << tempStats.currentPopulation << "/" << tempStats.populationRoom <<
        "  |  Idle: " << currentPlayer.getIdleVillagers() <<
        "  |  Building: " << currentPlayer.getTotalBuilding() <<
        "  |                |  Team: " << tempStats.team << " |" <<
        "  |                |  Time: " << time.str() << " |";

    text.setString(resourcesText.str());
    text.setCharacterSize(24);
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(1.f);
    text.setFillColor(sf::Color::White);
    text.setPosition(10, 0);
    window.draw(text);
    window.setView(worldView);
}

void gameState::drawToolTip()
{
    if (mouseFakePosition.y > mainWindowHeigth * 0.8)
    {
        for (auto& Button : listOfButtons)
        {
            Button.isHoverd(mouseFakePosition);
        }
    }
}

void gameState::drawPaths()
{
    if (this->showPaths) {
        for (int i = 0; i < listOfActors.size(); i++)
        {
            listOfActors[i].renderPath();
        }
    }
}

void drawProjectiles()
{
    for (int i = 0; i < listOfProjectiles.size(); i++) {
        listOfProjectiles[i].drawProjectile();
    }
}

void drawCommandCursors()
{
    for (int i = 0; i < listOfOrderCursors.size(); i++)
    {
        listOfOrderCursors[i].drawCursor();
    }
}

void gameState::drawGame()
{
    window.clear(sf::Color(0, 0, 0));
    worldView.setCenter(static_cast<float>(viewOffsetX), static_cast<float>(viewOffsetY));
    window.setView(totalView);
    window.draw(spriteTotalBackground);
    window.setView(worldView);
    createFogOfWar();
    drawTopBar();
    drawMap();
    drawProjectiles();
    drawToolbar();
    drawMiniMap();
    drawMouseInteraction();
    drawCommandCursors();
    window.setView(totalView);
    gameText.drawMessages();
    drawToolTip();
    window.setView(worldView);
    drawPaths();
    window.display();
}

float gameState::getTime()
{
    return this->elapsedTime;
}

void gameState::loadMap()
{
    for(int i = 0; i < MAP_HEIGHT; i++)
    {
        for(int j = 0; j < MAP_WIDTH; j++)
        {
            this->currentMap[j][i] = 1;
            this->buildingLocationList[j][i] = -1;
            this->occupiedByBuildingList[j][i] = -1;
            this->objectLocationList[j][i] = -1;
            this->occupiedByActorList[j][i] = -1;
            this->visability.push_back(0);
        }
    }
    generateRandomMap(this->players,16,16,16,5,0);
}

void gameState::loadBuildings()
{
    //food, wood, stone, gold
    //house 0
    footprintOfBuildings.push_back({2,2});
    priceOfBuilding.push_back({0,25,0,0});
    //towncenter 1
    footprintOfBuildings.push_back({4,4});
    priceOfBuilding.push_back({0,300,100,0});
    //mill 2
    footprintOfBuildings.push_back({ 3,3 });
    priceOfBuilding.push_back({ 0,100,0,0 });
    //Lumbercamp 3
    footprintOfBuildings.push_back({ 3,3 });
    priceOfBuilding.push_back({ 0,100,0,0 });
    //barracks 4
    footprintOfBuildings.push_back({ 3,3 });
    priceOfBuilding.push_back({ 0,175,0,0 });
    //Mining camp 5
    footprintOfBuildings.push_back({ 3,3 });
    priceOfBuilding.push_back({ 0,100,0,0 });
    //Mining camp 6
    footprintOfBuildings.push_back({ 3,3 });
    priceOfBuilding.push_back({ 0,100,0,0 });
}

void loadActors()
{
    //food, wood, stone, gold, production points
    priceOfActor.push_back({ 50,0,0,0,25 });   //villager    0
    priceOfActor.push_back({ 60,0,0,20,25 });  //Swordsman   1
}

void gameState::loadFonts()
{
    if (!this->font.loadFromFile("fonts/arial.ttf"))
    {
        std::cout << "Could not load font arial.ttf!\n";
    }
    this->text.setFont(this->font);
}

void setViewports()
{
    totalView.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
    worldView.setViewport(sf::FloatRect(0.f, 0.03f, 1.f, 0.77f));
    topBar.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 0.03f));
    toolBar.setViewport(sf::FloatRect(0.f, 0.8f, 1.f, 0.2f));
    miniMap.setViewport(sf::FloatRect(0.8f, 0.8f, 0.2f, 0.2f));
    window.setFramerateLimit(60);
}

void createMiniMapTexture()
{
    minimapTexture.create(20 * MAP_WIDTH, 10 * MAP_HEIGHT);
    minimapActorsTexture.create(20 * MAP_WIDTH, 10 * MAP_HEIGHT);
    minimapMistTexture.create(20 * MAP_WIDTH, 10 * MAP_HEIGHT);
    minimapObjectsTexture.create(20 * MAP_WIDTH, 10 * MAP_HEIGHT);
    minimapBuildingsTexture.create(20 * MAP_WIDTH, 10 * MAP_HEIGHT);
}

void gameState::setDefaultValues()
{
    this->focus = true;
    this->mousePressedLeft = false;
    this->equalIsPressed = false;
    this->isPressedB = false;
    this->mousePressedRight = false;
    this->mapPixelHeigth = MAP_HEIGHT * 32;
    this->mapPixelWidth = MAP_WIDTH * 64;
    this->miniMapWidth = mainWindowWidth * 0.2f;
    this->miniMapHeigth = mainWindowHeigth * 0.2f;
    this->topBarHeigth = mainWindowHeigth * 0.03f;
    this->viewBoxX = mainWindowWidth / (this->mapPixelWidth / this->miniMapWidth);
    this->viewBoxY = (mainWindowHeigth * 0.77f) / (this->mapPixelHeigth / this->miniMapHeigth);
    this->toolBarWidth = static_cast<int>(mainWindowWidth - miniMapWidth);
    this->isPlacingBuilding = false;
    this->buildingSelectedId = -1;
    this->objectSelectedId = -1;
    this->buildingTypeSelected = 0;
    this->objectTypeSelected = 0;
    this->showPaths = false;
    this->lastMistDraw = -1.0f;
    listOfBuildings.resize(1);
    listOfObjects.resize(1);
    this->players = 2;
    this->noFogOfWar = false;
}

void setTeam() {
    for (int i = 0; i < 8; i++)
    {
        listOfPlayers[i].setTeam(i);
    }
}

void showLoadingScreen() {
    currentGame.text.setCharacterSize(64);
    currentGame.text.setOutlineColor(sf::Color::White);
    currentGame.text.setOutlineThickness(1.f);
    currentGame.text.setFillColor(sf::Color::Yellow);
    currentGame.text.setString("Loading...");
    sf::FloatRect textRect = currentGame.text.getLocalBounds();
    currentGame.text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    currentGame.text.setPosition(static_cast<float>(mainWindowWidth/2), static_cast<float>(mainWindowHeigth/2));
    window.clear();
    window.draw(currentGame.text);
    window.display();
    currentGame.text.setOrigin(0.0f, 0.0f);
}

void gameState::loadGame()
{
    loadFonts();
    setViewports();
    showLoadingScreen();
    createMiniMapTexture();
    loadTextures();
    loadActors();
    setDefaultValues();
    loadBuildings();
    loadMap();
    setTeam();
}

void gameState::createFogOfWar() 
{
    if (lastFogOfWarUpdated + 0.5f < this->elapsedTime) {
        if (!noFogOfWar) {
            for (int i = 0; i < MAP_HEIGHT * MAP_WIDTH; i++) {
                if (this->visability[i] == 2) {
                    this->visability[i] = 1;
                }
            }
            for (int i = 0; i < listOfActors.size(); i++) {
                if (listOfActors[i].getTeam() == currentPlayer.getTeam()) {
                    std::list<cords> tempList = getListOfCordsInCircle(listOfActors[i].getLocation().x, listOfActors[i].getLocation().y, 6);
                    for (const cords& cord : tempList)
                    {
                        this->visability[(cord.x * MAP_HEIGHT) + cord.y] = 2;
                    }
                }
            }
            for (int i = 1; i < listOfBuildings.size(); i++) {
                if (listOfBuildings[i].getTeam() == currentPlayer.getTeam()) {
                    int visRadius = 1;
                    if (listOfBuildings[i].getCompleted()) {
                        visRadius = 8;
                    }
                    std::list<cords> buidlingFootprint = listOfBuildings[i].getFootprintOfBuilding();
                    for (const cords& footprintTile : buidlingFootprint) {
                        std::list<cords> tempList = getListOfCordsInCircle(footprintTile.x, footprintTile.y, visRadius);
                        for (const cords& cord : tempList)
                        {
                            this->visability[(cord.x * MAP_HEIGHT) + cord.y] = 2;
                        }
                    }
                }
            }
        }
        else {
            for (int i = 0; i < MAP_HEIGHT * MAP_WIDTH; i++) {
                this->visability[i] = 2;
            }
        }
        lastFogOfWarUpdated = this->elapsedTime;
    }
}

