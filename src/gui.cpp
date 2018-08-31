#include "gui.h"
#include "gldebug.h"
#include "debug.h"
#include "vao.h"
#include "quad.h"
#include "line.h"
#include "quadcolors.h"
#include "framebuffer.h"
#include "widget.h"
#include "input.h"
#include "signal.h"
#include "functor.h"
#include "mouse.h"
#include "signal.h"
#include "app.h"
#include "light.h"
#include "viewport.h"
#include "layouts/button.h"
#include "layouts/window.h"

static glm::vec2 pixel_size;

using gl::Color;
using gl::ColorID;

void gui::init()
{
    pixel_size = glm::vec2(
            2.0f / gl::getWidth(),
            2.0f / gl::getHeight());
}

float gui::pixel_round_x(const float pIn)
{
    return pixel_size.x * round(pIn / pixel_size.x);
}
float gui::pixel_round_y(const float pIn)
{
    return pixel_size.y * round(pIn / pixel_size.y);
}
glm::vec2 gui::pixel_round(const glm::vec2 pIn)
{
    return pixel_size * round(pIn / pixel_size);
}

size_t gui::toPixelsX(const float screenX)
{
    return (size_t)round(screenX / pixel_size.x);
}
size_t gui::toPixelsY(const float screenY)
{
    return (size_t)round(screenY / pixel_size.y);
}
glm::uvec2 gui::toPixels(const glm::vec2 screen)
{
    return glm::uvec2(toPixelsX(screen.x), toPixelsY(screen.y));
}
float gui::toScreenX(const size_t pixelsX)
{
    return pixelsX * pixel_size.x;
}
float gui::toScreenY(const size_t pixelsY)
{
    return pixelsY * pixel_size.y;
}
glm::vec2 gui::toScreen(const glm::uvec2 pixels)
{
    return glm::vec2(toScreenX(pixels.x), toScreenY(pixels.y));
}

