#ifndef VERTEX_H
#define VERTEX_H
#include <glm\glm.hpp>
#include "Material.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 surfaceNormal;
	glm::vec3 ambientMaterial;
	glm::vec3 diffuseMaterial;
	glm::vec3 specularMaterial;
	glm::vec3 shininessMaterial;
};

#endif