#include <cassert>
#include <cstring>
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "Camera.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Shape.h"
#include "Materials.cpp"
#include "Texture.h"

using namespace std;

GLFWwindow* window; // Main application window
string RESOURCE_DIR = "./"; // Where the resources are loaded from
bool OFFLINE = false;
bool shiftPressed = false;

shared_ptr<Camera> camera;

shared_ptr<Program> prog;
shared_ptr<Program> prog2;
shared_ptr<Program> progPass1;
shared_ptr<Program> progPass1A;
shared_ptr<Program> progPass2;

shared_ptr<Shape> bunny;
shared_ptr<Shape> teapot;
shared_ptr<Shape> sun;
shared_ptr<Shape> ground;

string shapeName;

float min1;
float min2;
float min3;
float min4;

Light light1(glm::vec3(0.0, 0.0, 1.0), glm::vec3(4.0, 0.325, 4.0), "");
Light light2(glm::vec3(0.0, 1.0, 0.0), glm::vec3(2.0, 0.32, 0.0), "");
Light light3(glm::vec3(1.0, 0.0, 0.0), glm::vec3(-4.0, 0.33, 4.0), "");
Light light4(glm::vec3(0.6, 0.3, 0.7), glm::vec3(-2.0, 0.325, 0.0), "");
Light light5(glm::vec3(0.7, 0.5, 0.3), glm::vec3(0.0, 0.32, 4.0), "");
Light light6(glm::vec3(0.3, 0.7, 0.4), glm::vec3(-4.0, 0.33, -4.0), "");
Light light7(glm::vec3(0.7, 0.3, 0.5), glm::vec3(0.0, 0.325, -2.0), "");
Light light8(glm::vec3(0.3, 0.1, 0.7), glm::vec3(0.0, 0.32, 2.0), "");
Light light9(glm::vec3(0.2, 0.7, 0.3), glm::vec3(4.0, 0.325, -4.0), "");
Light light10(glm::vec3(0.6, 0.2, 0.6), glm::vec3(0.0, 0.33, -4.0), "");

vector<glm::vec3> worldLights;
vector<glm::vec3> cameraLights;
vector<glm::vec3> lightColors;

Materials defaultMat;
Materials blueMat(glm::vec3(0.0, 0.0, 0.4), glm::vec3(0.0, 0.0, 0.7), glm::vec3(0.0, 1.0, 0.8), 100.0);
Materials yellowMat(glm::vec3(1.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), 100.0);
Materials greyMat(glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.2, 0.2, 0.3), glm::vec3(0.3, 0.3, 0.4), 5.0);

vector<glm::vec3> translations;
vector<int> randomNumbers;
float planeAng;

map<string, GLuint> bufIDs;
int indCount;

int textureWidth = 1024;
int textureHeight = 1024;
GLuint framebufferID;
GLuint posTexture;
GLuint norTexture;
GLuint keTexture;
GLuint kdTexture;

bool keyToggles[256] = { false }; // only for English keyboards!

float RANDOM_COLORS[7][3] = {
	{0.0000,    0.4470,    0.7410},
	{0.8500,    0.3250,    0.0980},
	{0.9290,    0.6940,    0.1250},
	{0.4940,    0.1840,    0.5560},
	{0.4660,    0.6740,    0.1880},
	{0.3010,    0.7450,    0.9330},
	{0.6350,    0.0780,    0.1840},
};

// This function is called when a GLFW error occurs
static void error_callback(int error, const char* description)
{
	cerr << description << endl;
}

