#pragma once
#include "../widget.h"
#include "../gui.h"
#include "../color.h"
#include "../mouse.h"
#include "button.h"

namespace gui
{
    struct WindowLayout
    {
        static constexpr const size_t ELEMENT_COUNT = 7;

        static constexpr const size_t headerHeight = 10;
        static constexpr const size_t marginx = 5;
        static constexpr const size_t marginy = 5;
        static const std::array<glm::vec2, ELEMENT_COUNT> movepolicy;
        static const std::array<glm::vec4, ELEMENT_COUNT> resizepolicy;

        static const typename utils::tuple_generator<ELEMENT_COUNT, glm::vec4>::type genQuads(const glm::vec4 q)
        {
            return typename utils::tuple_generator<ELEMENT_COUNT, glm::vec4>::type{
                glm::vec4(q.x, q.y - toScreenY(headerHeight), toScreenX(marginx), q.w - toScreenY(marginy + headerHeight)),
                glm::vec4(q.x + toScreenX(marginx), q.y - toScreenY(marginy), q.z - toScreenX(marginx*2), q.w - toScreenY(marginy + headerHeight)),
                glm::vec4(q.x + (q.z - toScreenX(marginx)), q.y - toScreenY(marginy), toScreenX(marginx), q.w - toScreenY(marginy + headerHeight)),

                glm::vec4(q.x, q.y - (q.w - toScreenY(headerHeight)), toScreenX(marginx), toScreenY(marginy)),
                glm::vec4(q.x + toScreenX(marginx), q.y - (q.w - toScreenY(headerHeight)), q.z - toScreenX(marginx*2), toScreenY(marginy)),
                glm::vec4(q.x + q.z - toScreenX(marginx), q.y - (q.w - toScreenY(headerHeight)), toScreenX(marginx), toScreenY(marginy)),

                glm::vec4(q.x, q.y, q.z, toScreenY(headerHeight))};
        }
        template<typename... Elems>
        static void setup(const Widget<WindowLayout, Elems...>& w)
        {
            using namespace signals;
            using Colors = typename Widget<WindowLayout, Elems...>::Colors::Colors;
            using Elements = typename Widget<WindowLayout, Elems...>::Elements::Elements;

            link(std::get<6>(w.elements).hold, refFunc(moveWidget<Widget<WindowLayout, Elems...>>, (Widget<WindowLayout, Elems...>)w, input::cursorFrameDelta));
            link(std::get<5>(w.elements).hold, refFunc(resizeWidget<Widget<WindowLayout, Elems...>>, (Widget<WindowLayout, Elems...>)w, input::cursorFrameDelta));
        }
    };
        const std::array<glm::vec2, WindowLayout::ELEMENT_COUNT>
            WindowLayout::movepolicy = {glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f)};
        const std::array<glm::vec4, WindowLayout::ELEMENT_COUNT>
            WindowLayout::resizepolicy = {glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)};

        using Window = Widget<WindowLayout, QuadElement<gl::ColorID>, QuadElement<gl::ColorID>, QuadElement<gl::ColorID>, QuadElement<gl::ColorID>, QuadElement<gl::ColorID>, QuadElement<gl::ColorID>, Button>;

}
