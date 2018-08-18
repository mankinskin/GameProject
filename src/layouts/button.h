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
        template<typename Elements, typename Colors>
        static void setup(const Widget<Elements, Colors>& w)
        {
            link(w.enter, func(applyColor<std::tuple_element_t<0, typename Colors::Colors>, std::tuple_element_t<0, typename Elements::Elements>>, gl::getColor("white"), std::get<0>(w.elements)));
            link(w.leave, func(applyColor<std::tuple_element_t<0, typename Colors::Colors>, std::tuple_element_t<0, typename Elements::Elements>>, std::get<0>(w.colors), std::get<0>(w.elements)));

            link(w.press, func(applyColor<std::tuple_element_t<1, typename Colors::Colors>, std::tuple_element_t<1, typename Elements::Elements>>, gl::getColor("white"), std::get<1>(w.elements)));
            link(w.leave, func(applyColor<std::tuple_element_t<1, typename Colors::Colors>, std::tuple_element_t<1, typename Elements::Elements>>, std::get<1>(w.colors), std::get<1>(w.elements)));

        }
    };
        const std::array<glm::vec2, ButtonLayout::ELEMENT_COUNT>
            ButtonLayout::movepolicy = {glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f)};
        const std::array<glm::vec4, ButtonLayout::ELEMENT_COUNT>
            ButtonLayout::resizepolicy = {glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
                    glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)};

    namespace Button
    {
        using Elements = WidgetElements<ButtonLayout, QuadElement, QuadElement>;
        using Colors = WidgetColors<gl::ColorID, gl::ColorID>;
        using Widget = Widget<Elements, Colors>;
        using ElementPreset = typename Widget::ElementPreset;
        using Preset = typename Widget::Preset;
    };

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
        template<typename Elements, typename Colors>
        static void setup(const Widget<Elements, Colors>& w)
        {
            using namespace signals;
        }
    };
        const std::array<glm::vec2, ButtonListLayout::ELEMENT_COUNT>
            ButtonListLayout::movepolicy = {glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f)};
        const std::array<glm::vec4, ButtonListLayout::ELEMENT_COUNT>
            ButtonListLayout::resizepolicy = {glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
                    glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)};

    namespace ButtonList
    {
        using Elements = WidgetElements<ButtonListLayout, Button::Elements, Button::Elements>;
        using Colors = WidgetColors<Button::Colors, Button::Colors>;
        using Widget = Widget<Elements, Colors>;
        using ElementPreset = typename Widget::ElementPreset;
        using Preset = typename Widget::Preset;
    }
}
