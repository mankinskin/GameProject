#include "../widget.h"
#include "../gui.h"
#include "../color.h"
#include "../mouse.h"

namespace gui
{
    struct Button : public Widget<WidgetElements<QuadElement, QuadElement>, WidgetColors<gl::ColorID, gl::ColorID>>
    {
        using Elements = WidgetElements<QuadElement, QuadElement>;
        using Colors = WidgetColors<gl::ColorID, gl::ColorID>;
        using Wid = Widget<Elements, Colors>;
        struct ElementPreset : public Wid::ElementPreset
        {
            ElementPreset(Wid::ElementPreset preset)
                : Wid::ElementPreset(preset)
            {}
            ElementPreset(size_t pixWidth, size_t pixHeight, size_t pixMarginX, size_t pixMarginY)
                : Wid::ElementPreset({
                            glm::vec4(0.0f, 0.0f, toScreenX(pixWidth), toScreenY(pixHeight)),
                            glm::vec4(toScreenX(pixMarginX), -toScreenY(pixMarginY),
                                toScreenX(pixWidth) - toScreenX(pixMarginX*2),
                                toScreenY(pixHeight) - toScreenY(pixMarginY*2))},
                    {glm::vec2(1.0f, 1.0f),
                    glm::vec2(1.0f, 1.0f)},
                    {glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
                    glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)})
            {}
        };
        struct Preset : public Wid::Preset
        {
            Preset(const ElementPreset e, const Colors c)
                : Wid::Preset(e, c)
            {}
        };

        Button(Preset preset)
            : Wid(preset)
        {
            using namespace signals;
            using namespace input;
            auto click_button = ifAll(hover(), Mouse::lmb.down());
            auto release_button = ifAny(leave(), ifAll(hover(), Mouse::lmb.up()));

            link(enter(), functor(colorQuad, std::get<0>(elements), gl::getColor("yellow")));
            link(leave(), functor(colorQuad, std::get<0>(elements), std::get<0>(colors)));

            link(click_button, functor(colorQuad, std::get<1>(elements), gl::getColor("red")));
            link(release_button, functor(colorQuad, std::get<1>(elements), std::get<1>(colors)));
        }
    };
}
