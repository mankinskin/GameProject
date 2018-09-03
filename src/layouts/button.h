#pragma once
#include "../widget.h"
#include "../gui.h"
#include "../color.h"
#include "../mouse.h"

namespace gui
{
    using ButtonBase = Widget<QuadElement<gl::ColorID>, QuadElement<gl::ColorID>>;
    struct Button : public ButtonBase
    {
        static constexpr size_t ELEMENT_COUNT = 2;
        using Base = ButtonBase;
        using Colors = typename Base::Colors;
        using Preset = typename Base::Preset;

        Button(const glm::vec4 q, const Preset pre)
            : Base(q, genQuads(q), pre)
        {
            setup();
        }

        static constexpr size_t marginx = 2;
        static constexpr size_t marginy = 2;

        static const typename Base::Quads genQuads(const glm::vec4 q)
        {
            return typename Base::Quads{
                glm::vec4(q.x, q.y, q.z, q.w),
                glm::vec4(q.x + toScreenX(marginx), q.y - toScreenY(marginy),
                            q.z - toScreenX(marginx*2),
                            q.w - toScreenY(marginy*2))};
        }

        void setup() const
        {
            using namespace signals;
            using Colors = typename Base::Colors::Colors;
            using Elements = typename Base::Elements::Elements;
            const Base& w = *this;
            link(w.enter, func(applyColor<std::tuple_element_t<0, Elements>>, std::get<0>(w.elements), gl::getColor("white")));
            link(w.leave, func(applyColor<std::tuple_element_t<0, Elements>>, std::get<0>(w.elements), std::get<0>(w.elements).color));

            link(w.press, func(applyColor<std::tuple_element_t<1, Elements>>, std::get<1>(w.elements), gl::getColor("white")));
            link(w.press, func(applyColor<std::tuple_element_t<0, Elements>>, std::get<0>(w.elements), gl::getColor("white")));

            link(w.release, func(applyColor<std::tuple_element_t<1, Elements>>, std::get<1>(w.elements), std::get<1>(w.elements).color));
            link(w.release, func(applyColor<std::tuple_element_t<0, Elements>>, std::get<0>(w.elements), std::get<0>(w.elements).color));
        }
    };
    const typename Button::Preset buttonPreset = typename Button::Preset({glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f)}, {glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)}, {gl::ColorID(1), gl::ColorID(2)});
}
