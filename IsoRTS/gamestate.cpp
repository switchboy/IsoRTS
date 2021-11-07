#include "actors.h"
#include "buildings.h"
#include "buildings.h"
#include "gamestate.h"
#include "gametext.h"
#include "objects.h"
#include "orderCursor.h"
#include "player.h"
#include "projectile.h"
#include "randomMapGenerator.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>
#include "commandSync.h"
#include "connection.h"


sf::VertexArray miniMapBackGroundPoints(sf::Quads, MAP_HEIGHT* MAP_WIDTH * 4);
sf::VertexArray miniMapBuildingPoints(sf::Quads, MAP_HEIGHT* MAP_WIDTH * 4);
sf::VertexArray miniMapActorPoints(sf::Quads, MAP_HEIGHT* MAP_WIDTH * 4);
sf::VertexArray miniMapMistPoints(sf::Quads, MAP_HEIGHT* MAP_WIDTH * 4);
sf::VertexArray miniMapObjectPoints(sf::Quads, MAP_HEIGHT* MAP_WIDTH * 4);

sf::VertexArray worldMap(sf::Quads, MAP_HEIGHT* MAP_WIDTH * 4);
sf::VertexArray mistMap(sf::Quads, MAP_HEIGHT* MAP_WIDTH * 4);
sf::RenderStates worldMapStates;

bool noNewBuildings;

sf::Texture splashScreenTexture;
sf::Sprite splashScreenSprite;

namespace
{
    const sf::Color teamColors[] =
    {
        {0, 0, 255},
        {0, 255, 0},
        {255, 0, 0},
        {255, 255, 0},
        {0, 255, 255},
        {255, 0, 255},
        {255, 127, 0},
        {127, 127, 127}
    };
}


