#include "globalfunctions.h"
#include "gamestate.h"

bool sortCord(rectangleCord& lhs, rectangleCord& rhs)
{
    return (lhs.x < rhs.x) || (lhs.y < rhs.y);
}

bool compareCord(const rectangleCord& lhs, const rectangleCord& rhs)
{
    return ((lhs.x == rhs.x) && (lhs.y == rhs.y));
}

bool sortCordByX(const rectangleCord& lhs, const rectangleCord& rhs)
{
    if (lhs.x != rhs.x) {
        return lhs.x < rhs.x;
    }
    else {
        return lhs.y < rhs.y;
    }
}

bool rectCord(rectangleCord& lhs, rectangleCord& rhs)
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

std::list<mouseWorldCord> getListOfCordsInCircle(int startX, int startY, int r)
{
    std::list<mouseWorldCord> tempList;
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