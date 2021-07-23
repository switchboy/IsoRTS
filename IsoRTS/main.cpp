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
#include "player.h"
#include "simpleAI.h"

gameState currentGame;

void routeHelper(int i)
{
    if (listOfActors[i].isInitialized())
    {
        listOfActors[i].calculateRoute();
    }
}

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

void projectileHelper(int i) {
    listOfProjectiles[i].updatePosition();
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

void updateGameState(int& lastActor, int& lastBuilding, int& lastPath, int& lastProjectile){
    for (int i = 0; i < currentGame.getPlayerCount(); i++) {
        listOfPlayers[i].clearIdleVillagerList();
    }
    for (int n = 0; n < listOfActors.size(); n++) {
        if (listOfActors[n].idle() && listOfActors[n].getType() == 0 && listOfActors[n].isAlive()) {
            listOfPlayers[listOfActors[n].getTeam()].insertIdIntoIdleVillagerList(n);
        }
    }    
    if (!listOfAI.empty()) {
        for (int i = 0; i < listOfAI.size(); i++) {
            listOfAI[i].update();
        }
    }
    if (!listOfProjectiles.empty()) {
        int endProjectile = lastProjectile + 100;
        if (listOfProjectiles.size() > 100) {
            if (endProjectile > listOfProjectiles.size()) {
                endProjectile = listOfProjectiles.size();
            }
        }
        //update porjectile positions
        for (int i = lastProjectile; i < endProjectile; i++) {
            std::async(std::launch::async, projectileHelper, i);
        }
        //Erase old projectiles (older then 30 sec)
        auto iter = std::find_if(listOfProjectiles.begin(), listOfProjectiles.end(),
            [&](projectile& p) {return p.getTimeLastUpdate() + 30.0f < currentGame.getTime(); });
        if (iter != listOfProjectiles.end())
            listOfProjectiles.erase(iter);
        if (endProjectile = listOfProjectiles.size()) {
            lastProjectile = 0;
        }
        else {
            lastProjectile = endProjectile;
        }
    }
    if (!listOfActors.empty())
    {
        if (!listOfActorsWhoNeedAPath.empty())
        {
            int begin = listOfActorsWhoNeedAPath.size();
            int endPath = lastPath + 5;
            if (endPath > listOfActorsWhoNeedAPath.size()) {
                endPath = listOfActorsWhoNeedAPath.size();
            }
            for (int i = lastPath; i < endPath; i++){
                std::async(std::launch::async, routeHelper, listOfActorsWhoNeedAPath[i]);
            }
            int after = listOfActorsWhoNeedAPath.size();
            if (endPath == listOfActorsWhoNeedAPath.size()) {
                lastPath = 0;
                listOfActorsWhoNeedAPath.clear();
            }
            else {
               lastPath = endPath;
            }
        }

        int endActor = lastActor + 25;
        if (endActor > listOfActors.size()) {
            endActor = listOfActors.size();
        }
        for (int i = lastActor; i < endActor; i++)
        {
            std::async(std::launch::async, updateActorHelper, i);
        }
        if (endActor == listOfActors.size()) {
            lastActor = 0;
        }
        else {
            lastActor = endActor;
        }
    }
    if (!listOfBuildings.empty())
    {
        int endBuilding = lastBuilding + 10;
        if (endBuilding > listOfBuildings.size()) {
            endBuilding = listOfBuildings.size();
        }
        for (int i = lastBuilding; i < endBuilding; i++)
        {
            std::async(std::launch::async, updateBuildingsHelper, i);
        }
        if (endBuilding == listOfBuildings.size()) {
            lastBuilding = 0;
        }
        else {
            lastBuilding = endBuilding;
        }
    }
  
}

int main()
{
    sf::Clock clockMain;
    int lastActor =0;
    int lastBuilding=0;
    int lastPath=0;
    int lastProjectile=0;
    currentGame.loadGame();
    simpleAI newAIPlayer(0, 0);
    listOfAI.push_back(newAIPlayer);
    while(window.isOpen())
    {
        sf::Time elapsedMain = clockMain.getElapsedTime();
        currentGame.elapsedTime = elapsedMain.asSeconds();
        gameText.throwOutOldMessages();
        clearOldCommandCursors();
        currentGame.interact();
        updateGameState(lastActor, lastBuilding, lastPath, lastProjectile);
        currentGame.drawGame();
    }
    return 0;
}
