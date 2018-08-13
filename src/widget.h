#pragma once
#include <array>
#include <tuple>
#include <utility>
#include <vector>
#include <glm.hpp>
#include "color.h"
#include "quad.h"
#include "quadcolors.h"
#include "utils/id.h"
#include "utils/array_utils.h"
#include "utils/tuple_utils.h"
#include "signal.h"
#include "functor.h"
#include "mouse.h"
#include "event.h"


//-----Widgets
// This class is rather complex. It also does a little too many things in one place.
// However it provides a very efficient and stable, yet extremely flexible foundation for a class hierarchy
//
// To create a Widget composed of 2D quads you have to provide some data:
// - the initial data to be loaded into each element(quad) of the widget
// - the policies about how the element should pass an input command to its
// elements

namespace gui
{
    template<typename... Ts>
    struct WidgetSignals
    {
        using SignalType = signals::SignalListener<signals::And, signals::SignalListener<signals::Or, typename Ts::SignalType...>, signals::SignalListener<signals::Nor, typename Ts::SignalType...>>;
        using HoverType = signals::SignalListener<signals::Flip, SignalType, SignalType>;
        using HoldType = signals::SignalListener<signals::Flip, signals::SignalListener<signals::And, HoverType, signals::ButtonSignals<input::MouseKey>::SignalType>, signals::ButtonSignals<input::MouseKey>::SignalType>;
        using PressType = signals::SignalListener<signals::Clicker, typename HoldType::Set, typename HoldType::Reset>;
        using ReleaseType = signals::SignalListener<signals::Clicker, typename HoldType::Set, typename HoldType::Reset>;

        template<bool S, size_t... Ns>
            constexpr SignalType gen_event(const std::tuple<Ts...> elems, const std::index_sequence<Ns...>) const
            {
                return signals::ifAll(
                        signals::ifAny(std::get<Ns>(elems).event(utils::_bool<S>())...),
                        signals::ifNone(std::get<Ns>(elems).event(utils::_bool<!S>())...));
            }

        constexpr const SignalType event(utils::_bool<true>) const
        {
            return enter;
        }
        constexpr const SignalType event(utils::_bool<false>) const
        {
            return leave;
        }
        constexpr WidgetSignals(const std::tuple<Ts...> os)
            : enter(gen_event<true>(os, std::make_index_sequence<sizeof...(Ts)>()))
            , leave(gen_event<false>(os, std::make_index_sequence<sizeof...(Ts)>()))
            , hover(flip(enter, leave))
            , hold(flip(ifAll(hover, input::Mouse::lmb->on), input::Mouse::lmb->off))
            , press(clicker(hold, true))
            , release(clicker(hold, false))
        {}

        const SignalType enter;
        const SignalType leave;
        const HoverType hover;
        const HoldType hold;
        const PressType press;
        const ReleaseType release;
    };


    template<typename Layout, typename... Elems>
        struct WidgetElements : public WidgetSignals<Elems...>, public Layout
        {
            static_assert(sizeof...(Elems) == Layout::ELEMENT_COUNT);
            using Elements = std::tuple<Elems...>;
            struct Preset
            {
                Preset(const size_t w, const size_t h)
                    : subs(utils::convert_tuple<typename Elems::Preset...>(Layout::genQuads(glm::vec4(0.0f, 0.0f, toScreenX(w), toScreenY(h)))))
                {}
                Preset(const glm::vec4 q)
                    : subs(utils::convert_tuple<typename Elems::Preset...>(Layout::genQuads(q)))
                {}
                const std::tuple<typename Elems::Preset...> subs;
            };

            struct Data
            {
                Data(const Preset pre)
                    : elems(utils::convert_tuple<typename Elems::Data...>(pre.subs))
                {}
                const std::tuple<typename Elems::Data...> elems;
            };

            WidgetElements(const Data data)
                : WidgetSignals<Elems...>(data.elems)
                , elements(data.elems)
            {}

            const Elements elements;

            using Layout::ELEMENT_COUNT;
            using Layout::movepolicy;
            using Layout::resizepolicy;
            void move_n(utils::_index<0> i, const glm::vec2 v) const
            {}

