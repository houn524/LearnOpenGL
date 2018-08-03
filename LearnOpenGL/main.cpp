#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// ¼¼ÆÃ
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float ratio = 0.2f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

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

	glEnable(GL_DEPTH_TEST);

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
	
	glDepthFunc(GL_LESS);
	while (!glfwWindowShouldClose(window)) {

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightingShader.use();

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		lightingShader.setMat4("projection", projection);

		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -10.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		lightingShader.setMat4("model", model);

		lightingShader.setVec3("dirLight.direction", glm::mat3(view) * glm::vec3(-1.0f, -1.0f, -1.0f));
		lightingShader.setVec3("pointLights[0].position", glm::vec3(view * glm::vec4(-10.0f, 0.0f, -10.0f, 1.0f)));
		lightingShader.setVec3("pointLights[1].position", glm::vec3(view * glm::vec4(0.0f, 10.0f, -20.0f, 1.0f)));
		

		ourModel.Draw(lightingShader);
		
		model = glm::mat4(1.0);
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

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

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