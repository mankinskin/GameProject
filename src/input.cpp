#include "Input.h"
#include "app.h"
#include "Debug.h"
#include "Camera.h"
#include "glDebug.h"
#include <functional>
#include "Mouse.h"
#include <algorithm>
#include "Functor.h"
#include "Mesh.h"
#include <array>
#include "Text.h"
#include "Lights.h"
#include <glm\glm.hpp>
#include "GUI.h"
#include "Line.h"
#include "Widget.h"
#include <tuple>

/*
button-pipeline
to set up
- bind quads as buttons
- calculateQuadRanges

every frame
- fetch Events
	- iterate through buttons
	- get button state and compare it to previous state
	- if different -> generate event
- check for relevant events/wheater callbacks should be called
	- iterate through all events
	- check buttonID-specific range in functor array
- callButtons
*/

app::Input::KeySignal app::Input::key_esc;
app::Input::KeySignal app::Input::key_c;
app::Input::KeySignal app::Input::key_g;
app::Input::KeySignal app::Input::key_h;
app::Input::KeySignal app::Input::key_i;
app::Input::KeySignal app::Input::key_w;
app::Input::KeySignal app::Input::key_s;
app::Input::KeySignal app::Input::key_a;
app::Input::KeySignal app::Input::key_d;
app::Input::KeySignal app::Input::key_space;
app::Input::KeySignal app::Input::key_z;
app::Input::KeySignal app::Input::key_f;
app::Input::KeySignal app::Input::key_n;
app::Input::KeySignal app::Input::key_j;
app::Input::KeySignal app::Input::key_up;
app::Input::KeySignal app::Input::key_down;
app::Input::KeySignal app::Input::key_left;
app::Input::KeySignal app::Input::key_right;
app::Input::KeySignal app::Input::key_o;
app::Input::KeySignal app::Input::key_l;
app::Input::KeySignal app::Input::key_lshift;
events::ButtonEvents<events::Event> app::Input::lmb;
events::ButtonEvents<events::Event> app::Input::rmb;
size_t app::Input::always;

