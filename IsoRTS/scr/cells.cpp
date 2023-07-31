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

#include "Actors/actorStructs.h" 
bool canReachTarget(const cords& source, const cords& target, bool _cantPassActors) {
    std::vector<Cells> cellsList = baseCellList;
    int startCell = (source.x * MAP_HEIGHT) + source.y; // Source position
    int endCell = (target.x * MAP_HEIGHT) + target.y; // Target position
    updateCells(endCell, startCell, cellsList, _cantPassActors);
    std::vector<Cells*> listToCheck;
    std::vector<Cells*> checkedList;
    bool pathFound = false;
    if (((source.x - target.x == 0) || (source.x - target.x == -1) || (source.x - target.x == 1)) && ((source.y - target.y == 0) || (source.y - target.y == -1) || (source.y - target.y == 1)))
    {
        return true;
    }
    cellsList[startCell].addNeighbours(cellsList);
    cellsList[startCell].totalCostGuess = dist(cellsList[startCell].position.x, cellsList[startCell].position.y, cellsList[endCell].position.x, cellsList[endCell].position.y);
    cellsList[startCell].visited = true;
    cellsList[startCell].parentCellId = -1;
    cellsList[startCell].costToGoal = dist(cellsList[startCell].position.x, cellsList[startCell].position.y, cellsList[endCell].position.x, cellsList[endCell].position.y);
    listToCheck.push_back(&cellsList[startCell]);
    while (!listToCheck.empty() && startCell != endCell)
    {
        int cellId = 0;
        double lowestCostGuess = (*listToCheck.front()).totalCostGuess;
        int iterator = 0;
        for (Cells* cell : listToCheck) {
            if (cell->totalCostGuess < lowestCostGuess) {
                lowestCostGuess = cell->totalCostGuess;
                cellId = iterator;
            }
            iterator++;
        }
        if ((*listToCheck[cellId]).cellId == endCell)
        {
            return true;
        }
        for (std::vector<int>::const_iterator iterator = (*listToCheck[cellId]).neighbours.begin(), end = (*listToCheck[cellId]).neighbours.end(); iterator != end; ++iterator)
        {
            if (!cellsList[*iterator].visited)
            {
                cellsList[*iterator].addNeighbours(cellsList);
                cellsList[*iterator].parentCellId = (*listToCheck[cellId]).cellId;
                cellsList[*iterator].cummulativeCost = (*listToCheck[cellId]).cummulativeCost + 1;
                listToCheck.push_back(&cellsList[*iterator]);
                cellsList[*iterator].costToGoal = dist(cellsList[*iterator].position.x, cellsList[*iterator].position.y, cellsList[endCell].position.x, cellsList[endCell].position.y);
                cellsList[*iterator].totalCostGuess = cellsList[*iterator].costToGoal + cellsList[*iterator].cummulativeCost;
                cellsList[*iterator].visited = true;
            }
            else
            {
                if ((*listToCheck[cellId]).cummulativeCost + 1 < cellsList[*iterator].cummulativeCost)
                {
                    cellsList[*iterator].parentCellId = (*listToCheck[cellId]).cellId;
                    cellsList[*iterator].cummulativeCost = (*listToCheck[cellId]).cummulativeCost + 1;
                    cellsList[*iterator].totalCostGuess = cellsList[*iterator].costToGoal + cellsList[*iterator].cummulativeCost;
                }
            }
        }
        checkedList.push_back(&cellsList[(*listToCheck[cellId]).cellId]);
        listToCheck.erase(std::next(listToCheck.begin(), cellId));
    }
    return false;
}