#pragma once
#include <glm/glm.hpp>

namespace GPU {
struct BVHNode
{
	glm::vec3 minBounds;
	int leftChild;
	glm::vec3 maxBounds;
	int rightChild;
	int primitiveIndex;
	float pad[3];
};

//	For Triangle:
//	vertexData = vertices, three points in space.
//
//	For Sphere
//	first vec3 is center position,
//	second vec3.x is radius
// 
//  Type: 0 is triangle, 1 is sphere
struct Primitive
{
	glm::vec4 vertexData[3];
	int materialId;
	int type;
	float pad[2];
};


struct Material
{
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::vec4 mirror;
	float phong_exponent;
	int isMirror;
	float pad[2];
};

struct Light
{
	glm::vec3 position;
	int lightsSize;
	glm::vec3 intensity;
	float pad2;
};
}