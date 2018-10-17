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
#include "nodes.h"

// Initialization
void sequencer::initialize()
{
  includeShaders();
  initModules();
  initializeVAOs();
  buildShaders();
}
void sequencer::includeShaders()
{
  gui::initLineShader();
  gui::initColorQuadShader();
  text::initFontShader();
  model::mesh::initMeshShader();
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
  model::mesh::setupMeshShader();
  //lights::setupLightShader();
  //model::mesh::setupBlendMeshShader();
  //model::mesh::setupMeshNormalShader();
  //voxelization::setupShader();
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

  puts("Widgets...");
  gui::initWidgets();

  puts("Text...");
  text::loadFonts();

  puts("Models...");
  model::initModels();
  model::setupModels();
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

  nodes::initNodeBuffers();

  gui::initQuadBuffer();

  gui::initColorQuadVAO();

  model::mesh::initMeshVAO();
}

void sequencer::clearFramebuffers()
{
  static const GLubyte gui_clear_index[4] = { 0, 0, 0, 0 };
  static const GLfloat g_clear_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
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

  input::fetch();
  camera::main_camera.look(input::Cursor::frameDelta);
  camera::main_camera.update();

  gl::updateGeneralUniformBuffer();

  gl::updateColorBuffer();
  gui::updateQuadBuffer();
  gui::updateColorQuads();
  gui::updateLinePositions();
  gui::updateLineColors();
  text::updateFonts();

  nodes::updateNodeBuffers();
  model::mesh::updateMeshBuffers();

  gui::renderLines();
  model::mesh::renderMeshes();
  // GUI
  gui::draw();

  glfwSwapBuffers(app::mainWindow.window);
  input::end();
  simtime::update();
  debug::printErrors();
}

void sequencer::gameloop()
{
  puts("Entering gameloop");
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  text::Textbox::ID term_box = text::Textbox::all.makeID(text::Textbox(glm::vec2(0.0f, 0.90f), glm::vec2(0.5f, 1.0f)));
  term_box->setChars("1.\tfirst item\n2.\tsecond item\n3.\tthird item\n...\n10.\ttenth item");
  text::updateTextboxes();
  while (app::state == app::Running) {
	frame();
  }
  signals::clearSignals();
  signals::clearFunctors();
}

