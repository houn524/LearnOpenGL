#pragma once
#ifndef __MESH_H__
#define __MESH_H__

#include "Shader.h"
#include <glm/glm.hpp>
#include <vector>
#include <iostream>

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture
{
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void Draw(Shader shader);
private:
	unsigned int VAO, VBO, EBO;
	
	void setupMesh();
};

#endif