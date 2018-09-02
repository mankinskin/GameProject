#pragma once
#include "../widget.h"
#include "../gui.h"
#include "../color.h"
#include "../mouse.h"

namespace gui
{
    template<typename Outline, typename Center>
    struct ButtonLayout : public WidgetLayout<Outline, Center>
    {
        constexpr ButtonLayout(const size_t mx = 1, const size_t my = 1)
            : marginx(mx)
            , marginy(my)
        {}
        static constexpr const size_t ELEMENT_COUNT = 2;

        const size_t marginx;
        const size_t marginy;
        static const std::array<glm::vec2, ELEMENT_COUNT> movepolicy;
        static const std::array<glm::vec4, ELEMENT_COUNT> resizepolicy;

        const typename utils::tuple_generator<ELEMENT_COUNT, glm::vec4>::type genQuads(const glm::vec4 q) const
        {
            return typename utils::tuple_generator<ELEMENT_COUNT, glm::vec4>::type{
                glm::vec4(q.x, q.y, q.z, q.w),
                glm::vec4(q.x + toScreenX(marginx), q.y - toScreenY(marginy),
                            q.z - toScreenX(marginx*2),
                            q.w - toScreenY(marginy*2))};
        }

        static void setup(const Widget<ButtonLayout, Outline, Center>& w)
        {
            using namespace signals;
            using Colors = typename Widget<ButtonLayout, Outline, Center>::Colors::Colors;
            using Elements = typename Widget<ButtonLayout, Outline, Center>::Elements::Elements;
            //link(w.enter, func(applyColor<std::tuple_element_t<0, Colors>, std::tuple_element_t<0, Elements>>, gl::getColor("white"), std::get<0>(w.elements)));
            //link(w.leave, func(applyColor<std::tuple_element_t<0, Colors>, std::tuple_element_t<0, Elements>>, std::get<0>(w.elements).color, std::get<0>(w.elements)));

            link(w.press, func(applyColor<std::tuple_element_t<1, Colors>, std::tuple_element_t<1, Elements>>, gl::getColor("white"), std::get<1>(w.elements)));
            link(w.press, func(applyColor<std::tuple_element_t<0, Colors>, std::tuple_element_t<0, Elements>>, gl::getColor("white"), std::get<0>(w.elements)));

            //link(w.release, func(applyColor<std::tuple_element_t<1, Colors>, std::tuple_element_t<1, Elements>>, std::get<1>(w.elements).color, std::get<1>(w.elements)));
            link(w.release, func(applyColor<std::tuple_element_t<0, Colors>, std::tuple_element_t<0, Elements>>, std::get<0>(w.elements).color, std::get<0>(w.elements)));
        }
    };

    template<typename Outline, typename Center>
        const std::array<glm::vec2, ButtonLayout<Outline, Center>::ELEMENT_COUNT>
            ButtonLayout<Outline, Center>::movepolicy = {glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f)};
    template<typename Outline, typename Center>
        const std::array<glm::vec4, ButtonLayout<Outline, Center>::ELEMENT_COUNT>
            ButtonLayout<Outline, Center>::resizepolicy = {glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
                    glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)};

    using Button = Widget<ButtonLayout, QuadElement<gl::ColorID>, QuadElement<gl::ColorID>>;
}
