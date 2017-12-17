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

#include "../GUI/Text/Text.h"
#include "../Lighting/Lights.h"
#include "../GUI/UI/Widget.h"

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

	EventSignal<MouseKeyEvent>::reserve(4);
	size_t lmb_hold = createSignal(1);
	createEvent(MouseKeyEvent(0, 1, 0), lmb_hold, 1);
	createEvent(MouseKeyEvent(0, 0, 0), lmb_hold, 0);
	size_t lmb_click = createEvent(MouseKeyEvent(0, 1, 0));
	size_t rmb_press = EventSignal<MouseKeyEvent>(MouseKeyEvent(1, KeyCondition(1, 0))).index();
	size_t rmb_release = EventSignal<MouseKeyEvent>(MouseKeyEvent(1, KeyCondition(0, 0))).index();
	size_t lmb_press = EventSignal<MouseKeyEvent>(MouseKeyEvent(0, KeyCondition(1, 0))).index();
	size_t lmb_release = EventSignal<MouseKeyEvent>(MouseKeyEvent(0, KeyCondition(0, 0))).index();


	
	using WindowBodyWidget = Widget<Quad, Quad, Quad, Quad, Quad, Quad, Quad, Quad, Quad>;
	using WindowBodyInitializer = Initializer<WindowBodyWidget, initMargin>;

	using WindowHeaderWidget = Widget<Quad>;
	using WindowHeaderInitializer = Initializer<WindowHeaderWidget, initSimple>;

	using SliderWidget = Widget<Quad, Quad>;
	using SliderInitializer = Initializer<SliderWidget, initMargin>;

	using WindowWidget = Widget<WindowBodyWidget, WindowHeaderWidget>;
	using WindowInitializer = Initializer<WindowWidget, initJoined, WindowBodyInitializer, WindowHeaderInitializer>;
	
	
	using WindowColors = WidgetColors<WidgetColors<ConstColor, ConstColor, ConstColor, ConstColor, ConstColor, ConstColor, ConstColor, ConstColor, ConstColor>, WidgetColors<ConstColor>>;
	
	
	WindowBodyInitializer body_init(0.0f, 0.0f, 0.3f, 1.0f, 0.01f);
	WindowHeaderInitializer head_init(0.0f, 0.0f, 0.3f, 0.02f);
	
	
	WindowInitializer window_init(body_init, head_init);
	
	WindowWidget win1 = window_init;

	win1.pos.set(glm::vec2(-0.5f, 0.5f));
	//win1.get<2>().pos.set(glm::vec2(0.05f, -0.1f));
	
	WindowColors winColors(
		WidgetColors<ConstColor, ConstColor, ConstColor, ConstColor, ConstColor, ConstColor, ConstColor, ConstColor, ConstColor>(ConstColor("white"), ConstColor("white"), ConstColor("white"), ConstColor("white"), ConstColor("grey"), ConstColor("white"), ConstColor("white"), ConstColor("white"), ConstColor("white")),
		WidgetColors<ConstColor>(ConstColor("nocolor")));
	win1.color(winColors);
	
	size_t header = win1.get<1>().get<0>().index;
	size_t right_border = win1.get<0>().get<5>().index;
	size_t bottom_border = win1.get<0>().get<7>().index;
	size_t bottom_right_corner = win1.get<0>().get<8>().index;
	
	size_t header_hover = createSignal(1);
	createEvent(QuadEvent(header, 1), header_hover, 1);
	createEvent(QuadEvent(header, 0), header_hover, 0);
	
	size_t right_border_hover = createSignal(1);
	createEvent(QuadEvent(right_border, 1), right_border_hover, 1);
	createEvent(QuadEvent(right_border, 0), right_border_hover, 0);
	
	size_t bottom_border_hover = createSignal(1);
	createEvent(QuadEvent(bottom_border, 1), bottom_border_hover, 1);
	createEvent(QuadEvent(bottom_border, 0), bottom_border_hover, 0);
	
	size_t bottom_right_corner_hover = createSignal(1);
	createEvent(QuadEvent(bottom_right_corner, 1), bottom_right_corner_hover, 1);
	createEvent(QuadEvent(bottom_right_corner, 0), bottom_right_corner_hover, 0);
	
	Functor<WindowWidget, glm::vec2&> move_widget_func(move_widget<WindowWidget>, win1, cursorFrameDelta);
	
	Functor<WindowWidget, float&, float> stretch_bodyx_func(stretch_widget_element<WindowWidget, 0, float&, float>, win1, cursorFrameDelta.x, 0.0f);
	Functor<WindowWidget, float, float&>stretch_bodyy_func(stretch_widget_element<WindowWidget, 0, float, float&>, win1, 0.0f, cursorFrameDelta.y);
	
	Functor<WindowWidget, float&, float> stretch_headerx_func(stretch_widget_element<WindowWidget, 1, float&, float>, win1, cursorFrameDelta.x, 0.0f);
	
	
	FunctorTrigger<SequenceGate, WindowWidget, glm::vec2&>(move_widget_func, { header_hover, lmb_click, lmb_hold });
	FunctorTrigger<SequenceGate, WindowWidget, float&, float>(stretch_bodyx_func, { right_border_hover, lmb_click, lmb_hold });
	FunctorTrigger<SequenceGate, WindowWidget, float&, float>(stretch_headerx_func, { right_border_hover, lmb_click, lmb_hold });
	FunctorTrigger<SequenceGate, WindowWidget, float, float&>(stretch_bodyy_func, { bottom_border_hover, lmb_click, lmb_hold });
	
	FunctorTrigger<SequenceGate, WindowWidget, float&, float>(stretch_bodyx_func, { bottom_right_corner_hover, lmb_click, lmb_hold });
	FunctorTrigger<SequenceGate, WindowWidget, float, float&>(stretch_bodyy_func, { bottom_right_corner_hover, lmb_click, lmb_hold });
	FunctorTrigger<SequenceGate, WindowWidget, float&, float>(stretch_headerx_func, { bottom_right_corner_hover, lmb_click, lmb_hold });
	
	
	
	
	
	//win1.get<2>().color(sldColors);
	//
	//size_t slider_box_hover = createSignal(1);
	//createEvent(QuadEvent(win1.get<2>().get<0>().get(), 1), slider_box_hover, 1);
	//createEvent(QuadEvent(win1.get<2>().get<0>().get(), 0), slider_box_hover, 0);
	//
	//size_t slider_slide_hover = createSignal(1);
	//createEvent(QuadEvent(win1.get<2>().get<1>().get(), 1), slider_slide_hover, 1);
	//createEvent(QuadEvent(win1.get<2>().get<1>().get(), 0), slider_slide_hover, 0);
	//
	//Functor<SliderWidget, float&, float> move_slide_func(move_widget_element<SliderWidget, 1, float&, float>, win1.get<2>(), cursorFrameDelta.x, 0.0f);
	//
	//Functor<size_t, float&> set_slide_pos_func(
	//	[](size_t tar, float& pPos) {
	//	allQuads[tar - 1].x = pPos - allQuads[tar - 1].z/2.0f;
	//}
	//, win1.get<2>().get<1>().get<0>().get(), relativeCursorPosition.x);
	//
	//Functor<size_t, size_t> limit_slide_func(
	//	[](size_t tar, size_t bound) {
	//
	//	allQuads[tar - 1].x += std::max(allQuads[bound-1].x - allQuads[tar - 1].x, 0.0f)+std::min((allQuads[bound - 1].x + allQuads[bound - 1].z) - (allQuads[tar - 1].x + allQuads[tar - 1].z), 0.0f);
	//}
	//, win1.get<2>().get<1>().get<0>().get(), win1.get<2>().get<0>().get<0>().get());
	//
	//
	//
	//FunctorTrigger<SequenceGate, SliderWidget, float&, float>(move_slide_func, { slider_slide_hover, lmb_click,lmb_hold });
	//FunctorTrigger<SequenceGate, size_t, float&>(set_slide_pos_func, { slider_slide_hover, lmb_click, lmb_hold });
	//FunctorTrigger<SequenceGate, size_t, float&>(set_slide_pos_func, { slider_box_hover, lmb_click, lmb_hold });
	//FunctorTrigger<AlwaysGate, size_t, size_t>(limit_slide_func, {});




	using ButtonQuad = Widget<Quad, Quad>;
	using ButtonColor = WidgetColors<ConstColor, ConstColor >;
	
	Initializer<ButtonQuad, initMargin> button_initializer(0.0f, 0.0f, 0.2f, 0.07f, 0.005f);
	ButtonColor buttonColor = ButtonColor(ConstColor(6), ConstColor(7));

	ButtonQuad playButton(button_initializer);
	playButton.move(glm::vec2(-0.99f, -0.75f));

	ButtonQuad quitButton(button_initializer);
	quitButton.move(glm::vec2(-0.99f, -0.87f));

	playButton.color(buttonColor);
	quitButton.color(buttonColor);
			
	//text
	Initializer<Widget<Quad>, initSimple> fps_box_init(0.0f, 0.0f, 0.05f, 0.05f);
	Widget<Quad> fps_box(fps_box_init);
	gui::text::createTextboxMetrics(0, 1.0, 1.0, 1.0, 1.0);
	size_t pl_tb = gui::text::createTextbox(playButton.get<0>().get(), 0, TEXT_LAYOUT_CENTER_Y);
	size_t qu_tb = gui::text::createTextbox(quitButton.get<0>().get(), 0, TEXT_LAYOUT_CENTER_Y);
	size_t fps_tb = gui::text::createTextbox(fps_box.get<0>().get(), 0, TEXT_LAYOUT_CENTER_Y);

	gui::text::setTextboxString(qu_tb, " QUIT");
	gui::text::setTextboxString(pl_tb, " Play");
	gui::text::setTextboxString(fps_tb, "FPS");
	gui::text::loadTextboxes();

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

	size_t quit_button_hover = createSignal(1);
	createEvent(QuadEvent(quitButton.get<1>().get(), 1), quit_button_hover, 1);
	createEvent(QuadEvent(quitButton.get<1>().get(), 0), quit_button_hover, 0);
	createEvent(QuadEvent(quitButton.get<0>().get(), 1), quit_button_hover, 1);
	createEvent(QuadEvent(quitButton.get<0>().get(), 0), quit_button_hover, 0);
	//general functions
	Functor<> quit_func(quit);
	FunctorTrigger<SequenceGate>(quit_func, { quit_button_hover, lmb_click });
	FunctorTrigger<AnyGate>(quit_func, { key_esc.press });

	Functor<> toggle_cull_func(mesh::toggleCullFace);
	FunctorTrigger<AnyGate>(toggle_cull_func, { key_f.press });

	Functor<> toggle_cursor_func(app::Input::toggleCursor);
	FunctorTrigger<AnyGate>(toggle_cursor_func, { key_c.press, rmb_press, rmb_release });

	Functor<camera::Camera&> toggle_look_func(camera::toggleLook, camera::main_camera);
	FunctorTrigger<AnyGate, camera::Camera&>(toggle_look_func, { key_c.press, rmb_press, rmb_release });

	Functor<> toggle_grid_func(glDebug::toggleGrid);
	FunctorTrigger<AnyGate>(toggle_grid_func, { key_g.press });

	Functor<> toggle_coord_func(glDebug::toggleCoord);
	FunctorTrigger<AnyGate>(toggle_coord_func, { key_h.press });

	Functor<> toggle_info_func(debug::togglePrintInfo);
	FunctorTrigger<AnyGate>(toggle_info_func, { key_i.press });

	Functor<> toggle_normals_func(mesh::toggleNormals);
	FunctorTrigger<AnyGate>(toggle_normals_func, { key_n.press });

	Functor<camera::Camera&> cycle_modes_func(camera::cycleModes, camera::main_camera);
	FunctorTrigger<AnyGate, camera::Camera&>(cycle_modes_func, { key_j.press });

	{//camera
		Functor<camera::Camera&> forward_func(camera::forward, camera::main_camera);
		Functor<camera::Camera&> backward_func(camera::back, camera::main_camera);
		Functor<camera::Camera&> left_func(camera::left, camera::main_camera);
		Functor<camera::Camera&> right_func(camera::right, camera::main_camera);
		Functor<camera::Camera&> up_func(camera::up, camera::main_camera);
		Functor<camera::Camera&> down_func(camera::down, camera::main_camera);
		

		FunctorTrigger<AnyGate, camera::Camera&>(forward_func, { key_w.held });
		FunctorTrigger<AnyGate, camera::Camera&>(backward_func, { key_s.held });
		FunctorTrigger<AnyGate, camera::Camera&>(left_func, { key_a.held });
		FunctorTrigger<AnyGate, camera::Camera&>(right_func, { key_d.held });
		FunctorTrigger<AnyGate, camera::Camera&>(up_func, { key_space.held });
		FunctorTrigger<AnyGate, camera::Camera&>(down_func, { key_z.held });

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