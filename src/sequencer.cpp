#include "sequencer.h"

#include "shader_loader.h"
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

// Initialization
void sequencer::includeShaders()
{
    gui::initLineShader();
    gui::initColorQuadShader();
    //mesh::initMeshShader();
    //mesh::initBlendMeshShader();
    //mesh::initMeshNormalShader();
    //lights::initLightShader();
    //gui::initQuadIndexShader();
    //gui::text::initFontShader();
    //voxelization::init();
}

void sequencer::initializeVAOs()
{
	puts( "Primitives..." );
    gl::initPrimitiveVBO();

	puts( "General Uniform Buffer..." );
    gl::initGeneralUniformBuffer();

    puts( "Colors..." );
    gl::initColors();

	puts( "GUI..." );
	gui::init();

	puts( "Lines..." );
	gui::initLineVAO();
}

void sequencer::buildShaders()
{
	puts( "Building Shaders..." );
    shader::Loader::buildShaderPrograms();
	gui::setupLineShader();
	gui::setupColorQuadShader();
	//mesh::setupMeshShader();
	//lights::setupLightShader();
	//mesh::setupBlendMeshShader();
	//mesh::setupMeshNormalShader();
	//gui::setupQuadIndexShader();
	//voxelization::setupShader();
}

void sequencer::initModules()
{
	puts( "Input..." );
    input::init();

	puts( "Camera..." );
	camera::main_camera.init();

    input::setupControls();

	puts( "Debug Geometry..." );
    glDebug::createDebugGeometry();

	puts( "Widgets..." );
	gui::initWidgets();
}

void sequencer::fetchInput()
{
	input::updateMouse();
	input::fetchGLFWEvents();

	//input::getCursorQuad();
	input::getMouseKeyEvents();
	events::checkEvents();
	signals::checkSignals();
	functors::callFunctors();

	events::resetEvents();
	signals::resetSignals();
	input::end();
}

void sequencer::gameloop()
{	
	puts( "Entering gameloop" );
	GLuint gui_clear_index[4] = { 0, 0, 0, 0 };
	GLfloat g_clear_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	GLfloat g_clear_depth = 1.0f;

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	while ( app::state == app::Running ) {
		fetchInput();

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		camera::main_camera.look( input::cursorFrameDelta );
		camera::main_camera.update();

		gl::updateGeneralUniformBuffer();
        gl::updateColorBuffer();

		gui::updateLineBuffers();

		gui::updateQuadBuffer();
	    gui::updateColorQuads();

		gui::renderLines();

		gui::renderColorQuads();

		glfwSwapBuffers( app::mainWindow.window );

        debug::printErrors();
    }
    app::run();

	while ( app::state == app::Running ) {
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
		//glClearNamedFramebufferuiv( texture::guiFBO, GL_COLOR, 0, gui_clear_index );
		//glClearNamedFramebufferfv( texture::guiFBO, GL_DEPTH, 0, &g_clear_depth );

		//glBindFramebuffer( GL_FRAMEBUFFER, texture::guiFBO );
		//gui::rasterQuadIndices();
		//gui::readQuadIndexBuffer();
		//glBindFramebuffer( GL_FRAMEBUFFER, 0 );
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


	gui::clearQuads();
	functors::clearFunctors();
	signals::clearSignals();
}
