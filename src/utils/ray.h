#pragma once

#include <cstdint>
#include <glm/glm.hpp>

struct Ray {
    glm::vec4 p;
    glm::vec4 d;
    float t;
};
