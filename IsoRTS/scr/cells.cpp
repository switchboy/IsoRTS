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
    const int row = cellId / MAP_HEIGHT;
    const int col = cellId % MAP_HEIGHT;
    neighbours.reserve(8);
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;
            const int newRow = row + i;
            const int newCol = col + j;
            if (newRow >= 0 && newRow < MAP_HEIGHT && newCol >= 0 && newCol < MAP_HEIGHT) {
                const int tempId = newRow * MAP_HEIGHT + newCol;
                if (!cellsList[tempId].obstacle) {
                    neighbours.push_back(tempId);
                }
            }
        }
    }
}
