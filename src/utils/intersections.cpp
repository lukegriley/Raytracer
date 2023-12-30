#include "intersections.h"
#include <array>
#include <algorithm>

void Intersections::copyData(const Intersections& i) noexcept {
    p_ = i.p_;
    d_ = i.d_;
    type_ = i.type_;
    t_ = i.t_;
    face_ = i.face_;
}

Intersections::Intersections(float t, RGBA& color) : color_(color) {
}

Intersections::Intersections(Ray& ray, PrimitiveType type, RGBA& color) : color_(color)
{
    p_ = ray.p;
    d_ = ray.d;
    type_ = type;
    t_ = -2.f;
    face_ = -1;
    rayPointer_ = &ray;
}


float Intersections::calculateIntersection() {
    switch(type_) {
    case(PrimitiveType::PRIMITIVE_CUBE):
        t_ = cubeIntersect();
        break;
    case(PrimitiveType::PRIMITIVE_CONE):
        t_ = coneIntersect();
        break;
    case(PrimitiveType::PRIMITIVE_CYLINDER):
        t_ = cylinderIntersect();
        break;
    case PrimitiveType::PRIMITIVE_SPHERE:
        t_ = sphereIntersect();
        break;
//    default: //mesh
//        throw std::runtime_error("Mesh in render data");
    }
    rayPointer_->t = t_;
    return t_;
}

float Intersections::coneIntersect() {
    //faces: 0 - flatbase, 1 - conical top
    float flatbaseT = flatbaseIntersect(-0.5f);
    float conicalTopT = conicalTopIntersect();
    if(flatbaseT>=0.0&&conicalTopT>=0.0) {
        if(flatbaseT<=conicalTopT) {
            face_ = 0;
            return flatbaseT;
        }
        else {
            face_ = 1;
            return conicalTopT;
        }
    }
    if(conicalTopT>=0.0) {
        face_ = 1;
        return conicalTopT;
    }
    if(flatbaseT>=0.0) {
        face_ = 0;
        return flatbaseT;
    }
    return -1.f;
}

float Intersections::cylinderIntersect() {
    //faces: 0 - surface, 1 - bottom, 2 - top
    std::array<float, 3> tVals = {cylinderSurfaceIntersect(),
                                  flatbaseIntersect(-0.5f),
                                  flatbaseIntersect(0.5f)};
    int min_i = -1;
    float min_t = std::numeric_limits<float>::max();
    for(int i=0;i<3;i++) {
        if(tVals[i]<min_t && tVals[i]>=0.f) {
            min_t = tVals[i];
            min_i = i;
        }
    }
    face_ = min_i;
    if(min_i>-1) {
        return min_t;
    }
    return -1.f;
}

float Intersections::cylinderSurfaceIntersect() {
    float a = d_.x*d_.x + d_.z*d_.z;
    float b = 2.f * p_.x * d_.x + 2.f * p_.z * d_.z;
    float c = p_.x * p_.x + p_.z * p_.z - 0.25f;
    float discrim = b * b - 4.f*a*c;
    if(discrim<0.f) {
        return -1.f;
    }
    float tA = (-b + sqrt(discrim))/(2.f*a);
    float tB = (-b - sqrt(discrim))/(2.f*a);
    float t;
    if(tA>=0.0&&tB>=0.0) {
        t = std::fmin(tA,tB);
    }
    else if(tA>=0.0) {
        t = tA;
    }
    else if(tB>=0.0) {
        t = tB;
    } else {
        t = -1.f;
    }

    glm::vec4 dir = p_ + t*d_;
    if(dir.y<=0.5f && dir.y>=-0.5f && t>=0.f){
        return t;
    }
    return -1.f;
}

float Intersections::sphereIntersect(){
    float a = d_.x * d_.x + d_.y * d_.y + d_.z * d_.z;
    float b = 2.f*p_.x*d_.x + 2.f*p_.y*d_.y + 2.f*p_.z*d_.z;
    float c = p_.x * p_.x + p_.y * p_.y + p_.z * p_.z - 0.25f;

    float discrim = b * b - 4.f * a * c;
    if (discrim<0.f) {
        return -1.f;
    }
    float tA = (-b + sqrt(discrim))/(2.f*a);
    float tB = (-b - sqrt(discrim))/(2.f*a);
    if(tA>=-epsilon&&tB>=-epsilon) {
        face_ = 0;
        return std::fmin(tA,tB);
    }
    if(tA>=0.0) {
        face_ = 0;
        return tA;
    }
    if(tB>=0.0) {
        face_ = 0;
        return tB;
    }
    return -1.f;
}

