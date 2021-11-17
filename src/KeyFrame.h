//
// Created by erik_ on 03.11.2021.
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
#include "RayTraceInfo.h"
#include <sstream>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#ifndef MINITY_KEYFRAME_H
#define MINITY_KEYFRAME_H

using namespace glm;

class KeyFrame {

public:
    KeyFrame(mat4 modelViewTransform, mat4 modelLightTransform,
            vec3 backgroundColor, float explosionDist);
    KeyFrame(mat4 modelViewTransform, mat4 modelLightTransform,
             vec3 backgroundColor, float explosionDist, RayTraceInfo rayTraceInfo);
    mat4 getModelViewTransform() const;
    mat4 getModelLightTransform() const;
    vec3 getLightIntensity() const;
    vec3 getLightColor() const;
    vec3 getBackgroundColor() const;
    float getExplosionDist() const;
    RayTraceInfo rayTraceInfo;

private:
    mat4 modelViewTransform{};
    mat4 modelLightTransform{};
    vec3 lightIntensity{};
    vec3 lightColor{};
    vec3 backgroundColor{};
    float explosionDist;

};


#endif //MINITY_KEYFRAME_H
