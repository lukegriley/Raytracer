#ifndef KDTREE_H
#define KDTREE_H

#include "sceneparser.h"
#include "aab.h"
#include "ray.h"
#include "intersections.h"

struct  Node {
    Node* left;
    Node* right;
    std::vector<RenderShapeData> shapes;
    AAB box;
};

class KDTree
{
public:
    KDTree();
    static Node* constructNode(Node* root, int axis);
    static float KDIntersection(Node* root, Ray ray, Intersections* closestI, RenderShapeData* closestShape);
    static void visualizeBinaryTree(Node* node, int depth = 0, char prefix = ' ');
private:
    static const int leaf_limit = 1;
    static float splitCost(int axis, float split, const std::vector<RenderShapeData>& sortedShapes);
    static float calculateSurfaceArea(const RenderShapeData& shape, PrimitiveType type);


};

#endif // KDTREE_H
