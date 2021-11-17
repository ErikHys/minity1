#include "RaytraceRenderer.h"
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

using namespace minity;
using namespace gl;
using namespace glm;
using namespace globjects;

RaytraceRenderer::RaytraceRenderer(Viewer* viewer) : Renderer(viewer)
{
	m_quadVertices->setStorage(std::array<vec2, 4>({ vec2(-1.0f, 1.0f), vec2(-1.0f,-1.0f), vec2(1.0f,1.0f), vec2(1.0f,-1.0f) }), gl::GL_NONE_BIT);
	auto vertexBindingQuad = m_quadArray->binding(0);
	vertexBindingQuad->setBuffer(m_quadVertices.get(), 0, sizeof(vec2));
	vertexBindingQuad->setFormat(2, GL_FLOAT);
	m_quadArray->enable(0);
	m_quadArray->unbind();

	createShaderProgram("raytrace", {
			{ GL_VERTEX_SHADER,"./res/raytrace/raytrace-vs.glsl" },
			{ GL_FRAGMENT_SHADER,"./res/raytrace/raytrace-fs.glsl" },
		}, 
		{ "./res/raytrace/raytrace-globals.glsl" });
}

void RaytraceRenderer::display()
{
	// Save OpenGL state
	auto currentState = State::currentState();

	// retrieve/compute all necessary matrices and related properties
	const mat4 modelViewProjectionMatrix = viewer()->modelViewProjectionTransform();
	const mat4 inverseModelViewProjectionMatrix = inverse(modelViewProjectionMatrix);
    const mat4 modelLightMatrix = viewer()->modelLightTransform();
    const mat4 modelViewMatrix = viewer()->modelViewTransform();
    const mat4 inverseModelLightMatrix = inverse(modelLightMatrix);
    const mat4 inverseModelViewMatrix = inverse(modelViewMatrix);
    static bool sphereTemp = false;
    static bool boxTemp = false;
    static bool cylinderTemp = false;
    static bool planeTemp = false;
    static glm::vec3 sphereColorTemp = glm::vec3(1.f, 1.f, 1.f);
    static glm::vec3 boxColorTemp = glm::vec3(1.f, 1.f, 1.f);
    static glm::vec3 cylinderColorTemp = glm::vec3(1.f, 1.f, 1.f);
    static glm::vec3 planeColorTemp = glm::vec3(1.f, 1.f, 1.f);
    static glm::vec3 spherePositionTemp = glm::vec3(0.f, 0.f, 0.f);
    static glm::vec3 boxPositionTemp = glm::vec3(0.f, 0.f, 0.f);
    static glm::vec3 cylinderPositionTemp = glm::vec3(0.f, 0.f, 0.f);
    static glm::vec3 planeNormalTemp = glm::vec3(0.f, 0.f, 1.f);
    static float planePositionTemp = 1.f;
    static float cylinderHeightTemp = 1.f;
    static float boxScaleTemp = 0.5f;
    static float sphereScaleTemp = 0.5f;
    static glm::vec3 lightColorTemp = glm::vec3 (1.0, 1.0, 1.0);
    static bool celShadingTemp = false;
    static int levelOfCelShadingTemp = 5;
    static float lightIntensityFrontTemp = 1.0;
    
    auto shaderProgramRaytrace = shaderProgram("raytrace");

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
    if (ImGui::BeginMenu("Raytrace")) {
        if(ImGui::CollapsingHeader("Shapes")) {
            if(ImGui::CollapsingHeader("Box settings")) {
                box = ImGui::Checkbox("box", &boxTemp) ? boxTemp : box;
                boxPosition = ImGui::DragFloat3("position", (float*) &boxPositionTemp, 0.01f, -3.f, 3.f)
                        ? boxPositionTemp : boxPosition;
                boxColor = ImGui::ColorEdit3("color", (float*) &boxColorTemp) ? boxColorTemp : boxColor;
                boxScale = ImGui::DragFloat("scale", &boxScaleTemp, 0.01f, -3.f, 3.f) ? boxScaleTemp : boxScale;
            }
            if(ImGui::CollapsingHeader("Sphere settings")) {
                sphere = ImGui::Checkbox("sphere", &sphereTemp) ? sphereTemp : sphere;
                spherePosition = ImGui::DragFloat3("position", (float*) &spherePositionTemp, 0.01f, -3.f, 3.f)
                        ? spherePositionTemp : spherePosition;
                sphereColor = ImGui::ColorEdit3("color", (float*) &sphereColorTemp) ? sphereColorTemp : sphereColor;
                sphereScale = ImGui::DragFloat("scale", &sphereScaleTemp, 0.01f, -3.f, 3.f) ?
                        sphereScaleTemp: sphereScale;
            }
            if(ImGui::CollapsingHeader("Cylinder settings")) {
                cylinder = ImGui::Checkbox("cylinder", &cylinderTemp) ? cylinderTemp : cylinder;
                cylinderPosition = ImGui::DragFloat3("position", (float*) &cylinderPositionTemp, 0.01f, -3.f, 3.f)
                        ? cylinderPositionTemp : cylinderPosition;
                cylinderColor = ImGui::ColorEdit3("color", (float*) &cylinderColorTemp) ?
                        cylinderColorTemp : cylinderColor;
                cylinderHeight = ImGui::DragFloat("scale", &cylinderHeightTemp, 0.01f, -3.f, 3.f) ?
                        cylinderHeightTemp : cylinderHeight;

            }
            if(ImGui::CollapsingHeader("Plane Settings")) {
                plane = ImGui::Checkbox("plane", &planeTemp) ? planeTemp : plane;
                planeNormal = ImGui::DragFloat3("normal", (float*) &planeNormalTemp, 0.01f, -3.f, 3.f)
                        ? planeNormalTemp : planeNormal;
                planeColor = ImGui::ColorEdit3("color", (float*) &planeColorTemp) ?
                        planeColorTemp : planeColor;
                planePosition = ImGui::DragFloat("position", &planePositionTemp, 0.01f, -3.f, 3.f)
                        ? planePositionTemp: planePosition;

            }
        }
        if(ImGui::CollapsingHeader("Lighting")) {
            lightIntensityFront = ImGui::SliderFloat("Light intensity", &lightIntensityFrontTemp, 0.0f, 3.0f)
                    ? lightIntensityFrontTemp: lightIntensityFront;
            lightColor = ImGui::ColorEdit3("Light Color", (float *) &lightColorTemp) ? lightColorTemp : lightColor;
            celShading = ImGui::Checkbox("Cel shading Enabled", &celShadingTemp) ? celShadingTemp : celShading;
            levelOfCelShading = ImGui::SliderInt("Level of cel shading", &levelOfCelShadingTemp, 1, 10)
                    ? levelOfCelShadingTemp: levelOfCelShading;
        }
        ImGui::EndMenu();

    }
    vec4 worldCameraPosition = inverseModelViewMatrix * vec4(0.0f, 0.0f, 0.0f, 1.0f);
    vec4 worldLightPosition = inverseModelLightMatrix * vec4(0.0f, 0.0f, 0.0f, 1.0f);
	shaderProgramRaytrace->setUniform("modelViewProjectionMatrix", modelViewProjectionMatrix);
	shaderProgramRaytrace->setUniform("inverseModelViewProjectionMatrix", inverseModelViewProjectionMatrix);
    shaderProgramRaytrace->setUniform("worldCameraPosition", vec3(worldCameraPosition));
    shaderProgramRaytrace->setUniform("worldLightPosition", vec3(worldLightPosition));
    shaderProgramRaytrace->setUniform("lightIntensity", lightIntensityFront);
    shaderProgramRaytrace->setUniform("lightColor", lightColor);
    shaderProgramRaytrace->setUniform("celShading", celShading);
    shaderProgramRaytrace->setUniform("levelOfCelShading", levelOfCelShading);
	shaderProgramRaytrace->setUniform("sphereBool", sphere);
    shaderProgramRaytrace->setUniform("boxBool", box);
    shaderProgramRaytrace->setUniform("cylinderBool", cylinder);
    shaderProgramRaytrace->setUniform("planeBool", plane);
    shaderProgramRaytrace->setUniform("sphereColor", sphereColor);
    shaderProgramRaytrace->setUniform("boxColor", boxColor);
    shaderProgramRaytrace->setUniform("cylinderColor", cylinderColor);
    shaderProgramRaytrace->setUniform("planeColor", planeColor);
    shaderProgramRaytrace->setUniform("planeNormal", planeNormal);
    shaderProgramRaytrace->setUniform("spherePosition", spherePosition);
    shaderProgramRaytrace->setUniform("boxPosition", boxPosition);
    shaderProgramRaytrace->setUniform("cylinderPosition", cylinderPosition);
    shaderProgramRaytrace->setUniform("planePosition", planePosition);
    shaderProgramRaytrace->setUniform("cylinderHeight", cylinderHeight);
    shaderProgramRaytrace->setUniform("boxScale", boxScale);
    shaderProgramRaytrace->setUniform("sphereScale", sphereScale);






    m_quadArray->bind();
	shaderProgramRaytrace->use();
	// we are rendering a screen filling quad (as a tringle strip), so we can cast rays for every pixel
	m_quadArray->drawArrays(GL_TRIANGLE_STRIP, 0, 4);
	shaderProgramRaytrace->release();
	m_quadArray->unbind();


	// Restore OpenGL state (disabled to to issues with some Intel drivers)
	// currentState->apply();
}

