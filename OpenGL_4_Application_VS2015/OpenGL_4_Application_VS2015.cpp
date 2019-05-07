//
//  main.cpp
//  OpenGL Shadows
//
//  Created by CGIS on 05/12/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC
#include "stdafx.h"
#include <iostream>
#include "glm/glm.hpp"//core glm functionality
#include "glm/gtc/matrix_transform.hpp"//glm extension for generating common transformation matrices
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLEW/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include "Shader.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"
#define TINYOBJLOADER_IMPLEMENTATION

#include "Model3D.hpp"
#include "Mesh.hpp"

int glWindowWidth = 1500;
int glWindowHeight = 750;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const GLuint SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat3 lightDirMatrix;
GLuint lightDirMatrixLoc;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor1;
GLuint lightColorLoc1;
glm::vec3 lightColor2;
GLuint lightColorLoc2;
glm::vec3 lightColor3;
GLuint lightColorLoc3;

glm::vec3 pointLightPosition1;
glm::vec3 pointLightPosition3;
GLuint pointLightPosition1Loc,  pointLightPosition3Loc;


gps::Camera myCamera(glm::vec3(0.0f, 1.0f, 2.5f), glm::vec3(0.0f, 0.0f, 0.0f));
GLfloat cameraSpeed = 0.09f;

bool pressedKeys[1024];
GLfloat lightAngle;

gps::Model3D ground;
gps::Model3D lightCube;
gps::Model3D airplane;
gps::Model3D car;
gps::Model3D house;
gps::Model3D sled;
gps::Model3D logs;
gps::Model3D stone1;
gps::Model3D stone2;
gps::Model3D cuttree1;
gps::Model3D cuttree2;
gps::Model3D fox;
gps::Model3D snowman;
gps::Model3D lamp1;
gps::Model3D lamp2;
gps::Model3D tree1;
gps::Model3D tree2;
gps::Model3D tree3;
gps::Model3D tree4;
gps::Model3D tree5;
gps::Model3D tree6;
gps::Model3D tree7;
gps::Model3D tree8;
gps::Model3D tree9;
gps::Model3D tree10;
gps::Model3D tree11;
gps::Model3D tree12;
gps::Model3D tree13;
gps::Model3D tree14;
gps::Model3D tree15;
gps::Model3D tree16;
gps::Model3D tree17;
gps::Model3D tree18;
gps::Model3D tree19;
gps::Model3D tree20;
gps::Model3D tree21;
gps::Model3D tree22;
gps::Model3D tree23;
gps::Model3D tree24;
gps::Model3D tree25;
gps::Model3D tree26;
gps::Model3D tree27;
gps::Model3D tree28;
gps::Model3D hills;
gps::Model3D lampint1;
gps::Model3D lampint2;
gps::Model3D sun;
gps::Model3D moon;
gps::Model3D fireplace;

gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader depthMapShader;

gps::SkyBox mySkyBox;
gps::Shader skyboxShader;
std::vector<const GLchar*> faces;

gps::SkyBox mySkyBox2;
std::vector<const GLchar*> faces2;

GLuint shadowMapFBO;
GLuint depthMapTexture;

//pentru animatie soaree si luna
float deltaSun = 0;
float movementSpeedSun = 3; // units per second

int day = 0;
int directional = 1;
int fog = 0;
bool broken = false;

