#ifndef ASTAR_H
#define ASTAR_H

#include "pathNode.h"
#include "..\..\Include\glminclude\glm\vec3.hpp"
#include <iostream>
#include <algorithm>
#include <vector>

enum nodeType {
    roadN, roadW, roadE, roadS, roadI,
    walkN, walkW, walkE, walkS,
    badPath
};

class AStar {
public:
    AStar(void);
    ~AStar(void);

    void FindPath(Vector3 currentPos, Vector3 targetPos);
    void PrintPath();
    void ClearOpenList() { openList.clear(); };
    void ClearVisitedList() { visitList.clear(); };
    void ClearPathToGoal() { pathToGoal.clear(); };
    bool CheckInit;
    bool CheckFound;
    nodeType** graphNodes;
    std::vector<PathNode*> pathToGoal;

private:
    void SetPosts(PathNode start, PathNode End);
    void PathOpened(float x, float z, float newCost, PathNode* parent);
    Point* GetNext();
    void ContinuePath();

    pathNode* startNode;
    pathNode* endNode;
    std::vector<PathNode*> openList;
    std::vector<PathNode*> visitList;
};

#endif //ASTAR_H