nearestBuildingTile findNearestBuildingTile(int buildingId, int actorId)
{
    std::list <nearestBuildingTile> listOfBuildLocations;
    std::vector<adjacentTile> tileList = listOfBuildings[buildingId].getFreeBuildingTile();
    if (!tileList.empty()) {
        for (int j = 0; j < tileList.size(); j++)
        {
            float tempDeltaDistance = static_cast<float>(dist(listOfActors[actorId].getActorCords().x, listOfActors[actorId].getActorCords().y, tileList[j].tileCords.x, tileList[j].tileCords.y));
            listOfBuildLocations.push_back({ tempDeltaDistance, tileList[j].tileCords.x, tileList[j].tileCords.y, tileList[j].goal.x , tileList[j].goal.y, tileList[j].tileId, true });
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
                if (reject.x == listOfBuildLocations.front().location.x && reject.y == listOfBuildLocations.front().location.y) {
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

void gameState::drawMousePosition(int x,int y, bool noProblem)
{
    if(noProblem)
    {
        spriteSelectedTile.setPosition(static_cast<float>(worldSpace({ x, y }).x), static_cast<float>(worldSpace({ x, y }).y));
        window.draw(spriteSelectedTile);
    }
    else
    {
        spriteTileObstructed.setPosition(static_cast<float>(worldSpace({ x, y }).x), static_cast<float>(worldSpace({ x, y }).y));
        window.draw(spriteTileObstructed);
    }
}

bool gameState::isPassable(cords location) const
{
    //check if the terrain is passable 1-6 and within map bounds
    if (location.x >= 0 && location.x < MAP_WIDTH && location.y >= 0 && location.y < MAP_HEIGHT) {
        bool noBuildingOrGateOpen = true;
        if (this->occupiedByBuildingList[location.x][location.y] != -1) {
            if (!listOfBuildings[this->occupiedByBuildingList[location.x][location.y]].getGateIsOpen()) {
                noBuildingOrGateOpen = false;
            }
        }

        if (
            (this->currentMap[location.x][location.y] > 0 && this->currentMap[location.x][location.y] < 3) &&
            this->objectLocationList[location.x][location.y] == -1 &&
            noBuildingOrGateOpen &&
            this->occupiedByActorList[location.x][location.y].empty()
            )
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

bool gameState::isPassableButMightHaveActor(cords location) const
{
    //check if the terrain is passable 1-6 and within map bounds
    if (location.x >= 0 && location.x < MAP_WIDTH && location.y >= 0 && location.y < MAP_HEIGHT) {
        bool noBuildingOrGateOpen = true;
        if (this->occupiedByBuildingList[location.x][location.y] != -1) {
            if (!listOfBuildings[this->occupiedByBuildingList[location.x][location.y]].getGateIsOpen()) {
                noBuildingOrGateOpen = false;
            }
        }
        if (
            (this->currentMap[location.x][location.y] > 0 && this->currentMap[location.x][location.y] < 3) &&
            noBuildingOrGateOpen &&
            this->objectLocationList[location.x][location.y] == -1
            )
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

void setTerrainTexturCordsForQuad(sf::Vertex* quad, int tu, int tv) {
    quad[0].texCoords = sf::Vector2f(static_cast<float>(tu), static_cast<float>(tv));
    quad[1].texCoords = sf::Vector2f(static_cast<float>(tu) + 64.f, static_cast<float>(tv));
    quad[2].texCoords = sf::Vector2f(static_cast<float>(tu) + 64.f, static_cast<float>(tv) + 128.f);
    quad[3].texCoords = sf::Vector2f(static_cast<float>(tu), static_cast<float>(tv) + 128.f);
}

void setTerrainWorldCordsForQuad(sf::Vertex* Quad, int i, int j) {
    Quad[0].position = sf::Vector2f(static_cast<float>(worldSpace({ i, j }).x), static_cast<float>(worldSpace({ i, j }).y));                  // 0 , 0 = 0 , 0
    Quad[1].position = sf::Vector2f(static_cast<float>(worldSpace({ i, j }).x) + 64.f, static_cast<float>(worldSpace({ i, j }).y));           // 1 , 0 = 20 , 0
    Quad[2].position = sf::Vector2f(static_cast<float>(worldSpace({ i, j }).x) + 64.f, static_cast<float>(worldSpace({ i, j }).y) + 128.f);    // 1 , 1 = 20 , 10
    Quad[3].position = sf::Vector2f(static_cast<float>(worldSpace({ i, j }).x), static_cast<float>(worldSpace({ i, j }).y) + 128.f);           // 0 , 1 = 0 , 10
}


void gameState::drawGround(int i, int j)
{
    int spriteCordsX = this->tileBitmask[i][j] * 64;
    int spriteCordsY = (this->currentMap[i][j] - 1) * 512;
    while (spriteCordsX >= 4096) {
        spriteCordsX -= 4096;
        spriteCordsY += 128;
    }

    setTerrainTexturCordsForQuad(&worldMap[((i * MAP_HEIGHT) + j) * 4], spriteCordsX, spriteCordsY);
    setTerrainWorldCordsForQuad(&worldMap[((i * MAP_HEIGHT) + j) * 4], i, j);
}

void gameState::setIsPlacingBuilding()
{
    this->isPlacingBuilding = true;
}

void gameState::setAttackMove()
{
    this->attackMove = true;
}

void gameState::setToolbarSubMenu(int subMenu)
{
    this->showToolbarSubMenu = subMenu;
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
    else if(!this->occupiedByActorList[i][j].empty() && this->visability[i][j] > 1)
    {
        for (int& id : this->occupiedByActorList[i][j]) {
            listOfActors[id].drawActor();
        }
    }
}

bool gameState::isInSelectedActors(int id) const
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

bool gameState::buildingIsSelected(int id) const
{
    if (this->buildingSelectedId == id) {
        return true;
    }
    else {
        return false;
    }
}

void setMistLevel(sf::Vertex* quad, sf::Color alpha) {
    quad[0].color = alpha;
    quad[1].color = alpha;
    quad[2].color = alpha;
    quad[3].color = alpha;
}

void setMistWorldCordsForQuad(sf::Vertex* Quad, int i, int j) {
    Quad[0].position = sf::Vector2f(static_cast<float>(worldSpace({ i, j }).x) + 32.f, static_cast<float>(worldSpace({ i, j }).y));
    Quad[1].position = sf::Vector2f(static_cast<float>(worldSpace({ i, j }).x) + 64.0f, static_cast<float>(worldSpace({ i, j }).y) + 16.f);
    Quad[2].position = sf::Vector2f(static_cast<float>(worldSpace({ i, j }).x) + 32.f, static_cast<float>(worldSpace({ i, j }).y) + 32.f);
    Quad[3].position = sf::Vector2f(static_cast<float>(worldSpace({ i, j }).x), static_cast<float>(worldSpace({ i, j }).y) + 16.f);
}

void gameState::drawMap()
{
    int lowX = toWorldMousePosition(viewOffsetX - halfOfMainWindowWidth, viewOffsetY - visableHalfOfMainWindowWidth).x;
    int highX = toWorldMousePosition(viewOffsetX + halfOfMainWindowWidth, viewOffsetY + visableHalfOfMainWindowWidth).x;
    int lowY = toWorldMousePosition(viewOffsetX, viewOffsetY - halfOfMainWindowWidth).y;
    int highY = toWorldMousePosition(viewOffsetX, viewOffsetY + halfOfMainWindowWidth).y;
    if (lowX < 0) { lowX = 0; }
    if (highX >= MAP_WIDTH) { highX = MAP_WIDTH; }
    if (lowY < 0) { lowY = 0; }
    if (highY >= MAP_HEIGHT) { highY = MAP_HEIGHT; }
    for (int j = lowY; j < highY; j++)
    {
        for (int i = lowX; i < highX; i++)
        {
                drawGround(i, j);
        }
    }
    window.draw(worldMap, worldMapStates);
    for (int j = lowY; j < highY; j++)
    {
        for (int i = lowX; i < highX; i++)
        {
            if (this->showPaths) {
                if (!isPassable({ i, j })) {
                    spriteTileObstructed.setPosition(static_cast<float>(worldSpace({ i,j }).x), static_cast<float>(worldSpace({ i, j }).y));
                    window.draw(spriteTileObstructed);
                }
            }
            if (this->visability[i][j] > 0) {
                drawThingsOnTile(i, j);
            }
        }
    }
    for (int j = lowY; j < highY; j++)
    {
        for (int i = lowX; i < highX; i++)
        {
            setMistWorldCordsForQuad(&mistMap[((i * MAP_HEIGHT) + j) * 4], i, j);
            if (this->visability[i][j] == 0) {
                setMistLevel(&mistMap[((i * MAP_HEIGHT) + j) * 4], { 0,0,0,255 });
            }
            else if (this->visability[i][j] == 1) {
                setMistLevel(&mistMap[((i * MAP_HEIGHT) + j) * 4], { 0,0,0,75 });
            }
            else {
                setMistLevel(&mistMap[((i * MAP_HEIGHT) + j) * 4], { 0,0,0,0 });
            }

        }
    }
    window.draw(mistMap);
}

void gameState::loadTextures()
{
    this->selectionRectangle.setOutlineThickness(1);
    this->selectionRectangle.setOutlineColor(sf::Color(255, 255, 255));
    this->selectionRectangle.setFillColor(sf::Color(255, 255, 255, 40));

    if (groundTextureSheet.loadFromFile("textures/groundTextureSheet.png"))
    {
        worldMapStates.texture = &groundTextureSheet;
    }
    else
    {
        std::cout << "Error loading texture: groundTextureSheet.png \n" << std::endl;
    }

    if (textureMousePointer.loadFromFile("textures/mouseHints.png"))
    {
        spriteMousePointer.setTexture(textureMousePointer);
        spriteMousePointer.setTextureRect(sf::IntRect(0, 0, 32, 32));
    }
    else
    {
        std::cout << "Error loading texture: mouseHints.png \n" << std::endl;
    }


    if (textureArrow.loadFromFile("textures/arrow.png"))
    {
        spriteArrow.setTexture(textureArrow);
    }
    else
    {
        std::cout << "Error loading texture: arrow.png \n" << std::endl;
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

    if(textureTotalBackground.loadFromFile("textures/totalBackground.png"))
    {
        spriteTotalBackground.setTexture(textureTotalBackground);
    }
    else
    {
        std::cout << "Error loading texture: totalbackground.png \n" << std::endl;
    }


    if(textureSelectedTile.loadFromFile("textures/tileSelected.png"))
    {
        spriteSelectedTile.setTexture(textureSelectedTile);
    }
    else
    {
        std::cout << "Error loading texture: tileSelected.png \n" << std::endl;
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


    if(!textureCheatTile.loadFromFile("textures/cheatTile.png"))
    {
        std::cout << "Error loading texture: cheatTile.png \n" << std::endl;
    }
    cheatTile = currentGame.textureCheatTile.copyToImage();

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

void gameState::changeViewFromMiniMap() const {
    viewOffsetX = static_cast<int>(((mouseFakePosition.x - (mainWindowWidth * 0.8f)) / (0.2f * mainWindowWidth)) * (MAP_WIDTH * 64));
    viewOffsetY = static_cast<int>(((mouseFakePosition.y - (mainWindowHeigth * 0.8f)) / (0.2f * mainWindowHeigth)) * (MAP_HEIGHT * 32));
}

void gameState::clickUIButton() const {
    //check if a UI button was pressed
    for (auto& Button : listOfButtons)
    {
        Button.isClicked(this->mouseFakePosition);
    }
}

void gameState::clickToPlaceBuilding() {
    if (!listOfBuildingTemplates[this->buildingTypeSelected].getIsWall()) {
        if (!(this->mouseWorldPosition.x - listOfBuildingTemplates[this->buildingTypeSelected].getBuildingFootprint().amountOfXFootprint < -1) && !(this->mouseWorldPosition.y - listOfBuildingTemplates[this->buildingTypeSelected].getBuildingFootprint().amountOfYFootprint < -1) && !(this->mouseWorldPosition.x >= MAP_WIDTH) && !(this->mouseWorldPosition.y >= MAP_HEIGHT))
        {
            //check of het gebouw hier kan staan:
            bool buildingPlacable = true;

            for (int i = 0; i < listOfBuildingTemplates[this->buildingTypeSelected].getBuildingFootprint().amountOfXFootprint; i++)
            {
                for (int j = 0; j < listOfBuildingTemplates[this->buildingTypeSelected].getBuildingFootprint().amountOfYFootprint; j++)
                {
                    if (
                        this->occupiedByBuildingList[this->mouseWorldPosition.x - i][this->mouseWorldPosition.y - j] != -1 ||
                        this->objectLocationList[mouseWorldPosition.x - i][mouseWorldPosition.y - j] != -1 ||
                        (!this->occupiedByActorList[this->mouseWorldPosition.x - i][this->mouseWorldPosition.y - j].empty()) ||
                        this->currentMap[this->mouseWorldPosition.x - i][this->mouseWorldPosition.y - j] == 3
                        )
                    {
                        buildingPlacable = false;
                    }
                }
            }
            if (buildingPlacable)
            {
                //command naar lijst voor het bouwen van een gebouw
                gameDirector.addCommand(
                    {
                        this->elapsedTimeMS,
                        currentPlayer.getTeam(),
                        this->buildingTypeSelected,
                        true,
                        false,
                        this->mouseWorldPosition,
                        worldObject::none,
                        stackOrderTypes::none,
                        actionTypes::none
                    }
                );


                for (int i = 0; i < this->selectedUnits.size(); i++)
                {
                    if (listOfActors[this->selectedUnits[i]].getType() == 0 && listOfActors[this->selectedUnits[i]].getTeam() == currentPlayer.getTeam())
                    {
                        gameDirector.addCommand(
                            {
                                this->elapsedTimeMS + 1, //this makes sure the command is excecuted after the place building command
                                currentPlayer.getTeam(),
                                this->selectedUnits[i],
                                false,
                                false,
                                this->mouseWorldPosition,
                                worldObject::actor,
                                stackOrderTypes::stackActionBuild,
                                actionTypes::none
                            }
                        );
                    }
                }

                this->isPlacingBuilding = false;
                this->mousePressOutofWorld = true;
            }
        }
    }
    else {
        if (this->firstWallClick.x != -1) {
            //First location was allready established!
            if (isPassable(this->mouseWorldPosition))//Check is second location is valid
            {
                //Determine tiles the wall will occupy with Bresenham's Line Drawing Algorithm for paths between the two points
                std::list<cords> mapPointsCrossed = bresenham(this->mouseWorldPosition, this->firstWallClick);

                //check if the entire path is clear, if so place building
                bool allPointsAreBuildable = true;
                for (cords& thisCord : mapPointsCrossed) {
                    if (!isPassable(thisCord)) { allPointsAreBuildable = false; }
                }
                //check if the player can pay for this wall, if so place it, if not throw error in his face!
                if (allPointsAreBuildable) {
                    if (
                        (listOfBuildingTemplates[this->buildingTypeSelected].getPriceOfBuilding().food * mapPointsCrossed.size() <= currentPlayer.getStats().amountOfFood &&
                            listOfBuildingTemplates[this->buildingTypeSelected].getPriceOfBuilding().wood * mapPointsCrossed.size() <= currentPlayer.getStats().amountOfWood &&
                            listOfBuildingTemplates[this->buildingTypeSelected].getPriceOfBuilding().stone * mapPointsCrossed.size() <= currentPlayer.getStats().amountOfStone &&
                            listOfBuildingTemplates[this->buildingTypeSelected].getPriceOfBuilding().gold * mapPointsCrossed.size() <= currentPlayer.getStats().amountOfGold) ||
                        !isPlacingBuilding //Check to see if building is placed from the debugging hotkeys or map builder
                        ) {
                        //place the building
                        for (cords& thisCord : mapPointsCrossed) {

                            gameDirector.addCommand(
                                {
                                    this->elapsedTimeMS,
                                    currentPlayer.getTeam(),
                                    this->buildingTypeSelected,
                                    true,
                                    false,
                                    thisCord,
                                    worldObject::none,
                                    stackOrderTypes::none,
                                    actionTypes::none
                                }
                            );
                            if (this->isPlacingBuilding) //Check to see if building is placed from the debugging hotkeys or map builder
                            {
                                for (int i = 0; i < this->selectedUnits.size(); i++)
                                {
                                    if (listOfActors[this->selectedUnits[i]].getType() == 0 && listOfActors[this->selectedUnits[i]].getTeam() == currentPlayer.getTeam())
                                    {
                                        gameDirector.addCommand(
                                            {
                                                this->elapsedTimeMS + 1, //this makes sure the command is excecuted after the place building command
                                                currentPlayer.getTeam(),
                                                this->selectedUnits[i],
                                                false,
                                                true,
                                                thisCord,
                                                worldObject::actor,
                                                stackOrderTypes::stackActionBuild,
                                                actionTypes::none
                                            }
                                        );
                                    }
                                }

                            }
                        }
                        this->isPlacingBuilding = false;
                        this->mousePressOutofWorld = true;
                    }
                    else {
                        //throw error!
                        std::stringstream errortext;
                        errortext << "Not enough resources to build this length of wall! Cost" <<
                            " Food: " << listOfBuildingTemplates[this->buildingTypeSelected].getPriceOfBuilding().food * mapPointsCrossed.size() <<
                            " Wood: " << listOfBuildingTemplates[this->buildingTypeSelected].getPriceOfBuilding().wood * mapPointsCrossed.size() <<
                            " Stone: " << listOfBuildingTemplates[this->buildingTypeSelected].getPriceOfBuilding().stone * mapPointsCrossed.size() <<
                            " Gold: " << listOfBuildingTemplates[this->buildingTypeSelected].getPriceOfBuilding().gold * mapPointsCrossed.size();
                        gameText.addNewMessage(errortext.str(), 1);
                    }
                    this->firstWallClick = { -1, -1 };
                }
                else {
                    std::stringstream errortext;
                    errortext << "Cant build a wall here, one of the tiles is obstructed!";
                    gameText.addNewMessage(errortext.str(), 1);
                }
            }
        }
        else {
            if (isPassable(this->mouseWorldPosition))
            {
                this->firstWallClick = this->mouseWorldPosition;
            }
        }
    }
}



void gameState::clickToPlaceObject() const
{
    if (!(this->mouseWorldPosition.x < 0) && !(this->mouseWorldPosition.y < 0) && !(this->mouseWorldPosition.x >= MAP_WIDTH) && !(this->mouseWorldPosition.y >= MAP_HEIGHT))
    {
        //check of het object hier kan staan:
        if (this->objectLocationList[mouseWorldPosition.x][mouseWorldPosition.y] == -1 && this->occupiedByBuildingList[this->mouseWorldPosition.x][this->mouseWorldPosition.y] == -1)
        {
            //Zet het object neer
            listOfObjects.push_back(objects(static_cast<objectTypes>(this->objectTypeSelected), this->mouseWorldPosition, static_cast<int>(listOfObjects.size())));
        }
    }
}

void gameState::clickToPlaceActor() const
{
    if (!(this->mouseWorldPosition.x < 0) && !(this->mouseWorldPosition.y < 0) && !(this->mouseWorldPosition.x >= MAP_WIDTH) && !(this->mouseWorldPosition.y >= MAP_HEIGHT))
    {
        //check of de actor hier kan staan:
        if (this->objectLocationList[mouseWorldPosition.x][mouseWorldPosition.y] == -1 && this->occupiedByBuildingList[this->mouseWorldPosition.x][this->mouseWorldPosition.y] == -1 && this->occupiedByActorList[mouseWorldPosition.x][mouseWorldPosition.y].empty())
        {
            //Zet de actor neer
            listOfActors.push_back(actors(0, this->mouseWorldPosition, currentPlayer.getTeam(), static_cast<int>(listOfActors.size())));
        }
    }
}

void gameState::clickToSelectObjectOrBuilding()
{
    this->objectSelectedId = -1;
    this->buildingSelectedId = -1;

    //sf::IntRect selection = sf::IntRect(mousePosition.x, mousePosition.y, 1, 1);
    this->spriteMouseCord.setPosition(static_cast<float>(round(mousePosition.x)), static_cast<float>(round(mousePosition.y)));


    if (listOfBuildings.size() > 0) {
        //Check if a bulding is clicked
        for (const buildings& building : listOfBuildings) {
            listOfBuildingTemplates[building.getType()].setSpritePosition(worldSpace(building.getLocation()));
            //using a pixel cord
            if (Collision::singlePixelTest(listOfBuildingTemplates[building.getType()].getBuildingSprite(), mousePosition, 128)) {
                this->buildingSelectedId = building.getBuildingId();
            }
            /*/if (Collision::PixelPerfectTest(listOfBuildingTemplates[building.getType()].getBuildingSprite(), this->spriteMouseCord, 128)) {
                this->buildingSelectedId = building.getBuildingId();
            }
            */

        }
    }

    if (listOfObjects.size() > 0) {
        int highestTop = 0;
        for (const objects& object : listOfObjects) {
            listOfObjectTemplates[static_cast<uint32_t>(object.getType())].setPosition(worldSpace(object.getLocation()));

            //using a pixel cord
            if (Collision::singlePixelTest(listOfObjectTemplates[static_cast<uint32_t>(object.getType())].getSprite(), mousePosition, 128)) {
                this->objectSelectedId = object.getObjectId();
            }

            /*Using a sprite
            if (Collision::PixelPerfectTest(listOfObjectTemplates[static_cast<uint32_t>(object.getType())].getSprite(), this->spriteMouseCord, 128)) {
                this->objectSelectedId = object.getObjectId();
            }*/

            /* Old style detection
            sf::IntRect result;
            if (selection.intersects(object.getLastIntRect(), result)) {
                if (object.getLastIntRect().top > highestTop) {
                    this->objectSelectedId = object.getObjectId();
                    highestTop = object.getLastIntRect().top;
                }
            }
            */
        }
    }
}

void gameState::clickToSelect()
{
    this->startLocation[0] = this->mouseWorldPosition.x;
    this->startLocation[1] = this->mouseWorldPosition.y;
    this->startMouseCords[0] = static_cast<int>(this->mousePosition.x);
    this->startMouseCords[1] = static_cast<int>(this->mousePosition.y);
    clickToSelectObjectOrBuilding();
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
            else if (attackMove) {
                if (this->isPassable(this->mouseWorldPosition))
                {
                    for (int i = 0; i < this->selectedUnits.size(); i++)
                    {
                        if (listOfActors[this->selectedUnits[i]].getTeam() == currentPlayer.getTeam())
                        {
                            gameDirector.addCommand(
                                {
                                    this->elapsedTimeMS,
                                    currentPlayer.getTeam(),
                                    this->selectedUnits[i],
                                    false,
                                    this->isPressedShift,
                                    this->mouseWorldPosition,
                                    worldObject::actor,
                                    stackOrderTypes::none,
                                    actionTypes::actionAttackMove
                                }
                            );
                        }
                    }
                    listOfOrderCursors.push_back(orderCursor(this->mousePosition));
                    this->attackMove = false;
                }
            }
            {
                this->setToolbarSubMenu(0);
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
    if (!this->isPlacingBuilding) {
        if (this->mousePressedLeft && !(mouseFakePosition.y > mainWindowHeigth * 0.8f))
        {
            this->selectedUnits.clear();
            sf::IntRect selection;

            if (listOfActors.size() > 0) {
                if (!(this->startLocation[0] == this->mouseWorldPosition.x && this->startLocation[1] == this->mouseWorldPosition.y)) {
                    //there is a selection box find all actors in this box! (we will use old style intRect collision detection)
                    selection = static_cast<sf::IntRect>(this->selectionRectangle.getGlobalBounds());
                    for (const actors& actor : listOfActors) {
                        sf::IntRect result;
                        if (selection.intersects(actor.getLastIntRect(), result)) {
                            selectedUnits.push_back({ actor.getActorId() });
                        }
                    }
                }
                else {
                    //One coordinate was clicked find actor there we will be pixel perfect!

                    //using a pixel cord
                    //this->spriteMouseCord.setPosition(mousePosition.x, mousePosition.y);

                    for (const actors& actor : listOfActors) {
                        listOfActorTemplates[actor.getType()].setSpritePosition(worldSpace(actor.getActorCords()));
                        if (Collision::singlePixelTest(listOfActorTemplates[actor.getType()].getActorSprite(), mousePosition, 128)) {
                            selectedUnits.push_back({ actor.getActorId() });
                        }

                        /* using a mouse sprite which was buggy
                        if (Collision::PixelPerfectTest(listOfActorTemplates[actor.getType()].getActorSprite(), this->spriteMouseCord, 128)) {
                            selectedUnits.push_back({ actor.getActorId() });
                        }
                        */
                    }
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
            if (!this->selectedUnits.empty()) {
                this->objectSelectedId = -1;
                this->buildingSelectedId = -1;
            }
        }
    }
}

bool gameState::clickToMove(cords pos, bool minimap) const
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
                    if ((pos.x <= MAP_WIDTH && pos.y <= MAP_HEIGHT)) {
                        tempCords = currentGame.getNextCord(pos);
                    }
                }
                else {
                    tempCords = currentGame.getNextCord(this->mouseWorldPosition);
                }
                gameDirector.addCommand(
                    {
                        this->elapsedTimeMS,
                        currentPlayer.getTeam(),
                        this->selectedUnits[i],
                        false,
                        this->isPressedShift,
                        tempCords,
                        worldObject::actor,
                        stackOrderTypes::stackActionMove,
                        actionTypes::none
                    }
                );
                actionDone = true;
            }
        }
        else
        {
            if (listOfActors[this->selectedUnits[i]].getTeam() == currentPlayer.getTeam())
            {
                cords tempcords = { 0,0 };
                if (minimap) {
                    if ((pos.x <= MAP_WIDTH && pos.y <= MAP_HEIGHT)) {
                        tempcords = pos;
                    }
                }
                else {
                    tempcords = this->mouseWorldPosition;
                }
                gameDirector.addCommand(
                    {
                        this->elapsedTimeMS,
                        currentPlayer.getTeam(),
                        this->selectedUnits[i],
                        false,
                        this->isPressedShift,
                        tempcords,
                        worldObject::actor,
                        stackOrderTypes::stackActionMove,
                        actionTypes::none
                    }
                );
                actionDone = true;
            }
        }
    }
    return actionDone;
}

bool gameState::clickToGatherResource() const
{
    bool actionDone = false;
    for (int i = 0; i < this->selectedUnits.size(); i++)
    {
        if (listOfActors[this->selectedUnits[i]].getType() == 0 && listOfActors[this->selectedUnits[i]].getTeam() == currentPlayer.getTeam())
        {
            gameDirector.addCommand(
                {
                    this->elapsedTimeMS,
                    currentPlayer.getTeam(),
                    this->selectedUnits[i],
                    false,
                    this->isPressedShift,
                    this->mouseWorldPosition,
                    worldObject::actor,
                    stackOrderTypes::stackActionGather,
                    actionTypes::none
                }
            );
            actionDone = true;
        }
    }
    return actionDone;
}

bool gameState::clickToBuildOrRepairBuilding() const
{
    bool actionDone = false;
    if (!listOfBuildings[this->occupiedByBuildingList[this->mouseWorldPosition.x][this->mouseWorldPosition.y]].getCompleted())
    {
        for (int i = 0; i < this->selectedUnits.size(); i++)
        {
            if (listOfActors[this->selectedUnits[i]].getType() == 0 && listOfActors[this->selectedUnits[i]].getTeam() == currentPlayer.getTeam())
            {
                gameDirector.addCommand(
                    {
                        this->elapsedTimeMS,
                        currentPlayer.getTeam(),
                        this->selectedUnits[i],
                        false,
                        this->isPressedShift,
                        this->mouseWorldPosition,
                        worldObject::actor,
                        stackOrderTypes::stackActionBuild,
                        actionTypes::none
                    }
                );
                actionDone = true;
            }
        }
    }
    return actionDone;
}

bool gameState::clickToAttack() const {
    bool actionDone = false;
    for (int i = 0; i < this->selectedUnits.size(); i++)
    {
        if (listOfActors[this->selectedUnits[i]].getTeam() == currentPlayer.getTeam())
        {
            gameDirector.addCommand(
                {
                    this->elapsedTimeMS,
                    currentPlayer.getTeam(),
                    this->selectedUnits[i],
                    false,
                    this->isPressedShift,
                    this->mouseWorldPosition,
                    worldObject::actor,
                    stackOrderTypes::stackActionAttack,
                    actionTypes::none
                }
            );
            actionDone = true;
        }
    }
    return actionDone;
}

void gameState::changeBuildingType()
{
    this->buildingTypeSelected += 1;
    if (this->buildingTypeSelected > 7)
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



void gameState::clickToGiveCommand()
{
    this->firstRound = true;
    this->lastIandJ[0] = 0;
    this->lastIandJ[1] = 0;
    bool actionPreformed = false;

    cords actionPosition = this->mouseWorldPosition;

    if (listOfBuildings.size() > 0) {
        for (const buildings& building : listOfBuildings) {
            listOfBuildingTemplates[building.getType()].setSpritePosition(worldSpace(building.getLocation()));
            if (Collision::singlePixelTest(listOfBuildingTemplates[building.getType()].getBuildingSprite(), mousePosition, 128)) {
                actionPosition = building.getLocation();
            }
        }
    }

    if (listOfObjects.size() > 0) {
        for (const objects& object : listOfObjects) {
            listOfObjectTemplates[static_cast<uint32_t>(object.getType())].setPosition(worldSpace(object.getLocation()));
            if (Collision::singlePixelTest(listOfObjectTemplates[static_cast<uint32_t>(object.getType())].getSprite(), mousePosition, 128)) {
                actionPosition = object.getLocation();
            }
        }
    }

    if (listOfActors.size() > 0 ) {
        for (const actors& actor : listOfActors) {
            listOfActorTemplates[actor.getType()].setSpritePosition(worldSpace(actor.getActorCords()));
            if (Collision::singlePixelTest(listOfActorTemplates[actor.getType()].getActorSprite(), mousePosition, 128)) {
                actionPosition = actor.getActorCords();
            }
        }
    }

    this->mouseWorldPosition = actionPosition;

    if (this->isPassable(actionPosition))
    {
        actionPreformed = clickToMove({ 0,0 }, false);
    }
    else if (this->objectLocationList[actionPosition.x][actionPosition.y] != -1)
    {
        actionPreformed = clickToGatherResource();
    }
    else if (this->occupiedByBuildingList[actionPosition.x][actionPosition.y] != -1)
    {
        if (listOfBuildings[this->occupiedByBuildingList[actionPosition.x][actionPosition.y]].getTeam() == currentPlayer.getTeam())
        {
            actionPreformed = clickToBuildOrRepairBuilding();
        }
        else
        {
            actionPreformed = clickToAttack();
        }

    }
    else if (!this->occupiedByActorList[actionPosition.x][actionPosition.y].empty()) {
        if (listOfActors[this->occupiedByActorList[actionPosition.x][actionPosition.y].front()].getTeam() != currentPlayer.getTeam())
        {
            clickToAttack();
        }
    }

    if (actionPreformed) {
        listOfOrderCursors.push_back(orderCursor(this->mousePosition));
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
            clickToMove({ posX, posY }, true);
            listOfOrderCursors.push_back(orderCursor(this->mousePosition));
            return;
        }
    }
 }

void gameState::orderRallyPoint() const {
    if (this->isPassable(this->mouseWorldPosition))
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
    else if (!this->occupiedByActorList[this->mouseWorldPosition.x][this->mouseWorldPosition.y].empty()) {
        for (auto& id : this->occupiedByActorList[this->mouseWorldPosition.x][this->mouseWorldPosition.y]) {
            if (listOfActors[id].getTeam() != currentPlayer.getTeam())
            {
                listOfBuildings[this->buildingSelectedId].setRallyPoint({ this->mouseWorldPosition.x, this->mouseWorldPosition.y }, stackOrderTypes::stackActionAttack);
            }
        }
    }
    listOfOrderCursors.push_back(orderCursor(this->mousePosition));
}

void gameState::mouseRightClick()
{
    this->rectangleCords.clear();
    this->mousePressedRight = true;
    this->firstWallClick = { -1,-1 };
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
        this->attackMove = false;
    }
}

void gameState::changeTiles()
{
    for (int i = 0; i < this->rectangleCords.size(); i++)
    {
        this->currentMap[this->rectangleCords[i].x][this->rectangleCords[i].y] += +1;
        if (this->currentMap[this->rectangleCords[i].x][this->rectangleCords[i].y] > 3)
        {
            this->currentMap[this->rectangleCords[i].x][this->rectangleCords[i].y] = 1;
        }
    }
    minimapTextureExist = false;
    this->equalIsPressed = true;
}

void gameState::edgeScrolling() const
{
    if (mouseFakePosition.x < 125 && mouseFakePosition.y > static_cast<float>(mainWindowHeigth * 0.03f) && mouseFakePosition.y < static_cast<float>(mainWindowHeigth * 0.8f)) {
        viewOffsetX += -10;
    }
    else if (mouseFakePosition.x > mainWindowWidth-125 && mouseFakePosition.y > static_cast<float>(mainWindowHeigth * 0.03f) && mouseFakePosition.y < static_cast<float>(mainWindowHeigth * 0.8f))
    {
        viewOffsetX += 10;
    }
    if (mouseFakePosition.y < static_cast<float>(mainWindowHeigth * 0.03f) + 75 && mouseFakePosition.y > static_cast<float>(mainWindowHeigth * 0.03f)) {
        viewOffsetY += -10;
    }
    else if (mouseFakePosition.y > static_cast<float>(mainWindowHeigth * 0.8f) - 75 && mouseFakePosition.y <  static_cast<float>(mainWindowHeigth * 0.8f))
    {
        viewOffsetY += 10;
    }
}

void gameState::interact()
{
    this->mouseFakePosition = sf::Mouse::getPosition(window);
    this->mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));

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
        if (mouseFakePosition.x >= 0 && static_cast<int>(mouseFakePosition.x) <= static_cast<int>(window.getSize().x) && mouseFakePosition.y >= 0 && static_cast<int>(mouseFakePosition.y) <= static_cast<int>(window.getSize().y)) {
            edgeScrolling();
        }
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
        if (this->isPressedShift) {
            if (currentPlayer.getTeam() + 1 > this->players-1)
            {
                currentPlayer = listOfPlayers[0];
            }
            else
            {
                currentPlayer = listOfPlayers[currentPlayer.getTeam() + 1];
            }
        }

    }
    else if(!sf::Keyboard::isKeyPressed(sf::Keyboard::Tab))
    {
        this->isPressedTab = false;
    }


    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F) && this->focus) {
        if (this->isPressedShift) {
            this->noFogOfWar = (!this->noFogOfWar);
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) && this->focus) {
        if (this->isPressedShift) {
            //this->useVertex = (!this->useVertex);
        }
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

void gameState::setObjectsHaveChanged()
{
    this->objectsChanged = true;
}

cords gameState::getFirstWallClick() const
{
    return this->firstWallClick;
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
        buildings temp;
        temp.drawBuildingFootprint(this->buildingTypeSelected, mouseWorldPosition);
    }
    if(isPressedO)
    {
        objects temp;
        temp.drawObjectFootprint(static_cast<objectTypes>(this->objectTypeSelected), mouseWorldPosition);
    }
}

cords gameState::getNextCord(cords pos)
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
            pos.x = pos.x + this->lastIandJ[0];
            pos.y = pos.y + this->lastIandJ[1];
            this->addSubstractY = false;
        }
        else
        {
            pos.x = pos.x + this->lastIandJ[0];
            pos.y = pos.y - this->lastIandJ[1] ;
            this->addSubstractX = false;
            this->addSubstractY = true;
        }
    }
    else
    {
        if(this->addSubstractY)
        {
            pos.x = pos.x - this->lastIandJ[0];
            pos.y = pos.y + this->lastIandJ[1];
            this->addSubstractY = false;
        }
        else
        {
            pos.x = pos.x - this->lastIandJ[0];
            pos.y = pos.y - this->lastIandJ[1] ;
            this->roundDone = true;
        }

    }

    if(currentGame.isPassable(pos))
    {
        return {pos.x, pos.y};
    }
    else
    {
        return currentGame.getNextCord(pos);
    }
}

int gameState::getPlayerCount() const
{
    return players;
}

void gameState::drawMiniMapBackground(sf::RectangleShape& miniMapPixel)
{
    if (!this->miniMapBackGroundDrawn)
    {
        static const sf::Color colors[] =
        {
            {0, 0, 0},
            {152, 205, 115},
            {200, 160, 80},
            {69, 164, 208},
        };

        if (!minimapTextureExist)
        {
            minimapTexture.clear(sf::Color(0, 0, 0, 0));
            for (int j = 0; j < MAP_HEIGHT; j++)
            {
                for (int i = 0; i < MAP_WIDTH; i++)
                {
                    sf::Vertex* currentMiniMapQuad = &miniMapBackGroundPoints[((i * MAP_HEIGHT) + j) * 4];
                    //Each quad should be 20px wide and 10px high. We define them clockwise from the top left (x + 0 and y + 0)
                    currentMiniMapQuad[0].color = colors[currentGame.currentMap[i][j]];
                    currentMiniMapQuad[1].color = colors[currentGame.currentMap[i][j]];
                    currentMiniMapQuad[2].color = colors[currentGame.currentMap[i][j]];
                    currentMiniMapQuad[3].color = colors[currentGame.currentMap[i][j]];
                }

            }
            minimapTextureExist = true;
        }
        this->miniMapBackGroundDrawn = true;
    }
}

void drawMiniMapBuildings(sf::RectangleShape& miniMapPixel)
{
    if(!noNewBuildings)
    {
            for (int j = 0; j < MAP_HEIGHT; j++)
            {
                for (int i = 0; i < MAP_WIDTH; i++)
                {
                    sf::Vertex* currentMiniMapQuad = &miniMapBuildingPoints[((i * MAP_HEIGHT) + j) * 4];
                    if (currentGame.occupiedByBuildingList[i][j] != -1)
                    {

                        //Each quad should be 20px wide and 10px high. We define them clockwise from the top left (x + 0 and y + 0)
                        currentMiniMapQuad[0].color = teamColors[listOfBuildings[currentGame.occupiedByBuildingList[i][j]].getTeam()];
                        currentMiniMapQuad[1].color = teamColors[listOfBuildings[currentGame.occupiedByBuildingList[i][j]].getTeam()];
                        currentMiniMapQuad[2].color = teamColors[listOfBuildings[currentGame.occupiedByBuildingList[i][j]].getTeam()];
                        currentMiniMapQuad[3].color = teamColors[listOfBuildings[currentGame.occupiedByBuildingList[i][j]].getTeam()];
                    }
                    else {
                        //Pixel is not occupied so make it tansparant
                        currentMiniMapQuad[0].color = sf::Color({ 0,0,0,0 });
                        currentMiniMapQuad[1].color = sf::Color({ 0,0,0,0 });
                        currentMiniMapQuad[2].color = sf::Color({ 0,0,0,0 });
                        currentMiniMapQuad[3].color = sf::Color({ 0,0,0,0 });
                    }
                }
            }
            noNewBuildings = true;
    }
}

void gameState::drawMiniMapActors(sf::RectangleShape& miniMapPixel)
{
    for(int j = 0; j < MAP_HEIGHT; j++)
    {
        for(int i = 0; i < MAP_WIDTH; i++ )
        {
            sf::Vertex* currentMiniMapQuad = &miniMapActorPoints[((i * MAP_HEIGHT) + j) * 4];
            if (this->visability[i][j] == 2 && !currentGame.occupiedByActorList[i][j].empty()) {
                currentMiniMapQuad[0].color = teamColors[listOfActors[currentGame.occupiedByActorList[i][j].front()].getTeam()];
                currentMiniMapQuad[1].color = teamColors[listOfActors[currentGame.occupiedByActorList[i][j].front()].getTeam()];
                currentMiniMapQuad[2].color = teamColors[listOfActors[currentGame.occupiedByActorList[i][j].front()].getTeam()];
                currentMiniMapQuad[3].color = teamColors[listOfActors[currentGame.occupiedByActorList[i][j].front()].getTeam()];
            }
            else {
                //Pixel is not occupied so make it tansparant
                currentMiniMapQuad[0].color = sf::Color({ 0,0,0,0 });
                currentMiniMapQuad[1].color = sf::Color({ 0,0,0,0 });
                currentMiniMapQuad[2].color = sf::Color({ 0,0,0,0 });
                currentMiniMapQuad[3].color = sf::Color({ 0,0,0,0 });
            }
        }
    }
}

void gameState::drawMiniMapMist(sf::RectangleShape& miniMapPixel)
{
    if (this->lastMistDraw + 200 < this->elapsedTimeMS || !this->fogOfWarDrawnOnce) {           //make the fog of war update every 3 seconds to reduce system load

        this->lastMistDraw = this->elapsedTimeMS;
        //Determine the bounderies of the sector to be drawn
        int drawStartX = 0;
        int drawStartY = 0;
        if (this->lastFogOfWarMinimapSectorUpdated <= 3) {
            drawStartX = this->lastFogOfWarMinimapSectorUpdated * spaceBetweenFogOfWarSectorsOnXAxis;
            drawStartY = 0;
        }
        else if (this->lastFogOfWarMinimapSectorUpdated > 3 && this->lastFogOfWarMinimapSectorUpdated <= 7) {
            drawStartX = (this->lastFogOfWarMinimapSectorUpdated - 4) * spaceBetweenFogOfWarSectorsOnXAxis;
            drawStartY = spaceBetweenFogOfWarSectorsOnYAxis;
        }
        else if (this->lastFogOfWarMinimapSectorUpdated > 7 && this->lastFogOfWarMinimapSectorUpdated <= 11) {
            drawStartX = (this->lastFogOfWarMinimapSectorUpdated - 8) * spaceBetweenFogOfWarSectorsOnXAxis;
            drawStartY = spaceBetweenFogOfWarSectorsOnYAxis*2;
        }
        else if (this->lastFogOfWarMinimapSectorUpdated > 11 && this->lastFogOfWarMinimapSectorUpdated <= 15) {
            drawStartX = (this->lastFogOfWarMinimapSectorUpdated - 12) * spaceBetweenFogOfWarSectorsOnXAxis;
            drawStartY = spaceBetweenFogOfWarSectorsOnYAxis*3;
        }
        int drawEndX = drawStartX + spaceBetweenFogOfWarSectorsOnXAxis;
        int drawEndY = drawStartY + spaceBetweenFogOfWarSectorsOnYAxis;

        //Draw the sector
        for (int j = drawStartY; j < drawEndY; j++)
        {
            for (int i = drawStartX; i < drawEndX; i++)
            {
                sf::Vertex* currentMiniMapQuad = &miniMapMistPoints[((i * MAP_HEIGHT) + j) * 4];

                if (this->visability[i][j] == 0) {              //not visible so overlay black tile
                    currentMiniMapQuad[0].color = sf::Color({ 0,0,0 });
                    currentMiniMapQuad[1].color = sf::Color({ 0,0,0 });
                    currentMiniMapQuad[2].color = sf::Color({ 0,0,0 });
                    currentMiniMapQuad[3].color = sf::Color({ 0,0,0 });
                }
                else if (this->visability[i][j] == 1) {         //in shadow so overlay transparent tile
                    currentMiniMapQuad[0].color = sf::Color({ 0,0,0,75 });
                    currentMiniMapQuad[1].color = sf::Color({ 0,0,0,75 });
                    currentMiniMapQuad[2].color = sf::Color({ 0,0,0,75 });
                    currentMiniMapQuad[3].color = sf::Color({ 0,0,0,75 });
                }
                else {
                    //visable
                    currentMiniMapQuad[0].color = sf::Color({ 0,0,0,0 });
                    currentMiniMapQuad[1].color = sf::Color({ 0,0,0,0 });
                    currentMiniMapQuad[2].color = sf::Color({ 0,0,0,0 });
                    currentMiniMapQuad[3].color = sf::Color({ 0,0,0,0 });
                }
            }
        }
        this->lastFogOfWarMinimapSectorUpdated++;
        if (this->lastFogOfWarMinimapSectorUpdated > 15) {
            this->lastFogOfWarMinimapSectorUpdated = 0;
        }
    }
}

void gameState::drawMiniMapObjects(sf::RectangleShape& miniMapPixel)
{
    if (this->objectsChanged) {
        static const sf::Color resourceColors[] =
        {
            {33, 77, 33}, //Wood
            {150, 88, 88}, //Food
            {65, 65, 65}, //Stone
            {110, 90, 0} //Gold
        };
        for (int j = 0; j < MAP_HEIGHT; j++)
        {
            for (int i = 0; i < MAP_WIDTH; i++)
            {
                sf::Vertex* currentMiniMapQuad = &miniMapObjectPoints[((i * MAP_HEIGHT) + j) * 4];

                if (currentGame.objectLocationList[i][j] != -1)
                {
                    currentMiniMapQuad[0].color = resourceColors[static_cast<int>(listOfObjects[currentGame.objectLocationList[i][j]].getTypeOfResource())];
                    currentMiniMapQuad[1].color = resourceColors[static_cast<int>(listOfObjects[currentGame.objectLocationList[i][j]].getTypeOfResource())];
                    currentMiniMapQuad[2].color = resourceColors[static_cast<int>(listOfObjects[currentGame.objectLocationList[i][j]].getTypeOfResource())];
                    currentMiniMapQuad[3].color = resourceColors[static_cast<int>(listOfObjects[currentGame.objectLocationList[i][j]].getTypeOfResource())];
                }
                else {
                    currentMiniMapQuad[0].color = sf::Color({ 0,0,0,0 });
                    currentMiniMapQuad[1].color = sf::Color({ 0,0,0,0 });
                    currentMiniMapQuad[2].color = sf::Color({ 0,0,0,0 });
                    currentMiniMapQuad[3].color = sf::Color({ 0,0,0,0 });
                }
            }
        }
        this->objectsChanged = false;
    }
}

void gameState::drawMiniMap()
{
    window.setView(miniMap);
    sf::RectangleShape miniMapPixel(sf::Vector2f(20.f,10.f));
    sf::Sprite miniMapBackground;

    if (this->lastMiniMapRefresh + 1000 < this->elapsedTimeMS) {
        drawMiniMapBackground(miniMapPixel);
        drawMiniMapObjects(miniMapPixel);
        drawMiniMapBuildings(miniMapPixel);
        drawMiniMapActors(miniMapPixel);
        this->lastMiniMapRefresh = this->elapsedTimeMS;
    }
    drawMiniMapMist(miniMapPixel);

    minimapTexture.clear(sf::Color(0, 0, 0, 0));;
    minimapTexture.draw(miniMapBackGroundPoints);
    minimapTexture.draw(miniMapBuildingPoints);
    minimapTexture.draw(miniMapActorPoints);
    minimapTexture.draw(miniMapObjectPoints);
    minimapTexture.draw(miniMapMistPoints);
    miniMapBackground.setTexture(minimapTexture.getTexture());
    miniMapBackground.setTextureRect(sf::IntRect(0, minimapTexture.getSize().y, minimapTexture.getSize().x, -static_cast<int>(minimapTexture.getSize().y)));
    miniMapBackground.setScale((this->miniMapWidth/(20*MAP_WIDTH)), (this->miniMapHeigth/(10*MAP_HEIGHT)));

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



void gameState::createVillagerButtons(int startX, int startY, int incrementalXOffset,  bool& villagerButtonsAreThere)
{
    if (!villagerButtonsAreThere) {

        int startXOr = startX;
        int buttonCounter = 0;
        std::list<actorButtonVariables> listOfActorButtons = listOfActorTemplates[listOfActors[this->selectedUnits[0]].getType()].getActorButtonsOfMenu(this->showToolbarSubMenu);
        for (actorButtonVariables currentButton : listOfActorButtons) {
            buttonCounter++;
            if (buttonCounter <= 5) {
                if (buttons::requirementForButtonIsMet(currentButton.action, this->selectedUnits[0], listOfActors[this->selectedUnits[0]].getTeam())) {
                    listOfButtons.push_back({ startX, startY, currentButton.sprite, currentButton.action, this->selectedUnits[0], static_cast<int>(listOfButtons.size()), 0 });
                }
                startX += incrementalXOffset;
            }
            else {
                buttonCounter = 1;
                startX = startXOr;
                startY += incrementalXOffset;
                if (buttons::requirementForButtonIsMet(currentButton.action, this->selectedUnits[0], listOfActors[this->selectedUnits[0]].getTeam())) {
                    listOfButtons.push_back({ startX, startY, currentButton.sprite, currentButton.action, this->selectedUnits[0], static_cast<int>(listOfButtons.size()), 0 });
                }
                startX += incrementalXOffset;
            }
        }

        villagerButtonsAreThere = true;
    }
}

void gameState::commandExcecutor(std::vector<command> commandsToBeExcecuted)
{
    for (command& c : commandsToBeExcecuted) {

        if (c.placingBuilding) {
            if (
                (
                    listOfBuildingTemplates[c.subjectId].getPriceOfBuilding().food <= listOfPlayers[c.playerId].getStats().amountOfFood &&
                    listOfBuildingTemplates[c.subjectId].getPriceOfBuilding().wood <= listOfPlayers[c.playerId].getStats().amountOfWood &&
                    listOfBuildingTemplates[c.subjectId].getPriceOfBuilding().stone <= listOfPlayers[c.playerId].getStats().amountOfStone &&
                    listOfBuildingTemplates[c.subjectId].getPriceOfBuilding().gold <= listOfPlayers[c.playerId].getStats().amountOfGold
                ) ||
                !isPlacingBuilding
                ) {
                buildings newBuilding(c.subjectId, c.commandCords, static_cast<int>(listOfBuildings.size()), c.playerId);
                listOfBuildings.push_back(newBuilding);
                listOfPlayers[c.playerId].substractResources(resourceTypes::resourceWood, listOfBuildingTemplates[c.subjectId].getPriceOfBuilding().wood);
                listOfPlayers[c.playerId].substractResources(resourceTypes::resourceFood, listOfBuildingTemplates[c.subjectId].getPriceOfBuilding().food);
                listOfPlayers[c.playerId].substractResources(resourceTypes::resourceStone, listOfBuildingTemplates[c.subjectId].getPriceOfBuilding().stone);
                listOfPlayers[c.playerId].substractResources(resourceTypes::resourceGold, listOfBuildingTemplates[c.subjectId].getPriceOfBuilding().gold);
            }
        }
        else {
            switch (c.subjectType) {
            case worldObject::actor:
                if (c.actionToPerform == actionTypes::actionAttackMove) {
                    listOfActors[c.subjectId].updateGoal(c.commandCords, 0);
                    listOfActors[c.subjectId].setIsDoingAttack(true);
                    listOfActors[c.subjectId].setDoAttackMoveTrue();
                }
                else {
                    switch (c.orderType) {
                    case stackOrderTypes::stackActionAttack:
                        if (c.isStackedCommand) {
                            listOfActors[c.subjectId].stackOrder(c.commandCords, stackOrderTypes::stackActionAttack);
                        }
                        else {
                            listOfActors[c.subjectId].clearCommandStack();
                            listOfActors[c.subjectId].updateGoal(c.commandCords, 0);
                            listOfActors[c.subjectId].setIsDoingAttack(false);
                        }
                        break;
                    case stackOrderTypes::stackActionBuild:
                        if (currentGame.occupiedByBuildingList[c.commandCords.x][c.commandCords.y] != -1) {
                            if (!c.isStackedCommand) {
                                nearestBuildingTile tempTile = findNearestBuildingTile(currentGame.occupiedByBuildingList[c.commandCords.x][c.commandCords.y], c.subjectId);
                                if (tempTile.isSet) {

                                    listOfActors[c.subjectId].clearCommandStack();
                                    listOfActors[c.subjectId].updateGoal(tempTile.location, 0);
                                    listOfBuildings[currentGame.occupiedByBuildingList[c.commandCords.x][c.commandCords.y]].claimFreeBuiildingTile(tempTile.buildingId, c.subjectId);
                                    listOfActors[c.subjectId].setIsBuildingTrue(listOfBuildings[currentGame.occupiedByBuildingList[c.commandCords.x][c.commandCords.y]].getBuildingId(), tempTile.actionLocation);
                                }
                            } else {
                                    listOfActors[c.subjectId].stackOrder(c.commandCords, stackOrderTypes::stackActionBuild);
                            }
                        }
                        break;
                    case stackOrderTypes::stackActionGather:
                        if (c.isStackedCommand) {
                            listOfActors[c.subjectId].stackOrder(c.commandCords, stackOrderTypes::stackActionGather);
                        }
                        else {
                            listOfActors[c.subjectId].clearCommandStack();
                            listOfActors[c.subjectId].updateGoal(c.commandCords, 0);
                            listOfActors[c.subjectId].setCommonGoalTrue();
                            listOfActors[c.subjectId].setGatheringRecource(true);
                        }
                        break;
                    case stackOrderTypes::stackActionMove:
                        if (!c.isStackedCommand) {
                            listOfActors[c.subjectId].clearCommandStack();
                            listOfActors[c.subjectId].updateGoal(c.commandCords, 0);
                            listOfActors[c.subjectId].setGatheringRecource(false);
                            break;
                        }
                        else {
                            listOfActors[c.subjectId].stackOrder(c.commandCords, stackOrderTypes::stackActionMove);
                        }
                    }
                }
                break;
            case worldObject::building:
                switch (c.actionToPerform) {
                case actionTypes::actionCancelBuilding:
                    if (listOfBuildings[c.subjectId].getExists()) {
                        listOfPlayers[c.playerId].addResources(resourceTypes::resourceFood, listOfBuildingTemplates[listOfBuildings[c.subjectId].getType()].getPriceOfBuilding().food / 2);
                        listOfPlayers[c.playerId].addResources(resourceTypes::resourceWood, listOfBuildingTemplates[listOfBuildings[c.subjectId].getType()].getPriceOfBuilding().wood / 2);
                        listOfPlayers[c.playerId].addResources(resourceTypes::resourceStone, listOfBuildingTemplates[listOfBuildings[c.subjectId].getType()].getPriceOfBuilding().stone / 2);
                        listOfPlayers[c.playerId].addResources(resourceTypes::resourceGold, listOfBuildingTemplates[listOfBuildings[c.subjectId].getType()].getPriceOfBuilding().gold / 2);
                        listOfBuildings[c.subjectId].removeBuilding();
                    }
                    break;
                case actionTypes::actionCancelProduction:
                    listOfBuildings[c.subjectId].removeTask(c.commandCords.x);
                    break;
                case actionTypes::actionMakeGate:
                    listOfBuildings[c.subjectId].setIsGate();
                    break;
                case actionTypes::actionOpenGate:
                    listOfBuildings[c.subjectId].setGateOpen(!listOfBuildings[c.subjectId].getGateIsOpen()); //Flip a bit!
                    break;
                case actionTypes::actionCloseGate:
                    listOfBuildings[c.subjectId].setGateOpen(!listOfBuildings[c.subjectId].getGateIsOpen()); //Flip a bit!
                    break;
                case actionTypes::actionMakeSwordsman:
                    listOfBuildings[c.subjectId].getTask(false, 1);
                    break;
                case actionTypes::actionMakeVillager:
                    listOfBuildings[c.subjectId].getTask(false, 0);
                    break;
                case actionTypes::none:
                    if (c.isStackedCommand) {
                        if (c.commandCords.y == 0) {
                            listOfBuildings[c.subjectId].getTask(false, c.commandCords.x);
                        }
                        else {
                            listOfBuildings[c.subjectId].getTask(true, c.commandCords.x);
                        }
                    }
                }
            case worldObject::object:
                break;
            }
        }
    }
}

void gameState::addActorSelectorButton(int i, int actorId, int startDeck, int tempY, int startY, int offSetToNextCard)
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

    int xPosition;
    if(i < ceil(static_cast<float>(this->selectedUnits.size())/2.f)){
        xPosition = startDeck + offSetToNextCard*i;
    }
    else if (this->selectedUnits.size() % 2) {
        xPosition = static_cast<int>(startDeck + offSetToNextCard * ceil(static_cast<float>(i) - (static_cast<float>(selectedUnits.size()) / 2.f)-1));
        tempY = tempY + this->preCalcincrementalYOffset;
    } else {
        xPosition = static_cast<int>(startDeck + offSetToNextCard * (static_cast<float>(i) - (static_cast<float>(selectedUnits.size()) / 2.f)));
        tempY = tempY + this->preCalcincrementalYOffset;
    }

    listOfButtons.push_back({ xPosition, tempY, static_cast<spriteTypes>(buttonType), actionTypes::actionUnitSelect, actorId, static_cast<int>(listOfButtons.size()), 0 });
}

void gameState::drawActorTitle(int actorId, int textStartX, int textStartY)
{
    text.setString(listOfActors[actorId].nameOfActor());
    text.setCharacterSize(26);
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(2.f);
    text.setFillColor(sf::Color::White);
    text.setPosition(static_cast<float>(textStartX), static_cast<float>(textStartY));
    window.draw(text);
}

void gameState::drawActorBigSprite(int actorId)
{
    this->spriteBigSelectedIcon.setTextureRect(sf::IntRect(128, listOfActorTemplates[listOfActors[actorId].getType()].getBigSpriteYOffset(), 128, 128));
    this->spriteBigSelectedIcon.setPosition(static_cast<float>(mainWindowWidth / 4.08), static_cast<float>(mainWindowHeigth / 30));
    window.draw(this->spriteBigSelectedIcon);
}

void gameState::drawActorStats(int actorId, int textStartX, int textStartY)
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
        text.setString(listOfActors[actorId].getResources());
        text.setPosition(static_cast<float>(textStartX), static_cast<float>(textStartY));
        window.draw(text);
    }
}

