#ifndef GLOBALFUNCTIONS_H
#define GLOBALFUNCTIONS_H
#define MAP_WIDTH	256
#define MAP_HEIGHT	256
#include <mutex>
#include <list>

extern int mapOffsetX;
extern int mapOffsetY;

struct cords {
    int x;
    int y;
};


static cords toWorldMousePosition(int mouseX, int mouseY);
int roll(int min, int max);


bool rectCord (cords& lhs, cords& rhs);
bool compareCord(const cords& lhs, const cords& rhs);
bool sortCordByX(const cords& lhs, const cords& rhs);
int worldSpace(int x, int y, bool getX);
int miniMapSpace(int x, int y, bool getX);
std::list<cords> getListOfCordsInCircle(int startX, int startY, int r);

extern std::mutex listOfActorsMutex;


#endif // GLOBALFUNCTIONS_H
