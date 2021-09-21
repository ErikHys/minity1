#version 400
#extension GL_ARB_shading_language_include : require
#include "/model-globals.glsl"

uniform vec3 worldCameraPosition;
uniform vec3 worldLightPosition;
uniform vec3 diffuseColor;
uniform sampler2D diffuseTexture;
uniform bool wireframeEnabled;
uniform vec4 wireframeLineColor;
uniform vec3 lightColor;

in fragmentData
{
	vec3 position;
	vec3 normal;
	vec2 texCoord;
	noperspective vec3 edgeDistance;
} fragment;

out vec4 fragColor;

void main()
{
	vec3 objectColor = vec3(1.0,1.0,1.0);
	float ambientLight = 0.1;
	vec3 ambientColor = ambientLight * lightColor * objectColor;
	vec3 lightDir = worldLightPosition - fragment.position;
	vec3 viewDir = worldCameraPosition - fragment.position;
	vec3 halfway = normalize(lightDir + viewDir);
	float dist = length(lightDir);
	dist = dist * dist;
	lightDir = normalize(lightDir);
	float normal_halfway = dot(halfway, fragment.normal);

	vec4 result = vec4(ambientColor + (objectColor * normal_halfway)/dist ,1.0);

	if (wireframeEnabled)
	{
		float smallestDistance = min(min(fragment.edgeDistance[0],fragment.edgeDistance[1]),fragment.edgeDistance[2]);
		float edgeIntensity = exp2(-1.0*smallestDistance*smallestDistance);
		result.rgb = mix(result.rgb,wireframeLineColor.rgb,edgeIntensity*wireframeLineColor.a);
	}

	fragColor = result;
}