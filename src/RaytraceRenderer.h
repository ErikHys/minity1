#pragma once
#include "Renderer.h"
#include "RayTraceInfo.h"
#include <memory>

#include <glm/glm.hpp>
#include <glbinding/gl/gl.h>
#include <glbinding/gl/enum.h>
#include <glbinding/gl/functions.h>

#include <globjects/VertexArray.h>
#include <globjects/VertexAttributeBinding.h>
#include <globjects/Buffer.h>
#include <globjects/Program.h>
#include <globjects/Shader.h>
#include <globjects/Framebuffer.h>
#include <globjects/Renderbuffer.h>
#include <globjects/Texture.h>
#include <globjects/base/File.h>
#include <globjects/TextureHandle.h>
#include <globjects/NamedString.h>
#include <globjects/base/StaticStringSource.h>

namespace minity
{
	class Viewer;

	class RaytraceRenderer : public Renderer
	{
	public:
		RaytraceRenderer(Viewer *viewer);
		virtual void display();
        RayTraceInfo getInfo();
        void setInfo(RayTraceInfo rayTraceInfo);

	private:
		std::unique_ptr<globjects::VertexArray> m_quadArray = std::make_unique<globjects::VertexArray>();
		std::unique_ptr<globjects::Buffer> m_quadVertices = std::make_unique<globjects::Buffer>();
        bool sphere = false;
        bool box = false;
        bool cylinder = false;
        bool plane = false;
        glm::vec3 sphereColor = glm::vec3(1.f, 1.f, 1.f);
        glm::vec3 boxColor = glm::vec3(1.f, 1.f, 1.f);
        glm::vec3 cylinderColor = glm::vec3(1.f, 1.f, 1.f);
        glm::vec3 planeColor = glm::vec3(1.f, 1.f, 1.f);
        glm::vec3 spherePosition = glm::vec3(0.f, 0.f, 0.f);
        glm::vec3 boxPosition = glm::vec3(0.f, 0.f, 0.f);
        glm::vec3 cylinderPosition = glm::vec3(0.f, 0.f, 0.f);
        glm::vec3 planeNormal = glm::vec3(0.f, 0.f, 1.f);
        float planePosition = 1.f;
        float cylinderHeight = 1.f;
        float boxScale = 0.5f;
        float sphereScale = 0.5f;
        glm::vec3 lightColor = glm::vec3 (1.0, 1.0, 1.0);
        bool celShading = false;
        int levelOfCelShading = 5;
        float lightIntensityFront = 1.0;
	};

}