#include "stdafx.h"
#include "gl.h"
#include "../Global\Debug.h"
#include <conio.h>
#include "../Global/App.h"
#include <array>
#include "../BaseGL/shader/Shader_Loader.h"
#include "../BaseGL/shader/shader.h"
#include "../BaseGL/camera.h"
#include "glDebug.h"
#include <glm\gtc\type_ptr.hpp>
#include "..\BaseGL\texture.h"
#include "..\gui\text\Font_Loader.h"
#include "..\gui\UI\gui.h"
#include "..\gui\UI\Quad.h"
#include "../Model/Model.h"
#include "../Model/Model_Loader.h"
#include "../gui/text/text.h"
#include "../BaseGL/Framebuffer.h"
#include "../lighting/Lights.h"
#include "../gui/UI/Colorings.h"
#include "../gui/UI/Line.h"
#include "../Model/Node.h"
#include "../Mesh/Mesh.h"
#include "../physics/physics.h"
#include "../physics/Collision.h"
#include "../Model/stdafx.h"
#include "../mesh/stdafx.h"


int gl::MAX_WORK_GROUP_COUNT = 0;
glm::ivec3 gl::MAX_WORK_GROUP_SIZE = {};
size_t gl::quadVBO = 0;
size_t gl::quadEBO = 0;
std::vector<std::string> gl::EXTENSION_LIST = {};
int gl::EXTENSIONS_SUPPORTED_NUM = 0;
int gl::OPENGL_VERSION[2] = {};
std::string gl::GLSL_VERSION = "";
std::string gl::SYSTEM_RENDERER = "";
float gl::resolution = 0.3f;
int gl::MAX_TEXTURE_UNIT_COUNT;
size_t gl::screenWidth = 0;
size_t gl::screenHeight = 0;
size_t gl::generalUniformBuffer = 0;
size_t gl::screenQuadVAO;
size_t gl::screenShaderProgram;


void gl::init()
{
	initGLEW();
	getOpenGLInitValues();
	setViewport(app::mainWindow);
	glDebug::init();
	camera::main_camera.init();
	initGeneralUniformBuffer();
	initQuadVBO();
	texture::initFramebuffers();
	shader::loadShaders();

	lighting::initLighting();
	node::initNodeBuffers();
	model::initModels();
	mesh::initMeshVAO();

	model::setupModels();

	node::updateNodeMatrices();
	model::revalidateModelMeshOffsets();
	mesh::revalidateMeshNodeOffsets();

	gui::init();
	bindUniformBufferLocations();
	glDebug::getGLError("init()4");
	debug::printErrors();
}
void gl::initQuadVBO()
{
	/*        Colored-Quad
	2---3     0,0------1,0
	|   |      |        |
	|   |      |        |
	|   |      |        |
	0---1     0,1------1,1
	*/
	float varr[4 * 2] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
	};
	size_t iarr[6] = {
		2, 0, 1, 1, 3, 2
	};
	quadVBO = vao::createStorage(sizeof(float) * 4 * 2, &varr[0], 0);
	quadEBO = vao::createStorage(sizeof(size_t) * 6, &iarr[0], 0);
}

void gl::bindUniformBufferLocations()
{
	//bind uniform buffers to shaders

	lighting::setupLightShader();
	mesh::setupMeshShader();
	mesh::setupBlendMeshShader();
	mesh::setupMeshNormalShader();
	gui::setupQuadIndexShader();
	//gui::setupLineShader();
	debug::printErrors();
	gui::setupColoringShaders();

	//shader::bindUniformBufferToShader(debug::lineShaderID, generalUniformBuffer, "GeneralUniformBuffer");
	debug::printErrors();
}

void gl::setViewport(app::ContextWindow::Window& pViewport) {
	screenWidth = pViewport.width;
	screenHeight = pViewport.height;
	glViewport(0, 0, screenWidth/gl::resolution, screenHeight/gl::resolution);
}

void gl::getOpenGLInitValues()
{
	glGetIntegerv(GL_MAJOR_VERSION, &OPENGL_VERSION[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OPENGL_VERSION[1]);
	GLSL_VERSION = std::string((char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
	SYSTEM_RENDERER = std::string((char*)glGetString(GL_RENDERER));

	glGetIntegerv(GL_NUM_EXTENSIONS, &EXTENSIONS_SUPPORTED_NUM);
	EXTENSION_LIST.resize(EXTENSIONS_SUPPORTED_NUM);
	for (int k = 0; k < EXTENSIONS_SUPPORTED_NUM; ++k) {
		EXTENSION_LIST[k] = std::string((char*)glGetStringi(GL_EXTENSIONS, k));
	}

	glGetIntegeri_v(GL_MAX_COMPUTE_FIXED_GROUP_SIZE_ARB, 0, &MAX_WORK_GROUP_SIZE.x);
	glGetIntegeri_v(GL_MAX_COMPUTE_FIXED_GROUP_SIZE_ARB, 1, &MAX_WORK_GROUP_SIZE.y);
	glGetIntegeri_v(GL_MAX_COMPUTE_FIXED_GROUP_SIZE_ARB, 2, &MAX_WORK_GROUP_SIZE.z);

	glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &vao::SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT);
	glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &vao::UNIFORM_BUFFER_OFFSET_ALIGNMENT);
	glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &vao::MAX_UNIFORM_BUFFER_BINDINGS);
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &MAX_TEXTURE_UNIT_COUNT);
	glGetIntegerv(GL_MIN_MAP_BUFFER_ALIGNMENT, &vao::MIN_MAP_BUFFER_ALIGNMENT);


	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDebug::getGLError("initOpenGL()");
}

void gl::initGLEW() {
	puts("Initializing OpenGL(GLEW)...\n");
	size_t glew = glewInit();
	if (glew != GLEW_OK) {

		debug::pushError(std::string("\napp::init() - Unable to initialize GLEW (glewInit() return code: %i)\nGLEW Error Log\n %s"
			+ glew) + std::string((const char*)glewGetErrorString(glew)), debug::Error::Severity::Fatal);
		while (!_getch()) {}
		exit(glew);
	}
}



void gl::initGeneralUniformBuffer()
{
	//contains: projectionMatrix(mat4), viewMatrix(mat4), camera position(vec4)

	size_t generalUniformDataSize = sizeof(float) * (16 + 16 + 4);

	generalUniformBuffer = vao::createStorage(generalUniformDataSize, nullptr, GL_MAP_WRITE_BIT | vao::MAP_PERSISTENT_FLAGS);
	vao::createStream(generalUniformBuffer, GL_MAP_WRITE_BIT);
	vao::bindStorage(GL_UNIFORM_BUFFER, generalUniformBuffer);
	glDebug::getGLError("initGeneralUniformBuffer()1:");
}



void gl::updateGeneralUniformBuffer()
{
	std::vector<float> generalUniformData(36);

	std::memcpy(&generalUniformData[0], glm::value_ptr(camera::main_camera.getProjection()), sizeof(float) * 16);
	std::memcpy(&generalUniformData[16], glm::value_ptr(camera::main_camera.getView()), sizeof(float) * 16);
	std::memcpy(&generalUniformData[32], glm::value_ptr(camera::main_camera.getPos()), sizeof(float) * 3);
	vao::uploadStorage(generalUniformBuffer, sizeof(float) * 36, &generalUniformData[0]);

	glDebug::getGLError("updateGeneralUniformBuffer():");
}


