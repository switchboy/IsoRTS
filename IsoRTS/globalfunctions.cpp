#include "globalfunctions.h"
#include "gamestate.h"

bool sortCord(cords& lhs, cords& rhs)
{
    return (lhs.x < rhs.x) || (lhs.y < rhs.y);
}

bool compareCord(const cords& lhs, const cords& rhs)
{
    return ((lhs.x == rhs.x) && (lhs.y == rhs.y));
}

bool sortCordByX(const cords& lhs, const cords& rhs)
{
    if (lhs.x != rhs.x) {
        return lhs.x < rhs.x;
    }
    else {
        return lhs.y < rhs.y;
    }
}

int roll(int min, int max)
{
    {
        return  min + (rand() % static_cast<int>(max - min + 1));
    }
}

bool rectCord(cords& lhs, cords& rhs)
{
    return(lhs.y<rhs.y);
}

int worldSpace(int x, int y, bool getX)
{
    int wX = mapOffsetX*64 + ((x - y) * (64/2));
    int wY = mapOffsetY*32 + ((x + y) * (32/2));
    if(getX)
    {
        return wX;
    }
    else
    {
        return wY;
    }
}

int miniMapSpace(int x, int y, bool getX)
{
    int wX = mapOffsetX*20 + (x - y) * (20/2);
    int wY = mapOffsetY*10 + (x + y) * (10/2);
    if(getX)
    {
        return wX;
    }
    else
    {
        return wY;
    }
}

std::list<cords> getListOfCordsInCircle(int startX, int startY, int r)
{
    std::list<cords> tempList;
    for (int x = startX - r; x < startX + r; x++) {
        for (int y = startY - r; y < startY + r; y++) {
            if (x >= 0 && y >= 0 && x < MAP_WIDTH && y < MAP_HEIGHT) {
                if (((x - startX) * (x - startX) + (y - startY) * (y - startY)) <= r * r) {
                    tempList.push_back({ x, y });
                }
            }
        }
    }
    return tempList;
}