// This function is called when a key is pressed
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		camera->eye = glm::vec3(camera->eye[0] - (0.2 * sin(camera->yaw)), camera->eye[1], camera->eye[2] - (0.2 * cos(camera->yaw)));
		camera->target = glm::vec3(camera->eye[0] + sin(camera->yaw), camera->eye[1] + camera->pitch, camera->eye[2] + cos(camera->yaw));
	}

	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		camera->eye = glm::vec3(camera->eye[0] + (0.2 * sin(camera->yaw)), camera->eye[1], camera->eye[2] + (0.2 * cos(camera->yaw)));
		camera->target = glm::vec3(camera->eye[0] + sin(camera->yaw), camera->eye[1] + camera->pitch, camera->eye[2] + cos(camera->yaw));
	}

	if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		camera->eye = glm::vec3(camera->eye[0] - (0.2 * cos(camera->yaw)), camera->eye[1], camera->eye[2] + (0.2 * sin(camera->yaw)));
		camera->target = glm::vec3(camera->eye[0] + sin(camera->yaw), camera->eye[1] + camera->pitch, camera->eye[2] + cos(camera->yaw));
	}

	if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		camera->eye = glm::vec3(camera->eye[0] + (0.2 * cos(camera->yaw)), camera->eye[1], camera->eye[2] - (0.2 * sin(camera->yaw)));
		camera->target = glm::vec3(camera->eye[0] + sin(camera->yaw), camera->eye[1] + camera->pitch, camera->eye[2] + cos(camera->yaw));
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
		if ((shiftPressed == true) && (camera->fovy < (114.0 * M_PI / 180.0))) {
			camera->fovy += (1.0 * M_PI / 180.0);
		}

		else if ((shiftPressed == false) && (camera->fovy > (4.0 * M_PI / 180.0))) {
			camera->fovy -= (1.0 * M_PI / 180.0);
		}
	}

	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS || key == GLFW_KEY_RIGHT_SHIFT && action == GLFW_PRESS) {
		shiftPressed = true;
	}

	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE || key == GLFW_KEY_RIGHT_SHIFT && action == GLFW_RELEASE) {
		shiftPressed = false;
	}
}

// This function is called when the mouse is clicked
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	// Get the current mouse position.
	double xmouse, ymouse;
	glfwGetCursorPos(window, &xmouse, &ymouse);
	// Get current window size.
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	if (action == GLFW_PRESS) {
		bool shift = (mods & GLFW_MOD_SHIFT) != 0;
		bool ctrl = (mods & GLFW_MOD_CONTROL) != 0;
		bool alt = (mods & GLFW_MOD_ALT) != 0;
		camera->mouseClicked((float)xmouse, (float)ymouse, shift, ctrl, alt);
	}
}

// This function is called when the mouse moves
static void cursor_position_callback(GLFWwindow* window, double xmouse, double ymouse)
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (state == GLFW_PRESS) {
		camera->mouseMoved((float)xmouse, (float)ymouse);
		camera->target = glm::vec3(camera->eye[0] + sin(camera->yaw), camera->eye[1] + camera->pitch, camera->eye[2] + cos(camera->yaw));
	}
}

static void char_callback(GLFWwindow* window, unsigned int key)
{
	keyToggles[key] = !keyToggles[key];
}

// If the window is resized, capture the new size and reset the viewport
static void resize_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// https://lencerf.github.io/post/2019-09-21-save-the-opengl-rendering-to-image-file/
static void saveImage(const char* filepath, GLFWwindow* w)
{
	int width, height;
	glfwGetFramebufferSize(w, &width, &height);
	GLsizei nrChannels = 3;
	GLsizei stride = nrChannels * width;
	stride += (stride % 4) ? (4 - stride % 4) : 0;
	GLsizei bufferSize = stride * height;
	std::vector<char> buffer(bufferSize);
	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	glReadBuffer(GL_BACK);
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
	stbi_flip_vertically_on_write(true);
	int rc = stbi_write_png(filepath, width, height, nrChannels, buffer.data(), stride);
	if (rc) {
		cout << "Wrote to " << filepath << endl;
	}
	else {
		cout << "Couldn't write to " << filepath << endl;
	}
}

