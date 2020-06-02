#ifndef GLOBALFUNCTIONS_H
#define GLOBALFUNCTIONS_H
#define MAP_WIDTH	256
#define MAP_HEIGHT	256
#include <mutex>
#include <list>

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
std::list<mouseWorldCord> getListOfCordsInCircle(int startX, int startY, int r);

extern std::mutex listOfActorsMutex;


#endif // GLOBALFUNCTIONS_H
