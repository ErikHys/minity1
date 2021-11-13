#version 400
#extension GL_ARB_shading_language_include : require
#include "/raytrace-globals.glsl"

uniform mat4 modelViewProjectionMatrix;
uniform mat4 inverseModelViewProjectionMatrix;
uniform bool sphereBool;
uniform bool boxBool;
uniform bool cylinderBool;
uniform bool planeBool;

in vec2 fragPosition;
out vec4 fragColor;

struct Sphere{
	vec3 center;
	float radius;
};
struct Box{
	vec3 maxxyz;
	vec3 minxyz;
};
struct Cylinder{
	vec3 center;
	float radius;
	float height;
};
struct Plane{
	vec3 normal;
	float distance;
};

float calcDepth(vec3 pos)
{
	float far = gl_DepthRange.far; 
	float near = gl_DepthRange.near;
	vec4 clip_space_pos = modelViewProjectionMatrix * vec4(pos, 1.0);
	float ndc_depth = clip_space_pos.z / clip_space_pos.w;
	return (((far - near) * ndc_depth) + near + far) / 2.0;
}

float collisionBox(vec3 origin, vec3 direction, Box box){
	vec3 boxMin = box.minxyz;
	vec3 boxMax = box.maxxyz;

	float t1 = (boxMin.x - origin.x) / direction.x;
	float t2 = (boxMax.x - origin.x) / direction.x;
	float t3 = (boxMin.y - origin.y) / direction.y;
	float t4 = (boxMax.y - origin.y) / direction.y;
	float t5 = (boxMin.z - origin.z) / direction.z;
	float t6 = (boxMax.z - origin.z) / direction.z;

	float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
	float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));
	if(tmax < 0){
		return -1;
	}
	if(tmin > tmax){
		return -1;
	}
	if(tmin < 0.0){
		return tmax;
	}
	return tmin;
}

float collisionSphere(vec3 origin, vec3 direction, Sphere sphere){
	// sphere = dot(p-c, p-c) = r^2
	// ray: p(t) = A +tB
	// combined = dot(A + tB - C, A + tB - C) = r^2
	// t^2 * dot(B,B) + 2t*dot(B, A-C) + dot(A-C,A-C) - r^2 = 0
	// at^2 b^t + c = 0
	// find b^2 - 4ac
	vec3 center = sphere.center;
	float radius = sphere.radius;
	float a = dot(direction, direction);
	float b = 2 * dot(direction, origin - center);
	float c = dot(origin - center, origin - center) - radius * radius;
	float discriminant = b*b - 4*a*c;
	if(discriminant < 0){
		return -1.0;
	}
	else {
		float sq = sqrt(discriminant);
		return (-b - sq) / (2.0*a);
	}
}

float collisionCylinder(vec3 origin, vec3 direction, Cylinder cylinder){
	vec3 center = cylinder.center;
	float radius = cylinder.radius;
	float height = cylinder.height;
	float a = dot(direction.xy, direction.xy);
	float b = 2 * dot(direction.xy, origin.xy - center.xy);
	float c = dot(origin.xy - center.xy, origin.xy - center.xy) - radius * radius;
	float discriminant = b*b - 4*a*c;
	if(discriminant < 0){
		return -1.0;
	}
	else {
		float sq = sqrt(discriminant);
		float t0 = (-b - sq) / (2.0*a);
		float t1 = (-b + sq) / (2.0*a);
		float collMin = origin.z + t0*direction.z;
		float collMax = origin.z + t1*direction.z;
		if (abs(collMin) < height){
			return t0;
		}
		if (abs(collMax) < height){
			return -1;
		}

		return -1;
	}
}
float collisionPlane(vec3 origin, vec3 direction, Plane plane){
	vec3 normal = plane.normal;
	float dirnorm = dot(direction, normal);
	float orinorm = dot(origin, normal);
	if(dirnorm == 0.0){
		return -1;
	}
	return (plane.distance - orinorm) / dirnorm;

}

float renderCylinderDisc(vec3 origin, vec3 direction, Cylinder cylinder){
	vec3 topNormal = vec3(0.0, 0.0, 1.0);
	vec3 bottomNormal = -topNormal;
	float dirTopNorm = dot(direction, topNormal);
	float oriTopNorm = dot(origin, topNormal);
	float dirBotNorm = dot(direction, bottomNormal);
	float oriBotNorm = dot(origin, bottomNormal);
	if (dirTopNorm == 0.0){
		return -1;
	}
	float topPlaneT = (cylinder.height - oriTopNorm) / dirTopNorm;
	float botPlaneT = (cylinder.height - oriBotNorm) / dirBotNorm;
	float t;
	int mult = 1;
	if(topPlaneT < 0 && botPlaneT >= 0){
		t = botPlaneT;
		mult = -1;
	}
	else if (botPlaneT < 0 && topPlaneT >= 0){
		t = topPlaneT;
	}else{
		t = min(topPlaneT, botPlaneT);
		mult = botPlaneT == t ? -1 : 1;
	}
	vec3 hit = origin + t*direction;
	float hitDist = dot(vec3(cylinder.center.xy, mult*cylinder.height) - hit, vec3(cylinder.center.xy, mult*cylinder.height) - hit);
	if(sqrt(hitDist) > cylinder.radius){
		return -1;
	}

	return t;

}

void main()
{
	vec4 near = inverseModelViewProjectionMatrix*vec4(fragPosition,-1.0,1.0);
	near /= near.w;
	vec4 far = inverseModelViewProjectionMatrix*vec4(fragPosition,1.0,1.0);
	far /= far.w;
	Sphere s = Sphere(vec3(-1.f), 0.8f);
	Box b = Box(vec3(0.5),vec3(-0.5));
	Cylinder c = Cylinder(vec3(0.7f), 0.8f, 1.0);
	Plane p = Plane(vec3(1., 0., 0.5), 0.7);

	// this is the setup for our viewing ray
	vec3 rayOrigin = near.xyz;
	vec3 rayDirection = normalize((far-near).xyz);
	float t = 10000.0;
	if(sphereBool){
		float tempT = collisionSphere(rayOrigin, rayDirection, s);
		t = tempT > 0 && tempT < t ? tempT : t;
	}
	if (boxBool){
		float tempT = collisionBox(rayOrigin, rayDirection, b);
		t = tempT > 0 && tempT < t ? tempT : t;

	}
	if(cylinderBool){
		float tempT = collisionCylinder(rayOrigin, rayDirection, c);
		t = tempT > 0 && tempT < t ? tempT : t;
		float newT = renderCylinderDisc(rayOrigin, rayDirection, c);
		if(newT > 0 && (newT <= t || t < 0)){
			t = newT;
		}
	}
	if(planeBool){
		float tempT = collisionPlane(rayOrigin, rayDirection, p);
		t = tempT > 0 && tempT < t ? tempT : t;

	}
	if(t == 10000.0) t = -1;



	fragColor = vec4(1.0);

	// using calcDepth, you can convert a ray position to an OpenGL z-value, so that intersections/occlusions with the
	// model geometry are handled correctly, e.g.: gl_FragDepth = calcDepth(nearestHit);
	// in case there is no intersection, you should get gl_FragDepth to 1.0, i.e., the output of the shader will be ignored

	gl_FragDepth = 1.0;
	if(t > 0){
		float newDepth = calcDepth(rayOrigin + t*rayDirection);
		if (newDepth < gl_FragDepth){
			gl_FragDepth = newDepth;
			fragColor = vec4(rayOrigin+t*rayDirection, 1.0);
		}
	}
}