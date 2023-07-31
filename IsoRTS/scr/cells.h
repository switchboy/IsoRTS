#pragma once
#include <vector>
#include "globalfunctions.h"


struct Cells
{
public:
    Cells() {
        position = { 0,0 };
        parentCellId = 0;
        cummulativeCost = 0;
        cellId = 0;
        backParent = 0;
        costToGoal = 0;
        totalCostGuess = 0;
    }
    Cells(cords cellPosition, int cellId);
    Cells(cords cellPosition, int cellId, bool obstacle);
    cords position;
    int parentCellId, cummulativeCost, cellId, backParent;
    double costToGoal, totalCostGuess;
    bool visited = false;
    bool visitedBack = false;
    bool obstacle = false;
    std::vector<int> neighbours;
    void addNeighbours(const std::vector<Cells>& cellsList);
};

extern std::vector<Cells> baseCellList;
extern void updateCells(int goalId, int startId, std::vector<Cells>& cellsList, bool cantPassActors);
bool canReachTarget(const cords& source, const cords& target, bool _cantPassActors);
