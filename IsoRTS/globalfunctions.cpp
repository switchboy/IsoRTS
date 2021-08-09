#include "globalfunctions.h"
#include "gamestate.h"

bool sortCord(const cords& lhs, const cords& rhs)
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
    return  min + (rand() % static_cast<int>(max - min + 1));
}

bool rectCord(const cords& lhs, const cords& rhs)
{
    return (lhs.y<rhs.y);
}

cords worldSpace(cords location)
{
    int wX = mapOffsetX*64 + ((location.x - location.y) * (64/2));
    int wY = mapOffsetY*32 + ((location.x + location.y) * (32/2));
    return { wX, wY };
}

cords miniMapSpace(cords location)
{
    int wX = mapOffsetX*20 + (location.x - location.y) * (20/2);
    int wY = mapOffsetY*10 + (location.x + location.y) * (10/2);
    return { wX, wY };
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

double dist(double x1, double y1, double x2, double y2)
{
    //Manhattan distance
    return fabs(x2 - x1) + fabs(y2 - y1);
}

double distEuclidean(double x1, double y1, double x2, double y2)
{
    //Euclidean distance
    return sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2));
}

cords toWorldMousePosition(int mouseX, int mouseY)
{
    if (!(mouseX < 0 || mouseY < 0))
    {
        auto cheatTile = currentGame.textureCheatTile.copyToImage();
        int cellX = mouseX / 64;
        int cellY = mouseY / 32;
        int offSetX = mouseX % 64;
        int offSetY = mouseY % 32;
        int worldX = (cellY - mapOffsetY) + (cellX - mapOffsetX);
        int worldY = (cellY - mapOffsetY) - (cellX - mapOffsetX);
        auto color = cheatTile.getPixel(offSetX, offSetY);
        if (color == sf::Color::Red)
        {
            worldX += -1;
        }
        else if (color == sf::Color::Green)
        {
            worldY += +1;
        }
        else if (color == sf::Color::Blue)
        {
            worldY += -1;
        }
        else if (color == sf::Color::Yellow)
        {
            worldX += +1;
        }
        return { worldX, worldY };
    }
    else
    {
        return { 0,0 };
    }
}
