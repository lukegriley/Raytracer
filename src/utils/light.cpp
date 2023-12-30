#include "light.h"
#include "intersections.h"
#include <limits>

Light::Light()
{

}

RGBA Light::toRGBA(const glm::vec4 &illumination) {
    // Task 1
    RGBA res;
    res.r = std::round(255.f*std::min(std::max(illumination[0],0.f),1.f));
    res.g = std::round(255.f*std::min(std::max(illumination[1],0.f),1.f));
    res.b = std::round(255.f*std::min(std::max(illumination[2],0.f),1.f));
    res.a = 255;
    return res;
}


float attentuation(glm::vec3 lightPos, glm::vec3 intersectionPos, glm::vec3 coeff) {
    float dist = sqrt((lightPos.x-intersectionPos.x)*(lightPos.x-intersectionPos.x) +
                 (lightPos.y-intersectionPos.y)*(lightPos.y-intersectionPos.y) +
                      (lightPos.z-intersectionPos.z)*(lightPos.z-intersectionPos.z));
    return std::min(1.f,1.f/(coeff[0] + dist*coeff[1] + coeff[2]*dist * dist));
}

float angularFalloff(float x, float inner, float outer) {
    return -2.0 * std::pow((x - inner)/(outer - inner),3) + 3.f * std::pow((x - inner)/(outer - inner),2);
}

glm::vec3 falloffIllumination(const SceneLightData& light, glm::vec3 intersectionPos) {
    glm::vec3 currentDir = glm::normalize(intersectionPos - glm::vec3(light.pos));
    float x = acos(glm::dot(currentDir,glm::normalize(glm::vec3(light.dir))));
    if(x <= light.angle - light.penumbra) {
        return light.color;
    } else if(x>light.angle){
        return glm::vec3(0,0,0);
    }
    return light.color * (1.f - angularFalloff(x,light.angle - light.penumbra,light.angle));
}

bool Light::occluded(const SceneLightData& light, const std::vector<RenderShapeData>& shapes, glm::vec3 position) {

        RGBA toColor = {0,0,0,0};
        for(const RenderShapeData& shape : shapes) {
            Ray pointToLight;
            float lightT;
            glm::vec4 pos_object = glm::inverse(shape.ctm)*glm::vec4(position,1.f);
            glm::vec4 light_object = glm::inverse(shape.ctm)*light.pos;
            if(light.type!=LightType::LIGHT_DIRECTIONAL) {
                pointToLight = {pos_object,glm::normalize(light_object - pos_object)};
                lightT = glm::length(glm::vec3(light_object) - glm::vec3(pos_object));
            }
            else {
                pointToLight = {pos_object,-glm::normalize(light.dir)};
                lightT = std::numeric_limits<float>::max();
            }

            Intersections i(pointToLight,shape.primitive.type,toColor);
                float t = i.calculateIntersection();
                if(t > 0.01f && t < lightT) {
                    return true;
                }

        }
        return false;
}


