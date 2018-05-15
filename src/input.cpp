#include "input.h"
#include "app.h"
#include "debug.h"
#include "camera.h"
#include "gldebug.h"
#include <functional>
#include "mouse.h"
#include <algorithm>
#include "functor.h"
#include "mesh.h"
#include <array>
#include "lights.h"
#include <glm.hpp>
#include "gui.h"
#include "line.h"
#include "widget.h"
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

input::KeySignal input::key_esc;
input::KeySignal input::key_c;
input::KeySignal input::key_g;
input::KeySignal input::key_h;
input::KeySignal input::key_i;
input::KeySignal input::key_w;
input::KeySignal input::key_s;
input::KeySignal input::key_a;
input::KeySignal input::key_d;
input::KeySignal input::key_space;
input::KeySignal input::key_z;
input::KeySignal input::key_f;
input::KeySignal input::key_n;
input::KeySignal input::key_j;
input::KeySignal input::key_up;
input::KeySignal input::key_down;
input::KeySignal input::key_left;
input::KeySignal input::key_right;
input::KeySignal input::key_o;
input::KeySignal input::key_l;
input::KeySignal input::key_lshift;
input::KeySignal input::key_x;
events::ButtonEvents<events::Event> input::lmb;
events::ButtonEvents<events::Event> input::rmb;
unsigned int input::always;

void input::init()
{
	puts( "Setting GLFW Callbacks..." );
	//initializes GLFW input and defines the keys to track
	glfwSetInputMode( app::mainWindow.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
	glfwSetKeyCallback( app::mainWindow.window, key_Callback );
	glfwSetCharCallback( app::mainWindow.window, char_Callback );
	//glfwSetCursorPosCallback( app::mainWindow.window, cursorPosition_Callback );
	glfwSetCursorEnterCallback( app::mainWindow.window, cursorEnter_Callback );
	glfwSetMouseButtonCallback( app::mainWindow.window, mouseKey_Callback );
	glfwSetScrollCallback( app::mainWindow.window, scroll_Callback );
}

void input::setupControls()
{
	using namespace gui;
	using namespace events;
	using namespace functors;
	using namespace signals;
	using namespace gates;

	reserveKeySignals( 20 );
	puts( "Initializing Key Listeners..." );
	key_esc = input::KeySignal( GLFW_KEY_ESCAPE );
	key_c = input::KeySignal( GLFW_KEY_C );
	key_g = input::KeySignal( GLFW_KEY_G );
	key_h = input::KeySignal( GLFW_KEY_H );
	key_i = input::KeySignal( GLFW_KEY_I );
	key_w = input::KeySignal( GLFW_KEY_W );
	key_s = input::KeySignal( GLFW_KEY_S );
	key_a = input::KeySignal( GLFW_KEY_A );
	key_d = input::KeySignal( GLFW_KEY_D );
	key_space = input::KeySignal( GLFW_KEY_SPACE );
	key_z = input::KeySignal( GLFW_KEY_Z );
	key_f = input::KeySignal( GLFW_KEY_F );
	key_n = input::KeySignal( GLFW_KEY_N );
	key_j = input::KeySignal( GLFW_KEY_J );
	key_up = input::KeySignal( GLFW_KEY_UP );
	key_down = input::KeySignal( GLFW_KEY_DOWN );
	key_left = input::KeySignal( GLFW_KEY_LEFT );
	key_right = input::KeySignal( GLFW_KEY_RIGHT );
	key_o = input::KeySignal( GLFW_KEY_O );
	key_l = input::KeySignal( GLFW_KEY_L );
	key_lshift = input::KeySignal( GLFW_KEY_LEFT_SHIFT );
	key_x = input::KeySignal( GLFW_KEY_X );
	lmb = events::ButtonEvents<events::Event>( events::createEvent( input::MouseKeyEvent( 0, 1 ) ), events::createEvent( input::MouseKeyEvent( 0, 0 ) ) );
	rmb = events::ButtonEvents<events::Event>( events::createEvent( input::MouseKeyEvent( 1, 1 ) ), events::createEvent( input::MouseKeyEvent( 1, 0 ) ) );
	always = signals::createSignal( signals::Source( true ), true );

	{
		FunctorRef<void> toggle_cull_func = createFunctor( mesh::toggleCullFace );
		FunctorRef<void> toggle_cursor_func = createFunctor( input::toggleCursor );
		FunctorRef<void, camera::Camera&> toggle_look_func = createFunctor<void, camera::Camera&>( camera::toggleLook, camera::main_camera );
		FunctorRef<void> toggle_grid_func = createFunctor( glDebug::toggleGrid );
		FunctorRef<void> toggle_coord_func = createFunctor( glDebug::toggleCoord );
		FunctorRef<void> toggle_info_func = createFunctor( debug::togglePrintInfo );
		FunctorRef<void> toggle_normals_func = createFunctor( mesh::toggleNormals );
		FunctorRef<void, camera::Camera&> cycle_modes_func = createFunctor<void, camera::Camera&>( camera::cycleModes, camera::main_camera );
		FunctorRef<void, camera::Camera&, float> higher_cam_speed_func = createFunctor<void, camera::Camera&, float>( camera::setSpeed, camera::main_camera, 1.0f );
		FunctorRef<void, camera::Camera&, float> normal_cam_speed_func = createFunctor<void, camera::Camera&, float>( camera::setSpeed, camera::main_camera, 0.3f );

		toggle_cull_func.set_triggers( { key_i.press } );
		toggle_cursor_func.set_triggers( { key_c.press, rmb.on, rmb.off } );
		toggle_look_func.set_triggers( { key_c.press, rmb.on, rmb.off } );
		toggle_grid_func.set_triggers( { key_g.press } );
		toggle_coord_func.set_triggers( { key_h.press } );
		toggle_info_func.set_triggers( { key_i.press } );
		toggle_normals_func.set_triggers( { key_n.press } );
		cycle_modes_func.set_triggers( { key_j.press } );

		higher_cam_speed_func.set_triggers( { key_lshift.press } );
		normal_cam_speed_func.set_triggers( { key_lshift.release } );
	}

	{//camera
		FunctorRef<void, camera::Camera&> forward_func = createFunctor<void, camera::Camera&>( camera::forward, camera::main_camera );
		FunctorRef<void, camera::Camera&> backward_func = createFunctor<void, camera::Camera&>( camera::back, camera::main_camera );
		FunctorRef<void, camera::Camera&> left_func = createFunctor<void, camera::Camera&>( camera::left, camera::main_camera );
		FunctorRef<void, camera::Camera&> right_func = createFunctor<void, camera::Camera&>( camera::right, camera::main_camera );
		FunctorRef<void, camera::Camera&> up_func = createFunctor<void, camera::Camera&>( camera::up, camera::main_camera );
		FunctorRef<void, camera::Camera&> down_func = createFunctor<void, camera::Camera&>( camera::down, camera::main_camera );

		forward_func.set_triggers( { key_w.hold } );
		backward_func.set_triggers( { key_s.hold } );
		left_func.set_triggers( { key_a.hold } );
		right_func.set_triggers( { key_d.hold } );
		up_func.set_triggers( { key_space.hold } );
		down_func.set_triggers( { key_z.hold } );
	}
}

void input::fetchGLFWEvents()
{
	//updates the states of the mouse buttons, mouse wheel and all tracked keys
	glfwPollEvents();
}


void input::end()
{
	resetMouse();
	if ( glfwWindowShouldClose( app::mainWindow.window ) ) {
		app::state = app::State::Exit;
	}
}
