#version 450
#extension GL_ARB_shading_language_include : require
#include "/model-globals.glsl"

uniform vec3 worldCameraPosition;
uniform vec3 worldLightPosition;
uniform sampler2D diffuseTexture;
uniform sampler2D ambientTexture;
uniform sampler2D specularTexture;
uniform sampler2D objectNormal;
uniform sampler2D tangentNormal;
uniform bool wireframeEnabled;
uniform vec4 wireframeLineColor;
uniform vec3 lightColor;
uniform vec3 ambientLightColor;
uniform vec3 diffuseLightColor;
uniform vec3 specularLightColor;
uniform float shininess;
uniform float lightIntensityFront;
uniform float lightIntensitySide;
uniform float lightIntensityBack;
uniform bool celShading;
uniform int levelOfCelShading;
uniform int mapping;
uniform bool bumps;
uniform float amplitude;
uniform float frequency;
uniform bool diffuseTextureActivate;
uniform bool ambientTextureActivate;
uniform bool specularTextureActivate;


in fragmentData
{
	vec3 position;
	vec3 normal;
	vec2 texCoord;
	noperspective vec3 edgeDistance;
	vec3 tangent;
	vec3 bitangent;
	mat3 TBN;
} fragment;

out vec4 fragColor;

vec3 getAmbientColor(){
	float ambientLight = 0.1;
	return ambientLight * lightColor * ambientLightColor;
}

vec3 getDiffuse(vec3 lightDir, vec3 normal){
	vec3 norm = normal;
	float diff = max(dot(norm, lightDir), 0.0);
	return diff * lightColor * diffuseLightColor;
}

vec3 getBlinnPhong(vec3 viewDir, vec3 lightDir, float specularStrength, vec3 normal){
	vec3 halfway = normalize(lightDir + viewDir);
	float normal_halfway = pow(max(dot(halfway, normal), 0), shininess);
	return specularStrength * normal_halfway * specularLightColor * lightColor;
}

float getCelShading(float levels, vec3 lightPos, vec3 normal){
	vec3 lightDir = normalize(lightPos - fragment.position);
	float dotLightDirNormal = max(dot(lightDir, normal), 0);
	float blackLine = mod(max(dotLightDirNormal*levels, 0), 1.0);
	float level = ceil(max(dotLightDirNormal, 0)*levels);
	if ((blackLine < 0.04 || blackLine > 0.96) && level < levels){
		return 0.05;
	}

	return level / levels;
}

vec4 getShading(vec3 lightPos, float lightIntensity, vec3 normal){
	float specStrength = 0.7;
	vec3 ambientColor = getAmbientColor();
	vec3 lightDir = normalize(lightPos - fragment.position);
	vec3 diffuse = getDiffuse(lightDir, normal);
	vec3 viewDir = normalize(worldCameraPosition - fragment.position);
	vec3 specular = getBlinnPhong(viewDir, lightDir, specStrength, normal);
	return vec4((0.15 * ambientColor + 0.4 * diffuse +  0.45 * specular) * lightColor * lightIntensity, 1.0);
}

float getProcedualBumps(float amp, float k, vec2 txc){
	float sinU = sin(k * txc.x);
	float sinV = sin(k * txc.y);
	return amp * pow(sinU, 2) * pow(sinV, 2);
}

void main()
{
	vec3 backlight = -worldCameraPosition;
	vec3 normal = normalize(fragment.normal);


	vec3 norm = normalize(texture(tangentNormal, fragment.texCoord).rgb * 2.0 - 1.0);
	vec3 bitangent = normalize(cross(normal, fragment.tangent));
	vec3 tangent = normalize(cross(bitangent, normal));
	mat3 TBN = mat3(tangent, bitangent, normal);

	if (bumps){
		//Does not work...

		float bu = dFdxFine(getProcedualBumps(amplitude, frequency, fragment.texCoord));
		float bv = dFdyFine(getProcedualBumps(amplitude, frequency, fragment.texCoord));
		norm = norm + bu * bitangent + bv * tangent;
	}
	if(mapping == 1){
		normal = normalize(norm*2 - 1);
	}else if(mapping == 2){
		//Only works paritally
		normal = normalize(TBN * norm);
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

	if(diffuseTextureActivate){
		result.rgb *= texture(diffuseTexture, fragment.texCoord).rgb;
	}
	if(ambientTextureActivate){
		result.rgb *= texture(ambientTexture, fragment.texCoord).rgb;
	}
	if(specularTextureActivate){
		result.rgb *= texture(specularTexture, fragment.texCoord).rgb;
	}
	if (wireframeEnabled)
	{
		float smallestDistance = min(min(fragment.edgeDistance[0],fragment.edgeDistance[1]),fragment.edgeDistance[2]);
		float edgeIntensity = exp2(-1.0*smallestDistance*smallestDistance);
		result.rgb = mix(result.rgb,wireframeLineColor.rgb,edgeIntensity*wireframeLineColor.a);
	}
	fragColor = result;
}