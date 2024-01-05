#define GLM_FORCE_CTOR_INIT 

#include <GL/glew.h>
#include <glfw3.h>

#include "Camera.h"
#include "Shader.h"
#include "Mesh.h"
#include "Model.h"
#include "SkyBox.h"

#pragma comment (lib, "glfw3dll.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "OpenGL32.lib")

// settings
const unsigned int SCR_WIDTH = 2560;
const unsigned int SCR_HEIGHT = 1440;

std::unique_ptr<Camera> pCamera;
std::unique_ptr<Mesh> floorObj;
std::unique_ptr<SkyBox> skyboxObj;
std::unique_ptr<Model> tankObj, helicopterObj, sunObj, moonObj, enemyTankObj;
float timeAcceleration = 0.1f;

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void MouseCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void ProcessKeyboard(GLFWwindow* window);

void LoadObjects();
void RenderTeam(Model* tank, Model* helicopter, Shader& shader, bool enemyTeam);
void RenderScene(Shader& shader);


double deltaTime = 0.0f;
double lastFrame = 0.0f;

int main(int argc, char** argv)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Tank Field Simulator", glfwGetPrimaryMonitor(), NULL);
	
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetScrollCallback(window, ScrollCallback);
	glfwSetKeyCallback(window, KeyCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewInit();

	pCamera = std::make_unique<Camera>(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0, 5.0, -30.0));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader shadowMappingShader("ShadowMapping.vs", "ShadowMapping.fs");
	Shader shadowMappingDepthShader("ShadowMappingDepth.vs", "ShadowMappingDepth.fs");
	Shader skyboxShader("Skybox.vs", "Skybox.fs");

	const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	skyboxShader.Use();
	skyboxShader.SetInt("skybox", 0);

	shadowMappingShader.Use();
	shadowMappingShader.SetInt("diffuseTexture", 0);
	shadowMappingShader.SetInt("shadowMap", 1);

	glEnable(GL_CULL_FACE);

	LoadObjects();

	glm::vec3 lightPos(0.0f, 20.0f, -0.01f);
	float hue = 1.0;
	float floorHue = 0.9;

	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		ProcessKeyboard(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 1. render depth of scene to texture (from light's perspective)
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 5.0f, far_plane = 50.f;
		lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;

		// render scene from light's point of view
		shadowMappingDepthShader.Use();
		shadowMappingDepthShader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		RenderScene(shadowMappingDepthShader);
		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// reset viewport
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 2. render scene as normal using the generated depth/shadow map 
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shadowMappingShader.Use();
		glm::mat4 projection = pCamera->GetProjectionMatrix();
		glm::mat4 view = pCamera->GetViewMatrix();
		shadowMappingShader.SetMat4("projection", projection);
		shadowMappingShader.SetMat4("view", view);
		shadowMappingShader.SetFloat("hue", floorHue);


		// set light uniforms
		shadowMappingShader.SetVec3("viewPos", pCamera->GetPosition());
		shadowMappingShader.SetVec3("lightPos", lightPos);
		shadowMappingShader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		RenderScene(shadowMappingShader);

		float sunPassingTime = currentFrame * timeAcceleration;
		lightPos = glm::vec3(0.0f, 20 * sin(sunPassingTime), 50 * cos(sunPassingTime));
		hue = std::max<float>(sin(sunPassingTime), 0.1);
		floorHue = std::max<float>(sin(sunPassingTime), 0.6);

		glm::mat4 sunModel = glm::mat4();
		sunModel = glm::translate(sunModel, lightPos);
		sunModel = glm::scale(sunModel, 0.004f * glm::vec3(1));
		sunObj->RenderModel(shadowMappingShader, sunModel);
		sunObj->RenderModel(shadowMappingDepthShader, sunModel);

		glm::mat4 moonModel = glm::mat4();
		moonModel = glm::translate(moonModel, -lightPos);
		moonModel = glm::scale(moonModel, 3.0f * glm::vec3(1));
		moonObj->RenderModel(shadowMappingShader, moonModel);
		moonObj->RenderModel(shadowMappingDepthShader, moonModel);
		
		skyboxShader.Use();
		skyboxShader.SetMat4("projection", projection);
		skyboxShader.SetMat4("view", glm::mat4(glm::mat3(pCamera->GetViewMatrix())));
		skyboxShader.SetFloat("hue", hue);
		skyboxShader.SetFloat("time", currentFrame);
		skyboxObj->Render(skyboxShader);
	


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void ProcessKeyboard(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		pCamera->ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		pCamera->ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		pCamera->ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		pCamera->ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		pCamera->ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		pCamera->ProcessKeyboard(DOWN, deltaTime);
		

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		pCamera->Reset(width, height);
	}
}

void LoadObjects()
{
	// Texture loading
	Texture floorTexture("../Resources/Floor.png");
	Texture skyboxTexture({
		"../Resources/Skybox/right.jpg",
		"../Resources/Skybox/left.jpg",
		"../Resources/Skybox/top.jpg",
		"../Resources/Skybox/bottom.jpg",
		"../Resources/Skybox/front.jpg",
		"../Resources/Skybox/back.jpg"
		});

	// Positions loading
	const float floorSize = 50.0f;
	std::vector<Vertex> floorVertices({
		// positions            // normals           // texcoords
	   { floorSize, 0.0f,  floorSize,  0.0f, 1.0f, 0.0f,    floorSize,  0.0f},
	   {-floorSize, 0.0f,  floorSize,  0.0f, 1.0f, 0.0f,    0.0f,  0.0f},
	   {-floorSize, 0.0f, -floorSize,  0.0f, 1.0f, 0.0f,    0.0f, floorSize},

	   { floorSize, 0.0f,  floorSize,  0.0f, 1.0f, 0.0f,    floorSize,  0.0f},
	   {-floorSize, 0.0f, -floorSize,  0.0f, 1.0f, 0.0f,    0.0f, floorSize},
	   { floorSize, 0.0f, -floorSize,  0.0f, 1.0f, 0.0f,    floorSize, floorSize}
	});


	// Objects loading
	skyboxObj = std::make_unique<SkyBox>(skyboxTexture);
	floorObj = std::make_unique<Mesh>(floorVertices, std::vector<unsigned int>(), std::vector<Texture>{floorTexture});
	tankObj = std::make_unique<Model>("../Models/Tank/IS.obj", false, glm::vec3(0, -0.8f, 0));
	enemyTankObj = std::make_unique<Model>("../Models/Tank2/Tiger_I.obj", false, glm::vec3(0, -0.8f, 0));
	helicopterObj = std::make_unique<Model>("../Models/Helicopter/uh60.dae", false, glm::vec3(0, 0, 0));
	sunObj = std::make_unique<Model>("../Models/Sun/13913_Sun_v2_l3.obj", false, glm::vec3(0, 0, 0));
	moonObj = std::make_unique<Model>("../Models/Moon/moon.obj", false, glm::vec3(0, 0, 0));
}

void RenderScene(Shader& shader)
{
	glDisable(GL_CULL_FACE);

	// Floor
	floorObj->RenderMesh(shader);

	// Teams
	RenderTeam(tankObj.get(), helicopterObj.get(), shader, false);
	RenderTeam(enemyTankObj.get(), helicopterObj.get(), shader, true);

}

void RenderTeam(Model* tank, Model* helicopter, Shader& shader, bool enemyTeam)
{
	// Constants
	const float startingPoint = 25.0f;
	const float intersectionPoint = 5.0f;
	const float tankAcceleration = 0.2f;
	const float helicopterAcceleration = 0.4f;
	const int tanksCounter = 5;
	const int helicopterCounter = 2;
	const float initPosition = enemyTeam ? -startingPoint : startingPoint;

	// Tanks
	float tankPosX = (enemyTeam ? glfwGetTime() : -glfwGetTime()) * tankAcceleration;
	if (enemyTeam && initPosition + tankPosX >= -intersectionPoint) tankPosX = -intersectionPoint - initPosition;
	else if (!enemyTeam && initPosition + tankPosX <= intersectionPoint) tankPosX = intersectionPoint - initPosition;

	glm::mat4 tankModel;
	tankModel = glm::translate(tankModel, glm::vec3(initPosition + tankPosX, 0, 0));
	tankModel = glm::scale(tankModel, 0.5f * glm::vec3(1));
	tankModel = glm::rotate(tankModel, glm::radians(270.0f), glm::vec3(0, 1, 0));
	if (enemyTeam) tankModel = glm::rotate(tankModel, glm::radians(180.0f), glm::vec3(0, 1, 0));
	tank->RenderModel(shader, tankModel);

	glm::mat4 firstHalfTanksModel, secondHalfTanksModel;
	firstHalfTanksModel = secondHalfTanksModel = tankModel;

	for (int count = 0; count < tanksCounter; count++)
	{
		firstHalfTanksModel = glm::translate(firstHalfTanksModel, glm::vec3(8.0f, 0.0f, -1.0f));
		secondHalfTanksModel = glm::translate(secondHalfTanksModel, glm::vec3(-8.0f, 0.0f, -1.0f));
		tank->RenderModel(shader, firstHalfTanksModel);
		tank->RenderModel(shader, secondHalfTanksModel);
	}

	// Helicopter
	float helicopterPosX = (enemyTeam ? glfwGetTime() : -glfwGetTime()) * helicopterAcceleration;

	if (enemyTeam && initPosition + helicopterPosX >= -intersectionPoint) helicopterPosX = -intersectionPoint - initPosition;
	else if (!enemyTeam && initPosition + helicopterPosX <= intersectionPoint) helicopterPosX = intersectionPoint - initPosition;

	// Helicopter transformations
	glm::mat4 helicopterModel;
	helicopterModel = glm::translate(helicopterModel, glm::vec3(initPosition + helicopterPosX, 7, 0));
	helicopterModel = glm::scale(helicopterModel, 0.4f * glm::vec3(1));
	helicopterModel = glm::rotate(helicopterModel, glm::radians(270.0f), glm::vec3(1, 0, 0));
	helicopterModel = glm::rotate(helicopterModel, glm::radians(enemyTeam ? 270.0f : 90.0f), glm::vec3(0, 0, 1));

	// Propeller rotation
	glm::mat4 topPropellerModel = helicopterModel;
	topPropellerModel = glm::rotate(topPropellerModel, glm::radians(1000 * (float)glfwGetTime()), glm::vec3(0, 0, 1));

	// Render helicopter with the rotation
	helicopter->RenderModelMesh(shader, helicopterModel, 10, topPropellerModel);	

	glm::mat4 firstHalfHelicoptersModel, secondHalfHelicoptersModel, firstHalfPropellerModel, secondHalfPropellerModel;
	firstHalfHelicoptersModel = secondHalfHelicoptersModel = helicopterModel;

	// Render more helicopters with the same model
	firstHalfHelicoptersModel = glm::translate(firstHalfHelicoptersModel, glm::vec3(30.0f, -10.0f, 0.0f));
	firstHalfPropellerModel = firstHalfHelicoptersModel;
	firstHalfPropellerModel = glm::rotate(firstHalfPropellerModel, glm::radians(1000 * (float)glfwGetTime()), glm::vec3(0, 0, 1));
	helicopter->RenderModelMesh(shader, firstHalfHelicoptersModel, 10, firstHalfPropellerModel);

	secondHalfHelicoptersModel = glm::translate(secondHalfHelicoptersModel, glm::vec3(-30.0f, -10.0f, 0.0f));
	secondHalfPropellerModel = secondHalfHelicoptersModel;
	secondHalfPropellerModel = glm::rotate(secondHalfPropellerModel, glm::radians(1000 * (float)glfwGetTime()), glm::vec3(0, 0, 1));
	helicopter->RenderModelMesh(shader, secondHalfHelicoptersModel, 10, secondHalfPropellerModel);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		timeAcceleration += 0.05f;
	}
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	pCamera->Reshape(width, height);
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	pCamera->MouseControl((float)xpos, (float)ypos);
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yOffset)
{
	pCamera->ProcessMouseScroll((float)yOffset);
}