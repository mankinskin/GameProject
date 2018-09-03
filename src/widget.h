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
#include "quadsignals.h"
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
        using SignalType = signals::SignalListener<signals::And, signals::SignalListener<signals::Or, typename Ts::Signals::SignalType...>, signals::SignalListener<signals::Nor, typename Ts::Signals::SignalType...>>;
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
        void setWidgetPos(const W& pW, const glm::vec2& pV)
        {
            pW.setPos(pV);
        }
    template<typename W>
        void resizeWidget(const W& pW, const glm::vec2& pV)
        {
            pW.resize(pV);
        }
    template<typename W>
        void setWidgetSize(const W& pW, const glm::vec2& pV)
        {
            pW.setSize(pV);
        }
    template<typename W>
        void resizeWidgetX(const W& pW, const float& pV)
        {
            pW.resize(glm::vec2(pV, 0.0f));
        }
    template<typename W>
        void resizeWidgetY(const W& pW, const float& pV)
        {
            pW.resize(glm::vec2(0.0f, pV));
        }

    template<typename... Cols>
        struct WidgetColors : public std::tuple<Cols...>
        {
            using Colors = std::tuple<Cols...>;
            constexpr WidgetColors(const Cols... cs)
                : Colors(cs...)
            {}
            constexpr WidgetColors(const Colors cs)
                : Colors(cs)
            {}
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

    template<typename... Elems>
        struct WidgetLayout
        {
            using Layouts = std::tuple<typename Elems::Layout...>;

            WidgetLayout()
            {}
            WidgetLayout(Layouts layouts)
                :sublayouts(layouts)
            {}
            WidgetLayout(typename Elems::Layout... layouts)
                :sublayouts(layouts...)
            {}
            Layouts sublayouts;
        };
    struct NullLayout
    {
    };

    template<typename Color>
    struct QuadElement : public utils::ID<Quad>, signals::QuadSignals<utils::ID<Quad>>
    {
        using Layout = NullLayout;
        using Colors = Color;
        using Signals = signals::QuadSignals<utils::ID<Quad>>;
        struct Preset
        {
            Preset(Layout layout, const Color col)
                : color(col)
            {}
            const Color color;
        };
        QuadElement(const Quad q, const Preset pre)
            : utils::ID<Quad>(utils::makeID(q))
            , Signals((utils::ID<Quad>)*this)
            , color(pre.color)
        {
            colorQuad((utils::ID<Quad>)*this, color);
        }
        const Color color;
    };
    template<typename Color>
        void applyColor_imp(const gl::ColorID col, const QuadElement<Color> elem)
        {
            colorQuad(elem, col);
        }

    template<typename Layout, typename... Elems>
        struct WidgetElements : public Layout
        {
            static_assert(sizeof...(Elems) == Layout::ELEMENT_COUNT);
            using Layout::ELEMENT_COUNT;
            using Layout::movepolicy;
            using Layout::resizepolicy;

            using Elements = std::tuple<Elems...>;
            const Elements elements;
            utils::ID<glm::vec4> box;

            WidgetElements(const glm::vec4 pB, const std::tuple<Elems...> elems)
                : elements(elems)
                , box(utils::makeID(pB))
            {}
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
                box->x += v.x;
                box->y += v.y;
                move_n(utils::_index<ELEMENT_COUNT>(), v);
            }
            void resize(const glm::vec2 v) const
            {
                box->z += v.x;
                box->w += v.y;
                resize_n(utils::_index<ELEMENT_COUNT>(), v);
            }
            void setPos(const glm::vec2 p)
            {
                move(p - glm::vec2(box->x, box->y));
            }
            void setSize(const glm::vec2 s)
            {
                resize(s - glm::vec2(box->z, box->w));
            }
        };
    template<template<typename...>typename L, typename... Elems>
        struct Widget : public WidgetElements<L<Elems...>, Elems...>, WidgetSignals<Elems...>
        {
            using Layout = L<Elems...>;
            using Colors = WidgetColors<typename Elems::Colors...>;
            using Signals = WidgetSignals<Elems...>;
            using Elements = WidgetElements<Layout, Elems...>;

            using WidgetElements<Layout, Elems...>::elements;
            using WidgetSignals<Elems...>::hold;

            struct Preset
            {
                Preset(const Layout l, const Colors cols)
                    : layout(l)
                    , subs(utils::convert_tuple<typename Elems::Preset...>(l.sublayouts, cols))
                {}
                const Layout layout;
                const std::tuple<typename Elems::Preset...> subs;
            };

            Widget(const glm::vec4 q, const Preset pre)
                : WidgetElements<Layout, Elems...>(q, utils::convert_tuple<Elems...>(pre.layout.genQuads(q), pre.subs))
                , WidgetSignals<Elems...>(elements)
            {
                Layout::setup(*this);
            }

            const Widget<L, Elems...>* operator->() const
            {
                return this;
            }
        };

    template<typename... Colors, template<typename...>typename L, typename... Elems>
        void applyColor_imp(const WidgetColors<Colors...> cols, const Widget<L, Elems...> wid)
        {
            applyColor_imp_n(cols, wid.elements, utils::_index<sizeof...(Colors)>());
        }

}