bool Intersections::isInsideFace(int axis1, int axis2, float t) {
    if(t<=-10*epsilon) {
        return false;
    }
    glm::vec3 point = p_ + d_*t;
    return point[axis1]>-0.5-epsilon && point[axis1]<0.5+epsilon &&
           point[axis2]>-0.5-epsilon && point[axis2]<0.5+epsilon;
}

float Intersections::cubeIntersect() {
    //faces: 0 - pos x facing, 1 - pos y facing, 2 - pos z facing,
    //       3 - neg x facing, 4 - neg y facing, 5 - neg z facing
//    float t_min = std::numeric_limits<float>::max();

    float txA = (-0.5f - p_.x) / d_.x;
    float txB = (0.5f - p_.x) / d_.x;
//    if(isInsideFace(1,2,txA) && txA<t_min){
//        t_min = txA;
//    }
//    if(isInsideFace(1,2,txB) && txB<t_min){
//        t_min = txB;
//    }

    float tyA = (-0.5f - p_.y) / d_.y;
    float tyB = (0.5f - p_.y) / d_.y;
//    if(isInsideFace(0,2,tyA) && tyA<t_min){
//        t_min = tyA;
//    }
//    if(isInsideFace(0,2,tyB) && tyB<t_min){
//        t_min = tyB;
//    }

    float tzA = (-0.5f - p_.z) / d_.z;
    float tzB = (0.5f - p_.z) / d_.z;
//    if(isInsideFace(0,1,tzA) && tzA<t_min){
//        t_min = tzA;
//    }
//    if(isInsideFace(0,1,tzB) && tzB<t_min){
//        t_min = tzB;
//    }
//    if(t_min<std::numeric_limits<float>::max()){
//        return t_min;
//    }

        float t_max = std::max(std::max(std::min(txA, txB), std::min(tyA, tyB)), std::min(tzA, tzB));
        float t_min = std::min(std::min(std::max(txA, txB), std::max(tyA, tyB)), std::max(tzA, tzB));
        if (t_min > t_max-epsilon && t_max >= -1*epsilon) {
            if (t_max == txA || t_max == txB) face_ = (txA < txB) ? 3 : 0;
            else if (t_max == tyA || t_max == tyB) face_ = (tyA < tyB) ? 4 : 1;
            else face_ = (tzA < tzB) ? 5 : 2;

            return t_max;
        }

    return -1.f;
}

float Intersections::flatbaseIntersect(float y) {
    float t = (y - p_.y)/d_.y;
    if(t>=0.f){
        float intPointX = p_.x + t*d_.x;
        float intPointZ = p_.z + t*d_.z;
        float distTo = intPointX * intPointX + intPointZ * intPointZ;
        if(distTo <= 0.25) {
            return t;
        }
        return -1.f;
    }
    return -1.f;
}

float Intersections::conicalTopIntersect() {
    float a = 4.f*d_.x*d_.x + 4.f*d_.z*d_.z - d_.y*d_.y;
    float b = 8.f*p_.x*d_.x + 8.f*p_.z*d_.z - 2.f*d_.y*p_.y + d_.y;
    float c = 4.f*p_.x*p_.x + 4.f*p_.z*p_.z - p_.y*p_.y + p_.y - 0.25f;
    float discrim = b*b - 4*a*c;
    if (discrim<0.f) {
        return -1.f;
    }
    float tA = (-b + sqrt(discrim))/(2.f*a);
    float tB = (-b - sqrt(discrim))/(2.f*a);
    float t;
    if(tA>=0.0&&tB>=0.0) {
        t = std::fmin(tA,tB);
    }
    else if(tA>=0.0) {
        t = tA;
    }
    else if(tB>=0.0) {
        t = tB;
    } else {
        t = -1.f;
    }
    glm::vec4 dir = p_ + t*d_;
    if(dir.y>=-0.5f && dir.y<=0.5f && t>=0.f) {
        return t;
    }
    return -1.f;
}