            template<size_t N>
                void move_n(utils::_index<N> i, const glm::vec2 v) const
                {
                    move_n(utils::_index<N-1>(), v);
                    std::get<N-1>(elements)->move(v * movepolicy[N-1]);
                }
            void resize_n(utils::_index<0> i, const glm::vec2 v) const
            {}
            template<size_t N>
                void resize_n(utils::_index<N> i, const glm::vec2 v) const
                {
                    resize_n(utils::_index<N-1>(), v);
                    const glm::vec4& pol = resizepolicy[N-1];
                    std::get<N-1>(elements)->move(v * glm::vec2(pol.x, pol.y));
                    std::get<N-1>(elements)->resize(v * glm::vec2(pol.z, pol.w));
                }
            void move(const glm::vec2 v) const
            {
                move_n(utils::_index<ELEMENT_COUNT>(), v);
            }
            void resize(const glm::vec2 v) const
            {
                resize_n(utils::_index<ELEMENT_COUNT>(), v);
            }
            const WidgetElements<Layout, Elems...>* operator->() const
            {
                return this;
            }
        };
    template<typename Element>
        void moveWidget(const utils::ID<Element>& pWidget, const glm::vec2& pV)
        {
            pWidget->move(pV);
        }

    template<typename Element>
        void resizeWidget(const utils::ID<Element>& pWidget, const glm::vec2& pV)
        {
            pWidget->resize(pV);
        }

    template<typename... Cols>
        struct WidgetColors
        {
            using Colors = std::tuple<Cols...>;
            constexpr WidgetColors(Cols... cs)
                : colors(cs...)
            {}
            constexpr WidgetColors(const Colors cs)
                : colors(cs)
            {}
            const Colors colors;
        };

        void applyColor(const gl::ColorID col, const QuadElement elem);

    template<typename... Colors, typename Layout, typename... Elems>
        void applyColor_n(const WidgetColors<Colors...> cols, const WidgetElements<Layout, Elems...> elem, utils::_index<0>)
        {}
    template<typename... Colors, typename Layout, typename... Elems, size_t N>
        void applyColor_n(const WidgetColors<Colors...> cols, const WidgetElements<Layout, Elems...> elem, utils::_index<N>)
        {
            applyColor_n(cols, elem, utils::_index<N-1>());
            applyColor(std::get<N-1>(cols.colors), std::get<N-1>(elem.elements));
        }
    template<typename... Colors, typename Layout, typename... Elems>
        void applyColor(const WidgetColors<Colors...> cols, const WidgetElements<Layout, Elems...> elem)
        {
            applyColor_n(cols, elem, utils::_index<sizeof...(Colors)>());
        }
    template<typename Color, typename Elem>
        void applyColor(const Color cols, const Elem elem)
        {
            applyColor(cols, elem);
        }


    template<typename Elements, typename Colors>
        struct Widget : public Colors
        {
            public:
                using ElementPreset = typename Elements::Preset;
                struct Preset
                {
                    Preset(const ElementPreset e, const Colors c)
                        : elem(e)
                        , col(c)
                    {}

                    const ElementPreset elem;
                    const Colors col;
                };
                using Data = typename Elements::Data;
                using Colors::colors;
                const utils::ID<Elements> e;

                const utils::ID<Elements> operator->() const
                {
                    return e;
                }

                Widget(Preset preset)
                    : Colors(preset.col)
                    , e(utils::makeID<Elements>(Data(preset.elem)))
                {
                    applyColor((Colors)*this, *e);
                    using namespace signals;

                    link(e->enter, func(applyColor<gl::ColorID, QuadElement>, gl::getColor("white"), std::get<0>(e->elements)));
                    link(e->leave, func(applyColor<gl::ColorID, QuadElement>, std::get<0>(colors), std::get<0>(e->elements)));

                    link(e->press, func(applyColor<gl::ColorID, QuadElement>, gl::getColor("white"), std::get<1>(e->elements)));
                    link(e->release, func(applyColor<gl::ColorID, QuadElement>, std::get<1>(colors), std::get<1>(e->elements)));

                    link(e->hold, refFunc(moveWidget<Elements>, (utils::ID<Elements>)e, (glm::vec2&)input::cursorFrameDelta));
                }
        };

}
