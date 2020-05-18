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


