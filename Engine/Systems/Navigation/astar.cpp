//
// Created by Kett on 2020-02-26.
//

#include "astar.h"
#define ASTAR_STEPSIZE 2
using namespace std;

bool sortingPathNodes(PathNode* p1, PathNode* p2) { return p1->GetF() < p2->GetF(); }

AStar::AStar() {
    CheckFound = false;
    CheckInit = false;
}
AStar::~AStar() {
    for (int i = 0; i < openList.size(); i++)
        delete openList[i];
    openList.clear();
    for (int i = 0; i < visitList.size(); i++)
        delete visitList[i];
    visitList.clear();
    for (int i = 0; i < pathToGoal.size(); i++)
        delete pathToGoal[i];
    pathToGoal.clear();
}

void AStar::FindPath(Vector3 sPos, Vector3 tPos) {
    if (CheckInit) {
        for (int i = 0; i < openList.size(); i++)
            delete openList[i];
        openList.clear();
        for (int i = 0; i < visitList.size(); i++)
            delete visitList[i];
        visitList.clear();
        for (int i = 0; i < pathToGoal.size(); i++)
            delete pathToGoal[i];
        pathToGoal.clear();

        PathNode start;
        start.my_x = sPos.x;
        start.my_z = sPos.z;

        PathNode end;
        end.my_x = tPos.x;
        end.my_z = tPos.z;

        SetPosts(start, end);
        CheckInit = true;
    }
    if (CheckInit) {
        ContinuePath();
    }
}

void AStar::SetPosts(PathNode start, PathNode end) {
    startNode = new PathNode(start.x, start.z, NULL);
    endNode = new PathNode(end.x, end.z, &end);

    startNode->costReach = 0;
    startNode->distTarget = start.GetDist(endNode);
    startNode->parent = 0;
    startNode->search_id = 0;            //TODO: give searches ids


    openList.push_back(startNode);
}

PathNode* AStar::GetNext() {
    PathNode* nextNode = NULL;

    sort(openList.begin(), openList.end(), sortingPathNodes);
    if (!openList.empty()) {
        nextNode = openList[0];
        visitList.push_back(nextNode);
        openList.erase(openList.begin());
    }
    return nextNode;
}

void AStar::PathOpened(float x, float z, float newCost, PathNode* parent) {

           //TODO: lower priority on buildings, sidewalks, and wrong direction roads
    /*if ((int)graphNodes[x][y] == 9) { return; }
    else if ((int)graphNodes[x][y] == 0 && parent->z < y) { newCost += 2; }
    else if ((int)graphNodes[x][y] == 1 && parent->x < x) { newCost += 2; }
    else if ((int)graphNodes[x][y] == 2 && parent->x > x) { newCost += 2; }
    else if ((int)graphNodes[x][y] == 3 && parent->z > y) { newCost += 2; }*/

    int id = static_cast<int>(x) * 64 + static_cast<int>(z); //TODO: map size not 64
    for (auto node : visitList) {
        if (id == node->my_id) { return; }
    }

    PathNode* newChild = new PathNode(x, z, parent);
    newChild->costReach = newCost;
    newChild->distTarget = newChild->GetDist(endNode);

    openList.push_back(newChild);
    return;
}

void AStar::ContinuePath() {
    while (!openList.empty()) {
        PathNode* current = GetNext();

        if (current->getDist < (ASTAR_STEPSIZE*ASTAR_STEPSIZE)+1) {
            endNode->parent = current->parent;
            for (PathNode* getPath = endNode; getPath != NULL; getPath = getPath->parent)
                pathToGoal.push_back(getPath);
            CheckFound = true;
            return;
        }
        else {
                PathOpened(current->x - ASTAR_STEPSIZE, current->z, current->costReach + ASTAR_STEPSIZE, current);
                PathOpened(current->x + ASTAR_STEPSIZE, current->z, current->costReach + ASTAR_STEPSIZE, current);
                PathOpened(current->x, current->z - ASTAR_STEPSIZE, current->costReach + ASTAR_STEPSIZE, current);
                PathOpened(current->x, current->z + ASTAR_STEPSIZE, current->costReach + ASTAR_STEPSIZE, current);
                PathOpened(current->x - (0.70711 * ASTAR_STEPSIZE), current->z - (0.70711* ASTAR_STEPSIZE), 
                    current->costReach + ASTAR_STEPSIZE, current);
                PathOpened(current->x + (0.70711 * ASTAR_STEPSIZE), current->z - (0.70711* ASTAR_STEPSIZE), 
                    current->costReach + ASTAR_STEPSIZE, current);
                PathOpened(current->x - (0.70711 * ASTAR_STEPSIZE), current->z + (0.70711* ASTAR_STEPSIZE), 
                    current->costReach + ASTAR_STEPSIZE, current);
                PathOpened(current->x + (0.70711 * ASTAR_STEPSIZE), current->z + (0.70711* ASTAR_STEPSIZE), 
                    current->costReach + ASTAR_STEPSIZE, current);

            for (int i = 0; i < openList.size(); i++) {
                if (openList[i]->my_id == current->my_id) {
                    openList.erase(openList.begin() + i);
                    i--;
                }
            }
        }
    }
}

void AStar::PrintPath() {
    if (CheckFound)
        for (auto loc : pathToGoal) { cout << (int)loc->my_x << ", " << (int)loc->my_z << endl; }
    else
        cout << "Not Found";
    return;
}