glm::vec4 Intersections::calculateNormal() {
    if(t_<0.f) {
        throw std::runtime_error("No intersection calculated.");
    }
    glm::vec4 normal;
    switch(type_) {
    case PrimitiveType::PRIMITIVE_CONE:
        if(face_==0) {
            normal = flatNormal(1,-1);
            break;
        } else if(face_==1){
            normal = conicalNormal();
            break;
        } else {
            throw std::runtime_error("No valid intersection found for normal to be calculated.");
        }
        break;
    case PrimitiveType::PRIMITIVE_CYLINDER:
        switch(face_) {
        case 0:
            normal =  cylindricalSurfaceNormal();
            break;
        case 1:
            normal =  flatNormal(1,-1);
            break;
        case 2:
            normal =  flatNormal(1,1);
            break;
        default:
            throw std::runtime_error("No valid intersection found for normal to be calculated.");
            break;
        }
        break;
    case PrimitiveType::PRIMITIVE_CUBE:

        normal = cubeNormal();
        break;
//        switch(face_) {
//        case 0:
//            normal =  flatNormal(0,-1);
//            break;
//        case 1:
//            normal =  flatNormal(1,1);
//            break;
//        case 2:
//            normal =  flatNormal(2,1);
//            break;
//        case 3:
//            normal =  flatNormal(0,1);
//            break;
//        case 4:
//            normal =  flatNormal(1,1);
//            break;
//        case 5:
//            normal =  flatNormal(2,-1);
//            break;
//        default:
//            throw std::runtime_error("No valid intersection found for normal to be calculated.");
//            break;
//        }
    case PrimitiveType::PRIMITIVE_SPHERE:
        normal =  sphereNormal();
        break;
    default://mesh
        throw std::runtime_error("Mesh in render data.");
        break;
    }
//    normal.y *= 1.f;
    color_.r = 255*(normal.x+1.f)/2.f;
    color_.g = 255*(normal.y+1.f)/2.f;
    color_.b = 255*(normal.z+1.f)/2.f;
    return normal;
}

glm::vec4 Intersections::cubeNormal() {
    glm::vec3 intPoint = p_+d_*t_;
    glm::vec3 gradient;
    if(fabs(intPoint.x-0.5)<epsilon) {
        gradient = glm::vec3(1,0,0);
    }
    else if(fabs(intPoint.x+0.5)<epsilon) {
        gradient = glm::vec3(-1,0,0);
    }
    else if(fabs(intPoint.y-0.5)<epsilon) {
        gradient = glm::vec3(0,1,0);
    }
    else if(fabs(intPoint.y+0.5)<epsilon) {
        gradient = glm::vec3(0,-1,0);
    }
    else if(fabs(intPoint.z-0.5)<epsilon) {
        gradient = glm::vec3(0,0,1);
    }
    else /*if(intPoint.z==-0.5)*/ {
        gradient = glm::vec3(0,0,-1);
    }
    return glm::vec4(gradient,0.f);
}

glm::vec4 Intersections::conicalNormal() {
    glm::vec3 point = (p_ + d_*t_);
    glm::vec3 gradient(point.x * 2.f, -0.5f*(point.y - 0.5f),point.z * 2.f);
    return glm::vec4(glm::normalize(gradient),0.f);
}

glm::vec4 Intersections::flatNormal(int axis, int neg) {
    glm::vec3 gradient;
    if(axis==0) {
        gradient = glm::vec3(1.f*neg,0.f,0.f);
    } else if (axis==1) {
        gradient = glm::vec3(0.f,1.f*neg,0.f);
    } else if(axis==2) {
        gradient = glm::vec3(0.f,0.f,1.f*neg);
    } else {
        throw std::runtime_error("Invalid axis.");
    }
    return glm::vec4(glm::normalize(gradient),0.f);
}

glm::vec4 Intersections::cylindricalSurfaceNormal(){
    glm::vec3 point = (p_ + d_*t_);

    glm::vec3 gradient(point.x*2.f,0.f,point.z*2.f);
    return glm::vec4(glm::normalize(gradient),0.f);
}

glm::vec4 Intersections::sphereNormal(){
    glm::vec3 gradient = 2.f*(p_ + d_*t_);
    return glm::vec4(glm::normalize(gradient),0.f);
}
