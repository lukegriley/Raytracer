#include "kdtree.h"
#include <algorithm>
#include <limits>
#include <cmath>
#include <iostream>

Node* KDTree::constructNode(Node* root, int axis) {
    if(root->shapes.size()<=leaf_limit) {
        root->left = nullptr;
        root->right = nullptr;
        return root;//return leaf node. idk what this entails yet
    }
    auto compareAxisTranslate = [axis](const RenderShapeData& a, const RenderShapeData& b) {
        return a.ctm[3][axis] < b.ctm[3][axis];
    };

    // Sorting the shapes vector based on the specified axis using the lambda function
    std::vector<RenderShapeData> sortedShapes = root->shapes;  // Copy the original vector
    std::sort(sortedShapes.begin(), sortedShapes.end(), compareAxisTranslate);

    float minCost = std::numeric_limits<float>::max();
    float minSplitPlane = 6.82f;//placeholder for debugging
    for(RenderShapeData& shape : root->shapes) {
        float minExtentCost = splitCost(axis,shape.aab[axis],sortedShapes);//bug
        float maxExtentCost = splitCost(axis,shape.aab[axis+3],sortedShapes);
        if(minExtentCost < minCost) {
            minCost = minExtentCost;
            minSplitPlane = shape.aab[axis];
        }
        if(maxExtentCost < minCost) {
            minCost = maxExtentCost;
            minSplitPlane = shape.aab[axis + 3];
        }
    }
    //split sorted into 2 vectors, one with shapes with sortedShapes[i].ctm[3][axis] <= minSplitPlane, and one with shapes greater than
    std::vector<RenderShapeData> leftSubtreeShapes, rightSubtreeShapes;
    auto splitPoint = std::lower_bound(sortedShapes.begin(),sortedShapes.end(),minSplitPlane,[axis](const RenderShapeData& a, float split) {
        return a.ctm[3][axis] < split;
    });

    leftSubtreeShapes.assign(sortedShapes.begin(),splitPoint);
    rightSubtreeShapes.assign(splitPoint,sortedShapes.end());


    //construct children nodes
    std::unique_ptr<Node> left(new Node);//to prevent danging pointers: deallocate memory for root children
    std::unique_ptr<Node> right(new Node);

    left->shapes = leftSubtreeShapes;
    right->shapes = rightSubtreeShapes;

    //reform axis aligned boxes based on split plane
    AAB leftBox(root->box), rightBox(root->box);
    leftBox[axis+3] = minSplitPlane;
    rightBox[axis] = minSplitPlane;

    left->box = leftBox;
    right->box = rightBox;

    root->left = constructNode(left.get(),axis<2 ? axis+1 : 0);//.get() to get pointer reference in unique memory
    root->right = constructNode(right.get(),axis<2 ? axis+1 : 0);//alternate to next axis

    return root;
}

float KDTree::splitCost(int axis, float split, const std::vector<RenderShapeData>& sortedShapes) {
    int costLeft = 0;
    int costRight = 0;
    float leftChildSurfaceArea = 0.f;
    float rightChildSurfaceArea = 0.f;
    for(int i=0;i<sortedShapes.size();i++) {
        if(sortedShapes[i].ctm[3][axis]<split) {

            leftChildSurfaceArea += calculateSurfaceArea(sortedShapes[i],sortedShapes[i].primitive.type);
            costLeft++;
        }else {
            rightChildSurfaceArea += calculateSurfaceArea(sortedShapes[i],sortedShapes[i].primitive.type);
            costRight++;
        }
    }
    float weightLeft = leftChildSurfaceArea/(leftChildSurfaceArea + rightChildSurfaceArea);
    float weightRight = rightChildSurfaceArea/(leftChildSurfaceArea + rightChildSurfaceArea);
    return weightLeft*costLeft + weightRight*costRight;
}

float KDTree::calculateSurfaceArea(const RenderShapeData& shape, PrimitiveType type) {
    float sx = shape.ctm[0][0];
    float sy = shape.ctm[1][1];
    float sz = shape.ctm[2][2];
    float a, b;
    switch(type) {
    case PrimitiveType::PRIMITIVE_CUBE:
        return sx*sy*2 + sx*sz*2 + sy*sz*2;
    case PrimitiveType::PRIMITIVE_CONE:
        a = std::max(sx,sz)/2.0;
        b = std::min(sx,sz)/2.0;
        return M_PI*a*b + M_PI * a * sqrt(b*b + sy * sy);
    case PrimitiveType::PRIMITIVE_CYLINDER:
        return M_PI*(sx/2+sz/2)*sy + 0.5*M_PI*sx*sz;
    case PrimitiveType::PRIMITIVE_SPHERE:
        return M_PI * sx * sx;
    default:
        throw std::runtime_error("Meshes not supported.");
    }
}

float KDTree::KDIntersection(Node* root, Ray ray, Intersections* closestI, RenderShapeData* closestShape) {
    if(root->left==nullptr && root->right==nullptr) {
        float t_min = std::numeric_limits<float>::max();
        for(const RenderShapeData& p : root->shapes) {
            //get Ray in object space
            glm::vec4 P_object = glm::inverse(p.ctm) * ray.p;
            glm::vec4 d_object_unnorm = glm::inverse(p.ctm) * ray.d;
            glm::vec4 d_object(glm::normalize(glm::vec3(d_object_unnorm)),0.f);
            Ray ray_o = {P_object,d_object};
            RGBA toColor = {255,255,255,255};

            Intersections inter(ray_o,p.primitive.type,toColor);
            float t = inter.calculateIntersection();

            if(t>=0.f&&t<t_min) {
                t_min = t;
                closestI->copyData(inter);
                *closestShape = p;
            }
        }
        return t_min;
    }
    float leftT = 100000000;
    float rightT = 100000000;
    root->left->box.intersects(ray,leftT);
    root->right->box.intersects(ray,rightT);
    if(leftT<=rightT) {
        return KDIntersection(root->left,ray,closestI,closestShape);
    } return KDIntersection(root->right,ray,closestI,closestShape);
}


void visualizeBinaryTree(Node* node, int depth = 0, char prefix = ' ') {
    if (node == nullptr) {
        return;
    }

    std::cout << std::string(depth * 4, ' ') << prefix << " Shapes size: " << node->shapes.size() << ", Box points: ";

    for (const auto& point : node->box.points_) {
        std::cout << point << ", ";
    }
    std::cout << std::endl;

    visualizeBinaryTree(node->left, depth + 1, 'L');
    visualizeBinaryTree(node->right, depth + 1, 'R');
}
