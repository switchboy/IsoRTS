#ifndef GAMESTATE_H
#define GAMESTATE_H
#include <list>
#include <set>
#include <array>
#include <sstream>
#include <SFML/Graphics.hpp>
#include "button.h"
#include "globalfunctions.h"
#include "humanReadableNames.h"
#include "templateBuildings.h"

struct adjacentTile
{
    int tileId;
    cords tileCords;
    cords goal;
    bool occupied;
    int actorId;
};

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
    bool clickToMove(cords pos, bool minimap) const;
    bool isInSelectedActors(int id) const;
    bool isPassable(cords location) const;
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
    void drawActorToolbar(int spriteYOffset, int tempY, int offSetTonextCard);
    void drawBuildingBigSprite(int buildingId);
    void drawBuildingConstructionToolbar(int startY);
    void drawBuildingTaskToolbar(int startY);
    void drawBuildingToolbar(int spriteYOffset, int tempY, int offSetTonextCard);
    void drawBuildingToolbarStats(int textStartX, int textStartY);
    void drawBuildingToolbarTitle(int textStartX, int textStartY);
    void drawGame();
    void drawGround(int i, int j);
    void drawMap();
    void drawMiniMap();
    void drawMiniMapActors(sf::RectangleShape& miniMapPixel);
    void drawMiniMapBackground(sf::RectangleShape& miniMapPixel);
    void drawMiniMapMist(sf::RectangleShape& miniMapPixel);
    void drawMiniMapObjects(sf::RectangleShape& miniMapPixel);
    void drawMouseBox();
    void drawMouseInteraction();
    void drawMousePosition(int x, int y, bool noProblem);
    void drawObjectToolbar(int spriteYOffset, int tempY, int offSetTonextCard);
    void drawPaths();
    void drawProgressBar(float pointsGained, float pointsRequired, int totalBarLength, int startBarX, int startBarY);
    void drawThingsOnTile(int i, int j);
    void drawToolbar();
    void drawToolTip();
    void drawTopBar();
    void edgeScrolling() const;
    void getDefinitiveSelection();
    void interact();
    void loadFonts();
    void loadGame();
    void loadMap();
    void loadTextures();
    void mouseLeftClick();
    void mouseRightClick();
    void orderRallyPoint() const;
    void selectUnit(int id);
    void setObjectsHaveChanged();

    cords getNextCord(cords pos);
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
    sf::Sprite  spriteSelectedTile,spriteSelectedTileForPath, spriteEmptyTile, spriteGrassTile, spriteSandTile, spriteSandTileNE, 
    spriteSandTileNW,spriteSandTileSE,spriteSandTileSW, spriteCactusTile, spriteCypressTrileTile, spriteBigSelectedIcon, spriteSwordsman, spriteCommandCursor, 
    spriteNormalTreeTile, spritePineTreeTile, spriteTileObstructed, spriteBlackTile, spriteArrow, spriteFlag,
    spriteStone, spriteGold, spriteBerryBush, spriteUIButton, spriteUnitSelectedTile, spriteMistTile, spriteWaterTile, spriteBeachTileNE, spriteBeachTileNW,spriteBeachTileSE,spriteBeachTileSW, spriteVillager, spriteTotalBackground;

    sf::Texture textureSelectedTile, textureSelectedTileForPath, textureEmptyTile, textureGrassTile, textureSandTile, textureSandTileNE, textureSwordsman,
    textureSandTileNW,textureSandTileSE,textureSandTileSW, textureCactusTile, textureCypressTrileTile, textureBigSelectedIcon, textureBlackTile, textureCommandCursor,
    textureNormalTreeTile, texturePineTreeTile, textureCheatTile, textureTileObstructed, textureTownCenter, textureMistTile, textureArrow, textureFlag,
    textureStone, textureGold, textureBerryBush, textureUIButton, textureUnitSelectedTile,
    textureWaterTile, textureBeachTileNE, textureBeachTileNW,textureBeachTileSE, textureBeachTileSW, textureVillager, textureTotalBackground;
    sf::RectangleShape selectionRectangle;
    sf::RectangleShape healthBarBackground;
    sf::RectangleShape healthBarGreenBar;

    std::array<std::array<int, MAP_HEIGHT>, MAP_WIDTH> buildingLocationList;
    std::array<std::array<int, MAP_HEIGHT>, MAP_WIDTH> currentMap;
    std::array<std::array<int, MAP_HEIGHT>, MAP_WIDTH> objectLocationList;
    std::array<std::array<int, MAP_HEIGHT>, MAP_WIDTH> occupiedByBuildingList;
    std::array<std::array<int, MAP_HEIGHT>, MAP_WIDTH> occupiedByActorList;
    std::array<std::array<int, MAP_HEIGHT>, MAP_WIDTH> visability;


    bool showPaths;

