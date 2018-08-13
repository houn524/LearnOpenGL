#pragma once
#ifndef __GRASS_H__
#define __GRASS_H__

#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>
#include <vector>

struct vertex
{
	glm::vec3 position;
	glm::vec2 texCoord;
};

class Grass
{
public:
	Grass();
	~Grass();
	void Draw(Shader shader, std::vector<glm::vec3> *positions = NULL);

private:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
	unsigned int textureID;

	void setupGrass();
};

#endif