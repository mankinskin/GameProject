#include "../../../Global/stdafx.h"
#include "stdafx.h"
#include "gui.h"
#include "..\..\GlobalGL\glDebug.h"
#include "../../../Global/Debug.h"
#include "../../../Graphics/BaseGL\VAO.h"
#include "Quad.h"
#include "Line.h"
#include "Colorings.h"
#include "../Text/Text.h"
#include "../../BaseGL/Framebuffer.h"
#include "Widget.h"
#include "../../../Input/Input.h"
#include "../../../Input/Gates.h"
#include "../../../Input/Event.h"
#include "../../../Input/Functor.h"
#include "../../../Input/Mouse.h"
#include "../../../Input/Signal.h"
#include "../../../Global/App.h"
#include "../../Lighting/Lights.h"
glm::vec2 gui::pixel_size;

void gui::init()
{
	pixel_size = glm::vec2(2.0f / gl::screenWidth, 2.0f / gl::screenHeight);
	initColors();
	initColoringVAOs();	
	initQuadBuffer();
	
	//text::initFonts();
	debug::printErrors();
}

void gui::initColors()
{
	createConstColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), "nocolor");
	createConstColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), "black");
	createConstColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "white");
	createConstColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), "red");
	createConstColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), "green");
	createConstColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), "blue");
	createConstColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), "yellow");
	createConstColor(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), "cyan");
	createConstColor(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), "magenta");
	createConstColor(glm::vec4(0.5f, 0.0f, 0.5f, 1.0f), "purple");
	createConstColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), "grey");
	createConstColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), "darkgrey");
	createConstColor(glm::vec4(0.7f, 0.7f, 0.7f, 1.0f), "lightgrey");
}
void gui::initWidgets()
{
	using namespace gates;
	using namespace events;
	using namespace functors;
	using namespace app::Input;
	using namespace signals;

	using Button = Widget<Quad, Quad>;
	using ButtonColors = WidgetColors<ConstColor, ConstColor>;

	ButtonColors buttonColors = ButtonColors(ConstColor("yellow"), ConstColor("darkgrey"));
	
	float button_width = gui::pixel_size.x * 100.0f;
	float button_height = gui::pixel_size.x * 55.0f;
	glm::vec2 margin = gui::pixel_size * 2.0f;
	WidgetLayout<Button> button_layout({ QuadData(0.0f, 0.0f, button_width, button_height), QuadData(margin.x, -margin.y, button_width - margin.x*2.0f, button_height - margin.y*2.0f) });
	WidgetMovePolicy<Button> button_move_policy({ glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f) });
	WidgetResizePolicy<Button> button_resize_policy({ glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f) });
	
	WidgetSetup<Button> button_setup(button_layout, button_move_policy, button_resize_policy);
	
	Button quitButton(button_setup);
	quitButton.move(gui::pixel_round(glm::vec2(-0.9f, -0.6f)));
	
	quitButton.color(buttonColors);
	
	ButtonEvents<Event> border_btn(createEvent(QuadEvent(quitButton.element<0>().index, 1)), createEvent(QuadEvent(quitButton.element<0>().index, 0)));
	ButtonEvents<Event> center_btn(createEvent(QuadEvent(quitButton.element<1>().index, 1)), createEvent(QuadEvent(quitButton.element<1>().index, 0)));
	
	//all enter and leave events through or_gates
	gate<or_op, Event, Event> button_enters_srcs(or_op(), border_btn.on_evt, center_btn.on_evt);
	gate<or_op, Event, Event> button_leaves_srcs(or_op(), border_btn.off_evt, center_btn.off_evt);
	
	//outputs true when an enter occurred and no leave occurred
	gate<and_op, decltype(button_enters_srcs), not_gate<decltype(button_leaves_srcs)>> button_enters_src(and_op(), button_enters_srcs, not_gate<decltype(button_leaves_srcs)>(button_leaves_srcs));
	//outputs true when a leave occurred and no enter occurred
	gate<and_op, decltype(button_leaves_srcs), not_gate<decltype(button_enters_srcs)>> button_leaves_src(and_op(), button_leaves_srcs, not_gate<decltype(button_enters_srcs)>(button_enters_srcs));
	//a switch which is turned on by
	gate<or_op, decltype(button_enters_src), decltype(button_leaves_src)> ent_or_lea(or_op(), button_enters_src, button_leaves_src);
	toggle_gate<decltype(ent_or_lea)> on_button(ent_or_lea);
	
	
	size_t button_enter = createSignal(button_enters_src);
	size_t button_leave = createSignal(button_leaves_src);
	
	
	FunctorRef<void, size_t, ConstColor> light_button = createFunctor(gui::colorQuad<ConstColor>, quitButton.element<0>().index, ConstColor("white"));
	FunctorRef<void, size_t, ConstColor> unlight_button = createFunctor(gui::colorQuad<ConstColor>, quitButton.element<0>().index, ConstColor("yellow"));
	light_button.set_triggers({ button_enter });
	unlight_button.set_triggers({ button_leave });
	
	gate<and_op, decltype(on_button), decltype(lmb.on_evt)> button_press(and_op(), on_button, lmb.on_evt);
	size_t quit_button_press = createSignal(button_press);


	FunctorRef<void> quit_func = createFunctor(app::quit);
	quit_func.set_triggers({ key_esc.press, quit_button_press });
	
	
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
	window.move(-1.0f, 0.0f);
	//general functions
	
	ButtonEvents<Event> header(createEvent(QuadEvent(window.element<1>().element<1>().index, 1)), createEvent(QuadEvent(window.element<1>().element<1>().index, 0)));
	gate<and_op, decltype(header.hold_evt), decltype(lmb.on_evt)> header_press_evt(and_op(), header.hold_evt, lmb.on_evt);
	ButtonEvents<decltype(header_press_evt), decltype(lmb.off_evt)> header_lmb(header_press_evt, lmb.off_evt);
	
	ButtonEvents<Event> right(createEvent(QuadEvent(window.element<0>().element<5>().index, 1)), createEvent(QuadEvent(window.element<0>().element<5>().index, 0)));
	gate<and_op, decltype(right.hold_evt), decltype(lmb.on_evt)> right_press_evt(and_op(), right.hold_evt, lmb.on_evt);
	ButtonEvents<decltype(right_press_evt), decltype(lmb.off_evt)> right_and_lmb(right_press_evt, lmb.off_evt);
	
	ButtonEvents<Event> bottom(createEvent(QuadEvent(window.element<0>().element<7>().index, 1)), createEvent(QuadEvent(window.element<0>().element<7>().index, 0)));
	gate<and_op, decltype(bottom.hold_evt), decltype(lmb.on_evt)> bottom_press_evt(and_op(), bottom.hold_evt, lmb.on_evt);
	ButtonEvents<decltype(bottom_press_evt), decltype(lmb.off_evt)> bottom_and_lmb(bottom_press_evt, lmb.off_evt);
	
	ButtonEvents<Event> bottom_right(createEvent(QuadEvent(window.element<0>().element<8>().index, 1)), createEvent(QuadEvent(window.element<0>().element<8>().index, 0)));
	gate<and_op, decltype(bottom_right.hold_evt), decltype(lmb.on_evt)> bottom_right_press_evt(and_op(), bottom_right.hold_evt, lmb.on_evt);
	ButtonEvents<decltype(bottom_right_press_evt), decltype(lmb.off_evt)> bottom_right_and_lmb(bottom_right_press_evt, lmb.off_evt);

	FunctorRef<void, Window, glm::vec2&> move_window_func = createFunctor<void, Window, glm::vec2&>(move_widget, window, cursorFrameDelta);
	move_window_func.set_triggers({ header_lmb.hold });
	
	FunctorRef<void, Window, glm::vec2&> resize_window_func = createFunctor<void, Window, glm::vec2&>(resize_widget, window, cursorFrameDelta);
	resize_window_func.set_triggers({ bottom_right_and_lmb.hold });
	
	FunctorRef<void, Window, float&, float> resize_window_x_func = createFunctor<void, Window, float&, float>(resize_widget, window, cursorFrameDelta.x, 0.0f);
	resize_window_x_func.set_triggers({ right_and_lmb.hold });

	FunctorRef<void, Window, float, float&> resize_window_y_func = createFunctor<void, Window, float, float&>(resize_widget, window, 0.0f, cursorFrameDelta.y);
	resize_window_y_func.set_triggers({ bottom_and_lmb.hold });
	
	////Slider
	using Slider = QuadGroup<2>;
	using SliderMovePolicy = WidgetMovePolicy<Slider>;
	using SliderResizePolicy = WidgetResizePolicy<Slider>;
	using SliderLayout = WidgetLayout<Slider>;
	using SliderColors = ColorGroup<2, ConstColor>;
	using SliderSetup = WidgetSetup<Slider>;
	
	SliderColors slider_colors(ConstColor("lightgrey"), ConstColor("darkgrey"));
	SliderLayout slider_layout({ glm::vec4(0.0f, 0.0f, 0.2f, 0.05f), glm::vec4(0.0f, 0.0f, 0.05f, 0.05f) });
	SliderMovePolicy slider_move_policy({ glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f) });
	SliderResizePolicy slider_resize_policy({ glm::vec4(0.0f, 0.0f,1.0f, 1.0f), glm::vec4(0.0f, 0.0f,1.0f, 1.0f) });
	SliderSetup slider_setup(slider_layout, slider_move_policy, slider_resize_policy);
	
	Slider slider(slider_setup);
	slider.color(slider_colors);
	slider.move(0.7f, -0.7f);
	
	
	Event slider_box_enter_evt = createEvent(QuadEvent(slider.element<0>().index, 1));
	size_t slider_box_enter = createSignal(slider_box_enter_evt);
	Event slider_box_leave_evt = createEvent(QuadEvent(slider.element<0>().index, 0));
	size_t slider_box_leave = createSignal(slider_box_leave_evt);
	
	Event slider_slide_enter_evt = createEvent(QuadEvent(slider.element<1>().index, 1));
	size_t slider_slide_enter = createSignal(slider_slide_enter_evt);
	Event slider_slide_leave_evt = createEvent(QuadEvent(slider.element<1>().index, 0));
	size_t slider_slide_leave = createSignal(slider_slide_leave_evt);
	
	
	gate<or_op, Event, Event> slider_any_enter_evt(or_op(), slider_slide_enter_evt, slider_box_enter_evt);
	gate<or_op, Event, Event> slider_any_leave_evt(or_op(), slider_slide_leave_evt, slider_box_leave_evt);
	
	gate<and_op, decltype(slider_any_enter_evt), not_gate<decltype(slider_any_leave_evt)>> slider_enter_evt(and_op(), slider_any_enter_evt, not_gate<decltype(slider_any_leave_evt)>(slider_any_leave_evt));
	gate<and_op, decltype(slider_any_leave_evt), not_gate<decltype(slider_any_enter_evt)>> slider_leave_evt(and_op(), slider_any_leave_evt, not_gate<decltype(slider_any_enter_evt)>(slider_any_enter_evt));
	switch_gate<decltype(slider_enter_evt), decltype(slider_leave_evt)> slider_hover_evt(slider_enter_evt, slider_leave_evt);
	
	gate<and_op, decltype(slider_hover_evt), decltype(lmb.on_evt)> slider_press_evt(and_op(), slider_hover_evt, lmb.on_evt);
	
	ButtonEvents<decltype(slider_press_evt), Event> slider_lmb(slider_press_evt, lmb.off_evt);
	
	void(*move_quad_to)(Quad, float&) = [](Quad pTar, float& pPos)->void {
		float dist = pPos - (pTar.get_pos().x + pTar.get_size().x / 2.0f);
		pTar.move(glm::vec2(dist, 0.0f));
	};
	
	FunctorRef<void, Quad, float&> move_slide_func = createFunctor<void, Quad, float&>(move_quad_to, slider.element<1>().index, relativeCursorPosition.x);
	move_slide_func.set_triggers({ slider_lmb.hold });
	
	void(*lim_quad)(Quad, Quad) = [](Quad pTar, Quad pLim)->void {
		float l_dist = glm::vec2(pLim.get_pos() - pTar.get_pos()).x;
		float r_dist = glm::vec2((pLim.get_pos() + pLim.get_size()) - (pTar.get_pos() + pTar.get_size())).x;
		pTar.move(glm::vec2(std::min(std::max(l_dist, 0.0f), r_dist), 0.0f));
	};
	
	FunctorRef<void, Quad, Quad> limit_slide_func = createFunctor(lim_quad, slider.element<1>(), slider.element<0>());
	limit_slide_func.set_triggers({ slider_lmb.hold });
	
	void(*set_slide_target)(glm::vec4&, Quad, Quad) = [](glm::vec4& pTarget, Quad pBox, Quad pSlide)->void {
		float half_width = pSlide.get_size().x / 2.0f;
		float slide_pos = pSlide.get_pos().x + half_width;
		float box_size = pBox.get_size().x;
		float box_pos = pBox.get_pos().x;
		float amt = (slide_pos - box_pos) / (box_size - half_width);
		pTarget = glm::vec4(amt, amt, amt, amt)*10.0f;
	};
	FunctorRef<void, glm::vec4&, Quad, Quad> set_slide_target_func = createFunctor<void, glm::vec4&, Quad, Quad>(set_slide_target, lighting::getLightColor(0), slider.element<0>(), slider.element<1>());
	set_slide_target_func.set_triggers({ slider_lmb.hold });
	//
	////text
	//gui::text::createTextboxMetrics(0, 1.0, 1.0, 1.0, 1.0);
	//
	//size_t qu_tb = gui::text::createTextbox(quitButton.element<1>().index, 0, TEXT_LAYOUT_CENTER_Y);
	////size_t fps_tb = gui::text::createTextbox(fps_box.element<0>().element(), 0, TEXT_LAYOUT_CENTER_Y);
	//
	//gui::text::setTextboxString(qu_tb, " QUIT");
	////gui::text::setTextboxString(pl_tb, " Play");
	////gui::text::setTextboxString(fps_tb, "FPS");
	//gui::text::loadTextboxes();
}

glm::vec2 gui::pixel_round(glm::vec2 pIn) {
	return pixel_size * round(pIn / pixel_size);
}
float gui::pixel_round_x(float pIn) {
	return pixel_size.x * round(pIn / pixel_size.x);
}
float gui::pixel_round_y(float pIn) {
	return pixel_size.y * round(pIn / pixel_size.y);
}