#ifndef PATHNODE_H
#define PATHNODE_H

#include <math.h>
#define SIZE 64

struct PathNode {
public:
    int my_id, search_id;
    PathNode* parent;
    bool open;
    float my_x, my_z, costReach, distTarget;

    PathNode() : parent(0) {}
    PathNode(float x, float z, PathNode* _parent = 0) : my_x(x), my_z(z), parent(_parent),
        my_id(static_cast<int>(x) * SIZE + static_cast<int>(y)), search_id(_parent->search_id) {};

    float GetF() { return costReach*costReach + distTarget; }
    float GetDist(Point* nodeEnd)
    {
        float x = (float)(pow(this->my_x - nodeEnd->my_x, 2));
        float z = (float)(pow(this->my_z - nodeEnd->my_z, 2));

        return x + z;
    }
};

#endif //PATHNODE_H