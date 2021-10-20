#version 400
#extension GL_ARB_shading_language_include : require
#include "/model-globals.glsl"

uniform vec3 worldCameraPosition;
uniform vec3 worldLightPosition;
uniform vec3 diffuseColor;
uniform sampler2D diffuseTexture;
uniform sampler2D ambientTexture;
uniform sampler2D specularTexture;
uniform sampler2D objectNormal;
uniform sampler2D tangentNormal;
uniform bool wireframeEnabled;
uniform vec4 wireframeLineColor;
uniform vec3 lightColor;
uniform float shininess;
uniform float lightIntensityFront;
uniform float lightIntensitySide;
uniform float lightIntensityBack;
uniform bool celShading;
uniform int levelOfCelShading;
uniform int mapping;
uniform bool bumps;
uniform vec2 akBumps;

in fragmentData
{
	vec3 position;
	vec3 normal;
	vec2 texCoord;
	noperspective vec3 edgeDistance;
	vec3 tangent;
	vec3 bitangent;
} fragment;

out vec4 fragColor;

vec3 getAmbientColor(vec3 objectColor){
	float ambientLight = 0.1;
	return ambientLight * lightColor * objectColor;
}

vec3 getDiffuse(vec3 lightDir, vec3 normal){
	vec3 norm = normalize(normal);
	float diff = max(dot(norm, lightDir), 0.0);
	return diff * lightColor;
}

vec3 getBlinnPhong(vec3 viewDir, vec3 lightDir, float specularStrength, vec3 normal){
	vec3 halfway = normalize(lightDir + viewDir);
	float normal_halfway = pow(max(dot(halfway, normal), 0), shininess);
	return specularStrength * normal_halfway * lightColor;
}

float getCelShading(float levels, vec3 lightPos, vec3 normal){
	vec3 lightDir = normalize(lightPos - fragment.position);
	float dotLightDirNormal = dot(lightDir, normalize(normal));
	float blackLine = mod(dotLightDirNormal*levels, 1.0);
	float level = ceil(dotLightDirNormal*levels);
	if ((blackLine < 0.04 || blackLine > 0.96) && level < levels){
		return 0.05;
	}

	return level / levels;
}

vec4 getShading(vec3 lightPos, float lightIntensity, vec3 normal){
	float specStrength = 0.7;
	vec3 objectColor = vec3(1.0, 1.0, 1.0);
	vec3 ambientColor = getAmbientColor(objectColor);
	vec3 lightDir = normalize(lightPos - fragment.position);
	vec3 diffuse = getDiffuse(lightDir, normal);
	vec3 viewDir = normalize(worldCameraPosition - fragment.position);
	vec3 specular = getBlinnPhong(viewDir, lightDir, specStrength, normal);
	return vec4((ambientColor + diffuse + specular) * objectColor * lightIntensity, 1.0);
}


void main()
{
	vec3 backlight = -worldCameraPosition;
	vec3 norm = texture(objectNormal, fragment.texCoord).rgb;
	vec3 normal = normalize(norm*2 - 1);
	if(mapping == 1){
		vec3 norm = texture(objectNormal, fragment.texCoord).rgb;
		vec3 normal = normalize(norm*2 - 1);
	}else if(mapping == 2){
		mat3 TBN = mat3(fragment.tangent, fragment.bitangent, normalize(fragment.normal));
		vec3 norm = texture(tangentNormal, fragment.texCoord).rgb * 2 - 1;
		vec3 normal = normalize(TBN * norm);
	}else{
		vec3 normal = fragment.normal;
	}
	vec3 fillingLightPosition = reflect(-worldLightPosition, -worldCameraPosition);

	vec4 frontShade = getShading(worldLightPosition, lightIntensityFront, normal);
	vec4 backShade = getShading(backlight, lightIntensityBack, normal);
	vec4 fillShade = getShading(fillingLightPosition, lightIntensitySide, normal);


	if (celShading){
		frontShade = frontShade * getCelShading(float(levelOfCelShading), worldLightPosition, normal);
		backShade = backShade * getCelShading(float(levelOfCelShading), backlight, normal);
		fillShade = fillShade * getCelShading(float(levelOfCelShading), fillingLightPosition, normal);

	}

	vec4 result = frontShade + backShade + fillShade;
	if (mapping == 0){
		result.rgb *= texture(diffuseTexture, fragment.texCoord).rgb * texture(specularTexture, fragment.texCoord).rgb  * texture(ambientTexture, fragment.texCoord).rgb;
	}
	if (wireframeEnabled)
	{
		float smallestDistance = min(min(fragment.edgeDistance[0],fragment.edgeDistance[1]),fragment.edgeDistance[2]);
		float edgeIntensity = exp2(-1.0*smallestDistance*smallestDistance);
		result.rgb = mix(result.rgb,wireframeLineColor.rgb,edgeIntensity*wireframeLineColor.a);
	}
	fragColor = result;
}