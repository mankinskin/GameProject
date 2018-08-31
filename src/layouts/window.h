#pragma once
#include "../widget.h"
#include "../gui.h"
#include "../color.h"
#include "../mouse.h"
#include "button.h"

namespace gui
{
    template<typename... Elems>
    struct WindowLayout : public WidgetLayout<Elems...>
    {
        static constexpr const size_t ELEMENT_COUNT = 7;
        constexpr WindowLayout(const size_t mx = 5, const size_t my = 5)
            : WidgetLayout<Elems...>(NullLayout(), NullLayout(), NullLayout(), NullLayout(), NullLayout(), NullLayout(), Button::Layout(mx, my))
            , marginx(mx)
            , marginy(my)
        {}

        static constexpr const size_t headerHeight = 15;
        const size_t marginx;
        const size_t marginy;
        static const std::array<glm::vec2, ELEMENT_COUNT> movepolicy;
        static const std::array<glm::vec4, ELEMENT_COUNT> resizepolicy;

        const typename utils::tuple_generator<ELEMENT_COUNT, glm::vec4>::type genQuads(const glm::vec4 q) const
        {
            return typename utils::tuple_generator<ELEMENT_COUNT, glm::vec4>::type{
                glm::vec4(q.x, q.y - toScreenY(headerHeight), toScreenX(marginx), q.w - toScreenY(marginy + headerHeight)),
                glm::vec4(q.x + toScreenX(marginx), q.y - toScreenY(headerHeight), q.z - toScreenX(marginx*2), q.w - toScreenY(marginy + headerHeight)),
                glm::vec4(q.x + (q.z - toScreenX(marginx)), q.y - toScreenY(headerHeight), toScreenX(marginx), q.w - toScreenY(marginy + headerHeight)),

                glm::vec4(q.x, q.y - (q.w - toScreenY(marginy)), toScreenX(marginx), toScreenY(marginy)),
                glm::vec4(q.x + toScreenX(marginx), q.y - (q.w - toScreenY(marginy)), q.z - toScreenX(marginx*2), toScreenY(marginy)),
                glm::vec4(q.x + q.z - toScreenX(marginx), q.y - (q.w - toScreenY(marginy)), toScreenX(marginx), toScreenY(marginy)),

                glm::vec4(q.x, q.y, q.z, toScreenY(headerHeight))};
        }
        static void setup(const Widget<WindowLayout, Elems...>& w)
        {
            using namespace signals;
            using Colors = typename Widget<WindowLayout, Elems...>::Colors::Colors;
            using Elements = typename Widget<WindowLayout, Elems...>::Elements::Elements;

            link(std::get<6>(w.elements).hold, refFunc(moveWidget<Widget<WindowLayout, Elems...>>, (Widget<WindowLayout, Elems...>)w, input::cursorFrameDelta));
            link(std::get<5>(w.elements).hold, refFunc(resizeWidget<Widget<WindowLayout, Elems...>>, (Widget<WindowLayout, Elems...>)w, input::cursorFrameDelta));
            link(std::get<2>(w.elements).hold, refFunc(resizeWidgetX<Widget<WindowLayout, Elems...>>, (Widget<WindowLayout, Elems...>)w, input::cursorFrameDelta.x));
            link(std::get<4>(w.elements).hold, refFunc(resizeWidgetY<Widget<WindowLayout, Elems...>>, (Widget<WindowLayout, Elems...>)w, input::cursorFrameDelta.y));
        }
    };
    template<typename... Elems>
        const std::array<glm::vec2, WindowLayout<Elems...>::ELEMENT_COUNT>
            WindowLayout<Elems...>::movepolicy = {glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f)};
    template<typename... Elems>
        const std::array<glm::vec4, WindowLayout<Elems...>::ELEMENT_COUNT>
            WindowLayout<Elems...>::resizepolicy = {glm::vec4(0.0f, 0.0f, 0.0f, -1.0f), glm::vec4(0.0f, 0.0f, 1.0f, -1.0f), glm::vec4(1.0f, 0.0f, 0.0f, -1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 1.0f, 0.0f), glm::vec4(1.0f, 1.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)};

        using Window = Widget<WindowLayout, QuadElement<gl::ColorID>, QuadElement<gl::ColorID>, QuadElement<gl::ColorID>, QuadElement<gl::ColorID>, QuadElement<gl::ColorID>, QuadElement<gl::ColorID>, Button>;

}
