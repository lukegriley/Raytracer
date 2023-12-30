#include <stdexcept>
#include "raytracescene.h"
#include "utils/sceneparser.h"
#include <cmath>


RayTraceScene::RayTraceScene(int width, int height, const RenderData &metaData)
    : width_(width), height_(height), metaData_(metaData), camera_(Camera(width,height,metaData.cameraData))
{

}


const int& RayTraceScene::width() const {
    return width_;
}

const int& RayTraceScene::height() const {
    return height_;
}

const SceneGlobalData& RayTraceScene::getGlobalData() const {
    // Implement this function to return the global data of the scene
    // For example: return globalData_;
    return metaData_.globalData;
}

const RenderData& RayTraceScene::getMetaData() const {
    return metaData_;
}

const Camera& RayTraceScene::getCamera() const {
    return camera_;
}
