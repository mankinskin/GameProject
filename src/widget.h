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
            constexpr SignalType gen_event(const std::tuple<Ts...>& elems, const std::index_sequence<Ns...>) const
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
        constexpr WidgetSignals(const std::tuple<Ts...>& os)
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


    template<typename W>
        void moveWidget(const W& pW, const glm::vec2& pV)
        {
            pW.move(pV);
        }

    template<typename W>
        void resizeWidget(const W& pW, const glm::vec2& pV)
        {
            pW.resize(pV);
        }

    template<typename... Cols>
        struct WidgetColors
        {
            using Colors = std::tuple<Cols...>;
            constexpr WidgetColors(const Cols... cs)
                : colors(cs...)
            {}
            constexpr WidgetColors(const Colors cs)
                : colors(cs)
            {}
            const Colors colors;
        };

    template<typename... Colors, typename... Elems>
        void applyColor_imp_n(const WidgetColors<Colors...> cols, const std::tuple<Elems...> elems, utils::_index<0>)
        {}
    template<typename... Colors, typename... Elems>
        void applyColor_imp(const WidgetColors<Colors...> cols, const std::tuple<Elems...> elems)
        {
            applyColor_imp_n(cols, elems, utils::_index<sizeof...(Colors)>());
        }
    template<typename Color, typename Wid>
        void applyColor(const Color col, const Wid wid)
        {
            applyColor_imp(col, wid);
        }
    template<typename... Colors, typename... Elems, size_t N>
        void applyColor_imp_n(const WidgetColors<Colors...> cols, const std::tuple<Elems...> elems, utils::_index<N>)
        {
            applyColor_imp_n(cols, elems, utils::_index<N-1>());
            applyColor(std::get<N-1>(cols.colors), std::get<N-1>(elems));
        }
    template<typename Color>
    struct QuadElement : public utils::ID<Quad>, signals::ButtonSignals<utils::ID<Quad>>
    {
        using Colors = Color;
        struct Preset
        {
            Preset(const Quad q, const Color col)
                : quad(q)
                , color(col)
            {}
            const Quad quad;
            const Color color;
        };
        struct Data
        {
            Data(const Preset pre)
                : quad(utils::makeID(pre.quad))
                , color(pre.color)
            {
                colorQuad(quad, color);
            }
            const utils::ID<Quad> quad;
            const Color color;
        };
        QuadElement(const Data data)
            : utils::ID<Quad>(data.quad)
            , signals::ButtonSignals<utils::ID<Quad>>(data.quad)
            , color(data.color)
        {}
        const Color color;
    };
    template<typename Color>
        void applyColor_imp(const gl::ColorID col, const QuadElement<Color> elem)
        {
            colorQuad(elem, col);
        }



    template<typename Layout, typename... Elems>
        struct Widget : public WidgetSignals<Elems...>, Layout
        {
            using Elements = std::tuple<Elems...>;
            using Layout::ELEMENT_COUNT;
            using Layout::movepolicy;
            using Layout::resizepolicy;
            using Colors = WidgetColors<typename Elems::Colors...>;
            static_assert(sizeof...(Elems) == Layout::ELEMENT_COUNT);

            struct Preset
            {
                Preset(const glm::vec4 q, const Colors cols)
                    : subs(utils::convert_tuple<typename Elems::Preset...>(Layout::genQuads(q), cols.colors))
                {}
                const std::tuple<typename Elems::Preset...> subs;
            };

            struct Data
            {
                Data(const Preset pre)
                    : elems(utils::convert_tuple<Elems...>(utils::convert_tuple<typename Elems::Data...>(pre.subs)))
                {}
                const Elements elems;
            };
            Widget(const Data data)
                : WidgetSignals<Elems...>(data.elems)
                , elements(data.elems)
            {
                using namespace signals;

                link(WidgetSignals<Elems...>::hold, refFunc(moveWidget<Widget<Layout, Elems...>>, (Widget<Layout, Elems...>)*this, (glm::vec2&)input::cursorFrameDelta));
            }

            const Elements elements;

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
                    std::get<N-1>(elements)->move(v * glm::vec2(resizepolicy[N-1].x, resizepolicy[N-1].y));
                    std::get<N-1>(elements)->resize(v * glm::vec2(resizepolicy[N-1].z, resizepolicy[N-1].w));
                }
            void move(const glm::vec2 v) const
            {
                move_n(utils::_index<ELEMENT_COUNT>(), v);
            }
            void resize(const glm::vec2 v) const
            {
                resize_n(utils::_index<ELEMENT_COUNT>(), v);
            }
            const Widget<Layout, Elems...>* operator->() const
            {
                return this;
            }
        };

    template<typename... Colors, typename Layout, typename... Elems>
        void applyColor_imp(const WidgetColors<Colors...> cols, const Widget<Layout, Elems...> wid)
        {
            applyColor_imp_n(cols, wid.elements, utils::_index<sizeof...(Colors)>());
        }

}

