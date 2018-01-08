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
#include <tuple>
#include "EventGates.h"
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
	using namespace events;
	using namespace functors;
	using namespace signals;

	size_t always = createSignal(Source(true), true);

	ButtonEvents<Event> lmb(createEvent(MouseKeyEvent(0, 1, 0)), createEvent(MouseKeyEvent(0, 0, 0)));
	ButtonEvents<Event> rmb(createEvent(MouseKeyEvent(1, 1, 0)), createEvent(MouseKeyEvent(1, 0, 0)));
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
				FunctorRef<void, size_t, ConstColor> light_button = createFunctor(gui::colorQuad<ConstColor>, quitButton.element<0>().index, ConstColor("white"));
				FunctorRef<void, size_t, ConstColor> unlight_button = createFunctor(gui::colorQuad<ConstColor>, quitButton.element<0>().index, ConstColor("yellow"));
				light_button.set_triggers({ button_enter });
				unlight_button.set_triggers({ button_leave });
			}
			
			and_gate<decltype(on_button), Event> button_press(on_button, lmb.on_evt);
			size_t quit_button_press = createSignal(button_press);
			FunctorRef<void> quit_func = createFunctor(quit);
			quit_func.set_triggers({ key_esc.press, quit_button_press });
		
	}
	//Window
	using WindowHeader = QuadGroup<2>;
	using WindowFrame = QuadGroup<9>;
	using Window = Widget<WindowFrame, WindowHeader>;
	using WindowFrameColors = ColorGroup<9, ConstColor>;
	using WindowHeaderColors = ColorGroup<2, ConstColor>;
	using WindowColors = WidgetColors<WindowFrameColors, WindowHeaderColors>;

	WindowFrameColors window_frame_colors({ ConstColor("grey"), ConstColor("grey"),  ConstColor("grey"),
		ConstColor("grey"),  ConstColor("white"),  ConstColor("grey"),
		ConstColor("grey"), ConstColor("grey"), ConstColor("grey") });

	WindowHeaderColors window_header_colors({ ConstColor("grey"), ConstColor("white") });
	WindowColors window_colors(window_frame_colors, window_header_colors);

	float window_width = gui::pixel_size.x * 300.0f;
	float window_height = gui::pixel_size.x * 235.0f;
	glm::vec2 border = gui::pixel_size * 4.0f;

	WidgetLayout<WindowFrame> window_frame_layout({
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

	WidgetMovePolicy<WindowFrame> window_frame_move_policy({
		glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f)
		});
	WidgetResizePolicy<WindowFrame> window_frame_resize_policy({
		glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
		glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 1.0f, 0.0f), glm::vec4(1.0f, 1.0f, 0.0f, 0.0f)
		});
	WidgetSetup<WindowFrame> window_frame_setup(window_frame_layout, window_frame_move_policy, window_frame_resize_policy);

	float header_height = gui::pixel_size.y * 25.0f;
	glm::vec2 header_border = gui::pixel_size * 4.0f;

	WidgetLayout<WindowHeader> window_header_layout({
			QuadData(0.0f, 0.0f, window_width, header_height),
			QuadData(header_border.x, -header_border.y, window_width - header_border.x*2.0f, header_height - header_border.y*2.0f)
		});
	WidgetMovePolicy<WindowHeader> window_header_move_policy({
		glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f)
		});
	WidgetResizePolicy<WindowHeader> window_header_resize_policy({
		glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)
	});
	WidgetSetup<WindowHeader> window_header_setup(window_header_layout, window_header_move_policy, window_header_resize_policy);

	WidgetLayout<Window> window_layout({ window_frame_setup, window_header_setup });
	WidgetMovePolicy<Window> window_move_policy({
		glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f)
		});
	WidgetResizePolicy<Window> window_resize_policy({
		glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)
		});
	

	WidgetSetup<Window> window_setup(window_layout, window_move_policy, window_resize_policy);
	Window window(window_setup);

	window.color(window_colors);
	//general functions

	ButtonEvents<Event> header(createEvent(QuadEvent(window.element<1>().element<1>().index, 1)), createEvent(QuadEvent(window.element<1>().element<1>().index, 0)));
	and_gate<decltype(header.hold_evt), decltype(lmb.on_evt)> header_press_evt(header.hold_evt, lmb.on_evt);
	ButtonEvents<decltype(header_press_evt), decltype(lmb.off_evt)> header_lmb(header_press_evt, lmb.off_evt);


	ButtonEvents<Event> right(createEvent(QuadEvent(window.element<0>().element<5>().index, 1)), createEvent(QuadEvent(window.element<0>().element<5>().index, 0)));
	and_gate<decltype(right.hold_evt), decltype(lmb.on_evt)> right_press_evt(right.hold_evt, lmb.on_evt);
	ButtonEvents<decltype(right_press_evt), decltype(lmb.off_evt)> right_and_lmb(right_press_evt, lmb.off_evt);

	ButtonEvents<Event> bottom(createEvent(QuadEvent(window.element<0>().element<7>().index, 1)), createEvent(QuadEvent(window.element<0>().element<7>().index, 0)));
	and_gate<decltype(bottom.hold_evt), decltype(lmb.on_evt)> bottom_press_evt(bottom.hold_evt, lmb.on_evt);
	ButtonEvents<decltype(bottom_press_evt), decltype(lmb.off_evt)> bottom_and_lmb(bottom_press_evt, lmb.off_evt);

	ButtonEvents<Event> bottom_right(createEvent(QuadEvent(window.element<0>().element<8>().index, 1)), createEvent(QuadEvent(window.element<0>().element<8>().index, 0)));
	and_gate<decltype(bottom_right.hold_evt), decltype(lmb.on_evt)> bottom_right_press_evt(bottom_right.hold_evt, lmb.on_evt);
	ButtonEvents<decltype(bottom_right_press_evt), decltype(lmb.off_evt)> bottom_right_and_lmb(bottom_right_press_evt, lmb.off_evt);

	FunctorRef<void, Window, glm::vec2&> move_window_func = createFunctor<void, Window, glm::vec2&>(move_widget, window, cursorFrameDelta);
	move_window_func.set_triggers({ header_lmb.hold });

	FunctorRef<void, Window, glm::vec2&> resize_window_func = createFunctor<void, Window, glm::vec2&>(resize_widget, window, cursorFrameDelta);
	resize_window_func.set_triggers({ bottom_right_and_lmb.hold });
	
	FunctorRef<void, Window, float&, float> resize_window_x_func = createFunctor<void, Window, float&, float>(resize_widget, window, cursorFrameDelta.x, 0.0f);
	resize_window_x_func.set_triggers({ right_and_lmb.hold });
	
	FunctorRef<void, Window, float, float&> resize_window_y_func = createFunctor<void, Window, float, float&>(resize_widget, window, 0.0f, cursorFrameDelta.y);
	resize_window_y_func.set_triggers({ bottom_and_lmb.hold });
	{
		FunctorRef<void> toggle_cull_func = createFunctor(mesh::toggleCullFace);
		FunctorRef<void> toggle_cursor_func = createFunctor(app::Input::toggleCursor);
		FunctorRef<void, camera::Camera&> toggle_look_func = createFunctor<void, camera::Camera&>(camera::toggleLook, camera::main_camera);
		FunctorRef<void> toggle_grid_func = createFunctor(glDebug::toggleGrid);
		FunctorRef<void> toggle_coord_func = createFunctor(glDebug::toggleCoord);
		FunctorRef<void> toggle_info_func = createFunctor(debug::togglePrintInfo);
		FunctorRef<void> toggle_normals_func = createFunctor(mesh::toggleNormals);
		FunctorRef<void, camera::Camera&> cycle_modes_func = createFunctor<void, camera::Camera&>(camera::cycleModes, camera::main_camera);
		
		toggle_cull_func.set_triggers({ key_i.press });
		toggle_cursor_func.set_triggers({ key_c.press, rmb.on, rmb.off });
		toggle_look_func.set_triggers({ key_c.press, rmb.on, rmb.off });
		toggle_grid_func.set_triggers({ key_g.press });
		toggle_coord_func.set_triggers({ key_h.press });
		toggle_info_func.set_triggers({ key_i.press });
		toggle_normals_func.set_triggers({ key_i.press });
		cycle_modes_func.set_triggers({ key_j.press });
	}


	using Slider = QuadGroup<2>;
	using SliderMovePolicy = WidgetMovePolicy<Slider>;
	using SliderResizePolicy = WidgetResizePolicy<Slider>;
	using SliderLayout = WidgetLayout<Slider>;
	using SliderColors = ColorGroup<2, ConstColor>;
	using SliderSetup = WidgetSetup<Slider>;

	SliderColors slider_colors(ConstColor("lightgrey"), ConstColor("darkgrey"));
	SliderLayout slider_layout({glm::vec4(0.0f, 0.0f, 0.2f, 0.05f), glm::vec4(0.0f, 0.0f, 0.05f, 0.05f)});
	SliderMovePolicy slider_move_policy({ glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f) });
	SliderResizePolicy slider_resize_policy({ glm::vec4(0.0f, 0.0f,1.0f, 1.0f), glm::vec4(0.0f, 0.0f,1.0f, 1.0f) });
	SliderSetup slider_setup(slider_layout, slider_move_policy, slider_resize_policy);

	Slider slider(slider_setup);
	slider.color(slider_colors);
	slider.move(0.7f, -0.7f);



	ButtonEvents<Event> slider_slide_button(createEvent(QuadEvent(slider.element<1>().index, 1)), createEvent(QuadEvent(slider.element<1>().index, 0)));

	and_gate<decltype(slide_button.hold_evt), decltype(lmb.on_evt)> slide_press_evt(slider_button.hold_evt, lmb.on_evt);
	ButtonEvents<decltype(slide_press_evt), Event> slider_lmb(slide_press_evt, lmb.off_evt);

	void(*move_quad_to)(Quad, float&) = [](Quad pTar, float& pPos)->void {
		float dist = pPos - (pTar.get_pos().x + pTar.get_size().x/2.0f);
		pTar.move(glm::vec2(dist, 0.0f));
	};

	FunctorRef<void, Quad, float&> move_slide_func = createFunctor<void, Quad, float&>(move_quad_to, slider.element<1>().index, relativeCursorPosition.x);
	move_slide_func.set_triggers({ slider_lmb.hold });

	void (*lim_quad)(Quad, Quad) = [](Quad pTar, Quad pLim)->void {
		float l_dist = glm::vec2(pLim.get_pos() - pTar.get_pos()).x;
		float r_dist = glm::vec2((pLim.get_pos() + pLim.get_size()) - (pTar.get_pos() + pTar.get_size())).x;
		pTar.move(glm::vec2(std::min(std::max(l_dist, 0.0f), r_dist), 0.0f));
	};

	FunctorRef<void, Quad, Quad> limit_slide_func = createFunctor(lim_quad, slider.element<1>(), slider.element<0>());
	limit_slide_func.set_triggers({ slide_button_lmb.hold });

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

	//text
	gui::text::createTextboxMetrics(0, 1.0, 1.0, 1.0, 1.0);
	
	size_t qu_tb = gui::text::createTextbox(quitButton.element<1>().index, 0, TEXT_LAYOUT_CENTER_Y);
	//size_t fps_tb = gui::text::createTextbox(fps_box.element<0>().element(), 0, TEXT_LAYOUT_CENTER_Y);

	gui::text::setTextboxString(qu_tb, " QUIT");
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