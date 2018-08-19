#include "../widget.h"
#include "../gui.h"
#include "../color.h"
#include "../mouse.h"

namespace gui
{
    struct ButtonLayout
    {
        static constexpr const size_t ELEMENT_COUNT = 2;

        static constexpr const size_t marginx = 1;
        static constexpr const size_t marginy = 1;
        static const std::array<glm::vec2, ELEMENT_COUNT> movepolicy;
        static const std::array<glm::vec4, ELEMENT_COUNT> resizepolicy;

        static const typename utils::tuple_generator<ELEMENT_COUNT, glm::vec4>::type genQuads(const glm::vec4 q)
        {
            return typename utils::tuple_generator<ELEMENT_COUNT, glm::vec4>::type{
                glm::vec4(q.x, q.y, q.z, q.w),
                glm::vec4(q.x + toScreenX(marginx), q.y - toScreenY(marginy),
                            q.z - toScreenX(marginx*2),
                            q.w - toScreenY(marginy*2))};
        }
        template<typename... Elems>
        static void setup(const Widget<ButtonLayout, Elems...>& w)
        {
            using namespace signals;
            using Colors = typename Widget<ButtonLayout, Elems...>::Colors::Colors;
            using Elements = typename Widget<ButtonLayout, Elems...>::Elements::Elements;
            link(w.enter, func(applyColor<std::tuple_element_t<0, Colors>, std::tuple_element_t<0, Elements>>, gl::getColor("white"), std::get<0>(w.elements)));
            link(w.leave, func(applyColor<std::tuple_element_t<0, Colors>, std::tuple_element_t<0, Elements>>, std::get<0>(w.elements).color, std::get<0>(w.elements)));

            link(w.press, func(applyColor<std::tuple_element_t<1, Colors>, std::tuple_element_t<1, Elements>>, gl::getColor("white"), std::get<1>(w.elements)));
            link(w.release, func(applyColor<std::tuple_element_t<1, Colors>, std::tuple_element_t<1, Elements>>, std::get<1>(w.elements).color, std::get<1>(w.elements)));
        }
    };
        const std::array<glm::vec2, ButtonLayout::ELEMENT_COUNT>
            ButtonLayout::movepolicy = {glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f)};
        const std::array<glm::vec4, ButtonLayout::ELEMENT_COUNT>
            ButtonLayout::resizepolicy = {glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
                    glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)};

    using Button = Widget<ButtonLayout, QuadElement<gl::ColorID>, QuadElement<gl::ColorID>>;

    struct ButtonListLayout
    {
        static constexpr const size_t ELEMENT_COUNT = 2;

        static const std::array<glm::vec2, ELEMENT_COUNT> movepolicy;
        static const std::array<glm::vec4, ELEMENT_COUNT> resizepolicy;

        static const typename utils::tuple_generator<ELEMENT_COUNT, glm::vec4>::type genQuads(const glm::vec4 q)
        {
            return typename utils::tuple_generator<ELEMENT_COUNT, glm::vec4>::type{
                glm::vec4(q.x, q.y, q.z, q.w/2.0f),
                glm::vec4(q.x, q.y - q.w/2.0f, q.z, q.w/2.0f)};
        }
        template<typename... Elems>
        static void setup(const Widget<ButtonListLayout, Elems...>& w)
        {
            using namespace signals;
            link(std::get<0>(w.elements).hold, refFunc(moveWidget<Widget<ButtonListLayout, Elems...>>, (Widget<ButtonListLayout, Elems...>)w, (glm::vec2&)input::cursorFrameDelta));
        }
    };
        const std::array<glm::vec2, ButtonListLayout::ELEMENT_COUNT>
            ButtonListLayout::movepolicy = {glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f)};
        const std::array<glm::vec4, ButtonListLayout::ELEMENT_COUNT>
            ButtonListLayout::resizepolicy = {glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
                    glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)};

    using ButtonList = Widget<ButtonListLayout, Button, Button>;
}
