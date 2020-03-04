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
    PathNode* GetNext();
    void ContinuePath();

    PathNode* startNode;
    PathNode* endNode;
    std::vector<PathNode*> openList;
    std::vector<PathNode*> visitList;
};

#endif //ASTAR_H