bool shadowAnim = false;
glm::mat4 sunLight, moonLight;

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height)
{
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO
	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	myCustomShader.useShaderProgram();

	//set projection matrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	//send matrix data to shader
	GLint projLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	
	lightShader.useShaderProgram();
	
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	//set Viewport transform
	glViewport(0, 0, retina_width, retina_height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

GLfloat yaw = -90.0f;
GLfloat pitch = 0.0f;
GLfloat roll = 0.0f;
GLfloat lastX = 500, lastY = 300;

bool firstMouse = true;
void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse) 
	{ 
		lastX = xpos;   
		lastY = ypos;   
		firstMouse = false; 
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos; // Reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	GLfloat sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)pitch = 89.0f;  
	if (pitch < -89.0f)pitch = -89.0f;


	myCamera.rotate(pitch, yaw);
}

void processMovement()
{

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_P]) {
		lightCube.Draw(lightShader);
	}

	if (pressedKeys[GLFW_KEY_J]) {

		lightAngle += 0.3f;
		if (lightAngle > 360.0f)
			lightAngle -= 180.0f;
		glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
		myCustomShader.useShaderProgram();
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
	}

	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle -= 0.3f; 
		if (lightAngle < 180.0f)
			lightAngle += 180.0f;
		glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
		myCustomShader.useShaderProgram();
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
	}	

	//solid
	if (pressedKeys[GLFW_KEY_1]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	//wireframe
	if (pressedKeys[GLFW_KEY_2]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	//point
	if (pressedKeys[GLFW_KEY_3]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}

	//se opreste animatia cu soare si luna
	if (pressedKeys[GLFW_KEY_4]) {
		movementSpeedSun = 0;
	}
	//animatie cu soare & luna viteza mica - default
	if (pressedKeys[GLFW_KEY_5]) {
		movementSpeedSun = 3;
	}
	//animatie cu soare & luna viteza mai mare
	if (pressedKeys[GLFW_KEY_6]) {
		movementSpeedSun = 30;
	}

	//(doar pentru noapte) se aprinde felinarul de langa casuta
	if (pressedKeys[GLFW_KEY_7]) {
		directional = 0;
		myCustomShader.useShaderProgram();
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "directional"), directional);
	}
	//(doar pentru noapte) se aprinde felinarul de langa omul de zapada
	if (pressedKeys[GLFW_KEY_8]) {
		directional = 2;
		myCustomShader.useShaderProgram();
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "directional"), directional);
	}
	//(doar pentru noapte) se aprinde felinarele
	if (pressedKeys[GLFW_KEY_Z]) {
		directional = 3;
		myCustomShader.useShaderProgram();
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "directional"), directional);
	}
	//lumina directionala doar in toata scena
	if (pressedKeys[GLFW_KEY_9]) {
		directional = 1;
		myCustomShader.useShaderProgram();
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "directional"), directional);
	}

	//se opreste ceata
	if (pressedKeys[GLFW_KEY_0]) {
		fog=0;
		myCustomShader.useShaderProgram();
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "fog"), fog);
	}
	//se porneste ceata
	if (pressedKeys[GLFW_KEY_F]) {
		fog = 1;
		myCustomShader.useShaderProgram();
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "fog"), fog);
	}

	//pornire animatie pentru bec stricat(trebuie ca tasta "8" sa fie apasata, adica felinarul sa fie aprins)
	if (pressedKeys[GLFW_KEY_B]) {
		broken = true;
	}
	//oprire animatie pentru bec stricat
	if (pressedKeys[GLFW_KEY_N]) {
		broken = false;
	}

	if (pressedKeys[GLFW_KEY_X]) {
		shadowAnim = true;
	}
	//oprire animatie pentru bec stricat
	if (pressedKeys[GLFW_KEY_C]) {
		shadowAnim = false;
	}
	

}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	//for Mac OS X
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwMakeContextCurrent(glWindow);

	glfwWindowHint(GLFW_SAMPLES, 4);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
    //glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	//glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initFBOs()
{
	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);

	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix()
{
	const GLfloat near_plane = 0.0f, far_plane = 50.0f;
	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);

	//glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
	glm::vec3 lightDirTr;
	if (shadowAnim == true)
	{
		
		
			if (day == 1)
			{
				lightDirTr = glm::vec3(sunLight * glm::vec4(lightDir, 1.0f));
				glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
			}
			else
			{
				if (directional == 1)
				{
					lightDirTr = glm::vec3(moonLight * glm::vec4(lightDir, 1.0f));
					glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
				}
				
			}

		
		
	}
	else {
		lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
	}
	glm::mat4 lightView = glm::lookAt(lightDirTr, myCamera.getCameraTarget(), glm::vec3(0.0f, 1.0f, 0.0f));

	return lightProjection * lightView;
}

