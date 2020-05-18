#ifndef GLOBALFUNCTIONS_H
#define GLOBALFUNCTIONS_H
#define MAP_WIDTH	200
#define MAP_HEIGHT	200
#include <mutex>

extern int mapOffsetX;
extern int mapOffsetY;

struct mouseWorldCord
{
    int x;
    int y;
};

static mouseWorldCord toWorldMousePosition(int mouseX, int mouseY);

struct rectangleCord
{
    int x, y;
};

bool rectCord (rectangleCord& lhs, rectangleCord& rhs);
bool compareCord(const rectangleCord& lhs, const rectangleCord& rhs);
bool sortCordByX(const rectangleCord& lhs, const rectangleCord& rhs);
int worldSpace(int x, int y, bool getX);
int miniMapSpace(int x, int y, bool getX);

extern std::mutex listOfActorsMutex;


#endif // GLOBALFUNCTIONS_H
