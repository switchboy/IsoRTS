#include "cells.h"
#include "globalfunctions.h"
#include "gamestate.h"

std::vector<Cells> baseCellList;

void updateCells(int goalId, int startId, std::vector<Cells>& cellsList, bool cantPassActors)
{
    int n = 0;
    for (int i = 0; i < MAP_WIDTH; i++)
    {
        for (int j = 0; j < MAP_HEIGHT; j++)
        {
            bool obstacle = false;
            if (cantPassActors) {
                if (!currentGame.isPassable({ i, j })) {
                    cellsList[n].obstacle = true;
                }
            }
            else if (!currentGame.isPassableButMightHaveActor({ i, j }))
            {
                cellsList[n].obstacle = true;
            }
            n++;
        }
    }
    if (goalId >= 0) {
        cellsList[goalId].obstacle = false;
        cellsList[startId].obstacle = false;
    }
}

Cells::Cells(cords cellPosition, int cellId)
{
    this->position = cellPosition;
    this->cellId = cellId;
    this->obstacle = false;
    this->costToGoal = NULL;
    this->visited = false;
    this->parentCellId = NULL;
    this->cummulativeCost = NULL;
    this->totalCostGuess = NULL;
    this->neighbours.reserve(8);
    backParent = 0;
}

Cells::Cells(cords cellPosition, int cellId, bool obstacle)
{
    this->position = cellPosition;
    this->cellId = cellId;
    this->obstacle = obstacle;
    this->costToGoal = NULL;
    this->visited = false;
    this->parentCellId = NULL;
    this->cummulativeCost = NULL;
    this->totalCostGuess = NULL;
    this->neighbours.reserve(8);
    backParent = 0;
}

void Cells::addNeighbours(const std::vector<Cells>& cellsList)
{
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            int tempId = this->cellId + (i * MAP_HEIGHT) + j;
            if (tempId >= 0 && tempId < cellsList.size()) {
                if (tempId != this->cellId) {
                    if (!cellsList[tempId].obstacle) {
                        this->neighbours.push_back(tempId);
                    }
                }
            }
        }
    }
}
