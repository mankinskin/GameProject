#include "gl.h"
#include "debug.h"
#include <ncurses.h>
#include "app.h"
#include <array>
#include "viewport.h"
#include "primitives.h"
#include "camera.h"
#include "gldebug.h"
#include <gtc/type_ptr.hpp>
#include "texture.h"
#include "quad.h"
#include "model.h"
#include "model_loader.h"
#include "framebuffer.h"
#include "lights.h"
#include "entities.h"
#include "mesh.h"
#include "physics.h"
#include "collision.h"
#include "voxelization.h"

int gl::MAX_UNIFORM_BLOCK_SIZE = 0;
int gl::MAX_WORK_GROUP_COUNT = 0;
glm::ivec3 gl::MAX_WORK_GROUP_SIZE = {};
std::vector<std::string> gl::EXTENSION_LIST = {};
int gl::EXTENSIONS_SUPPORTED_NUM = 0;
int gl::OPENGL_VERSION[2] = {};
std::string gl::GLSL_VERSION = "";
std::string gl::SYSTEM_RENDERER = "";
gl::Viewport screenViewport;
int gl::MAX_TEXTURE_UNIT_COUNT;
gl::StreamStorage<glm::vec4> gl::generalUniformBuffer;
gl::VAO gl::screenQuadVAO;
unsigned int gl::screenShaderProgram;

void gl::init()
{
    puts( "Initializing OpenGL..." );
    initGLEW();
    glDebug::init();
    getOpenGLInitValues();
    screenViewport = Viewport( app::mainWindow );
    screenViewport.bind();
    //lights::createLight( glm::vec4( 3.0f, 5.0f, -5.0f, 1.0f ), glm::vec4( 0.0f, 1.0f, 0.0f, 100.0f ) );
    //lights::createLight( glm::vec4( -1.0f, 4.0f, 3.0f, 1.0f ), glm::vec4( 0.0f, 0.0f, 1.0f, 100.0f ) );
    //lights::createLight( glm::vec4( 1.0f, 14.0f, 1.0f, 0.0f ), glm::vec4( 1.0f, 1.0f, 1.0f, 100.0f ) );
    //lights::createLight( glm::vec4( 4.0f, -4.0f, 3.0f, 1.0f ), glm::vec4( 1.0f, 0.0f, 0.0f, 100.0f ) );
    //lights::createLight( glm::vec4( 3.0f, 15.0f, -5.0f, 1.0f ), glm::vec4( 0.0f, 1.0f, 0.0f, 100.0f ) );

    //puts( "Lighting..." );
    //lights::initLighting();

    //puts( "Entities..." );
    //entities::initEntityBuffers();

    //puts( "Models..." );
    //model::initModels();
    //mesh::initMeshVAO();
    //model::setupModels();

    //entities::updateEntityMatrices();
    //model::revalidateModelMeshOffsets();
    //mesh::revalidateMeshEntityOffsets();

    debug::printErrors();
}

