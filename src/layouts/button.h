#include "../widget.h"
#include "../gui.h"
#include "../color.h"
#include "../mouse.h"

namespace gui
{
    using ColorQuad = QuadWidget<gl::ColorID>;

    struct Button : public Widget<ColorQuad, ColorQuad>
    {
        using Widget = Widget<ColorQuad, ColorQuad>;
        struct Preset : public Widget::Preset
        {
            Preset(Widget::Preset preset)
                : Widget::Preset(preset)
            {}
            Preset(size_t pixWidth, size_t pixHeight, size_t pixMarginX, size_t pixMarginY,
                    gl::ColorID outline, gl::ColorID fill)
                : Widget::Preset({
                        ColorQuad::Preset(
                            glm::vec4(0.0f, 0.0f, toScreenX(pixWidth), toScreenY(pixHeight)),
                            outline),
                        ColorQuad::Preset(
                            glm::vec4(toScreenX(pixMarginX), -toScreenY(pixMarginY),
                                toScreenX(pixWidth) - toScreenX(pixMarginX*2),
                                toScreenY(pixHeight) - toScreenY(pixMarginY*2)),
                            fill)},
                    {glm::vec2(1.0f, 1.0f),
                    glm::vec2(1.0f, 1.0f)},
                    {glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
                    glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)})
            {}
        };

        Button(Preset preset)
            : Widget(preset)
        {
            using namespace signals;
            using namespace input;
            auto click_button = ifAll(hover(), Mouse::lmb.down());
            auto release_button = ifAny(leave(), ifAll(hover(), Mouse::lmb.up()));

            link(enter(), functor(colorQuad, std::get<0>(elements).elem, gl::getColor("yellow")));
            link(leave(), functor(colorQuad, std::get<0>(elements).elem, std::get<0>(elements).color));

            link(click_button, functor(colorQuad, std::get<1>(elements).elem, gl::getColor("red")));
            link(release_button, functor(colorQuad, std::get<1>(elements).elem, std::get<1>(elements).color));
        }
    };
}