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
        static void setup(const utils::ID<Widget<Elements, Colors>> w)
        {
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

    //struct ButtonListLayout
    //{
    //    static constexpr const size_t ELEMENT_COUNT = 2;

    //    static const std::array<glm::vec2, ELEMENT_COUNT> movepolicy;
    //    static const std::array<glm::vec4, ELEMENT_COUNT> resizepolicy;

    //    static const typename utils::tuple_generator<ELEMENT_COUNT, glm::vec4>::type genQuads(const glm::vec4 q)
    //    {
    //        return typename utils::tuple_generator<ELEMENT_COUNT, glm::vec4>::type{
    //            glm::vec4(q.x, q.y, q.z, q.w/2.0f),
    //            glm::vec4(q.x, q.y - q.w/2.0f, q.z, q.w/2.0f)};
    //    }
    //    template<typename Elements, typename Colors>
    //    static void setup(const utils::ID<Widget<Elements, Colors>> w)
    //    {
    //        using namespace signals;
    //    }
    //};
    //    const std::array<glm::vec2, ButtonListLayout::ELEMENT_COUNT>
    //        ButtonListLayout::movepolicy = {glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f)};
    //    const std::array<glm::vec4, ButtonListLayout::ELEMENT_COUNT>
    //        ButtonListLayout::resizepolicy = {glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
    //                glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)};

    //struct ButtonList
    //{
    //    using Elements = WidgetElements<ButtonListLayout, Button::Elements, Button::Elements>;
    //    using Colors = WidgetColors<Button::Colors, Button::Colors>;
    //    using Wid = Widget<Elements, Colors>;
    //    using ElementPreset = typename Wid::ElementPreset;

    //    struct Preset : public Wid::Preset
    //    {
    //        Preset(const ElementPreset e, const Colors c)
    //            : Wid::Preset(e, c)
    //        {}
    //    };

    //    const utils::ID<Wid> wid;

    //    ButtonList(Preset preset)
    //        : wid(utils::makeID(Wid(preset)))
    //    {
    //        ButtonListLayout::setup(wid);
    //    }
    //};
}
