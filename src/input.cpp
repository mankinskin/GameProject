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
#include "signal.h"
#include "signal.h"
#include "signal.h"
#include "utils/id.h"
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
   - check for relevant signals/wheater callbacks should be called
   - iterate through all signals
   - check buttonID-specific range in func array
   - callButtons
   */

void input::init()
{
    puts("Setting GLFW Callbacks...");
    //initializes GLFW input and defines the keys to track
    glfwSetInputMode(app::mainWindow.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetKeyCallback(app::mainWindow.window, key_Callback);
    glfwSetCharCallback(app::mainWindow.window, char_Callback);
    //glfwSetCursorPosCallback(app::mainWindow.window, cursorPosition_Callback);
    glfwSetCursorEnterCallback(app::mainWindow.window, cursorEnter_Callback);
    glfwSetMouseButtonCallback(app::mainWindow.window, mouseKey_Callback);
    glfwSetScrollCallback(app::mainWindow.window, scroll_Callback);
}

void input::setupControls()
{
    using namespace gui;
    using namespace signals;
    using namespace gates;

    puts("Initializing Mouse Listeners...");
    Mouse::lmb = utils::makeID(MouseKeySignals(0));
    Mouse::rmb = utils::makeID(MouseKeySignals(1));
    Mouse::mmb = utils::makeID(MouseKeySignals(2));

    puts("Initializing Key Listeners...");
    key_esc = utils::makeID(KeySignals(GLFW_KEY_ESCAPE));
    key_c = utils::makeID(KeySignals(GLFW_KEY_C));
    key_g = utils::makeID(KeySignals(GLFW_KEY_G));
    key_h = utils::makeID(KeySignals(GLFW_KEY_H));
    key_i = utils::makeID(KeySignals(GLFW_KEY_I));
    key_w = utils::makeID(KeySignals(GLFW_KEY_W));
    key_s = utils::makeID(KeySignals(GLFW_KEY_S));
    key_a = utils::makeID(KeySignals(GLFW_KEY_A));
    key_d = utils::makeID(KeySignals(GLFW_KEY_D));
    key_space = utils::makeID(KeySignals(GLFW_KEY_SPACE));
    key_lshift = utils::makeID(KeySignals(GLFW_KEY_LEFT_SHIFT));
    key_z = utils::makeID(KeySignals(GLFW_KEY_Z));
    key_f = utils::makeID(KeySignals(GLFW_KEY_F));
    key_n = utils::makeID(KeySignals(GLFW_KEY_N));
    key_j = utils::makeID(KeySignals(GLFW_KEY_J));
    key_up = utils::makeID(KeySignals(GLFW_KEY_UP));
    key_down = utils::makeID(KeySignals(GLFW_KEY_DOWN));
    key_left = utils::makeID(KeySignals(GLFW_KEY_LEFT));
    key_right = utils::makeID(KeySignals(GLFW_KEY_RIGHT));
    key_o = utils::makeID(KeySignals(GLFW_KEY_O));
    key_l = utils::makeID(KeySignals(GLFW_KEY_L));
    key_lshift = utils::makeID(KeySignals(GLFW_KEY_LEFT_SHIFT));
    key_x = utils::makeID(KeySignals(GLFW_KEY_X));

    {
        auto exit_func = func(app::quit);
        auto toggle_cull_func = func(mesh::toggleCullFace);
        auto toggle_cursor_func = func(input::toggleCursor);
        auto toggle_look_func = func<void(camera::Camera&), camera::Camera&>(camera::toggleLook, camera::main_camera);
        auto toggle_grid_func = func(glDebug::toggleGrid);
        auto toggle_coord_func = func(glDebug::toggleCoord);
        auto toggle_info_func = func(debug::togglePrintInfo);
        auto toggle_normals_func = func(mesh::toggleNormals);
        auto cycle_cam_modes_func = func<void(camera::Camera&), camera::Camera&>(camera::cycleModes, camera::main_camera);
        auto higher_cam_speed_func = func<void(camera::Camera&, float), camera::Camera&, float>(camera::setSpeed, camera::main_camera, 1.0f);
        auto normal_cam_speed_func = func<void(camera::Camera&, float), camera::Camera&, float>(camera::setSpeed, camera::main_camera, 0.3f);

        link(ifAny(key_c->on, Mouse::rmb->on, Mouse::rmb->off), toggle_look_func);
        link(ifAny(key_c->on, Mouse::rmb->on, Mouse::rmb->off), toggle_cursor_func);
        link(key_esc->on, exit_func);
        link(key_g->on, toggle_grid_func);
        link(key_h->on, toggle_coord_func);
        link(key_n->on, toggle_normals_func);
        link(key_j->on, cycle_cam_modes_func);

        //higher_cam_speed_func.add_triggers({ key_lshift->on });
        //normal_cam_speed_func.add_triggers({ key_lshift->off });
    }

    {//camera
        auto forward_func = func<void(camera::Camera&), camera::Camera&>(camera::forward, camera::main_camera);
        auto backward_func = func<void(camera::Camera&), camera::Camera&>(camera::back, camera::main_camera);
        auto left_func = func<void(camera::Camera&), camera::Camera&>(camera::left, camera::main_camera);
        auto right_func = func<void(camera::Camera&), camera::Camera&>(camera::right, camera::main_camera);
        auto off_func = func<void(camera::Camera&), camera::Camera&>(camera::up, camera::main_camera);
        auto on_func = func<void(camera::Camera&), camera::Camera&>(camera::down, camera::main_camera);

        link(key_w->on, forward_func);
        link(key_w->off, backward_func);
        link(key_s->on, backward_func);
        link(key_s->off, forward_func);
        link(key_a->on, left_func);
        link(key_a->off, right_func);
        link(key_d->on, right_func);
        link(key_d->off, left_func);
        link(key_space->on, off_func);
        link(key_space->off, on_func);
        link(key_lshift->on, on_func);
        link(key_lshift->off, off_func);
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
    if (glfwWindowShouldClose(app::mainWindow.window)) {
        app::state = app::State::Exit;
    }
}
