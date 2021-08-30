#ifndef GLOBALFUNCTIONS_H
#define GLOBALFUNCTIONS_H

//#include <mutex>
#include <list>
#include <iostream>
#include <algorithm>
#include <vector>
#include <random>

const int MAP_WIDTH = 256;
const int MAP_HEIGHT = 256;
const int mainWindowWidth = 1920;
const int mainWindowHeigth = 1080;
const int halfOfMainWindowWidth = mainWindowWidth / 2;
const int halfOfMainWindowHeigth = mainWindowHeigth / 2;
const int visableHalfOfMainWindowWidth = static_cast<int>((mainWindowWidth * 0.8) / 2);

extern int mapOffsetX;
extern int mapOffsetY;

struct cords {
    int x;
    int y;
};

struct footprintOfBuilding
{
    int amountOfXFootprint;
    int amountOfYFootprint;
};

struct actorOrBuildingPrice
{
    int food;
    int wood;
    int stone;
    int gold;
    int productionPoints;
};

cords toWorldMousePosition(int mouseX, int mouseY);
int roll(int min, int max);

bool rectCord (const cords& lhs, const cords& rhs);
bool compareCord(const cords& lhs, const cords& rhs);
bool sortCordByX(const cords& lhs, const cords& rhs);
cords worldSpace(cords location);
cords miniMapSpace(cords location);
std::list<cords> getListOfCordsInCircle(int startX, int startY, int r);
std::list<cords> bresenham(cords first, cords second);
double dist(double x1, double y1, double x2, double y2);
double distEuclidean(double x1, double y1, double x2, double y2);
cords toWorldMousePosition(int mouseX, int mouseY);




#endif // GLOBALFUNCTIONS_H
