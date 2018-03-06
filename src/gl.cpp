#include "gl.h"
#include "debug.h"
#include <ncurses.h>
#include "app.h"
#include <array>
#include "shader_loader.h"
#include "shader.h"
#include "camera.h"
#include "gldebug.h"
#include <gtc/type_ptr.hpp>
#include "texture.h"
#include "font_loader.h"
#include "gui.h"
#include "quad.h"
#include "model.h"
#include "model_loader.h"
#include "text.h"
#include "framebuffer.h"
#include "lights.h"
#include "colorings.h"
#include "line.h"
#include "entities.h"
#include "mesh.h"
#include "physics.h"
#include "collision.h"
#include "voxelization.h"

int gl::MAX_WORK_GROUP_COUNT = 0;
glm::ivec3 gl::MAX_WORK_GROUP_SIZE = {};
size_t gl::quadVBO = 0;
size_t gl::quadEBO = 0;
std::vector<std::string> gl::EXTENSION_LIST = {};
int gl::EXTENSIONS_SUPPORTED_NUM = 0;
int gl::OPENGL_VERSION[2] = {};
std::string gl::GLSL_VERSION = "";
std::string gl::SYSTEM_RENDERER = "";
float gl::resolution = 1.0f;
int gl::MAX_TEXTURE_UNIT_COUNT;
size_t gl::screenWidth = 0;
size_t gl::screenHeight = 0;
size_t gl::generalUniformBuffer = 0;
size_t gl::screenQuadVAO;
size_t gl::screenShaderProgram;
size_t gl::cubeVBO = 0;
size_t gl::cubeEBO = 0;

void gl::init()
{
	initGLEW();
	puts("Initializing OpenGL...");
	getOpenGLInitValues();
	setViewport(app::mainWindow);

	lighting::createLight(glm::vec4(3.0f, 5.0f, -5.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 100.0f));
	lighting::createLight(glm::vec4(-1.0f, 4.0f, 3.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 100.0f));
	lighting::createLight(glm::vec4(1.0f, 14.0f, 1.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 100.0f));
	lighting::createLight(glm::vec4(4.0f, -4.0f, 3.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 100.0f));
	lighting::createLight(glm::vec4(3.0f, 15.0f, -5.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 100.0f));

	puts("Initializing Global Data...");
	initPrimitiveVBO();

	puts("glDebug...");
	glDebug::init();

	puts("Shaders...");
	loadShaders();

	puts("GUI...");
	gui::init();
	puts("Colors...");
	gui::uploadConstColors();

	puts("Lines...");
	gui::initLineVAO();

	puts("Widgets...");
	gui::initWidgets();

	puts("Camera...");
	camera::main_camera.init();

	initGeneralUniformBuffer();

	//puts("Framebuffers...");
	//texture::initFramebuffers();

	//puts("Lighting...");
	//lighting::initLighting();

	//puts("Entities...");
	//entities::initEntityBuffers();

	//puts("Models...");
	//model::initModels();
	//mesh::initMeshVAO();
	//model::setupModels();

	//entities::updateEntityMatrices();
	//model::revalidateModelMeshOffsets();
	//mesh::revalidateMeshEntityOffsets();

	puts("Binding uniform buffers...");
	bindUniformBufferLocations();
	debug::printErrors();
}

void gl::loadShaders()
{
    gui::initLineShader();
    //mesh::initMeshShader();
    //mesh::initBlendMeshShader();
    //mesh::initMeshNormalShader();
    //lighting::initLightShader();
    //gui::initQuadIndexShader();
    gui::initColoringShaders();
    //gui::text::initFontShader();
    //voxelization::init();
    shader::Loader::buildShaderPrograms();

}

void gl::bindUniformBufferLocations()
{
	gui::setupLineShader();
	//mesh::setupMeshShader();
	//lighting::setupLightShader();
	//mesh::setupBlendMeshShader();
	//mesh::setupMeshNormalShader();
	//gui::setupQuadIndexShader();
	gui::setupColoringShaders();
	//voxelization::setupShader();
	debug::printErrors();
}

void gl::setViewport(app::ContextWindow::Window& pViewport) {
	screenWidth = pViewport.width * resolution;
	screenHeight = pViewport.height * resolution;
	glViewport(0, 0, screenWidth, screenHeight);
}

