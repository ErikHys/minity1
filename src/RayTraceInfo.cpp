//
// Created by erik_ on 17.11.2021.
//

#include "RayTraceInfo.h"

RayTraceInfo::RayTraceInfo(bool sphere, bool box, bool cylinder, bool plane, glm::vec3 sphereColor, glm::vec3 boxColor,
                           glm::vec3 cylinderColor, glm::vec3 planeColor, glm::vec3 spherePosition,
                           glm::vec3 boxPosition, glm::vec3 cylinderPosition, glm::vec3 planeNormal,
                           float planePosition, float cylinderHeight, float boxScale, float sphereScale,
                           glm::vec3 lightColor, bool celShading, int levelOfCelShading, float lightIntensityFront) {
    this->sphere = sphere;
    this->box = box;
    this->cylinder = cylinder;
    this->plane = plane;
    this->sphereColor = sphereColor;
    this->boxColor = boxColor;
    this->cylinderColor = cylinderColor;
    this->planeColor = planeColor;
    this->spherePosition = spherePosition;
    this->boxPosition = boxPosition;
    this->cylinderPosition = cylinderPosition;
    this->planeNormal = planeNormal;
    this->planePosition = planePosition;
    this->cylinderHeight = cylinderHeight;
    this->boxScale = boxScale;
    this->sphereScale = sphereScale;
    this->lightColor = lightColor;
    this->celShading = celShading;
    this->levelOfCelShading = levelOfCelShading;
    this->lightIntensityFront = lightIntensityFront;

}

RayTraceInfo::RayTraceInfo() {

}
