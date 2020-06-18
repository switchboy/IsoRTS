#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include "main.h"
#include "actors.h"
#include "buildings.h"
#include "objects.h"
#include <future>
#include <mutex>
#include "gametext.h"
#include "projectile.h"
#include "orderCursor.h"

gameState currentGame;

void updateActorHelper(int i)
{
    if(listOfActors[i].isInitialized())
    {
        listOfActors[i].update();
    }
}


void updateBuildingsHelper(int i)
{
    listOfBuildings[i].update();
}

void routeHelper(int i)
{
    listOfActorsMutex.lock();
    if(listOfActors[i].isInitialized())
    {
        listOfActors[i].calculateRoute();
    }
    listOfActorsMutex.unlock();
}

void updateActorsWorker()
{
    while (window.isOpen())
    {
        if (!listOfActors.empty())
        {
            for (int i = 0; i < listOfActors.size(); i++)
            {
                std::async(std::launch::async, updateActorHelper, i);
            }
        }
    }
}
void updateBuildingsWorker()
{
    while (window.isOpen()) {
        if (!listOfBuildings.empty())
        {
            for (int i = 0; i < listOfBuildings.size(); i++)
            {
                std::async(std::launch::async, updateBuildingsHelper, i);
            }
        }
    }
}

void calculateRoutesWorker()
{
    while(window.isOpen())
    {
        if(!listOfActorsWhoNeedAPath.empty())
        {
            for (std::list<int>::const_iterator iterator = listOfActorsWhoNeedAPath.begin(), end = listOfActorsWhoNeedAPath.end(); iterator != end; ++iterator)
            {
                std::async(std::launch::async, routeHelper,*iterator);
            }
        }
    }
}

void projectileHelper(int i) {
    listOfProjectiles[i].updatePosition();
}

void updateProjectiles()
{
    while (window.isOpen())
    {
        if (!listOfProjectiles.empty()) {
            //update porjectile positions
            for (int i = 0; i < listOfProjectiles.size(); i++) {
                std::async(std::launch::async, projectileHelper, i);
            }

            //Erase old projectiles (older then 30 sec)
            auto iter = std::find_if(listOfProjectiles.begin(), listOfProjectiles.end(),
                [&](projectile& p) {return p.getTimeLastUpdate() + 30.0f < currentGame.getTime(); });
            if (iter != listOfProjectiles.end())
                listOfProjectiles.erase(iter);
        }
    }
}

void clearOldCommandCursors()
{
    if (!listOfOrderCursors.empty()) {
        auto iter = std::find_if(listOfOrderCursors.begin(), listOfOrderCursors.end(),
            [&](orderCursor& p) {return p.isFinished(); });
        if (iter != listOfOrderCursors.end())
            listOfOrderCursors.erase(iter);
    }
}

int main()
{
    sf::Clock clockMain;
    currentGame.loadGame();
    std::thread updateRoutesThread(calculateRoutesWorker);
    std::thread updateActorsThread(updateActorsWorker);
    std::thread updateArrowsThread(updateProjectiles);
    std::thread updateBuildingsThread(updateBuildingsWorker);
    while(window.isOpen())
    {
        sf::Time elapsedMain = clockMain.getElapsedTime();
        currentGame.elapsedTime = elapsedMain.asSeconds();
        gameText.throwOutOldMessages();
        clearOldCommandCursors();
        currentGame.interact();
        currentGame.drawGame();
    }
    updateActorsThread.join();
    updateRoutesThread.join();
    updateArrowsThread.join();
    updateBuildingsThread.join();
    return 0;
}
