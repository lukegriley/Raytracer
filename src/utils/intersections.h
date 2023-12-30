#ifndef INTERSECTIONS_H
#define INTERSECTIONS_H
#include <glm/glm.hpp>
#include "scenedata.h"
#include "rgba.h"
#include "ray.h"

class Intersections
{
public:
    void copyData(const Intersections& i) noexcept;
    Intersections(Ray& ray,PrimitiveType type, RGBA& color);
    Intersections(float t, RGBA& color);

    float calculateIntersection();
    glm::vec4 calculateNormal();
    int getIntersectedFace;
    glm::vec4 p_;
    glm::vec4 d_;
    PrimitiveType type_;
    float t_;
    int face_;
    RGBA& color_;
    Ray* rayPointer_;

private:
    bool isInsideFace(int axis1, int axis2, float t);
    float flatbaseIntersect(float x);
    float conicalTopIntersect();
    const double epsilon = 1e-3;

    float coneIntersect();
    float cylinderIntersect();
    float cubeIntersect();
    float sphereIntersect();
    float cylinderSurfaceIntersect();

    glm::vec4 conicalNormal();
    glm::vec4 flatNormal(int axis, int neg);
    glm::vec4 cylindricalSurfaceNormal();
    glm::vec4 sphereNormal();
    glm::vec4 cubeNormal();

};

#endif // INTERSECTIONS_H
