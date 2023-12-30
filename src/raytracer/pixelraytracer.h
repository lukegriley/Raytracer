#ifndef PIXELRAYTRACER_H
#define PIXELRAYTRACER_H
#include "raytracescene.h"
#include "utils/rgba.h"


class PixelRaytracer
{
public:
    PixelRaytracer(RGBA *imageData, const RayTraceScene& scene);
    RGBA operator()(int index);
    static RGBA traceRay(const RayTraceScene& scene, Ray ray_world, int depth);
private:
   const RayTraceScene& m_scene;
    RGBA* imageData_;
};

#endif // PIXELRAYTRACER_H