void initModels()
{

	
	lightCube = gps::Model3D("objects/cube/cube.obj", "objects/cube/");
	house = gps::Model3D("objects/house/house.obj", "objects/house/");
	sled = gps::Model3D("objects/sled/sled.obj", "objects/sled/");
	logs = gps::Model3D("objects/logs/logs.obj", "objects/logs/");
	stone1 = gps::Model3D("objects/stone1/stone1.obj", "objects/stone1/");
	stone2 = gps::Model3D("objects/stone2/stone2.obj", "objects/stone2/");
	cuttree1 = gps::Model3D("objects/cuttree1/cuttree1.obj", "objects/cuttree1/");
	cuttree2 = gps::Model3D("objects/cuttree2/cuttree2.obj", "objects/cuttree2/");
	fox = gps::Model3D("objects/fox/fox.obj", "objects/fox/");
	snowman = gps::Model3D("objects/snowman/snowman.obj", "objects/snowman/");
	lamp1 = gps::Model3D("objects/lamp1/lamp1.obj", "objects/lamp1/");
	lamp2 = gps::Model3D("objects/lamp2/lamp2.obj", "objects/lamp2/");
	tree1 = gps::Model3D("objects/tree1/tree1.obj", "objects/tree1/");
	tree2 = gps::Model3D("objects/tree2/tree2.obj", "objects/tree2/");
	tree3 = gps::Model3D("objects/tree3/tree3.obj", "objects/tree3/");
	tree4 = gps::Model3D("objects/tree4/tree4.obj", "objects/tree4/");
	tree5 = gps::Model3D("objects/tree5/tree5.obj", "objects/tree5/");
	tree6 = gps::Model3D("objects/tree6/tree6.obj", "objects/tree6/");
	tree7 = gps::Model3D("objects/tree7/tree7.obj", "objects/tree7/");
	tree8 = gps::Model3D("objects/tree8/tree8.obj", "objects/tree8/");
	tree9 = gps::Model3D("objects/tree9/tree9.obj", "objects/tree9/");
	tree10 = gps::Model3D("objects/tree10/tree10.obj", "objects/tree10/");
	tree11 = gps::Model3D("objects/tree11/tree11.obj", "objects/tree11/");
	tree12 = gps::Model3D("objects/tree12/tree12.obj", "objects/tree12/");
	tree13 = gps::Model3D("objects/tree13/tree13.obj", "objects/tree13/");
	tree14 = gps::Model3D("objects/tree14/tree14.obj", "objects/tree14/");
	tree15 = gps::Model3D("objects/tree15/tree15.obj", "objects/tree15/");
	tree16 = gps::Model3D("objects/tree16/tree16.obj", "objects/tree16/");
	tree17 = gps::Model3D("objects/tree17/tree17.obj", "objects/tree17/");
	tree18 = gps::Model3D("objects/tree18/tree18.obj", "objects/tree18/");
	tree19 = gps::Model3D("objects/tree19/tree19.obj", "objects/tree19/");
	tree20 = gps::Model3D("objects/tree20/tree20.obj", "objects/tree20/");
	tree21 = gps::Model3D("objects/tree21/tree21.obj", "objects/tree21/");
	tree22 = gps::Model3D("objects/tree22/tree22.obj", "objects/tree22/");
	tree23 = gps::Model3D("objects/tree23/tree23.obj", "objects/tree23/");
	tree24 = gps::Model3D("objects/tree24/tree24.obj", "objects/tree24/");
	tree25 = gps::Model3D("objects/tree25/tree25.obj", "objects/tree25/");
	tree26 = gps::Model3D("objects/tree26/tree26.obj", "objects/tree26/");
	tree27 = gps::Model3D("objects/tree27/tree27.obj", "objects/tree27/");
	tree28 = gps::Model3D("objects/tree28/tree28.obj", "objects/tree28/");
	hills = gps::Model3D("objects/hills/hills.obj", "objects/hills/");
	lampint1 = gps::Model3D("objects/lampint1/lampint1.obj", "objects/lampint1/");
	lampint2 = gps::Model3D("objects/lampint2/lampint2.obj", "objects/lampint2/");
	sun = gps::Model3D("objects/sun/sun.obj", "objects/sun/");
	moon = gps::Model3D("objects/moon/moon.obj", "objects/moon/");
	fireplace = gps::Model3D("objects/fireplace/fireplace.obj", "objects/fireplace/");
}

void initShaders()
{
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	depthMapShader.loadShader("shaders/simpleDepthMap.vert", "shaders/simpleDepthMap.frag");
}

void initUniforms()
{
	myCustomShader.useShaderProgram();

	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");

	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	
	lightDirMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDirMatrix");

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));	

	//set the light direction (direction towards the light)
	
	lightDir = glm::vec3(0.0f, 6.0f, 10.0f);
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set light color
	lightColor1 = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc1 = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor1");
	glUniform3fv(lightColorLoc1, 1, glm::value_ptr(lightColor1));

	lightColor2 = glm::vec3(0.584, 0.772, 0.996); //light blue moon light
	lightColorLoc2 = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor2");
	glUniform3fv(lightColorLoc2, 1, glm::value_ptr(lightColor2));

	lightColor3 = glm::vec3(0.984, 0.937, 0.752); //yellow for lampposts
	lightColorLoc3 = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor3");
	glUniform3fv(lightColorLoc3, 1, glm::value_ptr(lightColor3));

	pointLightPosition1 = glm::vec3(1.90317,1.08962,1.35780);
	pointLightPosition1Loc = glGetUniformLocation(myCustomShader.shaderProgram, "pointLightPosition1");
	glUniform3fv(pointLightPosition1Loc, 1, glm::value_ptr(pointLightPosition1));


	pointLightPosition3 = glm::vec3(-3.52531,1.43490,2.60801);
	pointLightPosition3Loc = glGetUniformLocation(myCustomShader.shaderProgram, "pointLightPosition3");
	glUniform3fv(pointLightPosition3Loc, 1, glm::value_ptr(pointLightPosition3));

	
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "directional"), directional);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "fog"), fog);

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}


