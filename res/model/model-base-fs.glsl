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
uniform float shininess;
uniform float lightIntensityFront;
uniform float lightIntensitySide;
uniform float lightIntensityBack;
uniform bool celShading;
uniform int levelOfCelShading;

in fragmentData
{
	vec3 position;
	vec3 normal;
	vec2 texCoord;
	noperspective vec3 edgeDistance;
} fragment;

out vec4 fragColor;

vec3 getAmbientColor(vec3 objectColor){
	float ambientLight = 0.1;
	return ambientLight * lightColor * objectColor;
}

vec3 getDiffuse(vec3 lightDir){
	vec3 norm = normalize(fragment.normal);
	float diff = max(dot(norm, lightDir), 0.0);
	return diff * lightColor;
}

vec3 getBlinnPhong(vec3 viewDir, vec3 lightDir, float specularStrength){
	vec3 halfway = normalize(lightDir + viewDir);
	float normal_halfway = pow(max(dot(halfway, fragment.normal), 0), shininess);
	return specularStrength * normal_halfway * lightColor;
}

float getCelShading(float levels, vec3 lightPos){
	vec3 lightDir = normalize(lightPos - fragment.position);
	float dotLightDirNormal = dot(lightDir, normalize(fragment.normal));
	float blackLine = mod(dotLightDirNormal*levels, 1.0);
	float level = ceil(dotLightDirNormal*levels);
	if ((blackLine < 0.04 || blackLine > 0.96) && level < levels){
		return 0.05;
	}

	return level / levels;
}

vec4 getShading(vec3 lightPos, float lightIntensity){
	float specStrength = 0.7;
	vec3 objectColor = vec3(1.0, 1.0, 1.0);
	vec3 ambientColor = getAmbientColor(objectColor);
	vec3 lightDir = normalize(lightPos - fragment.position);
	vec3 diffuse = getDiffuse(lightDir);
	vec3 viewDir = normalize(worldCameraPosition - fragment.position);
	vec3 specular = getBlinnPhong(viewDir, lightDir, specStrength);
	return vec4((ambientColor + diffuse + specular) * objectColor * lightIntensity, 1.0);
}

void main()
{
	vec3 backlight = -worldCameraPosition;
	vec3 fillingLightPosition = reflect(-worldLightPosition, -worldCameraPosition);

	vec4 frontShade = getShading(worldLightPosition, lightIntensityFront);
	vec4 backShade = getShading(backlight, lightIntensityBack);
	vec4 fillShade = getShading(fillingLightPosition, lightIntensitySide);


	if (celShading){
		frontShade = frontShade * getCelShading(float(levelOfCelShading), worldLightPosition);
		backShade = backShade * getCelShading(float(levelOfCelShading), backlight);
		fillShade = fillShade * getCelShading(float(levelOfCelShading), fillingLightPosition);

	}

	vec4 result = frontShade + backShade + fillShade;

	if (wireframeEnabled)
	{
		float smallestDistance = min(min(fragment.edgeDistance[0],fragment.edgeDistance[1]),fragment.edgeDistance[2]);
		float edgeIntensity = exp2(-1.0*smallestDistance*smallestDistance);
		result.rgb = mix(result.rgb,wireframeLineColor.rgb,edgeIntensity*wireframeLineColor.a);
	}
	fragColor = result;
}