void gl::getOpenGLInitValues()
{
    glGetIntegerv( GL_MAJOR_VERSION, &OPENGL_VERSION[0] );
    glGetIntegerv( GL_MINOR_VERSION, &OPENGL_VERSION[1] );
    GLSL_VERSION = std::string( ( char* )glGetString( GL_SHADING_LANGUAGE_VERSION ) );
    SYSTEM_RENDERER = std::string( ( char* )glGetString( GL_RENDERER ) );

    printf( "OpenGL %s.%s\n", std::to_string( OPENGL_VERSION[0] ).c_str(), std::to_string( OPENGL_VERSION[1] ).c_str() );
    printf( "GLSL %s\n", GLSL_VERSION.c_str() );
    printf( "Renderer %s\n", SYSTEM_RENDERER.c_str() );

    glGetIntegerv( GL_NUM_EXTENSIONS, &EXTENSIONS_SUPPORTED_NUM );
    printf( "%d Extensions found.\n", EXTENSIONS_SUPPORTED_NUM );
    EXTENSION_LIST.resize( EXTENSIONS_SUPPORTED_NUM );
    for ( int k = 0; k < EXTENSIONS_SUPPORTED_NUM; ++k ) {
        EXTENSION_LIST[k] = std::string( ( const char* )glGetStringi( GL_EXTENSIONS, k ) );
        //printf( "%s\n", EXTENSION_LIST[k].c_str() );
    }

    glGetIntegeri_v( GL_MAX_COMPUTE_FIXED_GROUP_SIZE_ARB, 0, &MAX_WORK_GROUP_SIZE.x );
    glGetIntegeri_v( GL_MAX_COMPUTE_FIXED_GROUP_SIZE_ARB, 1, &MAX_WORK_GROUP_SIZE.y );
    glGetIntegeri_v( GL_MAX_COMPUTE_FIXED_GROUP_SIZE_ARB, 2, &MAX_WORK_GROUP_SIZE.z );

    glGetIntegerv( GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT );
    glGetIntegerv( GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &UNIFORM_BUFFER_OFFSET_ALIGNMENT );
    glGetIntegerv( GL_MAX_UNIFORM_BUFFER_BINDINGS, &MAX_UNIFORM_BUFFER_BINDINGS );
    glGetIntegerv( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &MAX_TEXTURE_UNIT_COUNT );
    glGetIntegerv( GL_MIN_MAP_BUFFER_ALIGNMENT, &MIN_MAP_BUFFER_ALIGNMENT );
    glGetIntegerv( GL_MAX_UNIFORM_BLOCK_SIZE, &MAX_UNIFORM_BLOCK_SIZE );

    printf( "Max Uniform Block Size:\t%d\n", MAX_UNIFORM_BLOCK_SIZE );
    printf( "Max Uniform Buffer Bindings:\t%d\n", MAX_UNIFORM_BUFFER_BINDINGS );


    glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );
    glClearDepth( 1.0f );
    glDepthRange( 0.0f, 1.0f );
    glEnable( GL_CULL_FACE );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_ALPHA_TEST );
    glEnable( GL_TEXTURE_3D );
    glDepthFunc( GL_LESS );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable( GL_DEBUG_OUTPUT );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
}

void gl::initGLEW() 
{
    puts( "Initializing GLEW..." );
    glewExperimental = true;
    unsigned int glew = glewInit();
    if ( glew != GLEW_OK ) {
        debug::pushError( "Unable to initialize GLEW ( glewInit() return code: " + std::to_string( glew ) + " )\nGLEW Error Log:\n"
                + ( const char* )glewGetErrorString( glew ), debug::Error::Severity::Fatal );
        while ( !getch() ) {
        }
        exit( glew );
    }
    printf( "GLEW %s\n", ( const char* )glewGetString( GLEW_VERSION ) );
}

void gl::initGeneralUniformBuffer()
{
    //contains: projectionMatrix( mat4 ), viewMatrix( mat4 ), camera position( vec4 )( todo:remove ), voxelizationProjection

    unsigned int dataCount = ( 16 + 16 + 4 + 16 );

    generalUniformBuffer = StreamStorage<glm::vec4>( "GeneralUniformBuffer", dataCount, GL_MAP_WRITE_BIT );
    generalUniformBuffer.setTarget( GL_UNIFORM_BUFFER );
}

void gl::updateGeneralUniformBuffer()
{
    static std::vector<float> generalUniformData( 52 );

    std::memcpy( &generalUniformData[0], glm::value_ptr( camera::main_camera.getProjection() ), sizeof( float ) * 16 );
    std::memcpy( &generalUniformData[16], glm::value_ptr( camera::main_camera.getView() ), sizeof( float ) * 16 );
    std::memcpy( &generalUniformData[32], glm::value_ptr( camera::main_camera.getPos() ), sizeof( float ) * 3 );
    std::memcpy( &generalUniformData[36], glm::value_ptr( voxelization::projectionMatrix ), sizeof( float ) * 16 );
    uploadStorage( generalUniformBuffer, sizeof( float ) * 52, &generalUniformData[0] );
}