float timer = 0.0f;
void animateLight()
{
	myCustomShader.useShaderProgram();
	GLuint brokenLoc = glGetUniformLocation(myCustomShader.shaderProgram, "broken");

	if (timer < 5.0f) {
		timer += 0.1f; glUniform1i(brokenLoc, 0);
	}
	else if (timer < 5.5f) {
		timer += 0.1f; glUniform1i(brokenLoc, 1);
	}
	else if (timer < 6.0f) {
		timer += 0.1f; glUniform1i(brokenLoc, 0);
	}
	else if (timer < 6.2f) {
		timer += 0.1f; glUniform1i(brokenLoc, 1);
	}
	else if (timer < 6.4f) {
		timer += 0.1f; glUniform1i(brokenLoc, 0);
	}
	else if (timer < 6.6f) {
		timer += 0.1f; glUniform1i(brokenLoc, 1);
	}
	else if (timer < 6.8f) {
		timer += 0.1f; glUniform1i(brokenLoc, 0);
	}
	else if (timer < 12.0f) {
		timer += 0.1f; glUniform1i(brokenLoc, 1);
	}
	
	else timer = 0.0f;
}

void updateDelta(double elapsedSeconds) {
	deltaSun = deltaSun + movementSpeedSun * elapsedSeconds;
}
double lastTimeStamp = glfwGetTime();
//////////////////////////////////////////