RayTraceInfo RaytraceRenderer::getInfo() {
    return RayTraceInfo(sphere, box, cylinder, plane, sphereColor, boxColor, cylinderColor, planeColor, spherePosition,
                        boxPosition, cylinderPosition, planeNormal, planePosition, cylinderHeight, boxScale, sphereScale,
                        lightColor, celShading, levelOfCelShading, lightIntensityFront);
}

void RaytraceRenderer::setInfo(RayTraceInfo rayTraceInfo) {

    this->sphere = rayTraceInfo.sphere;
    this->box = rayTraceInfo.box;
    this->cylinder = rayTraceInfo.cylinder;
    this->plane = rayTraceInfo.plane;
    this->sphereColor = rayTraceInfo.sphereColor;
    this->boxColor = rayTraceInfo.boxColor;
    this->cylinderColor = rayTraceInfo.cylinderColor;
    this->planeColor = rayTraceInfo.planeColor;
    this->spherePosition = rayTraceInfo.spherePosition;
    this->boxPosition = rayTraceInfo.boxPosition;
    this->cylinderPosition = rayTraceInfo.cylinderPosition;
    this->planeNormal = rayTraceInfo.planeNormal;
    this->planePosition = rayTraceInfo.planePosition;
    this->cylinderHeight = rayTraceInfo.cylinderHeight;
    this->boxScale = rayTraceInfo.boxScale;
    this->sphereScale = rayTraceInfo.sphereScale;
    this->lightColor = rayTraceInfo.lightColor;
    this->celShading = rayTraceInfo.celShading;
    this->levelOfCelShading = rayTraceInfo.levelOfCelShading;
    this->lightIntensityFront = rayTraceInfo.lightIntensityFront;

}
