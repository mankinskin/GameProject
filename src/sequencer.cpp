#include "sequencer.h"

#include "Shader_loader.h"
#include "shader.h"
#include "gui.h"
#include "color.h"
#include "line.h"
#include "quadcolors.h"
#include "gl.h"
#include "primitives.h"
#include "app.h"
#include "input.h"
#include "camera.h"
#include "gldebug.h"
#include "mouse.h"
#include "keys.h"
#include "functor.h"
#include "debug.h"
#include "text.h"
#include "quad.h"
#include "quadindex.h"
#include "framebuffer.h"

// Initialization
void sequencer::initialize()
{
    includeShaders();
    initializeVAOs();
    initModules();
    buildShaders();
}
void sequencer::includeShaders()
{
    gui::initLineShader();
    gui::initColorquadShader();
    gui::initQuadIndexShader();
    //mesh::initmeshShader();
    //mesh::initBlendmeshShader();
    //mesh::initMeshNormalShader();
    //lights::initLightShader();
    //gui::text::initfontShader();
    //voxelization::init();
}

void sequencer::buildShaders()
{
	puts( "Building Shaders..." );
    Shader::Loader::buildShaderPrograms();
	gui::setupLineShader();
	gui::setupColorquadShader();
	gui::setupQuadIndexShader();
	//mesh::setupmeshShader();
	//lights::setupLightShader();
	//mesh::setupBlendmeshShader();
	//mesh::setupMeshNormalShader();
	//voxelization::setupShader();
}

void sequencer::initializeVAOs()
{
	puts( "Primitives..." );
    gl::initPrimitives();

	puts( "General Uniform Buffer..." );
    gl::initGeneralUniformBuffer();

	puts( "Lines..." );
	gui::initLineVAO();

    gui::initQuadBuffer();

    gui::initColorQuadVAO();	

    gui::initQuadIndexBuffer();
}

void sequencer::initModules()
{
	puts( "Framebuffers..." );
	texture::initFramebuffers();

    puts( "Colors..." );
    gl::initColors();

	puts( "Input..." );
    input::init();

	puts( "Camera..." );
	camera::main_camera.init();

    input::setupControls();

	puts( "Debug Geometry..." );
    glDebug::createDebugGeometry();

    puts( "GUI..." );
    gui::init();

    puts( "Widgets..." );
    gui::initWidgets();
}

void sequencer::fetchInput()
{

}

void sequencer::clearFramebuffers()
{
    static GLubyte gui_clear_index[4] = { 0, 0, 0, 0 };
    static GLfloat g_clear_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    static GLfloat g_clear_depth = 1.0f;
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glClearNamedFramebufferuiv( texture::guiFBO, 
            GL_COLOR, 0, ( GLuint* ) gui_clear_index );
    glClearNamedFramebufferfv( texture::guiFBO, 
            GL_DEPTH, 0, &g_clear_depth );
}
void sequencer::frame()
{
    clearFramebuffers();

    input::fetchGLFWEvents();
    input::updateMouse();

    input::getCursorQuadEvents();
    input::getMouseKeyEvents();
    events::checkEvents();
    signals::checkSignals();
    functors::callFunctors();

    camera::main_camera.look( input::cursorFrameDelta );
    camera::main_camera.update();

    gl::updateGeneralUniformBuffer();

    gl::updateColorBuffer();
    gui::updateQuadBuffer();
    gui::updateColorQuads();

    gui::updateLinePositions();
    gui::updateLineColors();

    // RENDERING
    glBindFramebuffer( GL_FRAMEBUFFER, texture::guiFBO );
    gui::rasterQuadIndices();
    gui::readQuadIndexBuffer();
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    gui::renderLines();
    gui::renderColorQuads();

    glfwSwapBuffers(app::mainWindow.window );

    events::resetEvents();
    signals::resetSignals();
    input::end();

    app::updateTime();
    app::limitFPS();

    debug::printErrors();
}
void sequencer::gameloop()
{	
    puts( "Entering gameloop" );

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    while ( app::state == app::Running ) 
    {
        frame();
    }

    while ( app::state == app::Running ) 
    {
        fetchInput();
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        //entities::translate( 0, node_mov * 0.01f );
        //node_mov = glm::vec3();

        camera::main_camera.look( input::cursorFrameDelta );
        camera::main_camera.update();

        gl::updateGeneralUniformBuffer();
        gl::updateColorBuffer();
        //lights::updateLightIndexRangeBuffer();
        //lights::updateLightDataBuffer();
        //entities::updateEntityMatrices();
        //entities::updateEntityBuffers();
        //mesh::updateMeshBuffers();

        //gui::text::updateCharStorage();

        //reset g_buffer
        //glClearNamedFramebufferfv( texture::gBuffer, GL_COLOR, 0, g_clear_color );
        //glClearNamedFramebufferfv( texture::gBuffer, GL_COLOR, 1, g_clear_color );
        //glClearNamedFramebufferfv( texture::gBuffer, GL_COLOR, 2, g_clear_color );
        //glClearNamedFramebufferfv( texture::gBuffer, GL_COLOR, 3, g_clear_color );
        //glClearNamedFramebufferfv( texture::gBuffer, GL_COLOR, 4, g_clear_color );
        //glClearNamedFramebufferfv( texture::gBuffer, GL_DEPTH, 0, &g_clear_depth );
        //clear screen buffer
        //voxelization::clearVolumeTexture();
        //glClearNamedFramebufferfv( 0, GL_COLOR, 0, g_clear_color );
        //glClearNamedFramebufferfv( 0, GL_DEPTH, 0, &g_clear_depth );
        ////reset guiFBO

        //voxelization::voxelizeMeshes();
        //glBindFramebuffer( GL_FRAMEBUFFER, texture::gBuffer );
        //mesh::renderMeshes();

        //glBindFramebuffer( GL_READ_FRAMEBUFFER, texture::gBuffer );

        //glBlitFramebuffer( 0, 0, gl::Viewport::current->width, gl::Viewport::current->height, 0, 0, gl::Viewport::current->width, gl::Viewport::current->height, GL_DEPTH_BUFFER_BIT, GL_NEAREST );

        //glBindFramebuffer( GL_FRAMEBUFFER, 0 );
        //lights::renderLights();

        //mesh::renderMeshNormals();

        //gui::text::renderGlyphs();

        glfwSwapBuffers( app::mainWindow.window );

        debug::printErrors();
        //updateTime();
        //updateTimeFactor();
        //limitFPS();
        //debug::printInfo();
    }
    gui::text::clearCharStorage();
    functors::clearFunctors();
    signals::clearSignals();
}
