#include <stdexcept>
#include "camera.h"
#include "utils/scenedata.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"


Camera::Camera(int width, int height, const SceneCameraData& cameraData) {
    focalLength_ = cameraData.focalLength;
    aperture_ = cameraData.aperture;
    heightAngle_ = cameraData.heightAngle;
    viewMatrix_ = Camera::calculateViewMatrix(glm::vec3(cameraData.pos),
                                              glm::vec3(cameraData.look),
                                              glm::vec3(cameraData.up));
    inverseViewMatrix_ = glm::inverse(viewMatrix_);
    aspectRatio_ = static_cast<float>(width)/static_cast<float>(height);
    pos_ = cameraData.pos;
}

glm::mat4 Camera::calculateViewMatrix(glm::vec3 pos,glm::vec3 look, glm::vec3 up) {
    glm::mat4 t = glm::mat4(1.0f);

    t[3][0] = -pos.x;
    t[3][1] = -pos.y;
    t[3][2] = -pos.z;
    glm::vec3 w = glm::normalize(-look);
    glm::vec3 v = glm::normalize(up - glm::dot(glm::vec3(up),w)*w);
    glm::vec3 u = glm::cross(v,w);

    glm::mat4 rotate = glm::mat4(
        u.x,v.x,w.x,0.f,
        u.y,v.y,w.y,0.f,
        u.z,v.z,w.z,0.f,
        0.f, 0.f, 0.f, 1.f);

    return rotate * t;
}

glm::mat4 Camera::getViewMatrix() const {
    return viewMatrix_;
}

glm::mat4 Camera::getInverseViewMatrix() const {
    return inverseViewMatrix_;
}

float Camera::getAspectRatio() const {
    return aspectRatio_;
}

float Camera::getHeightAngle() const {
    return heightAngle_;
}

float Camera::getFocalLength() const {
    return focalLength_;
}

float Camera::getAperture() const {
    return aperture_;
}

glm::vec3 Camera::getPos() const {
    return pos_;
}
