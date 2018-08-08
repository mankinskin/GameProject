#include "../widget.h"
#include "../gui.h"
#include "../color.h"
#include "../mouse.h"

namespace gui
{
    struct Button : public Widget<Widget<QuadID>, Widget<QuadID>>
    {
        using Wid = Widget<Widget<QuadID>, Widget<QuadID>>;
        struct Preset : public Wid::Preset
        {
            Preset(Wid::Preset preset)
                : Wid::Preset(preset)
            {}
            Preset(size_t pixWidth, size_t pixHeight, size_t pixMarginX, size_t pixMarginY)
                : Wid::Preset({
                        Widget<QuadID>::Preset(
                            glm::vec4(0.0f, 0.0f, toScreenX(pixWidth), toScreenY(pixHeight))),
                        Widget<QuadID>::Preset(
                            glm::vec4(toScreenX(pixMarginX), -toScreenY(pixMarginY),
                                toScreenX(pixWidth) - toScreenX(pixMarginX*2),
                                toScreenY(pixHeight) - toScreenY(pixMarginY*2)))},
                    {glm::vec2(1.0f, 1.0f),
                    glm::vec2(1.0f, 1.0f)},
                    {glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
                    glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)})
            {}
        };

        Button(Preset preset)
            : Wid(preset)
        {
            using namespace signals;
            using namespace input;
            auto click_button = ifAll(hover(), Mouse::lmb.down());
            auto release_button = ifAny(leave(), ifAll(hover(), Mouse::lmb.up()));

            link(enter(), functor(colorQuad, std::get<0>(std::get<0>(elements).elements), gl::getColor("yellow")));
            //link(leave(), functor(colorQuad, std::get<0>(std::get<0>(elements).elements), std::get<0>(elements).color));

            link(click_button, functor(colorQuad, std::get<0>(std::get<1>(elements).elements), gl::getColor("red")));
            //link(release_button, functor(colorQuad, std::get<0>(std::get<1>(elements).elements), std::get<1>(elements).color));
        }
    };
}
