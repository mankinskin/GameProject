#include "input.h"
#include "mouse.h"
#include "app.h"
#include "debug.h"
#include "camera.h"
#include "gldebug.h"
#include "functor.h"
#include "mesh.h"
#include "light.h"
#include "gui.h"
#include "line.h"
#include "widget.h"
#include "action.h"
#include <tuple>
#include <functional>
#include <algorithm>
#include <array>
#include <glm.hpp>

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
    using namespace events;
    using namespace gates;

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

    {
        FunctorID toggle_cull_func = createFunctor( mesh::toggleCullFace );
        FunctorID toggle_cursor_func = createFunctor( input::toggleCursor );
        FunctorID toggle_look_func = createFunctor<void, camera::Camera&>( camera::toggleLook, camera::main_camera );
        FunctorID toggle_grid_func = createFunctor( glDebug::toggleGrid );
        FunctorID toggle_coord_func = createFunctor( glDebug::toggleCoord );
        FunctorID toggle_info_func = createFunctor( debug::togglePrintInfo );
        FunctorID toggle_normals_func = createFunctor( mesh::toggleNormals );
        FunctorID cycle_modes_func = createFunctor<void, camera::Camera&>( camera::cycleModes, camera::main_camera );
        FunctorID higher_cam_speed_func = createFunctor<void, camera::Camera&, float>( camera::setSpeed, camera::main_camera, 1.0f );
        FunctorID normal_cam_speed_func = createFunctor<void, camera::Camera&, float>( camera::setSpeed, camera::main_camera, 0.3f );


        events::Action::ID toggle_look( events::Action( toggle_look_func, key_c.press ) );
        events::Action::ID toggle_cursor( events::Action( toggle_cursor_func, key_c.press ) );
        //toggle_cull_func.add_triggers( { key_i.press } );
        //toggle_cursor_func.add_triggers( { key_c.press } );
        //toggle_look_func.add_triggers( { key_c.press } );
        //toggle_grid_func.add_triggers( { key_g.press } );
        //toggle_coord_func.add_triggers( { key_h.press } );
        //toggle_info_func.add_triggers( { key_i.press } );
        //toggle_normals_func.add_triggers( { key_n.press } );
        //cycle_modes_func.add_triggers( { key_j.press } );

        //higher_cam_speed_func.add_triggers( { key_lshift.press } );
        //normal_cam_speed_func.add_triggers( { key_lshift.release } );
    }

    {//camera
        FunctorID forward_func = createFunctor( camera::forward, camera::main_camera );
        FunctorID backward_func = createFunctor( camera::back, camera::main_camera );
        FunctorID left_func = createFunctor( camera::left, camera::main_camera );
        FunctorID right_func = createFunctor( camera::right, camera::main_camera );
        FunctorID up_func = createFunctor( camera::up, camera::main_camera );
        FunctorID down_func = createFunctor( camera::down, camera::main_camera );

        ListenerID w_press = listenForEvent( KeyEvent( GLFW_KEY_W, 1 ) );

        events::Action::ID forward( events::Action( forward_func, w_press ) );
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
