#include "sequencer.h"
#include "app.h"
#include "debug.h"
#include "input.h"
#include "mouse.h"
#include "keys.h"
#include "gl.h"
#include "gldebug.h"
#include "primitives.h"
#include "color.h"
#include "camera.h"
#include "framebuffer.h"
#include "shader.h"
#include "gui.h"
#include "line.h"
#include "quad.h"
#include "quadcolors.h"
#include "font.h"
#include "text.h"
#include "functor.h"
#include "signal.h"
#include "simtime.h"
#include "mesh.h"
#include "model.h"
#include "modelfile.h"

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
  gui::initColorQuadShader();
  text::initFontShader();
  //model::mesh::initMeshShader();
  //model::mesh::initBlendMeshShader();
  //model::mesh::initMeshNormalShader();
  //lights::initLightShader();
  //gui::text::initFontShader();
  //voxelization::init();
}

void sequencer::buildShaders()
{
  puts("Building Shaders...");
  gui::setupLineShader();
  gui::setupColorQuadShader();
  text::setupFontShader();
  //model::mesh::setupMeshShader();
  //lights::setupLightShader();
  //model::mesh::setupBlendMeshShader();
  //model::mesh::setupMeshNormalShader();
  //voxelization::setupShader();
}

void sequencer::initializeVAOs()
{
  puts("Primitives...");
  gl::initPrimitives();

  puts("General Uniform Buffer...");
  gl::initGeneralUniformBuffer();

  puts("Lines...");
  gui::initLineVAO();

  puts("Fonts...");
  text::initFontVAO();

  gui::initQuadBuffer();

  gui::initColorQuadVAO();

  //model::mesh::initMeshVAO();
  //model::initModels();
}

void sequencer::initModules()
{
  puts("Framebuffers...");
  texture::initFramebuffers();

  puts("Colors...");
  gl::initColors();

  puts("Input...");
  input::init();

  input::setupControls();

  puts("Debug Geometry...");
  gl::debug::createDebugGeometry();

  puts("GUI...");
  gui::init();

  puts("Widgets...");
  gui::initWidgets();

  puts("Text...");
  text::loadFonts();

  //puts("Models...");
  //model::Loader::includeModel("", "");
  //model::Loader::loadModels();
}

void sequencer::fetchInput()
{
}

void sequencer::clearFramebuffers()
{
  static GLubyte gui_clear_index[4] = { 0, 0, 0, 0 };
  static GLfloat g_clear_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
  static GLfloat g_clear_depth = 1.0f;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearNamedFramebufferuiv(texture::guiFBO,
	  GL_COLOR, 0, (GLuint*) gui_clear_index);
  glClearNamedFramebufferfv(texture::guiFBO,
	  GL_DEPTH, 0, &g_clear_depth);
}

void sequencer::frame()
{
  clearFramebuffers();

  input::fetchGLFWEvents();
  input::updateMouse();

  input::getCursorQuadEvents();
  input::getMouseKeyEvents();
  signals::checkEvents();
  signals::processLinks();

  camera::main_camera.look(input::cursorFrameDelta);
  camera::main_camera.update();

  gl::updateGeneralUniformBuffer();

  gl::updateColorBuffer();
  gui::updateQuadBuffer();
  gui::updateColorQuads();

  gui::updateLinePositions();
  gui::updateLineColors();
  text::updateFonts();

  gui::renderLines();
  gui::renderColorQuads();
  text::renderFonts();

  glfwSwapBuffers(app::mainWindow.window);

  input::end();

  simtime::update();

  debug::printErrors();
}

void sequencer::gameloop()
{
  puts("Entering gameloop");
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  text::Text::ID term_box = text::Text::all.makeID(text::Text(glm::vec2(0.0f, 0.90f), glm::vec2(0.5f, 1.0f)));
  term_box->setChars("1.\tfirst item\n2.\tsecond item\n3.\tthird item\n...\n10.\ttenth item");
  text::updateTexts();
  while (app::state == app::Running) {
	frame();
  }
  signals::clearSignals();
  signals::clearFunctors();
}

