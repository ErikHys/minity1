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
    KeyFrame(mat4 modelViewProjectionMatrix, vec2 viewPortSize, vec3 worldCameraPosition, vec3 WorldLightPosition,
             vec3 lightIntensity, vec3 lightColor, vec3 backgroundColor, float explosionDist);
    mat4 getModelViewProjectionMatrix() const;
    vec2 getViewPortSize() const;
    vec3 getWorldCameraPosition() const;
    vec3 getWorldLightPosition() const;
    vec3 getLightIntensity() const;
    vec3 getLightColor() const;
    vec3 getBackgroundColor() const;
    float getExplosionDist() const;

private:
    mat4 modelViewProjectionMatrix;
    vec2 viewPortSize;
    vec3 worldCameraPosition;
    vec3 worldLightPosition;
    vec3 lightIntensity;
    vec3 lightColor;
    vec3 backgroundColor;
    float explosionDist;

};


#endif //MINITY_KEYFRAME_H
