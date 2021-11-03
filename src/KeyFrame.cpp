//
// Created by erik_ on 03.11.2021.
//

#include "KeyFrame.h"


KeyFrame::KeyFrame(mat4 modelViewTransform, mat4 modelLightTransform,
                   vec3 backgroundColor, float explosionDist) {
    this->modelViewTransform = modelViewTransform;
    this->modelLightTransform = modelLightTransform;
    this->backgroundColor = backgroundColor;
    this->explosionDist = explosionDist;

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

mat4 KeyFrame::getModelViewTransform() const {
    return modelViewTransform;
}

mat4 KeyFrame::getModelLightTransform() const {
    return modelLightTransform;
}
