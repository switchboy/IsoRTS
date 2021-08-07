#ifndef GAMESTATE_H
#define GAMESTATE_H
#include <list>
#include <set>
#include <sstream>
#include <SFML/Graphics.hpp>
#include "button.h"
#include "globalfunctions.h"
#include "humanReadableNames.h"

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

struct actorOrBuildingPrice
{
    int food;
    int wood;
    int stone;
    int gold;
    int productionPoints;
};

extern std::vector<actorOrBuildingPrice> priceOfBuilding;
extern std::vector<actorOrBuildingPrice> priceOfActor;

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

class gameState
{

public:
    bool buildingIsSelected(int id) const;
    bool clickToAttack() const;
    bool clickToBuildOrRepairBuilding() const;
    bool clickToGatherResource() const;
    bool clickToMove(int posX, int posY, bool minimap) const;
    bool isInSelectedActors(int id) const;
    bool isPassable(int x, int y) const;
    void calculateRectangle();
    void changeBuildingType();
    void changeObjectType();
    void changeTiles();
    void changeViewFromMiniMap() const;
    void clickToGiveCommand();
    void clickToGiveMinimapCommand();
    void clickToPlaceActor() const;
    void clickToPlaceBuilding();
    void clickToPlaceObject() const;
    void clickToSelect();
    void clickToSelectObjectOrBuilding();
    void clickUIButton() const;
    void createFogOfWar();
    void drawActorBigSprite(int actorId);
    void drawActorStats(int actorId, int textStartX, int textStartY);
    void drawActorTitle(int actorId, int textStartX, int textStartY);
    void drawActorToolbar(int startX, int startY, int incrementalXOffset, int spriteYOffset, int startDeck, int tempY, int incrementalYOffset, int offSetTonextCard);
    void drawBuildingBigSprite(int buildingId);
    void drawBuildingConstructionToolbar(int startDeck, int startY);
    void drawBuildingTaskToolbar(int startDeck, int startY);
    void drawBuildingToolbar(int startX, int startY, int incrementalXOffset, int spriteYOffset, int startDeck, int tempY, int incrementalYOffset, int offSetTonextCard);
    void drawBuildingToolbarStats(int textStartX, int textStartY);
    void drawBuildingToolbarTitle(int textStartX, int textStartY);
    void drawGame();
    void drawGround(int i, int j);
    void drawMap();
    void drawMiniMap();
    void drawMiniMapActors(sf::RectangleShape& miniMapPixel);
    void drawMiniMapMist(sf::RectangleShape& miniMapPixel);
    void drawMouseBox();
    void drawMouseInteraction();
    void drawMousePosition(int x, int y, bool noProblem);
    void drawObjectToolbar(int startX, int startY, int incrementalXOffset, int spriteYOffset, int startDeck, int tempY, int incrementalYOffset, int offSetTonextCard);
    void drawPaths();
    void drawProgressBar(float pointsGained, float pointsRequired, int totalBarLength, int startBarX, int startBarY);
    void drawThingsOnTile(int i, int j);
    void drawToolbar();
    void drawToolTip();
    void drawTopBar();
    void edgeScrolling() const;
    void getDefinitiveSelection();
    void interact();
    void loadBuildings();
    void loadFonts();
    void loadGame();
    void loadMap();
    void loadTextures();
    void mouseLeftClick();
    void mouseRightClick();
    void orderRallyPoint() const;
    void selectUnit(int id);

    cords getNextCord(int x, int y);
    float getTime() const;
    int   getPlayerCount() const;

    void setBuildingType(int id);
    void setDefaultValues();
    void setIsPlacingBuilding();

    float elapsedTime;
    int buildingSelectedId;
    int objectSelectedId;

    sf::Font font;
    sf::Text text;
    sf::Sprite  spriteSelectedTile,spriteSelectedTileForPath, spriteEmptyTile, spriteGrassTile, spriteSandTile, spriteSandTileNE, spriteBuildingMill, spriteBuildingLumberCamp, spriteBuildingBarracks,
    spriteSandTileNW,spriteSandTileSE,spriteSandTileSW, spriteCactusTile, spriteCypressTrileTile, spriteBigSelectedIcon, spriteSwordsman, spriteCommandCursor, spriteBuildingMiningCamp,
    spriteNormalTreeTile, spritePineTreeTile, spriteTileObstructed, spriteTownCenter, spriteBlackTile, spriteArrow, spriteFlag,
    spriteStone, spriteGold, spriteBerryBush, spriteUIButton, spriteUnitSelectedTile, spriteMistTile,
    spriteWaterTile, spriteBeachTileNE, spriteBeachTileNW,spriteBeachTileSE,spriteBeachTileSW, spriteBuildingHouse, spriteVillager, spriteTotalBackground;

    sf::Texture textureSelectedTile, textureSelectedTileForPath, textureEmptyTile, textureGrassTile, textureSandTile, textureSandTileNE, textureBuildingMill, textureBuildingLumberCamp, textureBuildingBarracks, textureSwordsman,
    textureSandTileNW,textureSandTileSE,textureSandTileSW, textureCactusTile, textureCypressTrileTile, textureBigSelectedIcon, textureBlackTile, textureCommandCursor, textureBuildingMiningCamp,
    textureNormalTreeTile, texturePineTreeTile, textureCheatTile, textureTileObstructed, textureTownCenter, textureMistTile, textureArrow, textureFlag,
    textureStone, textureGold, textureBerryBush, textureUIButton, textureUnitSelectedTile,
    textureWaterTile, textureBeachTileNE, textureBeachTileNW,textureBeachTileSE, textureBeachTileSW, textureBuildingHouse, textureVillager, textureTotalBackground;
    sf::RectangleShape selectionRectangle;
    sf::RectangleShape healthBarBackground;
    sf::RectangleShape healthBarGreenBar;

    int currentMap[MAP_WIDTH][MAP_HEIGHT];
    int buildingLocationList[MAP_WIDTH][MAP_HEIGHT];
    int objectLocationList[MAP_WIDTH][MAP_HEIGHT];
    int occupiedByBuildingList[MAP_WIDTH][MAP_HEIGHT];
    int occupiedByActorList[MAP_WIDTH][MAP_HEIGHT];
    bool showPaths;

private:
    bool addSubstractX;
    bool addSubstractY;
    bool equalIsPressed;
    bool firstRound;
    bool focus;
    bool isPlacingBuilding;
    bool isPressedA;
    bool isPressedB;
    bool isPressedO;
    bool isPressedS;
    bool isPressedShift;
    bool isPressedTab;
    bool mousePressedLeft;
    bool mousePressedRight;
    bool mousePressOutofWorld;
    bool noFogOfWar;
    bool roundDone;
    float lastMistDraw;
    float miniMapHeigth;
    float miniMapWidth;
    float topBarHeigth;
    float viewBoxX;
    float viewBoxY;
    int buildingTypeSelected;
    int lastIandJ[2];
    int mapPixelHeigth;
    int mapPixelWidth;
    int objectTypeSelected;
    int players;
    int startLocation[2];
    int startMouseCords[2];
    int toolBarWidth;

    cords mouseWorldPosition;
    sf::Event event;
    std::vector<int> selectedUnits;
    std::vector<int> visability;
    std::vector<cords> rectangleCords;
    sf::Vector2i mouseFakePosition;
    sf::Vector2f mousePosition;
};

extern gameState currentGame;

#endif // GAMESTATE_H
