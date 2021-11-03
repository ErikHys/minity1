//
// Created by erik_ on 03.11.2021.
//

#include "KeyFrame.h"


KeyFrame::KeyFrame(mat4 modelViewProjectionMatrix, vec2 viewPortSize, vec3 worldCameraPosition, vec3 worldLightPosition,
                   vec3 lightIntensity, vec3 lightColor, vec3 backgroundColor, float explosionDist) {
    this->modelViewProjectionMatrix = modelViewProjectionMatrix;
    this->viewPortSize = viewPortSize;
    this->worldCameraPosition = worldCameraPosition;
    this->worldLightPosition = worldLightPosition;
    this->lightIntensity = lightIntensity;
    this->lightColor = lightColor;
    this->backgroundColor = backgroundColor;
    this->explosionDist = explosionDist;

}

mat4 KeyFrame::getModelViewProjectionMatrix() const {
    return modelViewProjectionMatrix;
}

vec2 KeyFrame::getViewPortSize() const {
    return viewPortSize;
}

vec3 KeyFrame::getWorldCameraPosition() const {
    return worldCameraPosition;
}

vec3 KeyFrame::getWorldLightPosition() const {
    return worldLightPosition;
}

vec3 KeyFrame::getLightIntensity() const {
    return lightIntensity;
}

vec3 KeyFrame::getLightColor() const {
    return lightColor;
}

vec3 KeyFrame::getBackgroundColor() const {
    return backgroundColor;
}

float KeyFrame::getExplosionDist() const {
    return explosionDist;
}
