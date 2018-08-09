#include "../widget.h"
#include "../gui.h"
#include "../color.h"
#include "../mouse.h"

namespace gui
{
    struct Button
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

        const utils::ID<Wid> wid;

        Button(Preset preset)
            : wid(utils::makeID(Wid(preset)))
        {
            using namespace signals;

            link(wid->on, functor(colorQuad, std::get<0>(wid->elements), gl::getColor("white")));
            link(wid->off, functor(colorQuad, std::get<0>(wid->elements), std::get<0>(wid->colors)));

            link(wid->press, functor(colorQuad, std::get<1>(wid->elements), gl::getColor("white")));
            link(wid->release, functor(colorQuad, std::get<1>(wid->elements), std::get<1>(wid->colors)));

            link(wid->hold, functor<decltype(moveWidget<Wid>), utils::ID<Wid>, glm::vec2&>(moveWidget<Wid>, wid, input::cursorFrameDelta));
        }
    };
}
