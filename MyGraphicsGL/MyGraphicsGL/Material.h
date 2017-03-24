#ifndef MATERIAL_H
#define MATERIAL_H
#include <glm\glm.hpp>

struct Material
{	
	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;
	glm::vec3 Shininess;
};

#endif