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
	ClearPathToGoal();
}

void AStar::CleanAll() {
	for (int i = 0; i < fullList.size(); i++) { delete fullList[i]; };
	fullList.clear();
}
void AStar::ClearOpenList() {
	openList.clear();
}
void AStar::ClearVisitedList() {
	visitList.clear();
}
void AStar::ClearPathToGoal() {
	pathToGoal.clear();
}

void AStar::FindPath(glm::vec3 sPos, glm::vec3 tPos) {
	if (!CheckInit) {
		ClearOpenList();
		ClearVisitedList();
		ClearPathToGoal();
		CleanAll();

		PathNode start;
		start.my_x = sPos.x;
		start.my_z = sPos.z;

		PathNode end;
		end.my_x = tPos.x;
		end.my_z = tPos.z;

		//std::cout << "Starting at x: " << start.my_x << "Starting at z: " << start.my_z << endl;
		//std::cout << "Ending at x: " << end.my_x << "Ending at z: " << end.my_z << endl;

		SetPosts(start, end);
		CheckInit = true;
	}else {
		ClearOpenList();
		ClearVisitedList();

		PathNode start;
		start.my_x = sPos.x;
		start.my_z = sPos.z;

		PathNode end;
		end.my_x = tPos.x;
		end.my_z = tPos.z;

		//std::cout << "Starting at x: " << start.my_x << "Starting at z: " << start.my_z << endl;
		//std::cout << "Ending at x: " << end.my_x << "Ending at z: " << end.my_z << endl;

		SetPosts(start, end);
		CheckInit = true;
	}
	if (CheckInit) {
		ContinuePath();
	}
}

void AStar::ReachedPoint(glm::vec3 cPos) {
	if (pathToGoal.size() && samef(cPos.x - 4.0f, pathToGoal[0]->my_x, (float)pow(my_stepsize, 1.1))
			&& samef(cPos.z - 4.0f, pathToGoal[0]->my_z, (float)pow(my_stepsize, 1.1))) {
		pathToGoal.erase(pathToGoal.begin());
		std::cout << "Reached grid: " << static_cast<int>((cPos.x + 384.0f) / 12.0f) << ", " << static_cast<int>((cPos.z + 384.0f) / 12.0f) << std::endl;
	}
	return;
}

void AStar::SetPosts(PathNode start, PathNode end) {
	startNode = new PathNode(start.my_x, start.my_z, NULL);
	endNode = new PathNode(end.my_x, end.my_z, &end);
	fullList.push_back(startNode);
	fullList.push_back(endNode);

	startNode->costReach = 0;
	startNode->distTarget = start.GetDist(endNode);
	startNode->parent = 0;

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

	int col = static_cast<int>((x + 384.0f) / 12.0f);
	int row = static_cast<int>((z + 374.0f) / 12.0f);

	if (col < 0) col = 0;
	else if (col > 63) col = 63;
	if (row < 0) row = 0;
	else if (row > 63) row = 63;

        //This lowers the priority on non-roads and wrong direction roads
    if ((int)graphNodes[row * 64 + col] == 9) { newCost += 40000* my_stepsize;}
    else if ((int)graphNodes[row * 64 + col] == 0 && parent->my_z > z) { newCost += 400* my_stepsize; }
    else if ((int)graphNodes[row * 64 + col] == 1 && parent->my_x > x) { newCost += 400* my_stepsize; }
    else if ((int)graphNodes[row * 64 + col] == 2 && parent->my_x < x) { newCost += 400* my_stepsize; }
    else if ((int)graphNodes[row * 64 + col] == 3 && parent->my_z < z) { newCost += 400* my_stepsize; }
	else if ((int)graphNodes[row * 64 + col] > 3  && (int)graphNodes[row * 64 + col] < 8) { newCost += 2000 * my_stepsize; }
	else if ((int)graphNodes[row * 64 + col] == 8) { newCost += 2000000 * my_stepsize; }
	else { newCost += 0;}

	int id = static_cast<int>((x + 384)/my_stepsize)* static_cast <int>(768/my_stepsize) + static_cast<int>((z + 374)/my_stepsize);
	for (auto node : visitList) {
		if (id == node->my_id) { return; }
	}

	PathNode* newChild = new PathNode(x, z, parent);
	newChild->costReach = newCost;
	newChild->distTarget = newChild->GetDist(endNode);

	openList.push_back(newChild);
	fullList.push_back(newChild);
	return;
}

void AStar::ContinuePath() {
    while (!openList.empty()) {
        PathNode* current = GetNext();

        if (current->GetDist(endNode) < (my_stepsize* my_stepsize)+1) {
            endNode->parent = current->parent;
            for (PathNode* getPath = endNode; getPath != NULL; getPath = getPath->parent)
                pathToGoal.push_back(getPath);
            CheckFound = true;
            return;
        }
		bool found = false;
		PathNode* transfer = NULL;
		for (auto i = 0; i < pathToGoal.size(); i++) {
			if (found) {
				//delete pathToGoal[i];
				pathToGoal.erase(pathToGoal.begin() + i);
				i--;
			}
			if (current->GetDist(pathToGoal[i]) < (my_stepsize * my_stepsize) + 1) {
				found = true;
				transfer = pathToGoal[i];
			}
		}
		if(found){
			transfer->parent = current->parent;
			for (PathNode* getPath = transfer; getPath != NULL; getPath = getPath->parent)
				pathToGoal.push_back(getPath);
			CheckFound = true;
			return;
		}

		PathOpened(current->my_x - my_stepsize, current->my_z, current->costReach + (float)pow(my_stepsize, 2), current);
		PathOpened(current->my_x + my_stepsize, current->my_z, current->costReach + (float)pow(my_stepsize, 2), current);
		PathOpened(current->my_x, current->my_z - my_stepsize, current->costReach + (float)pow(my_stepsize, 2), current);
		PathOpened(current->my_x, current->my_z + my_stepsize, current->costReach + (float)pow(my_stepsize, 2), current);
		PathOpened(current->my_x - (0.70711 * my_stepsize), current->my_z - (0.70711 * my_stepsize),
			current->costReach + (float)pow(my_stepsize, 2), current);
		PathOpened(current->my_x + (0.70711 * my_stepsize), current->my_z - (0.70711 * my_stepsize),
			current->costReach + (float)pow(my_stepsize, 2), current);
		PathOpened(current->my_x - (0.70711 * my_stepsize), current->my_z + (0.70711 * my_stepsize),
			current->costReach + (float)pow(my_stepsize, 2), current);
		PathOpened(current->my_x + (0.70711 * my_stepsize), current->my_z + (0.70711 * my_stepsize),
			current->costReach + (float)pow(my_stepsize, 2), current);

		for (int i = 0; i < openList.size(); i++) {
			if (openList[i]->my_id == current->my_id) {
				openList.erase(openList.begin() + i);
				i--;
			}
		}

	}
}

void AStar::CleanPath() {
    if (CheckFound)
        for (auto i = 2; i < pathToGoal.size(); i++) { 
            if (samef(pathToGoal[i-2]->my_x, pathToGoal[i]->my_x) || samef(pathToGoal[i-2]->my_z, pathToGoal[i]->my_z)) {
                i--;
				//delete pathToGoal[i];
                pathToGoal.erase(pathToGoal.begin() + i);
            }
        }
    return;
}

void AStar::PrintPath() {
    if (CheckFound)
        for (auto node : pathToGoal)
            std::cout << static_cast<int>((node->my_x + 384.0f) / 12.0f) << ", " << static_cast<int>((node->my_z + 374.0f)/12.0f) << std::endl;
    else
        cout << "No Path Found\n";
}
