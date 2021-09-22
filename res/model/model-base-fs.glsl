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

vec3 getAmbientColor(vec3 objectColor){
	float ambientLight = 0.3;
	return ambientLight * lightColor * objectColor;
}

vec3 getDiffuse(vec3 lightDir){
	vec3 norm = normalize(fragment.normal);
	float diff = max(dot(norm, lightDir), 0.0);
	return diff * lightColor;
}

vec3 getBlinnPhong(vec3 viewDir, vec3 lightDir, float specularStrength){
	vec3 halfway = normalize(lightDir + viewDir);
	float normal_halfway =pow(max(dot(halfway, fragment.normal), 0), 68);
	return specularStrength * normal_halfway * lightColor;
}

void main()
{
	float specStrength = 0.7;
	vec3 objectColor = vec3(0.5, 0.0, 0.5);
	vec3 ambientColor = getAmbientColor(objectColor);
	vec3 lightDir = normalize(worldLightPosition - fragment.position);
	vec3 diffuse = getDiffuse(lightDir);
	vec3 viewDir = normalize(worldCameraPosition - fragment.position);
	vec3 specular = getBlinnPhong(viewDir, lightDir, specStrength);
	vec4 result = vec4((ambientColor + diffuse + specular) * objectColor,1.0);

	if (wireframeEnabled)
	{
		float smallestDistance = min(min(fragment.edgeDistance[0],fragment.edgeDistance[1]),fragment.edgeDistance[2]);
		float edgeIntensity = exp2(-1.0*smallestDistance*smallestDistance);
		result.rgb = mix(result.rgb,wireframeLineColor.rgb,edgeIntensity*wireframeLineColor.a);
	}

	fragColor = result;
}