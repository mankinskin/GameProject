#pragma once
#include "../widget.h"
#include "../gui.h"
#include "../color.h"
#include "../mouse.h"
#include "button.h"

namespace gui
{
    template<size_t Count>
    struct WindowGenerator
    {
        static constexpr const size_t ELEMENT_COUNT = 7;
        static_assert(Count == ELEMENT_COUNT, "Generator only fit for 7 elements!");

        constexpr WindowGenerator(const size_t mx = 5, const size_t my = 5, const size_t hy = 25, const size_t hmx = 5, const size_t hmy = 5)
            : marginx(mx)
            , marginy(my)
            , headerHeight(hy)
        {}

        const size_t headerHeight;
        const size_t marginx;
        const size_t marginy;
        static const std::array<glm::vec2, Count> movepolicy;
        static const std::array<glm::vec4, Count> resizepolicy;

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

        template<typename... Elems>
            using Window = Widget<WidgetLayout<WindowGenerator, Elems...>, Elems...>;
        template<typename... Elems>
        static void setup(const Window<Elems...>& w)
        {
            using namespace signals;
            using Colors = typename Window<Elems...>::Colors::Colors;
            using Elements = typename Window<Elems...>::Elements::Elements;

            link(std::get<6>(w.elements).hold, refFunc(moveWidget<Window<Elems...>>, (Window<Elems...>)w, input::cursorFrameDelta));
            link(std::get<5>(w.elements).hold, refFunc(resizeWidget<Window<Elems...>>, (Window<Elems...>)w, input::cursorFrameDelta));
            link(std::get<2>(w.elements).hold, refFunc(resizeWidgetX<Window<Elems...>>, (Window<Elems...>)w, input::cursorFrameDelta.x));
            link(std::get<4>(w.elements).hold, refFunc(resizeWidgetY<Window<Elems...>>, (Window<Elems...>)w, input::cursorFrameDelta.y));
        }
    };
    template<size_t Count>
        const std::array<glm::vec2, Count>
            WindowGenerator<Count>::movepolicy = {glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f)};
    template<size_t Count>
        const std::array<glm::vec4, Count>
            WindowGenerator<Count>::resizepolicy = {glm::vec4(0.0f, 0.0f, 0.0f, -1.0f), glm::vec4(0.0f, 0.0f, 1.0f, -1.0f), glm::vec4(1.0f, 0.0f, 0.0f, -1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 1.0f, 0.0f), glm::vec4(1.0f, 1.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)};

        using WindowLayout = WidgetLayout<WindowGenerator, QuadElement<gl::ColorID>, QuadElement<gl::ColorID>, QuadElement<gl::ColorID>, QuadElement<gl::ColorID>, QuadElement<gl::ColorID>, QuadElement<gl::ColorID>, Button>;
        using Window = Widget<WindowLayout, QuadElement<gl::ColorID>, QuadElement<gl::ColorID>, QuadElement<gl::ColorID>, QuadElement<gl::ColorID>, QuadElement<gl::ColorID>, QuadElement<gl::ColorID>, Button>;
}
