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

using namespace minity;
using namespace gl;
using namespace glm;
using namespace globjects;

ModelRenderer::ModelRenderer(Viewer* viewer) : Renderer(viewer)
{
	m_lightVertices->setStorage(std::array<vec3, 1>({ vec3(0.0f) }), GL_NONE_BIT);
	auto lightVertexBinding = m_lightArray->binding(0);
	lightVertexBinding->setBuffer(m_lightVertices.get(), 0, sizeof(vec3));
	lightVertexBinding->setFormat(3, GL_FLOAT);
	m_lightArray->enable(0);
	m_lightArray->unbind();

	createShaderProgram("model-base", {
		{ GL_VERTEX_SHADER,"./res/model/model-base-vs.glsl" },
		{ GL_GEOMETRY_SHADER,"./res/model/model-base-gs.glsl" },
		{ GL_FRAGMENT_SHADER,"./res/model/model-base-fs.glsl" },
		}, 
		{ "./res/model/model-globals.glsl" });

	createShaderProgram("model-light", {
		{ GL_VERTEX_SHADER,"./res/model/model-light-vs.glsl" },
		{ GL_FRAGMENT_SHADER,"./res/model/model-light-fs.glsl" },
		}, { "./res/model/model-globals.glsl" });
}

void ModelRenderer::display()
{
	// Save OpenGL state
	auto currentState = State::currentState();

	// retrieve/compute all necessary matrices and related properties
	const mat4 viewMatrix = viewer()->viewTransform();
	const mat4 inverseViewMatrix = inverse(viewMatrix);
	const mat4 modelViewMatrix = viewer()->modelViewTransform();
	const mat4 inverseModelViewMatrix = inverse(modelViewMatrix);
	const mat4 modelLightMatrix = viewer()->modelLightTransform();
	const mat4 inverseModelLightMatrix = inverse(modelLightMatrix);
	const mat4 modelViewProjectionMatrix = viewer()->modelViewProjectionTransform();
	const mat4 inverseModelViewProjectionMatrix = inverse(modelViewProjectionMatrix);
	const mat4 projectionMatrix = viewer()->projectionTransform();
	const mat4 inverseProjectionMatrix = inverse(projectionMatrix);
	const mat3 normalMatrix = mat3(transpose(inverseModelViewMatrix));
	const mat3 inverseNormalMatrix = inverse(normalMatrix);
	const vec2 viewportSize = viewer()->viewportSize();
    const float explosionDist = viewer()->getExplosionDist();


    auto shaderProgramModelBase = shaderProgram("model-base");

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	viewer()->scene()->model()->vertexArray().bind();

	const std::vector<Group> & groups = viewer()->scene()->model()->groups();
	const std::vector<Material> & materials = viewer()->scene()->model()->materials();

	static std::vector<bool> groupEnabled(groups.size(), false);
	static bool wireframeEnabled = false;
	static bool lightSourceEnabled = true;
	static vec4 wireframeLineColor = vec4(1.0f);
    static float lightIntensityFront = 1.0;
    static float lightIntensityBack = 0.0f;
    static float lightIntensitySide = 0.0f;
    static vec3 lightColor = vec3 (1.0, 1.0, 1.0);
    static bool celShading = false;
    static int levelOfCelShading = 5;
    static int mapping = 0;
    static bool bumps = false;
    static float amplitude = 1.f;
    static float frequency = 128.f;
    static bool diffTextures = false;
    static bool ambTextures = false;
    static bool specTextures = false;



    if (ImGui::BeginMenu("Model"))
	{
		ImGui::Checkbox("Wireframe Enabled", &wireframeEnabled);
		ImGui::Checkbox("Light Source Enabled", &lightSourceEnabled);
        ImGui::ColorEdit3("Light Color", (float*) &lightColor);
        ImGui::Checkbox("Cel shading Enabled", &celShading);
        ImGui::SliderInt("Level of cel shading", &levelOfCelShading, 1, 10);
        if(ImGui::CollapsingHeader("Light intensity")){
            ImGui::SliderFloat("Light intensity front", &lightIntensityFront, 0.0f, 2.0f);
            ImGui::SliderFloat("Light intensity back", &lightIntensityBack, 0.0f, 2.0f);
            ImGui::SliderFloat("Light intensity side", &lightIntensitySide, 0.0f, 2.0f);
        }
        if(ImGui::CollapsingHeader("Textures")) {
            ImGui::Checkbox("Ambient texture", &ambTextures);
            ImGui::Checkbox("Diffuse texture", &diffTextures);
            ImGui::Checkbox("Specular texture", &specTextures);
        }

        ImGui::SliderInt("Which mapping, standard, object, tangent", &mapping, 0, 2);
        if(ImGui::CollapsingHeader("Bumps")){
            ImGui::Checkbox("Bump mapping", &bumps);
            ImGui::SliderFloat("Amplitude", &amplitude, 0.0, 2.0);
            ImGui::SliderFloat("Frequency", &frequency, 1.0, 128.0);
        }





        if (wireframeEnabled)
		{
			if (ImGui::CollapsingHeader("Wireframe"))
			{
				ImGui::ColorEdit4("Line Color", (float*)&wireframeLineColor, ImGuiColorEditFlags_AlphaBar);
			}
		}

		if (ImGui::CollapsingHeader("Groups"))
		{
			for (uint i = 0; i < groups.size(); i++)
			{
				bool checked = groupEnabled.at(i);
				ImGui::Checkbox(groups.at(i).name.c_str(), &checked);
				groupEnabled[i] = checked;
			}

		}

		ImGui::EndMenu();
	}

	vec4 worldCameraPosition = inverseModelViewMatrix * vec4(0.0f, 0.0f, 0.0f, 1.0f);
	vec4 worldLightPosition = inverseModelLightMatrix * vec4(0.0f, 0.0f, 0.0f, 1.0f);
	shaderProgramModelBase->setUniform("modelViewProjectionMatrix", modelViewProjectionMatrix);
	shaderProgramModelBase->setUniform("viewportSize", viewportSize);
	shaderProgramModelBase->setUniform("worldCameraPosition", vec3(worldCameraPosition));
	shaderProgramModelBase->setUniform("worldLightPosition", vec3(worldLightPosition));
	shaderProgramModelBase->setUniform("wireframeEnabled", wireframeEnabled);
	shaderProgramModelBase->setUniform("wireframeLineColor", wireframeLineColor);
    shaderProgramModelBase->setUniform("lightIntensityFront", lightIntensityFront);
    shaderProgramModelBase->setUniform("lightIntensityBack", lightIntensityBack);
    shaderProgramModelBase->setUniform("lightIntensitySide", lightIntensitySide);
    shaderProgramModelBase->setUniform("lightColor", lightColor);
    shaderProgramModelBase->setUniform("celShading", celShading);
    shaderProgramModelBase->setUniform("levelOfCelShading", levelOfCelShading);
    shaderProgramModelBase->setUniform("mapping", mapping);
    shaderProgramModelBase->setUniform("bumps", bumps);
    shaderProgramModelBase->setUniform("modelMidPoint", viewer()->scene()->model()->midpoint());
    shaderProgramModelBase->setUniform("explosionDist", explosionDist);
    shaderProgramModelBase->setUniform("amplitude", amplitude);
    shaderProgramModelBase->setUniform("frequency", frequency);
    shaderProgramModelBase->setUniform("diffuseTextureActivate", diffTextures);
    shaderProgramModelBase->setUniform("ambientTextureActivate", ambTextures);
    shaderProgramModelBase->setUniform("specularTextureActivate", specTextures);





    shaderProgramModelBase->use();

	for (uint i = 0; i < groups.size(); i++)
	{
		if (groupEnabled.at(i))
		{
			const Material & material = materials.at(groups.at(i).materialIndex);

			shaderProgramModelBase->setUniform("ambientLightColor", material.ambient);
            shaderProgramModelBase->setUniform("diffuseLightColor", material.diffuse);
            shaderProgramModelBase->setUniform("specularLightColor", material.specular);
            shaderProgramModelBase->setUniform("shininess", material.shininess);

            if (material.diffuseTexture)
			{
                shaderProgramModelBase->setUniform("diffuseTexture", 0);
				material.diffuseTexture->bindActive(0);
			}
            if (material.ambientTexture)
            {
                shaderProgramModelBase->setUniform("ambientTexture", 1);
                material.ambientTexture->bindActive(1);
            }
            if (material.specularTexture)
            {
                shaderProgramModelBase->setUniform("specularTexture", 2);
                material.specularTexture->bindActive(2);
            }
            if (material.objectNormal)
            {
                shaderProgramModelBase->setUniform("objectNormal", 3);
                material.objectNormal->bindActive(3);
            }
            if (material.tangentNormal)
            {
                shaderProgramModelBase->setUniform("tangentNormal", 4);
                material.tangentNormal->bindActive(4);
            }
            shaderProgramModelBase->setUniform("groupMidPoint", groups[i].midpoint);

            viewer()->scene()->model()->vertexArray().drawElements(GL_TRIANGLES, groups.at(i).count(), GL_UNSIGNED_INT, (void*)(sizeof(GLuint)*groups.at(i).startIndex));

            if (material.tangentNormal)
            {
                material.tangentNormal->unbind();
            }
            if (material.objectNormal)
            {
                material.objectNormal->unbind();
            }
            if (material.specularTexture)
            {
                material.specularTexture->unbind();
            }
            if (material.ambientTexture)
            {
                material.ambientTexture->unbind();
            }
            if (material.ambientTexture)
            {
                material.ambientTexture->unbind();
            }
            if (material.ambientTexture)
            {
                material.ambientTexture->unbind();
            }
			if (material.diffuseTexture)
			{
				material.diffuseTexture->unbind();
			}




		}
	}

	shaderProgramModelBase->release();

	viewer()->scene()->model()->vertexArray().unbind();


	if (lightSourceEnabled)
	{
		auto shaderProgramModelLight = shaderProgram("model-light");
		shaderProgramModelLight->setUniform("modelViewProjectionMatrix", modelViewProjectionMatrix * inverseModelLightMatrix);
		shaderProgramModelLight->setUniform("viewportSize", viewportSize);
        shaderProgramModelLight->setUniform("lightColor", lightColor);

		glEnable(GL_PROGRAM_POINT_SIZE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_FALSE);

		m_lightArray->bind();

		shaderProgramModelLight->use();
		m_lightArray->drawArrays(GL_POINTS, 0, 1);
		shaderProgramModelLight->release();

		m_lightArray->unbind();

		glDisable(GL_PROGRAM_POINT_SIZE);
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}

	// Restore OpenGL state (disabled to to issues with some Intel drivers)
	// currentState->apply();
}