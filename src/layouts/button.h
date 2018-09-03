#pragma once
#include "../widget.h"
#include "../gui.h"
#include "../color.h"
#include "../mouse.h"

namespace gui
{
    template<size_t Count>
    struct ButtonGenerator
    {
        static constexpr size_t ELEMENT_COUNT = 2;
        static_assert(Count == ELEMENT_COUNT, "Generator only fit for 2 elements!");

        constexpr ButtonGenerator(const size_t mx = 1, const size_t my = 1)
            : marginx(mx)
            , marginy(my)
        {}

        const size_t marginx;
        const size_t marginy;
        static const std::array<glm::vec2, Count> movepolicy;
        static const std::array<glm::vec4, Count> resizepolicy;

        const typename utils::tuple_generator<ELEMENT_COUNT, glm::vec4>::type genQuads(const glm::vec4 q) const
        {
            return typename utils::tuple_generator<ELEMENT_COUNT, glm::vec4>::type{
                glm::vec4(q.x, q.y, q.z, q.w),
                glm::vec4(q.x + toScreenX(marginx), q.y - toScreenY(marginy),
                            q.z - toScreenX(marginx*2),
                            q.w - toScreenY(marginy*2))};
        }

        template<typename Layout, typename... Elems>
        static void setup(const Widget<Layout, Elems...>& w)
        {
            using namespace signals;
            using Colors = typename Widget<Layout, Elems...>::Colors::Colors;
            using Elements = typename Widget<Layout, Elems...>::Elements::Elements;
            link(w.enter, func(applyColor<std::tuple_element_t<0, Colors>, std::tuple_element_t<0, Elements>>, gl::getColor("white"), std::get<0>(w.elements)));
            link(w.leave, func(applyColor<std::tuple_element_t<0, Colors>, std::tuple_element_t<0, Elements>>, std::get<0>(w.elements).color, std::get<0>(w.elements)));

            link(w.press, func(applyColor<std::tuple_element_t<1, Colors>, std::tuple_element_t<1, Elements>>, gl::getColor("white"), std::get<1>(w.elements)));
            link(w.press, func(applyColor<std::tuple_element_t<0, Colors>, std::tuple_element_t<0, Elements>>, gl::getColor("white"), std::get<0>(w.elements)));

            link(w.release, func(applyColor<std::tuple_element_t<1, Colors>, std::tuple_element_t<1, Elements>>, std::get<1>(w.elements).color, std::get<1>(w.elements)));
            //link(w.release, func(applyColor<std::tuple_element_t<0, Colors>, std::tuple_element_t<0, Elements>>, std::get<0>(w.elements).color, std::get<0>(w.elements)));
        }
    };

    template<size_t Count>
        const std::array<glm::vec2, Count>
            ButtonGenerator<Count>::movepolicy = {glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f)};
    template<size_t Count>
        const std::array<glm::vec4, Count>
            ButtonGenerator<Count>::resizepolicy = {glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
                    glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)};

    using ButtonLayout = WidgetLayout<ButtonGenerator, QuadElement<gl::Color>, QuadElement<gl::Color>>;
    using Button = Widget<ButtonLayout, QuadElement<gl::ColorID>, QuadElement<gl::ColorID>>;
}
