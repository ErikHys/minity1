#version 400
#extension GL_ARB_shading_language_include : require
#include "/model-globals.glsl"

uniform mat4 modelViewProjectionMatrix;
uniform vec3 modelMidPoint;
uniform vec3 groupMidPoint;
uniform float explosionDist;

in vec3 position;
in vec3 normal;
in vec2 texCoord;

out vertexData
{
	vec3 position;
	vec3 normal;
	vec2 texCoord;
} vertex;

void main()
{
	vec3 newPosition = position + (groupMidPoint - modelMidPoint) * explosionDist;
	vec4 pos = modelViewProjectionMatrix*vec4(newPosition,1.0);

	vertex.position = position; 
	vertex.normal = normal;
	vertex.texCoord = texCoord;	
	
	gl_Position = pos;
}