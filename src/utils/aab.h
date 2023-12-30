#ifndef AAB_H
#define AAB_H
#include <glm/glm.hpp>
#include "ray.h"
#include "scenedata.h"

class AAB
{
public:
    AAB();
    AAB(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);
    AAB(float points[6]);
    bool intersects(Ray ray, float& tVal);
    float points_[6];//minx, miny, minz, maxx, maxy, maxz
    static AAB getTransformedPrimitiveAAB(glm::mat4 ctm);
    float& operator[](int index);
};

#endif // AAB_H
