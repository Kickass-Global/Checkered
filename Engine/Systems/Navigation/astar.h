#ifndef ASTAR_H
#define ASTAR_H

#include "pathNode.h"
#include "navigationGrid.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <glm\ext\vector_float3.hpp>

enum nodeType {
    roadN, roadW, roadE, roadS, roadI,
    walkN, walkW, walkE, walkS,
    badPath
};

class AStar {
public:
    AStar(void);
    ~AStar(void);

    void FindPath(glm::vec3 currentPos, glm::vec3 targetPos);
    void ReachedPoint(glm::vec3 cPos);
    void CleanPath();
    void PrintPath();
    void CleanAll();
    void ClearOpenList();
    void ClearVisitedList();
    void ClearPathToGoal();
    bool CheckInit;
    bool CheckFound;
    int* graphNodes;
    std::vector<PathNode*> pathToGoal;
    PathNode* startNode;
    PathNode* endNode;


private:
    void SetPosts(PathNode start, PathNode End);
    void PathOpened(float x, float z, float newCost, PathNode* parent);
    PathNode* GetNext();
    void ContinuePath();
    int my_stepsize = 6;
    std::vector<PathNode*> openList;
    std::vector<PathNode*> visitList;
    std::vector<PathNode*> fullList;
};

#endif //ASTAR_H
