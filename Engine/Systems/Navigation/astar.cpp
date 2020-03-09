#include "astar.h"
#include "Windows.h"
#include <string>
#define ASTAR_STEPSIZE 3
using namespace std;

bool sortingPathNodes(PathNode* p1, PathNode* p2) { return p1->GetF() < p2->GetF(); }
bool samef(float A, float B, float epsilon = 0.005f) { return (fabs(A - B) < epsilon); }

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

void AStar::FindPath(glm::vec3 sPos, glm::vec3 tPos) {
	if (!CheckInit) {
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

		std::cout << "Starting at x: " << start.my_x << "Starting at z: " << start.my_z << endl;
		std::cout << "Ending at x: " << end.my_x << "Ending at z: " << end.my_z << endl;

		SetPosts(start, end);
		CheckInit = true;
	}
	if (CheckInit) {
		ContinuePath();
	}
}

void AStar::ReachedPoint(glm::vec3 cPos) {
	if (pathToGoal.size() && samef(cPos.x - 1.5f, pathToGoal[0]->my_x, (float)pow(ASTAR_STEPSIZE, 2))
			&& samef(cPos.z - 1.5f, pathToGoal[0]->my_z, (float)pow(ASTAR_STEPSIZE, 2))) {
		std::cout << "reached point " << pathToGoal[0]->my_x << ", " << pathToGoal[0]->my_z << std::endl;
		pathToGoal.erase(pathToGoal.begin());
	}
	return;
}

void AStar::SetPosts(PathNode start, PathNode end) {
	startNode = new PathNode(start.my_x, start.my_z, NULL);
	endNode = new PathNode(end.my_x, end.my_z, &end);

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
		//std::cout << "next node to explore is at: " << nextNode->my_x << ", " << nextNode->my_z 
		//<< "  with cost: " << nextNode->GetF() << std::endl;
	}
	return nextNode;
}

void AStar::PathOpened(float x, float z, float newCost, PathNode* parent) {

	int col = static_cast<int>(x + 192) / 3;
	int row = static_cast<int>(z + 182) / 3;

	if (col < 0) col = 0;
	else if (col > 63) col = 63;
	if (row < 0) row = 0;
	else if (row > 63) row = 63;

        //TODO: lower priority on buildings, sidewalks, and wrong direction roads
    if ((int)graphNodes[col][row] == 9) { newCost += 200*ASTAR_STEPSIZE;}
    else if ((int)graphNodes[col][row] == 0 && parent->my_z > z) { newCost += 3*ASTAR_STEPSIZE; }
    else if ((int)graphNodes[col][row] == 1 && parent->my_x > x) { newCost += 3*ASTAR_STEPSIZE; }
    else if ((int)graphNodes[col][row] == 2 && parent->my_x < x) { newCost += 3*ASTAR_STEPSIZE; }
    else if ((int)graphNodes[col][row] == 3 && parent->my_z < z) { newCost += 3*ASTAR_STEPSIZE; }

	int id = (static_cast<int>(x + 192) / 3) * 64 + (static_cast<int>(z + 182) / 3);
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
        if (current->GetDist(endNode) < (ASTAR_STEPSIZE*ASTAR_STEPSIZE)+1) {
            endNode->parent = current->parent;
            for (PathNode* getPath = endNode; getPath != NULL; getPath = getPath->parent)
                pathToGoal.push_back(getPath);
            CheckFound = true;
            return;
        }
        else {
                PathOpened(current->my_x - ASTAR_STEPSIZE, current->my_z, current->costReach + (float)pow(ASTAR_STEPSIZE, 2), current);
                PathOpened(current->my_x + ASTAR_STEPSIZE, current->my_z, current->costReach + (float)pow(ASTAR_STEPSIZE, 2), current);
                PathOpened(current->my_x, current->my_z - ASTAR_STEPSIZE, current->costReach + (float)pow(ASTAR_STEPSIZE, 2), current);
                PathOpened(current->my_x, current->my_z + ASTAR_STEPSIZE, current->costReach + (float)pow(ASTAR_STEPSIZE, 2), current);
                PathOpened(current->my_x - (0.70711 * ASTAR_STEPSIZE), current->my_z - (0.70711* ASTAR_STEPSIZE),
                    current->costReach + (float)pow(ASTAR_STEPSIZE, 2), current);
                PathOpened(current->my_x + (0.70711 * ASTAR_STEPSIZE), current->my_z - (0.70711* ASTAR_STEPSIZE),
                    current->costReach + (float)pow(ASTAR_STEPSIZE, 2), current);
                PathOpened(current->my_x - (0.70711 * ASTAR_STEPSIZE), current->my_z + (0.70711* ASTAR_STEPSIZE),
                    current->costReach + (float)pow(ASTAR_STEPSIZE, 2), current);
                PathOpened(current->my_x + (0.70711 * ASTAR_STEPSIZE), current->my_z + (0.70711* ASTAR_STEPSIZE),
                    current->costReach + (float)pow(ASTAR_STEPSIZE, 2), current);

			for (int i = 0; i < openList.size(); i++) {
				if (openList[i]->my_id == current->my_id) {
					openList.erase(openList.begin() + i);
					i--;
				}
			}
		}
	}
}

void AStar::CleanPath() {
    if (CheckFound)
        for (auto i = 2; i < pathToGoal.size(); i++) { 
            if (samef(pathToGoal[i-2]->my_x, pathToGoal[i]->my_x) || samef(pathToGoal[i-2]->my_z, pathToGoal[i]->my_z)) {
                i--;
                pathToGoal.erase(pathToGoal.begin() + i);
            }
        }
    return;
}

void AStar::PrintPath() {
    if (CheckFound)
        for (auto node : pathToGoal)
            cout << node->my_x << ", " << node->my_z << std::endl;
    else
        cout << "No Path Found\n";
}
