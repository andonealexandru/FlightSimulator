#pragma once
#include <glm.hpp>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 texcoord;
	glm::vec3 normal;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	int colorID = -1;
};