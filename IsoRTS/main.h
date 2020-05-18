#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED
#include "globalfunctions.h"

int mainWindowWidth = 1920;
int mainWindowHeigth = 1080;

sf::RenderWindow window(sf::VideoMode(mainWindowWidth, mainWindowHeigth), "Isometric Demo");
sf::View totalView(sf::FloatRect(0.f, 0.f, mainWindowWidth, mainWindowHeigth));
sf::View worldView(sf::FloatRect(0.f, 0.03f, mainWindowWidth, mainWindowHeigth*0.77f));
sf::View topBar(sf::FloatRect(0.f, 0.f, mainWindowWidth, mainWindowHeigth*0.03f));
sf::View toolBar(sf::FloatRect(0.f, 0.f, mainWindowWidth, mainWindowHeigth*0.2f));
sf::View miniMap(sf::FloatRect(0.f, 0.f, mainWindowWidth*0.2f, mainWindowHeigth*0.2f));

int viewOffsetX = (MAP_WIDTH*64)/2;
int viewOffsetY = (MAP_HEIGHT*32)/2;

int mapOffsetX = (MAP_WIDTH/2);
int mapOffsetY = 0;

sf::RenderTexture minimapTexture;
bool minimapTextureExist = false;







#endif // MAIN_H_INCLUDED
