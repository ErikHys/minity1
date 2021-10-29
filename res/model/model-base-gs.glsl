#version 400
#extension GL_ARB_shading_language_include : require
#include "/model-globals.glsl"

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform vec2 viewportSize;

in vertexData
{
	vec3 position;
	vec3 normal;
	vec2 texCoord;
} vertices[];

out fragmentData
{
	vec3 position;
	vec3 normal;
	vec2 texCoord;
	noperspective vec3 edgeDistance;
	vec3 tangent;
	vec3 bitangent;
	mat3 TBN;
} fragment;

void main(void)
{
	vec2 p[3];
	vec2 v[3];

	for (int i=0;i<3;i++)
		p[i] = 0.5 * viewportSize *  gl_in[i].gl_Position.xy/gl_in[i].gl_Position.w;

	v[0] = p[2]-p[1];
	v[1] = p[2]-p[0];
	v[2] = p[1]-p[0];

	float area = abs(v[1].x*v[2].y - v[1].y * v[2].x);

	vec3 edge1 = vertices[1].position - vertices[0].position;
	vec3 edge2 = vertices[2].position - vertices[0].position;
	vec2 delta1 = vertices[1].texCoord - vertices[0].texCoord;
	vec2 delta2 = vertices[2].texCoord - vertices[0].texCoord;
	float f = 1.0f / (delta1.x * delta2.y - delta2.x * delta1.y);
	vec3 tangent = vec3(0.0);
	tangent.x = f * (delta2.y * edge1.x - delta1.y * edge2.x);
	tangent.y = f * (delta2.y * edge1.y - delta1.y * edge2.y);
	tangent.z = f * (delta2.y * edge1.z - delta1.y * edge2.z);
	vec3 bitangent = vec3(0.0);
	bitangent.x = f * (-delta2.y * edge1.x + delta1.y * edge2.x);
	bitangent.y = f * (-delta2.y * edge1.y + delta1.y * edge2.y);
	bitangent.z = f * (-delta2.y * edge1.z + delta1.y * edge2.z);
	bitangent = normalize(bitangent);
	tangent = normalize(tangent);

	for (int i=0;i<3;i++)
	{
		gl_Position = gl_in[i].gl_Position;
		fragment.position = vertices[i].position;
		fragment.normal = vertices[i].normal;
		fragment.texCoord = vertices[i].texCoord;
		fragment.tangent = tangent;
		fragment.bitangent = bitangent;
		fragment.TBN = mat3(tangent, bitangent, vertices[i].normal);
		
		vec3 ed = vec3(0.0);
		ed[i] = area / length(v[i]);
		fragment.edgeDistance = ed;

		EmitVertex();
	}

	EndPrimitive();
}