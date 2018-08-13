#include "Grass.h"

Grass::Grass()
{
	setupGrass();
}

Grass::~Grass()
{
	glDeleteBuffers(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Grass::setupGrass()
{
	vertex vertices[4] = {
		glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec2(0.0f, 1.0f),
		glm::vec3(0.5f, 0.5f, 0.0f), glm::vec2(1.0f, 1.0f),
		glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(0.0f, 0.0f),
		glm::vec3(0.5f, -0.5f, 0.0f), glm::vec2(1.0f, 0.0f)
	};

	unsigned int indices[6] = {
		0, 1, 2,
		1, 2, 3
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * 4, vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, texCoord));

	glBindVertexArray(0);

	glGenTextures(1, &textureID);

	int width, height;
	unsigned char* data = stbi_load("resources/textures/blending_transparent_window.png", &width, &height, 0, 0);
	if (data)
	{
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << "resources/textures/blending_transparent_window.png" << std::endl;
		stbi_image_free(data);
	}
}

void Grass::Draw(Shader shader, std::vector<glm::vec3> *positions)
{
	if (positions != NULL)
	{
		glm::mat4 model = glm::mat4(1.0f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);

		shader.setInt("diffuse", 0);

		glBindVertexArray(VAO);
		for(auto pos : *positions)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, pos);
			shader.setMat4("model", model);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
		glBindVertexArray(0);
	}
}