private:
    bool addSubstractX;
    bool addSubstractY;
    bool equalIsPressed;
    bool firstRound;
    bool focus;
    bool fogOfWarDrawnOnce = false;
    bool isPlacingBuilding;
    bool isPressedA;
    bool isPressedB;
    bool isPressedO;
    bool isPressedS;
    bool isPressedShift;
    bool isPressedTab;
    bool miniMapBackGroundDrawn = false;
    bool mousePressedLeft;
    bool mousePressedRight;
    bool mousePressOutofWorld;
    bool noFogOfWar;
    bool objectsChanged = true;
    bool roundDone;
    float lastFogOfWarUpdated = -0.75f;
    float lastMistDraw = -0.5f;
    float lastMiniMapRefresh = -1.0f;
    float miniMapHeigth;
    float miniMapWidth;
    float topBarHeigth;
    float viewBoxX;
    float viewBoxY;
    int buildingTypeSelected;
    int lastIandJ[2];
    int lastFogOfWarMinimapSectorUpdated = 0;
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
    std::vector<cords> rectangleCords;
    sf::Vector2i mouseFakePosition;
    sf::Vector2f mousePosition;

    //precalculate toolbar values Note: not orderd alphabethicly because some values are needed in other calculations
    const int preCalcStartX = static_cast<int>(round(mainWindowWidth / 60.f));
    const int preCalcStartY = static_cast<int>(round(mainWindowHeigth / 30.f));
    const int iconStartY = static_cast<int>(round(preCalcStartY + static_cast<float>(mainWindowHeigth / 27.f)));
    const int preCalcCardDeckSize = static_cast<int>(round(mainWindowWidth / 1.82f));
    const int preCalcIncrementalXOffset = static_cast<int>(round(64.f + (mainWindowWidth / 160.f)));
    const int preCalcincrementalYOffset = static_cast<int>(round(64.f + (mainWindowHeigth / 90.f)));
    const int preCalcStartDeck = static_cast<int>(round(mainWindowWidth / 4.2f));
    const int preCalcStartProgress = static_cast<int>(round(mainWindowWidth / 2.48f));
    const int iconStartX = static_cast<int>(round(preCalcStartDeck + static_cast<float>(mainWindowWidth / 30.f)));
    const int iconBarStartX = static_cast<int>(round(iconStartX + mainWindowWidth / 6.25f));
    const int startBarX = static_cast<int>(round(iconStartX + static_cast<float>(mainWindowWidth / 5.0f)));
    const int startBarY = static_cast<int>(round(iconStartY + static_cast<float>(mainWindowHeigth / 46.9f)));
    const int totalBarLength = static_cast<int>(round(static_cast<float>(mainWindowWidth / 6.4f)));
    const int spaceBetweenFogOfWarSectorsOnXAxis = MAP_WIDTH / 4;
    const int spaceBetweenFogOfWarSectorsOnYAxis = MAP_HEIGHT / 4;

};

extern gameState currentGame;

#endif // GAMESTATE_H