// Calculates the RGBA of a pixel from intersection infomation and globally-defined coefficients
RGBA Light::phong(glm::vec3  position,
                  glm::vec3  normal,
                  glm::vec3  directionToCamera,
                  SceneMaterial  &material,
                  const RayTraceScene& scene,
                  int depth){
        const std::vector<SceneLightData> &lights = scene.getMetaData().lights;
        SceneGlobalData globalData = scene.getGlobalData();
        const std::vector<RenderShapeData> &shapes = scene.getMetaData().shapes;
    // Normalizing directions
    normal            = glm::normalize(normal);
    directionToCamera = glm::normalize(directionToCamera);

    // Output illumination (we can ignore opacity)
    glm::vec4 illumination (0.f,0.f,0.f,1.f);

    // Task 3: add the ambient term
    illumination[0] = globalData.ka * material.cAmbient[0];
    illumination[1] = globalData.ka * material.cAmbient[1];
    illumination[2] = globalData.ka * material.cAmbient[2];
    glm::vec3 li;

    for (const SceneLightData &light : lights) {
        bool lit = !occluded(light,shapes,position);
        if(light.type==LightType::LIGHT_DIRECTIONAL && lit) {
            li = glm::normalize(glm::vec3(-light.dir));
            float diffuseCoeff = glm::dot(li,normal) * globalData.kd;
            if(diffuseCoeff>0) {
                illumination[0] += diffuseCoeff * material.cDiffuse[0] * light.color[0];
                illumination[1] += diffuseCoeff * material.cDiffuse[1] * light.color[1];
                illumination[2] += diffuseCoeff * material.cDiffuse[2] * light.color[2];
            }

            glm::vec3 Ri = glm::reflect(-li,normal);
            float reflectDot = glm::dot(Ri,directionToCamera);
            if(reflectDot>0) {
                float exp = std::pow(reflectDot,material.shininess);
                float specularCoeff = globalData.ks * exp;
                illumination[0] += specularCoeff * material.cSpecular[0] * light.color[0];
                illumination[1] += specularCoeff * material.cSpecular[1] * light.color[1];
                illumination[2] += specularCoeff * material.cSpecular[2] * light.color[2];
            }
        }
        else if(light.type==LightType::LIGHT_SPOT && lit) {
//            glm::vec3 li = glm::normalize(glm::vec3(-light.dir));
            li = glm::normalize(glm::vec3(light.pos)-position);
            float att = attentuation(light.pos,position,light.function);
            float diffuseCoeff = glm::dot(li,normal) * globalData.kd;
            glm::vec3 If = falloffIllumination(light,position);
            if(diffuseCoeff>0) {
                illumination[0] += diffuseCoeff * material.cDiffuse[0] * If[0] * att;
                illumination[1] += diffuseCoeff * material.cDiffuse[1] * If[1] * att;
                illumination[2] += diffuseCoeff * material.cDiffuse[2] * If[2] * att;
            }

            glm::vec3 Ri = glm::reflect(-li,normal);
            float reflectDot = glm::dot(Ri,directionToCamera);
            if(reflectDot>0) {
                float exp = std::pow(reflectDot,material.shininess);
                float specularCoeff = globalData.ks * exp;
                illumination[0] += specularCoeff * material.cSpecular[0] * If[0] * att;
                illumination[1] += specularCoeff * material.cSpecular[1] * If[1] * att;
                illumination[2] += specularCoeff * material.cSpecular[2] * If[2] * att;
            }
        }
        else if(light.type==LightType::LIGHT_POINT && lit) {
//            glm::vec3 li = glm::normalize(glm::vec3(-light.dir));
            li = glm::normalize(glm::vec3(light.pos)-position);//needs to be REFLECTED position
            float att = attentuation(light.pos,position,light.function);
            float diffuseCoeff = glm::dot(li,normal) * globalData.kd;
            if(diffuseCoeff>0) {
                illumination[0] += diffuseCoeff * material.cDiffuse[0] * light.color[0] * att;
                illumination[1] += diffuseCoeff * material.cDiffuse[1] * light.color[1] * att;
                illumination[2] += diffuseCoeff * material.cDiffuse[2] * light.color[2] * att;
            }

            glm::vec3 Ri = glm::reflect(-li,normal);
            float reflectDot = glm::dot(Ri,directionToCamera);
            if(reflectDot>0) {
                float exp = std::pow(reflectDot,material.shininess);
                float specularCoeff = globalData.ks * exp;
                illumination[0] += specularCoeff * material.cSpecular[0] * light.color[0] * att;
                illumination[1] += specularCoeff * material.cSpecular[1] * light.color[1] * att;
                illumination[2] += specularCoeff * material.cSpecular[2] * light.color[2] * att;
            }
        }
        if((lit&&material.cReflective[0]>0)||
            (material.cReflective[1]>0)||
            (material.cReflective[2]>0)){//reflect code
            Ray reflected;
            reflected.p = glm::vec4(position,1.f);
            reflected.d = glm::normalize(glm::vec4(-directionToCamera - 2.f*normal*glm::dot(normal,-directionToCamera),0.f));
            if(depth<4){
            depth+=1;
            RGBA reflectPixel = PixelRaytracer::traceRay(scene,reflected,depth);
            glm::vec4 reflectPixelNormalized = glm::vec4(
                reflectPixel.r / 255.0f,
                reflectPixel.g / 255.0f,
                reflectPixel.b / 255.0f,
                reflectPixel.a / 255.0f
                );

            glm::vec4 blendFactor = glm::vec4(
                material.cReflective[0] * reflectPixelNormalized.r,
                material.cReflective[1] * reflectPixelNormalized.g,
                material.cReflective[2] * reflectPixelNormalized.b,
                material.cReflective[3] * reflectPixelNormalized.a
                );

            glm::vec4 blendedReflective = blendFactor * globalData.ks;
            illumination = illumination * (glm::vec4(1.0f) - blendFactor) + blendedReflective;
            }
        }
    }

    RGBA returnValue = toRGBA(illumination);
    return returnValue;
}
