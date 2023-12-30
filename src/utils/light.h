#ifndef LIGHT_H
#define LIGHT_H
#include "utils/rgba.h"
#include "utils/scenedata.h"
#include <glm/glm.hpp>
#include "utils/sceneparser.h"
#include "raytracer/raytracescene.h"
#include "raytracer/pixelraytracer.h"

class Light
{
public:
    Light();
    RGBA toRGBA(const glm::vec4 &illumination);
    RGBA phong(glm::vec3  position,
               glm::vec3  normal,
               glm::vec3  directionToCamera,
               SceneMaterial  &material,
               const RayTraceScene& scene,
               int depth);
    bool occluded(const SceneLightData& light, const std::vector<RenderShapeData>& shapes, glm::vec3 position);

};

#endif // LIGHT_H
