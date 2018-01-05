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
#include <glm\glm.hpp>
#include "../GUI/UI/GUI.h"
#include "../GUI/UI/Widget.h"
#include "../GUI/UI/QuadLayout.h"
#include "../GUI/UI/Initializer.h"
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

	ButtonEvents<Event> lmb(createEvent(MouseKeyEvent(0, 1, 0)), createEvent(MouseKeyEvent(0, 0, 0)));

	//size_t lmb_hold = createSignal(toggle_gate<or_gate<EventSource, EventSource>>(or_gate<EventSource, EventSource>(EventSource(createEvent(MouseKeyEvent(0, 1, 0))), EventSource(createEvent(MouseKeyEvent(0, 0, 0))))));

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
	KeySignal key_space(GLFW_KEY_SPACE);
	KeySignal key_z(GLFW_KEY_Z);
	KeySignal key_f(GLFW_KEY_F);
	KeySignal key_n(GLFW_KEY_N);
	KeySignal key_j(GLFW_KEY_J);
	KeySignal key_up(GLFW_KEY_UP);
	KeySignal key_down(GLFW_KEY_DOWN);
	KeySignal key_left(GLFW_KEY_LEFT);
	KeySignal key_right(GLFW_KEY_RIGHT);
	KeySignal key_o(GLFW_KEY_O);
	KeySignal key_l(GLFW_KEY_L);


	using Button = Widget<Quad, Quad>;
	using ButtonColors = WidgetColors<ConstColor, ConstColor>;

	ButtonColors buttonColors = ButtonColors(ConstColor("yellow"), ConstColor("darkgrey"));

	float button_width = gui::pixel_size.x * 100.0f;
	float button_height = gui::pixel_size.x * 35.0f;
	glm::vec2 margin = gui::pixel_size * 2.0f;
	WidgetLayout<Button> button_layout({ QuadData(0.0f, 0.0f, button_width, button_height), QuadData(margin.x, -margin.y, button_width - margin.x*2.0f, button_height - margin.y*2.0f) });
	WidgetMovePolicy<Button> button_move_policy({ glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f) });
	WidgetResizePolicy<Button> button_resize_policy({ glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f) });

	WidgetSetup<Button> button_setup(button_layout, button_move_policy, button_resize_policy);

	Button quitButton(button_setup);
	quitButton.move(gui::pixel_round(glm::vec2(-0.9f, -0.6f)));

	quitButton.color(buttonColors);
	{
			ButtonEvents<Event> border(createEvent(QuadEvent(quitButton.element<0>().index, 1)), createEvent(QuadEvent(quitButton.element<0>().index, 0)));
			ButtonEvents<Event> center(createEvent(QuadEvent(quitButton.element<1>().index, 1)), createEvent(QuadEvent(quitButton.element<1>().index, 0)));
			
			//all enter and leave events through or_gates
			or_gate<Event, Event> button_enters_srcs(border.on_evt, center.on_evt);
			or_gate<Event, Event> button_leaves_srcs(border.off_evt, center.off_evt);
			
			//outputs true when an enter occurred and no leave occurred
			and_gate<decltype(button_enters_srcs), not_gate<decltype(button_leaves_srcs)>> button_enters_src(button_enters_srcs, not_gate<decltype(button_leaves_srcs)>(button_leaves_srcs));
			//outputs true when a leave occurred and no enter occurred
			and_gate<decltype(button_leaves_srcs), not_gate<decltype(button_enters_srcs)>> button_leaves_src(button_leaves_srcs, not_gate<decltype(button_enters_srcs)>(button_enters_srcs));
			//a switch which is turned on by 
			toggle_gate<or_gate<decltype(button_enters_src), decltype(button_leaves_src)>> on_button(or_gate<decltype(button_enters_src), decltype(button_leaves_src)>(button_enters_src, button_leaves_src));
			
			
			size_t button_enter = createSignal(button_enters_src);
			size_t button_leave = createSignal(button_leaves_src);
			
			{
				FunctorRef<size_t, ConstColor> light_button = createFunctor(gui::colorQuad<ConstColor>, quitButton.element<0>().index, ConstColor("white"));
				FunctorRef<size_t, ConstColor> unlight_button = createFunctor(gui::colorQuad<ConstColor>, quitButton.element<0>().index, ConstColor("yellow"));
				light_button.setTriggers({ button_enter });
				unlight_button.setTriggers({ button_leave });
			}
			
			and_gate<decltype(on_button), Event> button_press(on_button, lmb.on_evt);
			size_t quit_button_press = createSignal(button_press);
			FunctorRef<> quit_func = createFunctor(quit);
			quit_func.setTriggers({ key_esc.press, quit_button_press });
		
	}
	//Window
	using Window = QuadGroup<9>;
	using WindowColors = ColorGroup<9, ConstColor>;

	WindowColors window_colors({ ConstColor("grey"), ConstColor("white"),  ConstColor("grey"),
		ConstColor("red"),  ConstColor("green"),  ConstColor("red"),
		ConstColor("grey"), ConstColor("white"), ConstColor("grey") });

	float window_width = gui::pixel_size.x * 300.0f;
	float window_height = gui::pixel_size.x * 235.0f;
	glm::vec2 border = gui::pixel_size * 10.0f;

	WidgetLayout<Window> window_layout({ 
		QuadData(0.0f, 0.0f, border.x, border.y),
		QuadData(border.x, 0.0f, window_width - border.x*2.0f, border.y),
		QuadData(window_width - border.x, 0.0f, border.x, border.y),
		QuadData(0.0f, -border.y, border.x, window_height - border.y*2.0f),
		QuadData(border.x, -border.y, window_width - border.x*2.0f, window_height - border.y*2.0f),
		QuadData(window_width - border.x, -border.y, border.x, window_height - border.y*2.0f),
		QuadData(0.0f, -window_height + border.y, border.x, border.y),
		QuadData(border.x, -window_height + border.y, window_width - border.x*2.0f, border.y),
		QuadData(window_width - border.x, -window_height + border.y, border.x, border.y)
		});

	WidgetMovePolicy<Window> window_move_policy({ 
		glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f)
		});
	WidgetResizePolicy<Window> window_resize_policy({ 
		glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
		glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 1.0f, 0.0f), glm::vec4(1.0f, 1.0f, 0.0f, 0.0f)
		});

	WidgetSetup<Window> window_setup(window_layout, window_move_policy, window_resize_policy);
	Window window(window_setup);

	window.color(window_colors);
	//general functions

	ButtonEvents<Event> header(createEvent(QuadEvent(window.element<1>().index, 1)), createEvent(QuadEvent(window.element<1>().index, 0)));
	and_gate<decltype(header.hold_evt), decltype(lmb.on_evt)> header_press_evt(header.hold_evt, lmb.on_evt);
	and_gate<decltype(header.hold_evt), decltype(lmb.off_evt)> header_release_evt(header.hold_evt, lmb.off_evt);

	ButtonEvents<decltype(header_press_evt), decltype(header_release_evt)> header_and_lmb(header_press_evt, header_release_evt);

	FunctorRef<Window, glm::vec2&> move_window_func = createFunctor<Window, glm::vec2&>(move_widget, window, cursorFrameDelta);
	move_window_func.setTriggers({ header_and_lmb.hold });

	FunctorRef<Window, glm::vec2&> resize_window_func = createFunctor<Window, glm::vec2&>(resize_widget, window, cursorFrameDelta);
	resize_window_func.setTriggers({ header_and_lmb.hold });

	{
		FunctorRef<> toggle_cull_func = createFunctor(mesh::toggleCullFace);
		FunctorRef<> toggle_cursor_func = createFunctor(app::Input::toggleCursor);
		FunctorRef<camera::Camera&> toggle_look_func = createFunctor<camera::Camera&>(camera::toggleLook, camera::main_camera);
		FunctorRef<> toggle_grid_func = createFunctor(glDebug::toggleGrid);
		FunctorRef<> toggle_coord_func = createFunctor(glDebug::toggleCoord);
		FunctorRef<> toggle_info_func = createFunctor(debug::togglePrintInfo);
		FunctorRef<> toggle_normals_func = createFunctor(mesh::toggleNormals);
		FunctorRef<camera::Camera&> cycle_modes_func = createFunctor<camera::Camera&>(camera::cycleModes, camera::main_camera);
		cycle_modes_func.setTriggers({ key_j.press });
	}


	{//camera
		FunctorRef<camera::Camera&> forward_func = createFunctor<camera::Camera&>(camera::forward, camera::main_camera);
		FunctorRef<camera::Camera&> backward_func = createFunctor<camera::Camera&>(camera::back, camera::main_camera);
		FunctorRef<camera::Camera&> left_func = createFunctor<camera::Camera&>(camera::left, camera::main_camera);
		FunctorRef<camera::Camera&> right_func = createFunctor<camera::Camera&>(camera::right, camera::main_camera);
		FunctorRef<camera::Camera&> up_func = createFunctor<camera::Camera&>(camera::up, camera::main_camera);
		FunctorRef<camera::Camera&> down_func = createFunctor<camera::Camera&>(camera::down, camera::main_camera);
		FunctorRef<camera::Camera&> toggleLook_func = createFunctor<camera::Camera&>(camera::toggleLook, camera::main_camera);

		forward_func.setTriggers({ key_w.hold });
		backward_func.setTriggers({ key_s.hold });
		left_func.setTriggers({ key_a.hold });
		right_func.setTriggers({ key_d.hold });
		up_func.setTriggers({ key_space.hold });
		down_func.setTriggers({ key_z.hold });
		toggleLook_func.setTriggers({ key_c.press });
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