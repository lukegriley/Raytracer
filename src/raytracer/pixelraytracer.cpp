#include "pixelraytracer.h"
#include "raytracescene.h"
#include "utils/intersections.h"
#include <cmath>
#include <limits>
#include "utils/light.h"
#include "utils/ray.h"
#include <iostream>

PixelRaytracer::PixelRaytracer( RGBA *imageData,const RayTraceScene& scene): m_scene(scene), imageData_(imageData)
{

}

RGBA PixelRaytracer::traceRay(const RayTraceScene& scene, Ray ray_world, int depth) {
    //declare intersection minimums
    float t_min = std::numeric_limits<float>::max();
    RGBA toColor = {255,255,255,255};
    Intersections closestI(0.f,toColor);
    RenderShapeData closestShape;

    //loop primitives
    for(const RenderShapeData& p : scene.getMetaData().shapes) {
        //get Ray in object space
        glm::vec4 P_object = glm::inverse(p.ctm) * ray_world.p;
        glm::vec4 d_object_unnorm = glm::inverse(p.ctm) * ray_world.d;
        Ray ray_o = {P_object,d_object_unnorm};

        Intersections inter(ray_o,p.primitive.type,toColor);
        float t = inter.calculateIntersection();

        if(t>=0.f&&t<t_min) {
            t_min = t;
            closestI.copyData(inter);
            closestShape = p;
        }
    }

    if(t_min <  std::numeric_limits<float>::max()) {
        //calculate object normal in world space
        glm::vec4 normal_object = closestI.calculateNormal();
        glm::mat3 M3T1 = glm::inverse(glm::transpose(glm::mat3(closestShape.ctm)));
        glm::vec4 normal_world(M3T1 * glm::vec3(normal_object),0.f);

        //calculate direction to camera
        glm::vec4 pos_object(glm::vec3(closestI.p_ + closestI.t_*closestI.d_),1.f);
        glm::vec4 pos_world = closestShape.ctm * pos_object;
        glm::vec4 P_world = scene.getCamera().getInverseViewMatrix() * glm::vec4(0.f,0.f,0.f,1.f);
        glm::vec4 directionToCamera = depth==0 ? glm::vec4(P_world) - pos_world : -ray_world.d;

        Light l;
//        RGBA outputPixel = l.phong(pos_world,
//                                   normal_world,
//                                   directionToCamera,
//                                   closestShape.primitive.material,
//                                   scene,
//                                   depth);
        RGBA outputPixel = l.toRGBA(normal_world);
//        imageData_[j*m_scene.width()+i] = outputPixel;
        return outputPixel;
    } else {
//        imageData_[j*m_scene.width()+i] = {0,0,0,255};
        return {0,0,0,255};
    }
}


RGBA PixelRaytracer::operator()(int index) {

    int i = index % m_scene.width();  // Calculate i from the remainder of the index divided by width
    int j = index / m_scene.width();

    if(j==350&&i==425) {
        int re = 0;
    }


    const Camera& c = m_scene.getCamera();
    float V = 2.0 * tan(c.getHeightAngle() / 2.0);
    float U = c.getAspectRatio() * V;
    glm::vec4 P_view = glm::vec4(0.f,0.f,0.f,1.f);
    float x = (i + 0.5)/m_scene.width() - 0.5;
    float y  = (j + 0.5)/m_scene.height() - 0.5;

    //initialize Ray
    glm::vec4 uvk_view = glm::vec4(x*U,-y*V,-1.f,1.f);
    glm::vec4 d_view = uvk_view - P_view;
    glm::vec4 d_world_unnorm = c.getInverseViewMatrix() * d_view;
    glm::vec4 d_world(glm::normalize(glm::vec3(d_world_unnorm)),0.f);
    glm::vec4 P_world = c.getInverseViewMatrix() * P_view;
    Ray ray_world = {P_world,d_world};
    imageData_[j*m_scene.width()+i] = traceRay(m_scene,ray_world,0);

}