void app::Input::init()
{
	puts("Initilizing GLFW Input...\n");
	//initializes GLFW input and defines the keys to track
	glfwSetInputMode(app::mainWindow.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetKeyCallback(app::mainWindow.window, key_Callback);
	glfwSetCharCallback(app::mainWindow.window, char_Callback);
	//glfwSetCursorPosCallback(app::mainWindow.window, cursorPosition_Callback);
	glfwSetCursorEnterCallback(app::mainWindow.window, cursorEnter_Callback);
	glfwSetMouseButtonCallback(app::mainWindow.window, mouseKey_Callback);
	glfwSetScrollCallback(app::mainWindow.window, scroll_Callback);
	reserveKeySignals(20);
	key_esc = app::Input::KeySignal(GLFW_KEY_ESCAPE);
	key_c = app::Input::KeySignal(GLFW_KEY_C);
	key_g = app::Input::KeySignal(GLFW_KEY_G);
	key_h = app::Input::KeySignal(GLFW_KEY_H);
	key_i = app::Input::KeySignal(GLFW_KEY_I);
	key_w = app::Input::KeySignal(GLFW_KEY_W);
	key_s = app::Input::KeySignal(GLFW_KEY_S);
	key_a = app::Input::KeySignal(GLFW_KEY_A);
	key_d = app::Input::KeySignal(GLFW_KEY_D);
	key_space = app::Input::KeySignal(GLFW_KEY_SPACE);
	key_z = app::Input::KeySignal(GLFW_KEY_Z);
	key_f = app::Input::KeySignal(GLFW_KEY_F);
	key_n = app::Input::KeySignal(GLFW_KEY_N);
	key_j = app::Input::KeySignal(GLFW_KEY_J);
	key_up = app::Input::KeySignal(GLFW_KEY_UP);
	key_down = app::Input::KeySignal(GLFW_KEY_DOWN);
	key_left = app::Input::KeySignal(GLFW_KEY_LEFT);
	key_right = app::Input::KeySignal(GLFW_KEY_RIGHT);
	key_o = app::Input::KeySignal(GLFW_KEY_O);
	key_l = app::Input::KeySignal(GLFW_KEY_L);
	key_lshift = app::Input::KeySignal(GLFW_KEY_LEFT_SHIFT);
	lmb = events::ButtonEvents<events::Event>(events::createEvent(app::Input::MouseKeyEvent(0, 1)), events::createEvent(app::Input::MouseKeyEvent(0, 0)));
	rmb = events::ButtonEvents<events::Event>(events::createEvent(app::Input::MouseKeyEvent(1, 1)), events::createEvent(app::Input::MouseKeyEvent(1, 0)));
	always = signals::createSignal(signals::Source(true), true);
}

void app::Input::setupControls()
{
	using namespace gui;
	using namespace events;
	using namespace functors;
	using namespace signals;
	using namespace gates;



	{
		FunctorRef<void> toggle_cull_func = createFunctor(mesh::toggleCullFace);
		FunctorRef<void> toggle_cursor_func = createFunctor(app::Input::toggleCursor);
		FunctorRef<void, camera::Camera&> toggle_look_func = createFunctor<void, camera::Camera&>(camera::toggleLook, camera::main_camera);
		FunctorRef<void> toggle_grid_func = createFunctor(glDebug::toggleGrid);
		FunctorRef<void> toggle_coord_func = createFunctor(glDebug::toggleCoord);
		FunctorRef<void> toggle_info_func = createFunctor(debug::togglePrintInfo);
		FunctorRef<void> toggle_normals_func = createFunctor(mesh::toggleNormals);
		FunctorRef<void, camera::Camera&> cycle_modes_func = createFunctor<void, camera::Camera&>(camera::cycleModes, camera::main_camera);
		FunctorRef<void, camera::Camera&, float> higher_cam_speed_func = createFunctor<void, camera::Camera&, float>(camera::setSpeed, camera::main_camera, 1.0f);
		FunctorRef<void, camera::Camera&, float> normal_cam_speed_func = createFunctor<void, camera::Camera&, float>(camera::setSpeed, camera::main_camera, 0.3f);
		
		toggle_cull_func.set_triggers({ key_i.press });
		toggle_cursor_func.set_triggers({ key_c.press, rmb.on, rmb.off });
		toggle_look_func.set_triggers({ key_c.press, rmb.on, rmb.off });
		toggle_grid_func.set_triggers({ key_g.press });
		toggle_coord_func.set_triggers({ key_h.press });
		toggle_info_func.set_triggers({ key_i.press });
		toggle_normals_func.set_triggers({ key_n.press });
		cycle_modes_func.set_triggers({ key_j.press });

		higher_cam_speed_func.set_triggers({ key_lshift.press });
		normal_cam_speed_func.set_triggers({ key_lshift.release });
	}
	
	{//camera
		FunctorRef<void, camera::Camera&> forward_func = createFunctor<void, camera::Camera&>(camera::forward, camera::main_camera);
		FunctorRef<void, camera::Camera&> backward_func = createFunctor<void, camera::Camera&>(camera::back, camera::main_camera);
		FunctorRef<void, camera::Camera&> left_func = createFunctor<void, camera::Camera&>(camera::left, camera::main_camera);
		FunctorRef<void, camera::Camera&> right_func = createFunctor<void, camera::Camera&>(camera::right, camera::main_camera);
		FunctorRef<void, camera::Camera&> up_func = createFunctor<void, camera::Camera&>(camera::up, camera::main_camera);
		FunctorRef<void, camera::Camera&> down_func = createFunctor<void, camera::Camera&>(camera::down, camera::main_camera);

		forward_func.set_triggers({ key_w.hold });
		backward_func.set_triggers({ key_s.hold });
		left_func.set_triggers({ key_a.hold });
		right_func.set_triggers({ key_d.hold });
		up_func.set_triggers({ key_space.hold });
		down_func.set_triggers({ key_z.hold });
	}
}

void app::Input::fetchGLFWEvents()
{
	//updates the states of the mouse buttons, mouse wheel and all tracked keys
	glfwPollEvents();
}


void app::Input::end()
{
	resetMouse();
	if (glfwWindowShouldClose(app::mainWindow.window)) {
		app::state = app::State::Exit;
	}
}