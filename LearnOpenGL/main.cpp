#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Grass.h"

#include <iostream>
#include <map>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// ¼¼ÆÃ
const unsigned int SCR_WIDTH = 3840;
const unsigned int SCR_HEIGHT = 2160;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float ratio = 0.2f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

float quadVertices[] = {
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	1.0f, -1.0f,  1.0f, 0.0f,
	1.0f,  1.0f,  1.0f, 1.0f
};

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to Initialize GLAD" << std::endl;
		return -1;
	}

	Shader lightingShader("Shaders/Model.vertexshader", "Shaders/Model.fragmentshader");
	Shader lampShader("Shaders/lampVertex.glsl", "Shaders/lampFragment.glsl");
	Shader shaderSingleColor("Shaders/shaderSingleColor.vertexshader", "Shaders/shaderSingleColor.fragmentshader");
	Shader grassShader("Shaders/grass.vertexshader", "Shaders/grass.fragmentshader");
	Shader framebufferShader("Shaders/framebuffer.vertexshader", "Shaders/framebuffer.fragmentshader");
	
	

	Model ourModel("resources/Models/old_house_obj/house_01.obj");// Avent / Avent.obj");// nanosuit / nanosuit.obj");
	Model bulbModel("resources/Models/OBJ - Poly/Lightbulb_General_Poly_OBJ.obj");

	lightingShader.use();
	lightingShader.setVec3("dirLight.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
	lightingShader.setVec3("dirLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	lightingShader.setVec3("dirLight.specular", glm::vec3(0.9f, 0.9f, 0.9f));

	lightingShader.setFloat("pointLights[0].constant", 1.0f);
	lightingShader.setFloat("pointLights[0].linear", 0.045f);
	lightingShader.setFloat("pointLights[0].quadratic", 0.0075f);
	lightingShader.setVec3("pointLights[0].ambient", glm::vec3(0.1f, 0.1f, 0.1f));
	lightingShader.setVec3("pointLights[0].diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	lightingShader.setVec3("pointLights[0].specular", glm::vec3(0.9f, 0.9f, 0.9f));
	lightingShader.setFloat("pointLights[1].constant", 1.0f);
	lightingShader.setFloat("pointLights[1].linear", 0.045f);
	lightingShader.setFloat("pointLights[1].quadratic", 0.0075f);
	lightingShader.setVec3("pointLights[1].ambient", glm::vec3(0.1f, 0.1f, 0.1f));
	lightingShader.setVec3("pointLights[1].diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	lightingShader.setVec3("pointLights[1].specular", glm::vec3(0.9f, 0.9f, 0.9f));

	lightingShader.setFloat("material.shininess", 64.0f);
	
	Grass grass;
	std::map<float, glm::vec3> sorted;
	
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));

	glBindVertexArray(0);

	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	unsigned int texColorBuffer;
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	while (!glfwWindowShouldClose(window)) {
		
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE);
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();

		glm::mat4 model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 0.0, -10.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::scale(model, glm::vec3(0.0f, 0.1f, 0.1f));
		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		lampShader.setMat4("model", model);
		bulbModel.Draw(lampShader);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 10.0, -20.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		lampShader.setMat4("model", model);
		bulbModel.Draw(lampShader);

		lightingShader.use();
		
		lightingShader.setMat4("projection", projection);
		
		lightingShader.setMat4("view", view);

		glm::mat4 houseModelMat = glm::mat4(1.0f);
		houseModelMat = glm::translate(houseModelMat, glm::vec3(0.0f, 0.0f, -10.0f));
		houseModelMat = glm::scale(houseModelMat, glm::vec3(0.1f, 0.1f, 0.1f));
		lightingShader.setMat4("model", houseModelMat);

		lightingShader.setVec3("dirLight.direction", glm::mat3(view) * glm::vec3(-1.0f, -1.0f, -1.0f));
		lightingShader.setVec3("pointLights[0].position", glm::vec3(view * glm::vec4(-10.0f, 0.0f, -10.0f, 1.0f)));
		lightingShader.setVec3("pointLights[1].position", glm::vec3(view * glm::vec4(0.0f, 10.0f, -20.0f, 1.0f)));

		ourModel.Draw(lightingShader);

		glDisable(GL_CULL_FACE);
		grassShader.use();
		grassShader.setMat4("projection", projection);
		grassShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		std::vector<glm::vec3> positions;
		positions.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
		positions.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
		positions.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
		positions.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
		positions.push_back(glm::vec3(0.5f, 0.0f, -0.6f));

		sorted.clear();
		for (auto pos : positions)
		{
			float distance = glm::length(camera.Position - pos);
			sorted[distance] = pos;
		}
		positions.clear();
		for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); it++)
		{
			positions.push_back(it->second);
		}

		grass.Draw(grassShader, &positions);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		framebufferShader.use();
		glBindVertexArray(VAO);
		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		framebufferShader.setInt("screenTexture", 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteBuffers(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &framebuffer);
	glDeleteBuffers(1, &rbo);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		ratio += 0.1f;

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		ratio -= 0.1f;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}