#ifndef GAMESTATE_H
#define GAMESTATE_H
#include <SFML/Graphics.hpp>
#include "globalfunctions.h"
#include "button.h"
#include <list>
#include <sstream>


extern int mainWindowWidth;
extern int mainWindowHeigth;
extern sf::RenderWindow window;
extern sf::View totalView;
extern sf::View topBar;
extern sf::View worldView;
extern sf::View toolBar;
extern sf::View miniMap;
extern int viewOffsetX;
extern int viewOffsetY;
extern sf::RenderTexture minimapTexture;
extern sf::RenderTexture minimapBuildingsTexture;
extern sf::RenderTexture minimapActorsTexture;
extern sf::RenderTexture minimapObjectsTexture;
extern bool noNewBuildings;
extern bool minimapTextureExist;
extern std::list<button> listOfButtons;

extern double dist(double x1, double y1, double x2, double y2);

struct actorOrBuildingPrice
{
    int food;
    int wood;
    int stone;
    int gold;
};

extern std::vector<actorOrBuildingPrice> priceOfBuilding;
extern std::vector<actorOrBuildingPrice> priceOfActor;

struct adjacentTile
{
    int tileId;
    int tileX;
    int tileY;
    int goalX;
    int goalY;
    bool occupied;
    int actorId;
};

mouseWorldCord toWorldMousePosition(int mouseX, int mouseY);

class gameState
{

public:
    void drawMousePosition(int x,int y, int noProblem);
    void drawGround(int i, int j);
    void drawThingsOnTile(int i, int j);
    void drawMap();
    void loadTextures();
    void calculateRectangle();
    void interact();
    void drawMouseInteraction();
    void drawGame();

    void drawToolbar();
    void drawActorTitle(int& actorId, int& textStartX, int& textStartY);
    void drawActorBigSprite(int& actorId);
    void drawActorToolbar(int& startX, int& startY, int& incrementalXOffset, int& spriteYOffset, int& startDeck, int& tempY, int& incrementalYOffset, int& offSetTonextCard);
    void drawActorStats(int& actorId, int& textStartX, int& textStartY);
    void drawBuildingToolbar(int& startX, int& startY, int& incrementalXOffset, int& spriteYOffset, int& startDeck, int& tempY, int& incrementalYOffset, int& offSetTonextCard);
    void drawBuildingBigSprite(int& buildingId);
    void drawBuildingToolbarTitle(int& textStartX, int& textStartY);
    void drawBuildingToolbarStats(int& textStartX, int& textStartY);
    void drawBuildingConstructionToolbar(int& startDeck, int& startY);
    void drawBuildingTaskToolbar(int& startDeck, int& startY);
    void drawProgressBar(float pointsGained, float pointsRequired, int& totalBarLength, int& startBarX, int& startBarY);
    void drawObjectToolbar(int& startX, int& startY, int& incrementalXOffset, int& spriteYOffset, int& startDeck, int& tempY, int& incrementalYOffset, int& offSetTonextCard);

    void loadMap();
    void loadBuildings();
    void loadGame();
    void drawMiniMap();
    void drawTopBar();
    void mouseLeftClick();
    void changeViewFromMiniMap();
    void clickUIButton();
    void clickToPlaceBuilding();
    void clickToPlaceObject();
    void clickToPlaceActor();
    void clickToSelectObjectOrBuilding();
    void clickToSelect();
    void getDefinitiveSelection();
    void clickToMove();
    void clickToGatherResource();
    void clickToBuildOrRepairBuilding();
    void mouseRightClick();
    void changeTiles();
    void edgeScrolling();
    void changeBuildingType();
    void changeObjectType();
    void clickToGiveCommand();

    float elapsedTime;
    bool isPassable(int x, int y);
    bool isInSelectedActors(int id);
    void setBuildingType(int id);
    void setIsPlacingBuilding();
    void selectUnit(int id);
    float getTime();
    int buildingSelectedId;
    int objectSelectedId;
    mouseWorldCord getNextCord(int x, int y);
    sf::Font font;
    sf::Text text;
    sf::Sprite  spriteSelectedTile,spriteSelectedTileForPath, spriteEmptyTile, spriteGrassTile, spriteSandTile, spriteSandTileNE,
    spriteSandTileNW,spriteSandTileSE,spriteSandTileSW, spriteCactusTile, spriteCypressTrileTile, spriteBigSelectedIcon,
    spriteNormalTreeTile, spritePineTreeTile, spriteTileObstructed, spriteTownCenter,
    spriteStone, spriteGold, spriteBerryBush, spriteUIButton, spriteUnitSelectedTile,
    spriteWaterTile, spriteBeachTileNE, spriteBeachTileNW,spriteBeachTileSE,spriteBeachTileSW, spriteBuildingHouse, spriteVillager, spriteTotalBackground;

    sf::Texture textureSelectedTile, textureSelectedTileForPath, textureEmptyTile, textureGrassTile, textureSandTile, textureSandTileNE,
    textureSandTileNW,textureSandTileSE,textureSandTileSW, textureCactusTile, textureCypressTrileTile, textureBigSelectedIcon,
    textureNormalTreeTile, texturePineTreeTile, textureCheatTile, textureTileObstructed, textureTownCenter,
    textureStone, textureGold, textureBerryBush, textureUIButton, textureUnitSelectedTile,
    textureWaterTile, textureBeachTileNE, textureBeachTileNW,textureBeachTileSE, textureBeachTileSW, textureBuildingHouse, textureVillager, textureTotalBackground;
    int currentMap[MAP_WIDTH][MAP_HEIGHT];
    int buildingLocationList[MAP_WIDTH][MAP_HEIGHT];
    int objectLocationList[MAP_WIDTH][MAP_HEIGHT];
    int occupiedByBuildingList[MAP_WIDTH][MAP_HEIGHT];
    int occupiedByActorList[MAP_WIDTH][MAP_HEIGHT];


private:
    sf::Event event;
    bool roundDone;
    bool addSubstractX;
    bool addSubstractY;
    bool firstRound;
    int lastIandJ[2];
    float topBarHeigth;
    bool focus;
    bool mousePressedLeft;
    bool equalIsPressed;
    bool mousePressedRight;
    bool isPressedB;
    bool isPressedO;
    bool isPressedA;
    bool isPressedTab;
    bool isPlacingBuilding;
    bool mousePressOutofWorld;
    int buildingTypeSelected;
    int objectTypeSelected;
    int startMouseCords[2];
    int startLocation[2];
    float viewBoxX;
    float viewBoxY;
    int mapPixelWidth;
    int mapPixelHeigth;
    float miniMapWidth;
    float miniMapHeigth;
    int toolBarWidth;
    mouseWorldCord mouseWorldPosition;
    std::vector<rectangleCord> rectangleCords;
    sf::Vector2i mouseFakePosition;
    sf::Vector2f mousePosition;
    std::vector<int> selectedUnits;


};
extern gameState currentGame;


#endif // GAMESTATE_H
