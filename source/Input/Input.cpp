#include "..\Global\stdafx.h"
#include "stdafx.h"
#include "Input.h"
#include "..\Global/app.h"
#include "..\Global/Debug.h"
#include "../BaseGL/camera.h"
#include "../GlobalGL\glDebug.h"
#include <functional>
#include "Keys.h"
#include "Mouse.h"
#include <algorithm>
#include "Event.h"
#include "Functor.h"
#include "../Mesh/Mesh.h"
#include <array>
#include "../GUI/Text/Text.h"
#include "../Lighting/Lights.h"

#include "../GUI/UI/GUI.h"
#include "../GUI/UI/Widget.h"
#include "../GUI/UI/QuadLayout.h"
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

}

void app::Input::setupControls()
{
	using namespace gui;

	size_t lmb_press = createEvent(MouseKeyEvent(0, 1, 0));
	size_t lmb_press_sig = createSignal(EventSource(lmb_press));
	size_t lmb_release = createEvent(MouseKeyEvent(0, 0, 0));
	size_t lmb_release_sig = createSignal(EventSource(lmb_release));

	size_t lmb_hold = createSignal(toggle_gate<or_gate<EventSource, EventSource>>(or_gate<EventSource, EventSource>(EventSource(createEvent(MouseKeyEvent(0, 1, 0))), EventSource(createEvent(MouseKeyEvent(0, 0, 0))))));
	
	reserveKeySignals(11);
	KeySignal key_esc(GLFW_KEY_ESCAPE);
	KeySignal key_c(GLFW_KEY_C);
	KeySignal key_g(GLFW_KEY_G);
	KeySignal key_h(GLFW_KEY_H);
	KeySignal key_i(GLFW_KEY_I);
	KeySignal key_w(GLFW_KEY_W);
	KeySignal key_s(GLFW_KEY_S);
	KeySignal key_a(GLFW_KEY_A);
	KeySignal key_d(GLFW_KEY_D);
	KeySignal key_f(GLFW_KEY_F);
	KeySignal key_n(GLFW_KEY_N);
	KeySignal key_j(GLFW_KEY_J);
	KeySignal key_space(GLFW_KEY_SPACE);
	KeySignal key_z(GLFW_KEY_Z);
	KeySignal key_up(GLFW_KEY_UP);
	KeySignal key_down(GLFW_KEY_DOWN);
	KeySignal key_left(GLFW_KEY_LEFT);
	KeySignal key_right(GLFW_KEY_RIGHT);
	KeySignal key_o(GLFW_KEY_O);
	KeySignal key_l(GLFW_KEY_L);
	

	using Button = QuadGroup<2>;
	using ButtonColors = WidgetColors<ConstColor, ConstColor >;

	QuadLayout<2, 6> button_layout({ 0.0f, 0.0f, gui::pixel_size.x*100, gui::pixel_size.y*25, gui::pixel_size.x*2, gui::pixel_size.y*2 });

	button_layout.setQuadInitialization<0>({
		1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 2.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 2.0f });
	button_layout.setQuadInitialization<1>({
		1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f });

	ButtonColors buttonColors = ButtonColors(ConstColor("nocolor"), ConstColor("darkgrey"));
	
	Button playButton = button_layout.create();
	playButton.move(gui::pixel_round(glm::vec2(-0.9f, -0.5f)));
	
	Button quitButton = button_layout.create();
	quitButton.move(gui::pixel_round(glm::vec2(-0.9f, -0.6f)));
	
	playButton.color(buttonColors);
	quitButton.color(buttonColors);

	size_t enter_quit_button_border = createEvent(QuadEvent(quitButton.element<0>().index, 1));
	size_t leave_quit_button_border = createEvent(QuadEvent(quitButton.element<0>().index, 0));

	size_t enter_quit_button_center = createEvent(QuadEvent(quitButton.element<1>().index, 1));
	size_t leave_quit_button_center = createEvent(QuadEvent(quitButton.element<1>().index, 0));
	
	size_t quit_button_press = 
		createSignal(and_gate<toggle_gate<or_gate<EventSource, EventSource>>, EventSource>(toggle_gate<or_gate<EventSource, EventSource>>(or_gate<EventSource, EventSource>(enter_quit_button_border, enter_quit_button_center)), lmb_press));

	{
		FunctorRef<size_t, ConstColor> light_button = createFunctor(gui::colorQuad<ConstColor>, playButton.element<1>().index, ConstColor("white"));
		FunctorRef<size_t, ConstColor> unlight_button = createFunctor(gui::colorQuad<ConstColor>, playButton.element<1>().index, ConstColor("darkgrey"));
		light_button.setTriggers({ lmb_press_sig });
		unlight_button.setTriggers({ lmb_release_sig });
	}
	{
		FunctorRef<size_t, ConstColor> light_button = createFunctor(gui::colorQuad<ConstColor>, quitButton.element<1>().index, ConstColor("white"));
		FunctorRef<size_t, ConstColor> unlight_button = createFunctor(gui::colorQuad<ConstColor>, quitButton.element<1>().index, ConstColor("darkgrey"));
		light_button.setTriggers({ lmb_press_sig });
		unlight_button.setTriggers({ lmb_release_sig });
	}
	//general functions

	FunctorRef<> toggle_cull_func = createFunctor(mesh::toggleCullFace);
	FunctorRef<> toggle_cursor_func = createFunctor(app::Input::toggleCursor);
	FunctorRef<camera::Camera&> toggle_look_func = createFunctor<camera::Camera&>(camera::toggleLook, camera::main_camera);
	FunctorRef<> toggle_grid_func = createFunctor(glDebug::toggleGrid);
	FunctorRef<> toggle_coord_func = createFunctor(glDebug::toggleCoord);
	FunctorRef<> toggle_info_func = createFunctor(debug::togglePrintInfo);
	FunctorRef<> toggle_normals_func = createFunctor(mesh::toggleNormals);
	FunctorRef<camera::Camera&> cycle_modes_func = createFunctor<camera::Camera&>(camera::cycleModes, camera::main_camera);
	
	FunctorRef<> quit_func = createFunctor(quit);
	quit_func.setTriggers({ key_esc.press, quit_button_press });

	{//camera
		FunctorRef<camera::Camera&> forward_func = createFunctor<camera::Camera&>(camera::forward, camera::main_camera);
		FunctorRef<camera::Camera&> backward_func = createFunctor<camera::Camera&>(camera::back, camera::main_camera);
		FunctorRef<camera::Camera&> left_func = createFunctor<camera::Camera&>(camera::left, camera::main_camera);
		FunctorRef<camera::Camera&> right_func = createFunctor<camera::Camera&>(camera::right, camera::main_camera);
		FunctorRef<camera::Camera&> up_func = createFunctor<camera::Camera&>(camera::up, camera::main_camera);
		FunctorRef<camera::Camera&> down_func = createFunctor<camera::Camera&>(camera::down, camera::main_camera);
		

		forward_func.setTriggers({ key_w.hold });
		backward_func.setTriggers({ key_s.hold });
		left_func.setTriggers({ key_a.hold });
		right_func.setTriggers({ key_d.hold });
		up_func.setTriggers({ key_space.hold });
		down_func.setTriggers({ key_z.hold });

	}

	//text
	//Initializer<Widget<Quad>, initSimple> fps_box_init(0.0f, 0.0f, 0.05f, 0.05f);
	//Widget<Quad> fps_box(fps_box_init);
	gui::text::createTextboxMetrics(0, 1.0, 1.0, 1.0, 1.0);
	//size_t pl_tb = gui::text::createTextbox(playButton.element<0>().element(), 0, TEXT_LAYOUT_CENTER_Y);
	//size_t qu_tb = gui::text::createTextbox(quitButton.element<0>().element(), 0, TEXT_LAYOUT_CENTER_Y);
	//size_t fps_tb = gui::text::createTextbox(fps_box.element<0>().element(), 0, TEXT_LAYOUT_CENTER_Y);

	//gui::text::setTextboxString(qu_tb, " QUIT");
	//gui::text::setTextboxString(pl_tb, " Play");
	//gui::text::setTextboxString(fps_tb, "FPS");
	gui::text::loadTextboxes();
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