void gameState::drawActorToolbar(int spriteYOffset, int tempY, int offSetTonextCard)
{
    bool villagerButtonsAreThere = false;
    int textStartX = static_cast<int>(static_cast<float>((mainWindowWidth / 4.08f) + (128.f + (mainWindowWidth / 160.f))));
    int textStartY = static_cast<int>(static_cast<float>(mainWindowHeigth / 30.f));
    int startX = this->preCalcStartX;
    int startY = this->preCalcStartY;
    for (int i = 0; i < this->selectedUnits.size(); i++)
    {
        if (listOfActors[this->selectedUnits[i]].getTeam() == currentPlayer.getTeam())
        {
            createVillagerButtons(startX, startY, this->preCalcIncrementalXOffset, villagerButtonsAreThere);
        }
        if (this->selectedUnits.size() > 1)
        {
            startY = mainWindowHeigth / 30;
            addActorSelectorButton(i, this->selectedUnits[i], this->preCalcStartDeck, tempY, startY, offSetTonextCard);
        }
        else if (i == 0) {
            drawActorBigSprite(this->selectedUnits[i]);
            drawActorTitle(this->selectedUnits[i], textStartX, textStartY);
            drawActorStats(this->selectedUnits[i], textStartX, textStartY);
        }
    }
}