void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	processMovement();

	//render the scene to the depth buffer (first pass)

	depthMapShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	//*************************************

	model = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	house.Draw(depthMapShader);
	sled.Draw(depthMapShader);
	logs.Draw(depthMapShader);
	stone1.Draw(depthMapShader);
	stone2.Draw(depthMapShader);
	cuttree1.Draw(depthMapShader);
	cuttree2.Draw(depthMapShader);
	fox.Draw(depthMapShader);
	snowman.Draw(depthMapShader);
	lamp1.Draw(depthMapShader);
	lamp2.Draw(depthMapShader);
	tree1.Draw(depthMapShader);
	tree2.Draw(depthMapShader);
	tree3.Draw(depthMapShader);
	tree4.Draw(depthMapShader);
	tree5.Draw(depthMapShader);
	tree6.Draw(depthMapShader);
	tree7.Draw(depthMapShader);
	tree8.Draw(depthMapShader);
	tree9.Draw(depthMapShader);
	tree10.Draw(depthMapShader);
	tree11.Draw(depthMapShader);
	tree12.Draw(depthMapShader);
	tree13.Draw(depthMapShader);
	tree14.Draw(depthMapShader);
	tree15.Draw(depthMapShader);
	tree16.Draw(depthMapShader);
	tree17.Draw(depthMapShader);
	tree18.Draw(depthMapShader);
	tree19.Draw(depthMapShader);
	tree20.Draw(depthMapShader);
	tree21.Draw(depthMapShader);
	tree22.Draw(depthMapShader);
	tree23.Draw(depthMapShader);
	tree24.Draw(depthMapShader);
	tree25.Draw(depthMapShader);
	tree26.Draw(depthMapShader);
	tree27.Draw(depthMapShader);
	tree28.Draw(depthMapShader);
	hills.Draw(depthMapShader);
	lampint1.Draw(depthMapShader);
	lampint2.Draw(depthMapShader);
	fireplace.Draw(depthMapShader);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//--------------------------------------------render the scene (second pass)

	myCustomShader.useShaderProgram();

	//send lightSpace matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	//send view matrix to shader
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "view"),
		1,
		GL_FALSE,
		glm::value_ptr(view));

	//compute light direction transformation matrix
	lightDirMatrix = glm::mat3(glm::inverseTranspose(view));
	//send lightDir matrix data to shader
	glUniformMatrix3fv(lightDirMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightDirMatrix));

	glViewport(0, 0, retina_width, retina_height);

	myCustomShader.useShaderProgram();

	//bind the depth map
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

	//***********************+

	model = glm::mat4(1.0f);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	house.Draw(myCustomShader);
	sled.Draw(myCustomShader);
	logs.Draw(myCustomShader);
	stone1.Draw(myCustomShader);
	stone2.Draw(myCustomShader);
	cuttree1.Draw(myCustomShader);
	cuttree2.Draw(myCustomShader);
	fox.Draw(myCustomShader);
	snowman.Draw(myCustomShader);
	lamp1.Draw(myCustomShader);
	lamp2.Draw(myCustomShader);
	tree1.Draw(myCustomShader);
	tree2.Draw(myCustomShader);
	tree3.Draw(myCustomShader);
	tree4.Draw(myCustomShader);
	tree5.Draw(myCustomShader);
	tree6.Draw(myCustomShader);
	tree7.Draw(myCustomShader);
	tree8.Draw(myCustomShader);
	tree9.Draw(myCustomShader);
	tree10.Draw(myCustomShader);
	tree11.Draw(myCustomShader);
	tree12.Draw(myCustomShader);
	tree13.Draw(myCustomShader);
	tree14.Draw(myCustomShader);
	tree15.Draw(myCustomShader);
	tree16.Draw(myCustomShader);
	tree17.Draw(myCustomShader);
	tree18.Draw(myCustomShader);
	tree19.Draw(myCustomShader);
	tree20.Draw(myCustomShader);
	tree21.Draw(myCustomShader);
	tree22.Draw(myCustomShader);
	tree23.Draw(myCustomShader);
	tree24.Draw(myCustomShader);
	tree25.Draw(myCustomShader);
	tree26.Draw(myCustomShader);
	tree27.Draw(myCustomShader);
	tree28.Draw(myCustomShader);
	hills.Draw(myCustomShader);
	lampint1.Draw(myCustomShader);
	lampint2.Draw(myCustomShader);
	fireplace.Draw(myCustomShader);

	double currentTimeStamp = glfwGetTime();
	updateDelta(currentTimeStamp - lastTimeStamp);
	lastTimeStamp = currentTimeStamp;
	if (deltaSun > 360.0f)deltaSun -= 360.0f;

	if (deltaSun > 293 || deltaSun < 114)
	{
		day = 1;
	}
	else
	{
		day = 0;
	}
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "day"), day);
	glm::vec3 moonPlace = glm::vec3(0.0, -20.15463, 11.04426);
	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(deltaSun), glm::vec3(1, 0, 0));
	model = glm::translate(model, -moonPlace);
	sunLight = model;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	sun.Draw(myCustomShader);
	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(deltaSun), glm::vec3(1, 0, 0));
	model = glm::translate(model, moonPlace);
	moonLight = model;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	moon.Draw(myCustomShader);






	if (shadowAnim == true)
	{
		if (day == 1)
		{
			glm::vec3 lightDirTr = glm::vec3(sunLight * glm::vec4(lightDir, 1.0f));
			glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
		}
		else
		{
			if (directional == 1)
			{
				glm::vec3 lightDirTr = glm::vec3(moonLight * glm::vec4(lightDir, 1.0f));
				glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
			}
			
		}
	}

	
	//fprintf(stderr, "Timer %lf\n", timer);
	
	

	//draw a white cube around the light

	lightShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	model = glm::mat4(1.0f);
	model = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, lightDir);
	model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.15f));
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	//lightCube.Draw(lightShader);

	if(day==1)mySkyBox2.Draw(skyboxShader, view, projection);
	else mySkyBox.Draw(skyboxShader, view, projection);
}

int main(int argc, const char * argv[]) {

	
	

	initOpenGLWindow();
	initOpenGLState();
	initFBOs();
	initModels();
	initShaders();
	initUniforms();	
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
		glm::value_ptr(projection));
	glCheckError();
	
	faces.push_back("textures/skybox7/right.jpg");
	faces.push_back("textures/skybox7/left.jpg");
	faces.push_back("textures/skybox7/top.jpg");
	faces.push_back("textures/skybox7/bottom.jpg");
	faces.push_back("textures/skybox7/front.jpg");
	faces.push_back("textures/skybox7/back.jpg");

	faces2.push_back("textures/skybox8/right.jpg");
	faces2.push_back("textures/skybox8/left.jpg");
	faces2.push_back("textures/skybox8/top.jpg");
	faces2.push_back("textures/skybox8/bottom.jpg");
	faces2.push_back("textures/skybox8/front.jpg");
	faces2.push_back("textures/skybox8/back.jpg");



	mySkyBox.Load(faces);//night
	mySkyBox2.Load(faces2);//day


	while (!glfwWindowShouldClose(glWindow)) {
		renderScene();
		if(broken==true)animateLight();
		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	//close GL context and any other GLFW resources
	glfwTerminate();

	return 0;
}