void gl::getOpenGLInitValues()
{
	glGetIntegerv(GL_MAJOR_VERSION, &OPENGL_VERSION[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OPENGL_VERSION[1]);
	GLSL_VERSION = std::string((char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
	SYSTEM_RENDERER = std::string((char*)glGetString(GL_RENDERER));

	printf("OpenGL %s.%s\n", std::to_string(OPENGL_VERSION[0]).c_str(), std::to_string(OPENGL_VERSION[1]).c_str());
	printf("GLSL %s\n", GLSL_VERSION.c_str());
	printf("Renderer %s\n", SYSTEM_RENDERER.c_str());

	glGetIntegerv(GL_NUM_EXTENSIONS, &EXTENSIONS_SUPPORTED_NUM);
	printf("%d Extensions found.\n", EXTENSIONS_SUPPORTED_NUM);
	EXTENSION_LIST.resize(EXTENSIONS_SUPPORTED_NUM);
	for (int k = 0; k < EXTENSIONS_SUPPORTED_NUM; ++k) {
		EXTENSION_LIST[k] = std::string((const char*)glGetStringi(GL_EXTENSIONS, k));
		//printf("%s\n", EXTENSION_LIST[k].c_str());
	}


	glGetIntegeri_v(GL_MAX_COMPUTE_FIXED_GROUP_SIZE_ARB, 0, &MAX_WORK_GROUP_SIZE.x);
	glGetIntegeri_v(GL_MAX_COMPUTE_FIXED_GROUP_SIZE_ARB, 1, &MAX_WORK_GROUP_SIZE.y);
	glGetIntegeri_v(GL_MAX_COMPUTE_FIXED_GROUP_SIZE_ARB, 2, &MAX_WORK_GROUP_SIZE.z);

	glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &vao::SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT);
	glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &vao::UNIFORM_BUFFER_OFFSET_ALIGNMENT);
	glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &vao::MAX_UNIFORM_BUFFER_BINDINGS);
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &MAX_TEXTURE_UNIT_COUNT);
	glGetIntegerv(GL_MIN_MAP_BUFFER_ALIGNMENT, &vao::MIN_MAP_BUFFER_ALIGNMENT);


	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glDepthRange(0.0f, 1.0f);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_TEXTURE_3D);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEBUG_OUTPUT);
}

void gl::initGLEW() {
	puts("Initializing GLEW...");
	glewExperimental = true;
	size_t glew = glewInit();
	if (glew != GLEW_OK) {

		debug::pushError(std::string("/napp::init() - Unable to initialize GLEW (glewInit() return code: %i)/nGLEW Error Log/n %s"
					+ glew) + std::string((const char*)glewGetErrorString(glew)), debug::Error::Severity::Fatal);
		while (!getch()) {}
		exit(glew);
	}
	printf("GLEW %s\n", (const char*)glewGetString(GLEW_VERSION));
}

void gl::initGeneralUniformBuffer()
{
	//contains: projectionMatrix(mat4), viewMatrix(mat4), camera position(vec4)(todo:remove), voxelizationProjection

	size_t generalUniformDataSize = sizeof(float) * (16 + 16 + 4 + 16);

	generalUniformBuffer = vao::createStorage(generalUniformDataSize, nullptr, GL_MAP_WRITE_BIT | vao::MAP_PERSISTENT_FLAGS);
	vao::createStream(generalUniformBuffer, GL_MAP_WRITE_BIT);
	vao::bindStorage(GL_UNIFORM_BUFFER, generalUniformBuffer);
}



void gl::updateGeneralUniformBuffer()
{
	std::vector<float> generalUniformData(52);

	std::memcpy(&generalUniformData[0], glm::value_ptr(camera::main_camera.getProjection()), sizeof(float) * 16);
	std::memcpy(&generalUniformData[16], glm::value_ptr(camera::main_camera.getView()), sizeof(float) * 16);
	std::memcpy(&generalUniformData[32], glm::value_ptr(camera::main_camera.getPos()), sizeof(float) * 3);
	std::memcpy(&generalUniformData[36], glm::value_ptr(voxelization::projectionMatrix), sizeof(float) * 16);
	vao::uploadStorage(generalUniformBuffer, sizeof(float) * 52, &generalUniformData[0]);
}
void gl::initPrimitiveVBO()
{
	/*        Colored-Quad
		  2---3     0,0------1,0
		  |   |      |        |
		  |   |      |        |
		  |   |      |        |
		  0---1     0,1------1,1
		  */
	std::array<float, 8> varr = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
	};

	std::array<size_t, 6> iarr = {
		2, 0, 1, 1, 3, 2
	};
	quadVBO = vao::createStorage(sizeof(float) * 4 * 2, &varr[0], 0);
	quadEBO = vao::createStorage(sizeof(size_t) * 6, &iarr[0], 0);

	float cube_width = 1.0f;
	glm::vec3 cube_verts[8] = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(cube_width, 0.0f, 0.0f),
		glm::vec3(cube_width, 0.0f, cube_width),
		glm::vec3(0.0f, 0.0f, cube_width),

		glm::vec3(0.0f, cube_width , 0.0f),
		glm::vec3(cube_width, cube_width, 0.0f),
		glm::vec3(cube_width , cube_width, cube_width),
		glm::vec3(0.0f, cube_width, cube_width)
	};

	size_t cube_inds[36] = {
		0, 1, 2,
		0, 2, 3,

		0, 4, 5,
		0, 5, 1,

		0, 3, 4,
		3, 7, 4,

		3, 6, 7,
		3, 2, 6,

		1, 5, 6,
		1, 6, 2,

		4, 6, 5,
		4, 7, 6
	};
	cubeVBO = vao::createStorage(sizeof(glm::vec3) * 8, &cube_verts[0], 0);
	cubeEBO = vao::createStorage(sizeof(size_t) * 36, &cube_inds[0], 0);
}