// This function is called once to initialize the scene and OpenGL
static void init()
{
	// Initialize time.
	glfwSetTime(0.0);

	// Set background color.
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Enable z-buffer test.
	glEnable(GL_DEPTH_TEST);

	prog = make_shared<Program>();
	prog->setShaderNames(RESOURCE_DIR + "blinn_vert.glsl", RESOURCE_DIR + "blinn_frag.glsl");
	defaultMat.loadProgram(prog);

	prog2 = make_shared<Program>();
	prog2->setShaderNames(RESOURCE_DIR + "vert.glsl", RESOURCE_DIR + "blinn_frag.glsl");
	prog2->setVerbose(true);
	prog2->init();
	prog2->addAttribute("aPos");
	prog2->addUniform("MV");
	prog2->addUniform("P");
	prog2->addUniform("MVIT");
	prog2->addUniform("t");
	prog2->addUniform("lightColors");
	prog2->addUniform("lightPositions");
	prog2->addUniform("ka");
	prog2->addUniform("kd");
	prog2->addUniform("ks");
	prog2->addUniform("s");
	prog2->setVerbose(false);

	progPass1 = make_shared<Program>();
	progPass1->setShaderNames(RESOURCE_DIR + "pass1_vert.glsl", RESOURCE_DIR + "pass1_frag.glsl");
	progPass1->setVerbose(false);
	progPass1->init();
	progPass1->addAttribute("aPos");
	progPass1->addAttribute("aNor");
	progPass1->addUniform("P");
	progPass1->addUniform("MV");
	progPass1->addUniform("MVIT");
	progPass1->addUniform("ke");
	progPass1->addUniform("kd");

	progPass1A = make_shared<Program>();
	progPass1A->setShaderNames(RESOURCE_DIR + "vert.glsl", RESOURCE_DIR + "pass1_frag.glsl");
	progPass1A->setVerbose(false);
	progPass1A->init();
	progPass1A->addAttribute("aPos");
	progPass1A->addAttribute("aNor");
	progPass1A->addUniform("P");
	progPass1A->addUniform("MV");
	progPass1A->addUniform("MVIT");
	progPass1A->addUniform("ke");
	progPass1A->addUniform("kd");
	progPass1A->addUniform("t");

	progPass2 = make_shared<Program>();
	progPass2->setShaderNames(RESOURCE_DIR + "pass2_vert.glsl", RESOURCE_DIR + "pass2_frag.glsl");
	progPass2->setVerbose(false);
	progPass2->init();
	progPass2->addAttribute("aPos");
	progPass2->addUniform("P");
	progPass2->addUniform("MV");
	progPass2->addUniform("windowSize");
	progPass2->addUniform("posTexture");
	progPass2->addUniform("norTexture");
	progPass2->addUniform("keTexture");
	progPass2->addUniform("kdTexture");
	progPass2->addUniform("lightColors");
	progPass2->addUniform("lightPositions");
	progPass2->bind();
	glUniform1i(progPass2->getUniform("posTexture"), 0);
	glUniform1i(progPass2->getUniform("norTexture"), 1);
	glUniform1i(progPass2->getUniform("keTexture"), 2);
	glUniform1i(progPass2->getUniform("kdTexture"), 3);
	progPass2->unbind();

	camera = make_shared<Camera>();
	camera->setInitDistance(2.0f); // Camera's initial Z translation

	camera->eye = glm::vec3(camera->eye[0], camera->eye[1], camera->eye[2]);
	camera->target = glm::vec3(camera->eye[0] + sin(camera->yaw), camera->eye[1] + camera->pitch, camera->eye[2] + cos(camera->yaw));

	bunny = make_shared<Shape>();
	min1 = bunny->loadMesh(RESOURCE_DIR + "bunny.obj");
	bunny->init();

	teapot = make_shared<Shape>();
	min2 = teapot->loadMesh(RESOURCE_DIR + "teapot.obj");
	teapot->init();

	sun = make_shared<Shape>();
	min3 = sun->loadMesh(RESOURCE_DIR + "sphere.obj");
	sun->init();

	ground = make_shared<Shape>();
	min4 = ground->loadMesh(RESOURCE_DIR + "square.obj");
	ground->init();

	worldLights = { light1.lightPos, light2.lightPos, light3.lightPos, light4.lightPos, light5.lightPos, light6.lightPos, light7.lightPos, light8.lightPos, light9.lightPos, light10.lightPos };
	lightColors = { light1.lightColor, light2.lightColor, light3.lightColor, light4.lightColor, light5.lightColor, light6.lightColor, light7.lightColor, light8.lightColor, light9.lightColor, light10.lightColor };

	//////////////////////////////////////
	// TRANSLATIONS
	//////////////////////////////////////

	int transCount = 0;
	int randNum;

	for (int i = -4; i <= 5; i++) {
		for (int j = -4; j <= 5; j++) {
			translations.push_back(glm::vec3(i - 0.5, 0, j - 0.5));
			transCount++;
			randNum = rand() % 100;
			randomNumbers.push_back(randNum);
		}
	}

	//////////////////////////////////////
	// SURFACE OF REVOLUTION
	//////////////////////////////////////

	vector<float> posBuf;
	vector<unsigned int> indBuf;

	// number of points on each side
	int pointsPerSide = 50;

	// create buffers
	for (int i = 1; i <= pointsPerSide; i++) {
		for (int j = 1; j <= pointsPerSide; j++) {
			// texture scales
			float jScale = ((j - 1) * 1.0) / (pointsPerSide - 1);
			float iScale = ((i - 1) * 1.0) / (pointsPerSide - 1);

			float theta = 2 * M_PI * (1 - iScale);
			float x = 10.0f * jScale;

			posBuf.push_back(x);
			posBuf.push_back(theta);
			posBuf.push_back(0.0f);
		}
	}

	// index triangles
	for (int i = 0; i < pointsPerSide - 1; i++) {
		for (int j = 0; j < pointsPerSide - 1; j++) {
			int currPoint = i * pointsPerSide + j;
			indBuf.push_back(currPoint);
			indBuf.push_back(currPoint + 1);
			indBuf.push_back(currPoint + pointsPerSide + 1);

			indBuf.push_back(currPoint);
			indBuf.push_back(currPoint + pointsPerSide + 1);
			indBuf.push_back(currPoint + pointsPerSide);
		}
	}

	// Total number of indices
	indCount = (int)indBuf.size();

	// Generate buffer IDs and put them in the bufIDs map.
	GLuint tmp[2];
	glGenBuffers(2, tmp);
	bufIDs["bPos"] = tmp[0];
	bufIDs["bInd"] = tmp[1];
	glBindBuffer(GL_ARRAY_BUFFER, bufIDs["bPos"]);
	glBufferData(GL_ARRAY_BUFFER, posBuf.size() * sizeof(float), &posBuf[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIDs["bInd"]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indBuf.size() * sizeof(unsigned int), &indBuf[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	planeAng = -90 * (3.14159 / 180);

	//////////////////////////////////////
	// TEXTURES
	//////////////////////////////////////

	glGenFramebuffers(1, &framebufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);

	glGenTextures(1, &posTexture);
	glBindTexture(GL_TEXTURE_2D, posTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, textureWidth, textureHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, posTexture, 0);

	glGenTextures(1, &norTexture);
	glBindTexture(GL_TEXTURE_2D, norTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, textureWidth, textureHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, norTexture, 0);

	glGenTextures(1, &keTexture);
	glBindTexture(GL_TEXTURE_2D, keTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, textureWidth, textureHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, keTexture, 0);

	glGenTextures(1, &kdTexture);
	glBindTexture(GL_TEXTURE_2D, kdTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, textureWidth, textureHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, kdTexture, 0);

	GLuint depthrenderbuffer;
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, textureWidth, textureHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

	GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		cerr << "Framebuffer is not ok" << endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLSL::checkError(GET_FILE_LINE);
}

// This function is called every frame to draw the scene.
static void render()
{
	// Clear framebuffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (keyToggles[(unsigned)'c']) {
		glEnable(GL_CULL_FACE);
	}
	else {
		glDisable(GL_CULL_FACE);
	}

	// Get current frame buffer size.
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	camera->setAspect((float)width / (float)height);

	double t = glfwGetTime();
	double aspectRatio = (float)width / (float)height;

	// Matrix stacks
	auto P = make_shared<MatrixStack>();
	auto MV = make_shared<MatrixStack>();

	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
	glViewport(0, 0, textureWidth, textureHeight);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	progPass1->bind();
	camera->setAspect(1.0f);
	P->pushMatrix();
	camera->applyProjectionMatrix(P);
	MV->pushMatrix();
	camera->applyViewMatrix(MV);

	glm::vec4 light1World(light1.lightPos, 1.0);
	glm::vec4 light1Camera = MV->topMatrix() * light1World;

	glm::vec4 light2World(light2.lightPos, 1.0);
	glm::vec4 light2Camera = MV->topMatrix() * light2World;

	glm::vec4 light3World(light3.lightPos, 1.0);
	glm::vec4 light3Camera = MV->topMatrix() * light3World;

	glm::vec4 light4World(light4.lightPos, 1.0);
	glm::vec4 light4Camera = MV->topMatrix() * light4World;

	glm::vec4 light5World(light5.lightPos, 1.0);
	glm::vec4 light5Camera = MV->topMatrix() * light5World;

	glm::vec4 light6World(light6.lightPos, 1.0);
	glm::vec4 light6Camera = MV->topMatrix() * light6World;

	glm::vec4 light7World(light7.lightPos, 1.0);
	glm::vec4 light7Camera = MV->topMatrix() * light7World;

	glm::vec4 light8World(light8.lightPos, 1.0);
	glm::vec4 light8Camera = MV->topMatrix() * light8World;

	glm::vec4 light9World(light9.lightPos, 1.0);
	glm::vec4 light9Camera = MV->topMatrix() * light9World;

	glm::vec4 light10World(light10.lightPos, 1.0);
	glm::vec4 light10Camera = MV->topMatrix() * light10World;

	cameraLights = { light1Camera, light2Camera, light3Camera, light4Camera, light5Camera, light6Camera, light7Camera, light8Camera, light9Camera, light10Camera };

	// light sources
	for (int i = 0; i < worldLights.size(); i++) {
		MV->pushMatrix();
		MV->translate(worldLights[i][0], worldLights[i][1], worldLights[i][2]);
		MV->scale(0.05);
		glUniformMatrix4fv(progPass1->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
		glUniformMatrix4fv(progPass1->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
		glUniformMatrix4fv(progPass1->getUniform("MVIT"), 1, GL_FALSE, glm::value_ptr(inverse(transpose(MV->topMatrix()))));
		glUniform3f(progPass1->getUniform("ke"), lightColors[i][0], lightColors[i][1], lightColors[i][2]);
		glUniform3f(progPass1->getUniform("kd"), 0.0f, 0.0f, 0.0f);
		sun->draw(progPass1);
		MV->popMatrix();
	}

	// ground plane
	MV->pushMatrix();
	MV->rotate(planeAng, 1.0f, 0.0f, 0.0f);
	MV->scale(11);
	glUniformMatrix4fv(progPass1->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
	glUniformMatrix4fv(progPass1->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
	glUniformMatrix4fv(progPass1->getUniform("MVIT"), 1, GL_FALSE, glm::value_ptr(inverse(transpose(MV->topMatrix()))));
	glUniform3f(progPass1->getUniform("ke"), 0.0f, 0.0f, 0.0f);
	glUniform3f(progPass1->getUniform("kd"), 0.5f, 0.5f, 0.5f);
	ground->draw(progPass1);
	MV->popMatrix();
	
	// objects
	for (int i = 0; i < 100; i++) {
		MV->pushMatrix();
		MV->translate(translations[i]);
		MV->rotate(sin(i) * 2 * 3.1415, 0.0f, 1.0f, 0.0f);
		MV->scale(0.35 * ((0.2 * sin(randomNumbers[i] / 5)) + 1));
		if (randomNumbers[i] % 4 == 3) {
			progPass1->unbind();
			progPass1A->bind();
			MV->scale(1.0f / 5.0f);
			MV->translate(0, 10, 0);
			MV->rotate(M_PI / -2.0f, 0.0f, 0.0f, 1.0f);
			glUniform1f(progPass1A->getUniform("t"), t);
			glUniformMatrix4fv(progPass1A->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
			GLSL::checkError(GET_FILE_LINE);
			glEnableVertexAttribArray(progPass1A->getAttribute("aPos"));
			GLSL::checkError(GET_FILE_LINE);
			glBindBuffer(GL_ARRAY_BUFFER, bufIDs["bPos"]);
			glVertexAttribPointer(progPass1A->getAttribute("aPos"), 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIDs["bInd"]);
			glUniformMatrix4fv(progPass1A->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
			glUniformMatrix4fv(progPass1A->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
			glUniformMatrix4fv(progPass1A->getUniform("MVIT"), 1, GL_FALSE, glm::value_ptr(inverse(transpose(MV->topMatrix()))));
			glUniform3f(progPass1A->getUniform("ke"), 0.0f, 0.0f, 0.0f);
			glUniform3f(progPass1A->getUniform("kd"), RANDOM_COLORS[i % 7][0], RANDOM_COLORS[i % 7][1], RANDOM_COLORS[i % 7][2]);
			glDrawElements(GL_TRIANGLES, indCount, GL_UNSIGNED_INT, (void*)0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDisableVertexAttribArray(progPass1A->getAttribute("aPos"));
			progPass1A->unbind();
			progPass1->bind();
		}
		else {
			if (randomNumbers[i] % 3 == 1) {
				shapeName = "teapot";
				MV->translate(0.0, -1 * min2, 0.0);
				glm::mat4 S(1.0f);
				S[1][2] = cos(t);
				MV->multMatrix(S);
			}
			else if (randomNumbers[i] % 3 == 2) {
				shapeName = "bunny";
				MV->rotate(t, 0.0f, 1.0f, 0.0f);
				MV->translate(0.0, -1 * min1, 0.0);
			}
			else {
				shapeName = "sphere";
				MV->scale(0.7);
				MV->translate(0.0, -1 * min3, 0.0);
				float scaling = (-0.5 * (0.5 * cos((4 * M_PI) * ((t / 3.0) + 0.9)) + 0.5)) + 1;
				MV->scale(scaling, 1, scaling);
				MV->translate(0, 1.3 * (0.5 * sin((2 * M_PI) * ((t / 3.0) + 0.9)) + 0.5), 0);
			}
			glUniformMatrix4fv(progPass1->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
			glUniformMatrix4fv(progPass1->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
			glUniformMatrix4fv(progPass1->getUniform("MVIT"), 1, GL_FALSE, glm::value_ptr(inverse(transpose(MV->topMatrix()))));
			glUniform3f(progPass1->getUniform("ke"), 0.0f, 0.0f, 0.0f);
			glUniform3f(progPass1->getUniform("kd"), RANDOM_COLORS[i % 7][0], RANDOM_COLORS[i % 7][1], RANDOM_COLORS[i % 7][2]);

			if (shapeName == "teapot") {
				teapot->draw(progPass1);
			}
			else if (shapeName == "bunny") {
				bunny->draw(progPass1);
			}
			else {
				sun->draw(progPass1);
			}
		}
		MV->popMatrix();
	}

	progPass1->unbind();
	MV->popMatrix();
	P->popMatrix();

	//////////////////////////////////////////////////////
	// Render to the screen
	//////////////////////////////////////////////////////
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	progPass2->bind();
	camera->setAspect((float)width/(float)height);
	P->pushMatrix();
	camera->applyProjectionMatrix(P);
	MV->pushMatrix();
	MV->translate(0, 0, -0.75);
	glUniformMatrix4fv(progPass2->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
	glUniformMatrix4fv(progPass2->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
	glUniform3fv(progPass2->getUniform("lightColors"), 10, value_ptr(lightColors[0]));
	glUniform3fv(progPass2->getUniform("lightPositions"), 10, value_ptr(cameraLights[0]));
	glUniform2f(progPass2->getUniform("windowSize"), width, height);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, posTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, norTexture);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, keTexture);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, kdTexture);
	ground->draw(progPass2);
	glActiveTexture(GL_TEXTURE0);
	progPass2->unbind();
	MV->popMatrix();
	P->popMatrix();

	GLSL::checkError(GET_FILE_LINE);

	if (OFFLINE) {
		saveImage("output.png", window);
		GLSL::checkError(GET_FILE_LINE);
		glfwSetWindowShouldClose(window, true);
	}
}

int main(int argc, char** argv)
{
	if (argc < 2) {
		cout << "Usage: A3 RESOURCE_DIR" << endl;
		return 0;
	}
	RESOURCE_DIR = argv[1] + string("/");

	// Optional argument
	if (argc >= 3) {
		OFFLINE = atoi(argv[2]) != 0;
	}

	// Set error callback.
	glfwSetErrorCallback(error_callback);
	// Initialize the library.
	if (!glfwInit()) {
		return -1;
	}
	// Create a windowed mode window and its OpenGL context.
	window = glfwCreateWindow(640, 480, "YOUR NAME", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}
	// Make the window's context current.
	glfwMakeContextCurrent(window);
	// Initialize GLEW.
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		return -1;
	}
	glGetError(); // A bug in glewInit() causes an error that we can safely ignore.
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
	GLSL::checkVersion();
	// Set vsync.
	glfwSwapInterval(1);
	// Set keyboard callback.
	glfwSetKeyCallback(window, key_callback);
	// Set char callback.
	glfwSetCharCallback(window, char_callback);
	// Set cursor position callback.
	glfwSetCursorPosCallback(window, cursor_position_callback);
	// Set mouse button callback.
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	// Set the window resize call back.
	glfwSetFramebufferSizeCallback(window, resize_callback);
	// Initialize scene.
	init();
	// Loop until the user closes the window.
	while (!glfwWindowShouldClose(window)) {
		// Render scene.
		render();
		// Swap front and back buffers.
		glfwSwapBuffers(window);
		// Poll for and process events.
		glfwPollEvents();
	}
	// Quit program.
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