void gameState::drawBuildingBigSprite(int buildingId)
{
    this->spriteBigSelectedIcon.setTextureRect(sf::IntRect(0, listOfBuildingTemplates[listOfBuildings[buildingId].getType()].getBigSpriteYOffset(), 128, 128));
    this->spriteBigSelectedIcon.setPosition(static_cast<float>(mainWindowWidth / 4.08), static_cast<float>(mainWindowHeigth / 30));
    window.draw(this->spriteBigSelectedIcon);
}

void gameState::drawBuildingToolbarTitle(int textStartX, int textStartY)
{
    text.setString(listOfBuildings[this->buildingSelectedId].getName());
    text.setCharacterSize(26);
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(2.f);
    text.setFillColor(sf::Color::White);
    text.setPosition(static_cast<float>(textStartX), static_cast<float>(textStartY));
    window.draw(text);
}

void gameState::drawBuildingToolbarStats(int textStartX, int textStartY)
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

void gameState::drawProgressBar(float pointsGained, float pointsRequired, int totalBarLength, int startBarX, int startBarY)
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

std::string getBuildingIsProducingName(int buildingId)
{
    if (listOfBuildings[buildingId].getProductionQueue().front().isResearch)
    {
        switch (listOfBuildings[buildingId].getProductionQueue().front().idOfUnitOrResearch)
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
        switch (listOfBuildings[buildingId].getProductionQueue().front().idOfUnitOrResearch)
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

cords getSpriteOffSetTask(int buildingId)
{
    if (listOfBuildings[buildingId].getProductionQueue().front().isResearch)
    {
        switch (listOfBuildings[buildingId].getProductionQueue().front().idOfUnitOrResearch)
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
        switch (listOfBuildings[buildingId].getProductionQueue().front().idOfUnitOrResearch)
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

int getCardForButtonByTask(int buildingId, buildingQueue& task)
{
    if (task.isResearch)
    {
        switch (task.idOfUnitOrResearch)
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
        switch (task.idOfUnitOrResearch)
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

void gameState::drawBuildingTaskToolbar(int startY)
{
    float percentageCompleted = static_cast<float>(static_cast<float>(listOfBuildings[this->buildingSelectedId].getProductionQueue().front().productionPointsGained) / static_cast<float>(listOfBuildings[this->buildingSelectedId].getProductionQueue().front().productionPointsNeeded)) * 100;
    int tempXOffset = 0;
    int tempYOffset = 0;
    drawProgressBar(static_cast<float>(listOfBuildings[this->buildingSelectedId].getProductionQueue().front().productionPointsGained), static_cast<float>(listOfBuildings[this->buildingSelectedId].getProductionQueue().front().productionPointsNeeded), totalBarLength, startBarX, startBarY);
    text.setString("Producing: " + getBuildingIsProducingName(this->buildingSelectedId) + " " + std::to_string(static_cast<int>(percentageCompleted)) + "%...");
    text.setPosition(static_cast<float>(startBarX), static_cast<float>(iconStartY));
    window.draw(text);
    this->spriteUIButton.setTextureRect(sf::IntRect(getSpriteOffSetTask(this->buildingSelectedId).x, getSpriteOffSetTask(this->buildingSelectedId).y, 64, 64));
    this->spriteUIButton.setPosition(static_cast<float>(iconBarStartX), static_cast<float>(iconStartY));
    window.draw(this->spriteUIButton);
    button cancelTask = { static_cast<int>(startBarX + totalBarLength + static_cast<int>(mainWindowWidth / 174.54)), iconStartY, spriteTypes::spriteCancel, actionTypes::actionCancelProduction, this->buildingSelectedId, static_cast<int>(listOfButtons.size()), 0 };
    listOfButtons.push_back(cancelTask);
    if (listOfBuildings[this->buildingSelectedId].getProductionQueue().size() > 1)
    {
        tempXOffset = iconBarStartX + static_cast<int>(mainWindowWidth / 24.93);
        tempYOffset = iconStartY + static_cast<int>(mainWindowHeigth / 22.97);
        int i = 0;
        for (buildingQueue& task : listOfBuildings[this->buildingSelectedId].getProductionQueue()) {
            if (i != 0) {//First button needs to be skipped!
                button tempButton = { tempXOffset, tempYOffset, static_cast<spriteTypes>(getCardForButtonByTask(this->buildingSelectedId, task)), actionTypes::actionCancelProduction, this->buildingSelectedId, static_cast<int>(listOfButtons.size()), i };
                listOfButtons.push_back(tempButton);
                tempXOffset += 64 + static_cast<int>(mainWindowWidth / 160);
            }
            i++;
        }
    }
}

void gameState::drawBuildingConstructionToolbar(int startY)
{
    drawProgressBar(static_cast<float>(listOfBuildings[this->buildingSelectedId].getBuildingPoints().first), static_cast<float>(listOfBuildings[this->buildingSelectedId].getBuildingPoints().second), totalBarLength, startBarX, startBarY);
    float percentageCompleted = static_cast<float>(static_cast<float>(listOfBuildings[this->buildingSelectedId].getBuildingPoints().first) / static_cast<float>(listOfBuildings[this->buildingSelectedId].getBuildingPoints().second)) * 100;
    text.setString("Building: " + listOfBuildings[this->buildingSelectedId].getName() + " " + std::to_string(static_cast<int>(percentageCompleted)) + "%...");
    text.setPosition(static_cast<float>(startBarX), static_cast<float>(iconStartY));
    window.draw(text);
    this->spriteUIButton.setTextureRect(sf::IntRect(0, (listOfBuildingTemplates[listOfBuildings[this->buildingSelectedId].getType()].getBigSpriteYOffset() / 2), 64, 64));
    this->spriteUIButton.setPosition(static_cast<float>(iconBarStartX), static_cast<float>(iconStartY));
    window.draw(this->spriteUIButton);
    button cancelBuilding = { static_cast<int>(startBarX + totalBarLength + (mainWindowWidth / 174.54)), iconStartY, spriteTypes::spriteCancel, actionTypes::actionCancelBuilding, this->buildingSelectedId, static_cast<int>(listOfButtons.size()),0 };
    listOfButtons.push_back(cancelBuilding);
}

void gameState::drawBuildingToolbar(int spriteYOffset, int tempY, int offSetTonextCard)
{
    int textStartX = static_cast<int>(round(static_cast<float>((mainWindowWidth / 4.08f) + (128.f + (mainWindowWidth / 160.f)))));
    int textStartY = static_cast<int>(round(static_cast<float>(mainWindowHeigth / 30.f)));
    int startX = this->preCalcStartX;
    int startY = this->preCalcStartY;
    bool buttonsAreThere = false;
    if (currentPlayer.getTeam() == listOfBuildings[this->buildingSelectedId].getTeam()) {
        listOfBuildingTemplates[listOfBuildings[this->buildingSelectedId].getType()].createBuildingButtons(startX, startY, this->buildingSelectedId, this->preCalcIncrementalXOffset, buttonsAreThere);
    }
    drawBuildingBigSprite(this->buildingSelectedId);
    drawBuildingToolbarTitle(textStartX, textStartY);
    drawBuildingToolbarStats(textStartX, textStartY);

    if (currentPlayer.getTeam() == listOfBuildings[this->buildingSelectedId].getTeam()) {
        //Show what the building is doing ATM
        if (!listOfBuildings[this->buildingSelectedId].getCompleted())
        {
            drawBuildingConstructionToolbar(startY);
        }
        else if (listOfBuildings[this->buildingSelectedId].hasTask())
        {
            drawBuildingTaskToolbar(startY);
        }
    }
}

void gameState::drawObjectToolbar(int spriteYOffset, int tempY, int offSetTonextCard)
{
    this->spriteBigSelectedIcon.setTextureRect(sf::IntRect(256, listOfObjectTemplates[static_cast<uint32_t>(listOfObjects[this->objectSelectedId].getType())].getObjectBigSpriteYOffset(), 128, 128));
    this->spriteBigSelectedIcon.setPosition(static_cast<float>(mainWindowWidth / 4.08), static_cast<float>(mainWindowHeigth / 30));
    window.draw(this->spriteBigSelectedIcon);
    text.setString(listOfObjects[this->objectSelectedId].getName());
    text.setCharacterSize(26);
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(2.f);
    text.setFillColor(sf::Color::White);
    int textStartX = static_cast<int>(round((mainWindowWidth / 4.08f) + (128.f + (mainWindowWidth / 160.f))));
    int textStartY = static_cast<int>(round(mainWindowHeigth / 30.f));
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
    int tempY = this->preCalcStartY;
    int spriteYOffset = 0;
    int cardDeckSize = static_cast<int>(mainWindowWidth / 1.9);
    int amountOfCardsPerRow = static_cast<int>((this->selectedUnits.size()+1)/2);
    int requiredSize = amountOfCardsPerRow*64;
    int devider = static_cast<int>((this->selectedUnits.size()+1)/2);
    if(devider == 0) devider = 1;
    int spaceBetweenCards = (cardDeckSize - requiredSize)/devider;
    int offSetTonextCard = 64 + spaceBetweenCards;
;

    if(!this->selectedUnits.empty())
    {
        drawActorToolbar(spriteYOffset, tempY, offSetTonextCard);
    }
    else if(this->buildingSelectedId != -1)
    {
        drawBuildingToolbar(spriteYOffset, tempY, offSetTonextCard);
    }
    else if(this->objectSelectedId != -1)
    {
        drawObjectToolbar(spriteYOffset, tempY, offSetTonextCard);
    }
    drawButtons();
    window.setView(worldView);
}

void gameState::drawTopBar()
{
    window.setView(topBar);
    playerStats tempStats = currentPlayer.getStats();
    std::stringstream resourcesText;
    int seconds = currentGame.elapsedTimeMS/1000; //Tijd in seconden
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
        "  |                |  Team: " << tempStats.team <<
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
            Button.isHovered(mouseFakePosition);
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
        for (int& id : selectedUnits) {
            listOfActors[id].printActorDebugText();
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

void drawRallyPoints(){
    if (!currentGame.listOfFlagsToDraw.empty()) {
        for (cords& flags : currentGame.listOfFlagsToDraw) {
            currentGame.spriteFlag.setPosition(static_cast<float>(worldSpace(flags).x), static_cast<float>(worldSpace(flags).y));
            window.draw(currentGame.spriteFlag);
        }
    }
}

void gameState::drawMouse() {
    if (mouseFakePosition.y > visableWorldHeight || mouseFakePosition.y < toolbarHeight) {
        window.setView(totalView);
    }

    spriteMousePointer.setTextureRect(sf::IntRect(0, 0, 32, 32));
    if (!selectedUnits.empty()){
        bool villagerInSelection = false;
        bool cursorChanged = false;
        for (int& unitId : selectedUnits) {
            if (listOfActors[unitId].getType() == 0) {
                villagerInSelection = true;
            }
        }

        if (listOfBuildings.size() > 0) {
            for (const buildings& building : listOfBuildings) {
                listOfBuildingTemplates[building.getType()].setSpritePosition(worldSpace(building.getLocation()));
                if (Collision::singlePixelTest(listOfBuildingTemplates[building.getType()].getBuildingSprite(), mousePosition, 128)) {
                    //Mouse cursor for building either attack, build/repair or nothing
                    if (building.getTeam() == currentPlayer.getTeam()) {
                        if (villagerInSelection) {
                            spriteMousePointer.setTextureRect(sf::IntRect(160, 0, 32, 32));
                        }
                    }
                    else {
                        spriteMousePointer.setTextureRect(sf::IntRect(32, 0, 32, 32));
                    }
                    cursorChanged = true;
                }
            }
        }

        if (listOfObjects.size() > 0 && !cursorChanged && villagerInSelection) {
            for (const objects& object : listOfObjects) {
                listOfObjectTemplates[static_cast<uint32_t>(object.getType())].setPosition(worldSpace(object.getLocation()));
                if (Collision::singlePixelTest(listOfObjectTemplates[static_cast<uint32_t>(object.getType())].getSprite(), mousePosition, 128)) {
                    switch (object.getTypeOfResource()) {
                    case resourceTypes::resourceFood:
                        spriteMousePointer.setTextureRect(sf::IntRect(128, 0, 32, 32));
                        break;
                    case resourceTypes::resourceWood:
                        spriteMousePointer.setTextureRect(sf::IntRect(64, 0, 32, 32));
                        break;
                    case resourceTypes::resourceStone:
                        spriteMousePointer.setTextureRect(sf::IntRect(96, 0, 32, 32));
                        break;
                    case resourceTypes::resourceGold:
                        spriteMousePointer.setTextureRect(sf::IntRect(96, 0, 32, 32));
                        break;

                    }
                }
            }
        }

        if (listOfActors.size() > 0 && !cursorChanged){
            for (const actors& actor : listOfActors) {
                listOfActorTemplates[actor.getType()].setSpritePosition(worldSpace(actor.getActorCords()));
                if (Collision::singlePixelTest(listOfActorTemplates[actor.getType()].getActorSprite(), mousePosition, 128)) {
                    if (actor.getTeam() != currentPlayer.getTeam()) {
                        spriteMousePointer.setTextureRect(sf::IntRect(32, 0, 32, 32));
                    }
                }
            }
        }
    }

    if (attackMove) {
        spriteMousePointer.setTextureRect(sf::IntRect(192, 0, 32, 32));
    }
    if (this->isPlacingBuilding) {
        spriteMousePointer.setTextureRect(sf::IntRect(160, 0, 32, 32));
    }

    spriteMousePointer.setPosition(mousePosition);
    window.draw(spriteMousePointer);

}

void gameState::drawGame()
{
    window.clear(sf::Color(0, 0, 0));
    this->listOfFlagsToDraw.clear();
    worldView.setCenter(static_cast<float>(viewOffsetX), static_cast<float>(viewOffsetY));
    window.setView(totalView);
    window.draw(spriteTotalBackground);
    window.setView(worldView);
    createFogOfWar();
    drawTopBar();
    drawMap();
    drawRallyPoints();
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
    window.setView(totalView);
    gameText.drawDebugMessages();
    window.setView(worldView);
    drawMouse();
    window.display();
}

int gameState::getTime() const
{
    return this->elapsedTimeMS;
}

void gameState::loadMap()
{
    for(int i = 0; i < MAP_WIDTH; i++)
    {
        for(int j = 0; j < MAP_HEIGHT; j++)
        {
            this->currentMap[i][j] = 0;
            this->tileBitmask[i][j] = 0;
            this->buildingLocationList[i][j] = -1;
            this->occupiedByBuildingList[i][j] = -1;
            this->objectLocationList[i][j] = -1;
            this->occupiedByActorList[i][j].clear();
            this->visability[i][j] = 0;
        }
    }
    generateRandomMap(this->players,16,16,16,5,0);
    sf::RectangleShape miniMapPixel(sf::Vector2f(20.f, 10.f));
    for (int i = 0; i < 16; i++) {
        drawMiniMapMist(miniMapPixel);
    }
}

void gameState::loadFonts()
{
    if (!this->font.loadFromFile("fonts/arial.ttf"))
    {
        std::cout << "Could not load font arial.ttf!\n";
    }
    this->text.setFont(this->font);
}



void setQuadPosition(sf::Vertex* Quad, int i, int j) {
    Quad[0].position = sf::Vector2f(static_cast<float>(miniMapSpace({ i, j }).x), static_cast<float>(miniMapSpace({ i, j }).y));                  // 0 , 0 = 0 , 0
    Quad[1].position = sf::Vector2f(static_cast<float>(miniMapSpace({ i, j }).x) + 20.f, static_cast<float>(miniMapSpace({ i, j }).y));           // 1 , 0 = 20 , 0
    Quad[2].position = sf::Vector2f(static_cast<float>(miniMapSpace({ i, j }).x) + 20.f, static_cast<float>(miniMapSpace({ i, j }).y) + 10.f);    // 1 , 1 = 20 , 10
    Quad[3].position = sf::Vector2f(static_cast<float>(miniMapSpace({ i, j }).x), static_cast<float>(miniMapSpace({ i, j }).y) + 10.f);           // 0 , 1 = 0 , 10
}

void createMiniMapTexture()
{
    minimapTexture.create(20 * MAP_WIDTH, 10 * MAP_HEIGHT);
    //set pixel positions
    for (int j = 0; j < MAP_HEIGHT; j++)
    {
        for (int i = 0; i < MAP_WIDTH; i++)
        {
            setQuadPosition(&miniMapBackGroundPoints[((i * MAP_HEIGHT) + j) * 4], i, j);
            setQuadPosition(&miniMapBuildingPoints[((i * MAP_HEIGHT) + j) * 4], i, j);
            setQuadPosition(&miniMapActorPoints[((i * MAP_HEIGHT) + j) * 4], i, j);
            setQuadPosition(&miniMapMistPoints[((i * MAP_HEIGHT) + j) * 4], i, j);
            setQuadPosition(&miniMapObjectPoints[((i * MAP_HEIGHT) + j) * 4], i, j);
        }
    }
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
    this->lastMistDraw = -1;
    if (multiplayerPlayers != 1) {
        this->players = multiplayerPlayers;
    }
    else {
        this->players = 1 + AIPlayers;
    }
    this->noFogOfWar = false;
}

void setTeam() {
    for (int i = 0; i < 8; i++)
    {
        listOfPlayers[i].setTeam(i);
    }
}

void showLoadingScreen() {

    splashScreenSprite.setColor(sf::Color(40, 40, 40));
    currentGame.text.setCharacterSize(64);
    currentGame.text.setOutlineColor(sf::Color::White);
    currentGame.text.setOutlineThickness(1.f);
    currentGame.text.setFillColor(sf::Color::Yellow);
    currentGame.text.setString("Loading...");
    sf::FloatRect textRect = currentGame.text.getLocalBounds();
    currentGame.text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    currentGame.text.setPosition(static_cast<float>(mainWindowWidth/2), static_cast<float>(mainWindowHeigth/2));
    window.clear();
    window.draw(splashScreenSprite);
    window.draw(currentGame.text);
    window.display();
    currentGame.text.setOrigin(0.0f, 0.0f);
}

void loadBaseCellList() {
    int n = 0;
    for (int i = 0; i < MAP_WIDTH; i++)
    {
        for (int j = 0; j < MAP_HEIGHT; j++)
        {
            baseCellList.push_back(Cells({ i, j }, n));
            n++;
        }
    }
}

void sendReadyForLaunch() {
    sf::Packet readyPacket;
    sf::Uint8 header = dataType::startGame;
    readyPacket << header;
    currentConnection.getTcpSocket()->send(readyPacket);
}

void waitUntilAllReady() {
    sendReadyForLaunch();
    bool everyOneReady =false;
    while (!everyOneReady) {
        sf::Packet recievePacket;
        sf::Socket::Status statusOfSocket = currentConnection.getTcpSocket()->receive(recievePacket);
        if (statusOfSocket == sf::Socket::Disconnected) {
            //complain
        }
        else if (statusOfSocket == sf::Socket::Done) {
            sf::Uint8 recievedHeader;
            recievePacket >> recievedHeader;
            switch (recievedHeader) {
            case dataType::startGame:
                everyOneReady = true;
                break;
            }
        }
    }
}

void gameState::loadGame()
{
    showLoadingScreen();
    createMiniMapTexture();
    loadTextures();
    loadActors();
    loadObjects();
    setDefaultValues();
    loadBuildings();
    setTeam();
    if (currentConnection.isConnected() && multiplayerPlayerId != 0) {
        for (int i = 0; i < MAP_WIDTH; i++)
        {
            for (int j = 0; j < MAP_HEIGHT; j++)
            {
                this->currentMap[i][j] = 0;
                this->tileBitmask[i][j] = 0;
                this->buildingLocationList[i][j] = -1;
                this->occupiedByBuildingList[i][j] = -1;
                this->objectLocationList[i][j] = -1;
                this->occupiedByActorList[i][j].clear();
                this->visability[i][j] = 0;
            }
        }

        bool startStateRecieved = false;
        bool mapRecieved = false;
        bool actorBlobRecieved = false;
        bool buildingsBlobRecieved = false;
        bool objectsBlobRecieved = false;
        bool playerBlobRecieved = false;
        while (!startStateRecieved) {
            sf::Packet recievePacket;
            sf::Socket::Status statusOfSocket = currentConnection.getTcpSocket()->receive(recievePacket);
            if (statusOfSocket == sf::Socket::Disconnected) {
                //complain
            }
            else if (statusOfSocket == sf::Socket::Done) {
                sf::Uint8 recievedHeader;
                recievePacket >> recievedHeader;
                switch (recievedHeader) {

                case dataType::mapObjectBlob:
                {
                    int mapWidth, mapHeight;
                    recievePacket >> mapWidth;
                    recievePacket >> mapHeight;
                    for (int i = 0; i < mapWidth; i++) {
                        for (int j = 0; j < mapHeight; j++) {
                            recievePacket >> currentMap[i][j];
                        }
                    }
                    mapRecieved = true;
                }
                smoothTerrain();
                break;

                case dataType::actorsBlob:
                    sf::Int32 amountOfActors;
                    recievePacket >> amountOfActors;
                    for (int i = 0; i < amountOfActors; i++) {
                        int x;
                        int y;
                        int team;
                        int type;
                        recievePacket >> x;
                        recievePacket >> y;
                        recievePacket >> team;
                        recievePacket >> type;
                        listOfActors.push_back(actors(type, { x, y }, team, static_cast<int>(listOfActors.size())));
                    }
                    actorBlobRecieved = true;

                    break;

                case dataType::buildingsBlob:
                    sf::Int32 amountOfBuildings;
                    recievePacket >> amountOfBuildings;
                    for (int i = 0; i < amountOfBuildings; i++) {
                        int x;
                        int y;
                        int team;
                        int type;
                        recievePacket >> x;
                        recievePacket >> y;
                        recievePacket >> team;
                        recievePacket >> type;
                        listOfBuildings.push_back(buildings(type, { x,y }, static_cast<int>(listOfBuildings.size()), team));
                    }
                    buildingsBlobRecieved = true;
                    break;

                case dataType::objectsBlob:
                    sf::Int32 amountOfObjects;
                    recievePacket >> amountOfObjects;
                    for (int i = 0; i < amountOfObjects; i++) {
                        int x;
                        int y;
                        int type;
                        recievePacket >> x;
                        recievePacket >> y;
                        recievePacket >> type;
                        listOfObjects.push_back(objects(static_cast<objectTypes>(type), { x,y }, static_cast<int>(listOfObjects.size())));
                    }
                    objectsBlobRecieved = true;
                    break;

                case dataType::playersBlob:
                    for (int i = 0; i < 8; i++) {
                        int food;
                        int wood;
                        int stone;
                        int gold;
                        bool defeated;
                        recievePacket >> food;
                        recievePacket >> wood;
                        recievePacket >> stone;
                        recievePacket >> gold;
                        recievePacket >> defeated;
                        listOfPlayers[i].syncPlayer(food, wood, stone, gold, defeated);
                    }
                    playerBlobRecieved = true;
                    break;
                }
            }
            if (mapRecieved && actorBlobRecieved && buildingsBlobRecieved && objectsBlobRecieved && playerBlobRecieved) {
                startStateRecieved = true;
            }
        }

        sf::RectangleShape miniMapPixel(sf::Vector2f(20.f, 10.f));
        for (int i = 0; i < 16; i++) {
            drawMiniMapMist(miniMapPixel);
        }
        currentPlayer = listOfPlayers[multiplayerPlayerId];
        centerViewOnVillager();
        loadBaseCellList();
        waitUntilAllReady();
        return;
    }
    loadMap();
    loadBaseCellList();
    if (currentConnection.isConnected()) {
        //send data to recreate actors;
        sf::Packet actorDataPacket;
        sf::Uint8 actorDataHeader = dataType::actorsBlob;
        actorDataPacket << actorDataHeader;
        sf::Int32 amountOfActors = listOfActors.size();
        actorDataPacket << amountOfActors;

        for (actors& a : listOfActors) {
            int x = a.getActorCords().x;
            int y = a.getActorCords().y;
            int team = a.getTeam();
            int type = a.getType();
            actorDataPacket << x << y << team << type;
        }
        currentConnection.getTcpSocket()->send(actorDataPacket);

        //send data to recreate buildings
        sf::Packet buildingDataPacket;
        sf::Uint8 buildingDataHeader = dataType::buildingsBlob;
        buildingDataPacket << buildingDataHeader;
        sf::Int32 amountOfBuildings = listOfBuildings.size();
        buildingDataPacket << amountOfBuildings;

        for (buildings& a : listOfBuildings) {
            int x = a.getLocation().x;
            int y = a.getLocation().y;
            int team = a.getTeam();
            int type = a.getType();
            buildingDataPacket << x << y << team << type;
        }
        currentConnection.getTcpSocket()->send(buildingDataPacket);

        //send data to recreate objects
        sf::Packet objectDataPacket;
        sf::Uint8 objectDataHeader = dataType::objectsBlob;
        objectDataPacket << objectDataHeader;
        sf::Int32 amountOfObjects = listOfObjects.size();
        objectDataPacket << amountOfObjects;
        for (objects& a : listOfObjects) {
            int x = a.getLocation().x;
            int y = a.getLocation().y;
            int type = static_cast<int>(a.getType());
            objectDataPacket << x << y << type;
        }
        currentConnection.getTcpSocket()->send(objectDataPacket);

        //send data to recreate players
        sf::Packet playerDataPacket;
        sf::Uint8 playerDataHeader = dataType::playersBlob;
        playerDataPacket << playerDataHeader;
        for (player& a : listOfPlayers) {
            int food = a.getStats().amountOfFood;
            int wood = a.getStats().amountOfWood;
            int stone = a.getStats().amountOfStone;
            int gold = a.getStats().amountOfGold;
            bool defeated = a.isPlayerDefeated();
            playerDataPacket << food << wood << stone << gold << defeated;
        }
        currentConnection.getTcpSocket()->send(playerDataPacket);


        //sent map, with data on amount of actors, buildings and objects
        sf::Packet mapDataPacket;
        sf::Uint8 mapDataHeader = dataType::mapObjectBlob;
        mapDataPacket << mapDataHeader;
        mapDataPacket << MAP_WIDTH;
        mapDataPacket << MAP_HEIGHT;
        for (int i = 0; i < MAP_WIDTH; i++) {
            for (int j = 0; j < MAP_HEIGHT; j++) {
                mapDataPacket << currentMap[i][j];
            }
        }
        currentConnection.getTcpSocket()->send(mapDataPacket);
        waitUntilAllReady();
    }
}

void gameState::createFogOfWar()
{
    if (lastFogOfWarUpdated + 500 < this->elapsedTimeMS) {
        if (!noFogOfWar) {
            for (int i = 0; i <MAP_WIDTH; i++) {
                for (int j = 0; j < MAP_WIDTH; j++) {
                    if (this->visability[i][j] == 2) {
                        this->visability[i][j] = 1;
                    }
                }
            }
            for (int i = 0; i < listOfActors.size(); i++) {
                if (listOfActors[i].getTeam() == currentPlayer.getTeam()) {
                    std::list<cords> tempList = getListOfCordsInCircle(listOfActors[i].getActorCords().x, listOfActors[i].getActorCords().y, 6);
                    for (const cords& cord : tempList)
                    {
                        this->visability[cord.x][cord.y] = 2;
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
                            this->visability[cord.x][cord.y] = 2;
                        }
                    }
                }
            }
        }
        else {
            for (int i = 0; i < MAP_WIDTH; i++) {
                for (int j = 0; j < MAP_WIDTH; j++) {
                    this->visability[i][j] = 2;
                }
            }
        }
        lastFogOfWarUpdated = this->elapsedTimeMS;
    }
}
