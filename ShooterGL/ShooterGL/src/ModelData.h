#pragma once
#include <vector>
#include "Renderables/Mesh.h"

class MeshData
{
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};

class ModelData
{
public:
	unsigned int VAO, EBO, VBO;
	std::vector<MeshData> m_meshData;
};

//struct Vertex {
//	glm::vec3 Position;
//	glm::vec3 Normal;
//	glm::vec2 TexCoords;
//	glm::vec3 Tangent;
//	glm::vec3 Bitangent;
//};