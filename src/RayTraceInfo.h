//
// Created by erik_ on 17.11.2021.
//

#include "ModelRenderer.h"
#include <globjects/base/File.h>
#include <globjects/State.h>
#include <iostream>
#include <filesystem>
#include <imgui.h>
#include "Viewer.h"
#include "Scene.h"
#include "Model.h"
#include <sstream>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#ifndef MINITY_RAYTRACEINFO_H
#define MINITY_RAYTRACEINFO_H


class RayTraceInfo {

public:
    RayTraceInfo(bool sphere, bool box, bool cylinder, bool plane, glm::vec3 sphereColor,
                 glm::vec3 boxColor, glm::vec3 cylinderColor, glm::vec3 planeColor,
                 glm::vec3 spherePosition, glm::vec3 boxPosition, glm::vec3 cylinderPosition,
                 glm::vec3 planeNormal, float planePosition, float cylinderHeight,
                 float boxScale, float sphereScale, glm::vec3 lightColor, bool celShading,
                 int levelOfCelShading, float lightIntensityFront);
    RayTraceInfo();
    bool sphere;
    bool box;
    bool cylinder;
    bool plane;
    glm::vec3 sphereColor;
    glm::vec3 boxColor;
    glm::vec3 cylinderColor;
    glm::vec3 planeColor;
    glm::vec3 spherePosition;
    glm::vec3 boxPosition;
    glm::vec3 cylinderPosition;
    glm::vec3 planeNormal;
    float planePosition;
    float cylinderHeight;
    float boxScale;
    float sphereScale;
    glm::vec3 lightColor;
    bool celShading;
    int levelOfCelShading;
    float lightIntensityFront;

};


#endif //MINITY_RAYTRACEINFO_H
