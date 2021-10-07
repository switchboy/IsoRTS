#ifndef GLOBALFUNCTIONS_H
#define GLOBALFUNCTIONS_H

#include <list>
#include <iostream>
#include <algorithm>
#include <vector>
#include <random>
#include"SFML/Graphics.hpp"
#include "humanReadableNames.h"
#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include "server.h"

const int MAP_WIDTH = 256;
const int MAP_HEIGHT = 256;
const int mainWindowWidth = 1920;
const int mainWindowHeigth = 1080;
const int halfOfMainWindowWidth = mainWindowWidth / 2;
const int halfOfMainWindowHeigth = mainWindowHeigth / 2;
const int visableWorldHeight = static_cast<int>(mainWindowHeigth * 0.77);
const int toolbarHeight = static_cast<int>(mainWindowHeigth * 0.03);
const int visableHalfOfMainWindowWidth = static_cast<int>((mainWindowWidth * 0.8) / 2);

extern int mapOffsetX;
extern int mapOffsetY;
extern int AIPlayers;

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

struct command {
    int timeCommandGiven;
    int playerId;
    int subjectId;
    bool placingBuilding;
    bool isStackedCommand;
    cords commandCords;
    worldObject subjectType;
    stackOrderTypes orderType;
    actionTypes actionToPerform;
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
void setViewports();

enum dataType {
	Text,
	UserList,
	MessageStatus,
	Ping,
	playerReady
};

const sf::Color networkTeamColors[] =
{
	{0, 0, 255},
	{0, 255, 0},
	{255, 0, 0},
	{255, 255, 0},
	{0, 255, 255},
	{255, 0, 255},
	{255, 127, 0},
	{127, 127, 127}
};

struct connectedPlayers {
	sf::TcpSocket* playerSocket;
	std::string name;
	sf::IpAddress remoteIp;
	sf::Int32 lastPing;
	sf::Int32 lastPingPacketSend;
	bool isReady;
};

struct playersClient {
	std::string name;
	sf::Int32 lastPing;
	bool isReady;
};

extern sf::Thread serverThread;
namespace networkStuff {
	extern int port;
}


extern sf::Image cheatTile;

#endif // GLOBALFUNCTIONS_H