void gui::initWidgets()
{
    using namespace signals;
    using namespace input;
    Button::Colors buttonColors(gl::getColor("black"), gl::getColor("grey"));
    Button::Layout buttonLayout;
    Button::Preset buttonPreset(glm::vec4(0.0f, 0.0f, toScreenX(100), toScreenY(20)), buttonLayout, buttonColors);

    const float width = 1.0f;
    const size_t xcount = 5;
    //for (size_t w = 0; w < 100; ++w) {
    //    Button but(buttonPreset);
    //    but->move(glm::vec2(-1.0f + (width/xcount) * (w%xcount), 1.0f - (0.1f * floor(w/xcount))));
    //}

    Window::Colors windowColors(gl::getColor("black"), gl::getColor("white"), gl::getColor("black"), gl::getColor("black"), gl::getColor("black"), gl::getColor("black"), buttonColors);
    Window::Layout windowLayout;
    Window::Preset windowPreset(glm::vec4(0.0f, 0.0f, toScreenX(500), toScreenY(400)), windowLayout, windowColors);

    for (size_t w = 0; w < 1; ++w) {
        Window but(windowPreset);
        but->move(glm::vec2(-1.0f + (width/xcount) * (w%xcount), 1.0f - (0.1f * floor(w/xcount))));
    }

    ////Slider
    //using Slider = QuadGroup<2>;
    //using SliderResizePolicy = WidgetResizePolicy<Slider>;
    //using SliderColors = ColorGroup<2, gl::ColorID>;

    //SliderColors slider_colors(gl::getColor("lightgrey"), gl::getColor("darkgrey"));
    //Slider::initer_t slider_initer({ glm::vec4(0.0f, 0.0f, 0.2f, 0.05f), glm::vec4(0.0f, 0.0f, 0.05f, 0.05f) });
    //std::array<glm::vec2, Slider::ELEMENT_COUNT> slider_move_matrix{ glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f) };
    //SliderResizePolicy slider_resize_policy({ glm::vec4(0.0f, 0.0f,1.0f, 1.0f), glm::vec4(0.0f, 0.0f,1.0f, 1.0f) });

    //Slider slider(slider_initer);
    //slider.color(slider_colors);
    //slider.move(0.7f, -0.7f);


    //Event slider_box_enter_evt = createEvent(QuadEvent(slider.element<0>().index, 1));
    //unsigned int slider_box_enter = createSignal(slider_box_enter_evt);
    //Event slider_box_leave_evt = createEvent(QuadEvent(slider.element<0>().index, 0));
    //unsigned int slider_box_leave = createSignal(slider_box_leave_evt);

    //Event slider_slide_enter_evt = createEvent(QuadEvent(slider.element<1>().index, 1));
    //unsigned int slider_slide_enter = createSignal(slider_slide_enter_evt);
    //Event slider_slide_leave_evt = createEvent(QuadEvent(slider.element<1>().index, 0));
    //unsigned int slider_slide_leave = createSignal(slider_slide_leave_evt);


    //gate<or_op, Event, Event> slider_any_enter_evt(or_op(), slider_slide_enter_evt, slider_box_enter_evt);
    //gate<or_op, Event, Event> slider_any_leave_evt(or_op(), slider_slide_leave_evt, slider_box_leave_evt);

    //gate<and_op, decltype(slider_any_enter_evt), not_gate<decltype(slider_any_leave_evt)>> slider_enter_evt(and_op(), slider_any_enter_evt, not_gate<decltype(slider_any_leave_evt)>(slider_any_leave_evt));
    //gate<and_op, decltype(slider_any_leave_evt), not_gate<decltype(slider_any_enter_evt)>> slider_leave_evt(and_op(), slider_any_leave_evt, not_gate<decltype(slider_any_enter_evt)>(slider_any_enter_evt));
    //switch_gate<decltype(slider_enter_evt), decltype(slider_leave_evt)> slider_hover_evt(slider_enter_evt, slider_leave_evt);

    //gate<and_op, decltype(slider_hover_evt), decltype(lmb.on_evt)> slider_press_evt(and_op(), slider_hover_evt, lmb.on_evt);

    //ButtonEvents<decltype(slider_press_evt), Event> slider_lmb(slider_press_evt, lmb.off_evt);

    //void(*move_quad_to)(Quad, float&) = [](Quad pTar, float& pPos)->void {
    //    float dist = pPos - (pTar.get_pos().x + pTar.get_size().x / 2.0f);
    //    pTar.move(glm::vec2(dist, 0.0f));
    //};

    //FunctorRef<void, Quad, float&> move_slide_func = createFunctor<void, Quad, float&>(move_quad_to, slider.element<1>().index, relativeCursorPosition.x);
    //move_slide_func.add_triggers({ slider_lmb.hold });

    //void(*lim_quad)(Quad, Quad) = [](Quad pTar, Quad pLim)->void {
    //    float l_dist = glm::vec2(pLim.get_pos() - pTar.get_pos()).x;
    //    float r_dist = glm::vec2((pLim.get_pos() + pLim.get_size()) - (pTar.get_pos() + pTar.get_size())).x;
    //    pTar.move(glm::vec2(std::min(std::max(l_dist, 0.0f), r_dist), 0.0f));
    //};

    //FunctorRef<void, Quad, Quad> limit_slide_func = createFunctor(lim_quad, slider.element<1>(), slider.element<0>());
    //limit_slide_func.add_triggers({ slider_lmb.hold });

    //void(*set_slide_target)(glm::vec4&, Quad, Quad) = [](glm::vec4& pTarget, Quad pBox, Quad pSlide)->void {
    //    float half_width = pSlide.get_size().x / 2.0f;
    //    float slide_pos = pSlide.get_pos().x + half_width;
    //    float box_size = pBox.get_size().x;
    //    float box_pos = pBox.get_pos().x;
    //    float amt = (slide_pos - box_pos) / (box_size - half_width);
    //    pTarget = glm::vec4(amt, amt, amt, amt)*10.0f;
    //};
    //FunctorRef<void, glm::vec4&, Quad, Quad> set_slide_target_func = createFunctor<void, glm::vec4&, Quad, Quad>(set_slide_target, lights::getLightColor(0), slider.element<0>(), slider.element<1>());
    //set_slide_target_func.add_triggers({ slider_lmb.hold });

    //text
    //gui::text::createTextboxMetrics(0, 1.0, 1.0, 1.0, 1.0);

    //unsigned int qu_tb = gui::text::createTextbox(quit_button.subwidgets.element<1>(), 0, TEXT_LAYOUT_CENTER_Y);
    //unsigned int pl_tb = gui::text::createTextbox(play_button.subwidgets.element<1>(), 0, TEXT_LAYOUT_CENTER_Y);
    //unsigned int fps_tb = gui::text::createTextbox(fps_box.element<0>().element(), 0, TEXT_LAYOUT_CENTER_Y);

    //gui::text::setTextboxString(qu_tb, " QUIT");
    //gui::text::setTextboxString(pl_tb, " Play");
    //gui::text::setTextboxString(fps_tb, "FPS");
}

