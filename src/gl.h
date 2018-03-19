#pragma once
#include <glew.h>
#include <string>
#include <vector>
#include <glm.hpp>
#include "contextwindow.h"

namespace gl {
    
    struct Point3D{
        Point3D( glm::vec4 position )
            :pos( position ){}
        Point3D( float x, float y, float z, float w = 1.0f )
            :pos( glm::vec4( x, y, z, w ) ){}

        glm::vec4 pos;
    };

	void init();
	void loadShaders();
	void bindUniformBufferLocations();
	void setViewport( app::Window& pViewport );
	void getOpenGLInitValues();
	void initGLEW();

	void updateGeneralUniformBuffer();
	void initGeneralUniformBuffer();

	extern int MAX_WORK_GROUP_COUNT;
	extern glm::ivec3 MAX_WORK_GROUP_SIZE;
    extern int MAX_TEXTURE_UNIT_COUNT;
    extern int MAX_UNIFORM_BLOCK_SIZE;

	extern int OPENGL_VERSION[2];
	extern std::string GLSL_VERSION;
	extern std::string SYSTEM_RENDERER;
	extern std::vector<std::string> EXTENSION_LIST;
	extern int EXTENSIONS_SUPPORTED_NUM;

	extern unsigned int generalUniformBuffer;
	extern unsigned int screenShaderProgram;
	extern unsigned int screenQuadVAO;

}
