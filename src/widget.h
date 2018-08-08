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
        using SignalType = signals::SignalListener<signals::And, signals::SignalListener<signals::Or, typename Ts::SignalType...>,
              signals::SignalListener<signals::Nor, typename Ts::SignalType...>>;
        using HoldSignalType = signals::SignalListener<signals::Flip, SignalType, SignalType>;
        template<size_t... Ns>
            constexpr SignalType gen_event(const bool state, const std::tuple<Ts...> elems, const std::index_sequence<Ns...>) const
            {
                return signals::ifAll(
                        signals::ifAny(std::get<Ns>(elems).event(state)...),
                        signals::ifNone(std::get<Ns>(elems).event(!state)...));
            }
        constexpr WidgetSignals()
        {}
        constexpr WidgetSignals(const std::tuple<Ts...> os)
            : onSignal(gen_event(true, os, std::make_index_sequence<sizeof...(Ts)>()))
            , offSignal(gen_event(false, os, std::make_index_sequence<sizeof...(Ts)>()))
        {}
        constexpr const SignalType event(const bool state) const
        {
            return state ? onSignal : offSignal;
        }
        constexpr const SignalType on() const
        {
            return onSignal;
        }
        constexpr const SignalType off() const
        {
            return offSignal;
        }
        constexpr const HoldSignalType hold() const
        {
            return signals::flip(onSignal, offSignal);
        }

        constexpr const SignalType enter() const { return onSignal; }
        constexpr const SignalType leave() const { return offSignal; }
        constexpr const SignalType down() const { return onSignal; }
        constexpr const SignalType up() const { return offSignal; }
        constexpr const SignalType press() const { return onSignal; }
        constexpr const SignalType release() const { return offSignal; }
        constexpr const HoldSignalType hover() const { return hold(); }

        protected:
        SignalType onSignal;
        SignalType offSignal;
    };

    template<typename... Elems>
        struct WidgetElements : public WidgetSignals<Elems...>
        {
            constexpr static const size_t COUNT = sizeof...(Elems);
            using SubPresets = std::tuple<typename Elems::Preset...>;

            using MovePolicy = std::array<glm::vec2, COUNT>;
            using ResizePolicy = std::array<glm::vec4, COUNT>;

            struct Preset
            {
                Preset(const SubPresets qs, const MovePolicy mp, const ResizePolicy rp)
                    : subpresets(qs)
                      , movepolicy(mp)
                      , resizepolicy(rp)
                {}
                const SubPresets subpresets;
                const MovePolicy movepolicy;
                const ResizePolicy resizepolicy;
            };

            struct Data
            {
                Data(const Preset preset)
                    : elems(utils::convert_tuple<Elems...>(preset.subpresets))
                      , movepolicy(preset.movepolicy)
                      , resizepolicy(preset.resizepolicy)
                {}
                Data(const std::tuple<Elems...> qs, const MovePolicy mp, const ResizePolicy rp)
                    : elems(qs)
                      , movepolicy(mp)
                      , resizepolicy(rp)
                {}
                const std::tuple<Elems...> elems;
                const MovePolicy movepolicy;
                const ResizePolicy resizepolicy;
            };


            WidgetElements(const Data data)
                : WidgetSignals<Elems...>(data.elems)
                , elements(data.elems)
                , movepolicy(data.movepolicy)
                , resizepolicy(data.resizepolicy)
            {}
            const std::tuple<Elems...> elements;
            const MovePolicy movepolicy;
            const ResizePolicy resizepolicy;

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
                move_n(utils::_index<COUNT>(), v);
            }
            void resize(const glm::vec2 v) const
            {
                resize_n(utils::_index<COUNT>(), v);
            }
            const WidgetElements<Elems...>* operator->() const
            {
                return this;
            }
        };

    template<typename... Colors>
        struct WidgetColors
        {
            constexpr WidgetColors(Colors... cs)
                : colors(cs...)
            {}
            constexpr WidgetColors(const std::tuple<Colors...> cs)
                : colors(cs)
            {}
            const std::tuple<Colors...> colors;
        };

        void applyColor(const gl::ColorID col, const QuadElement elem);

    template<typename... Colors, typename... Elems>
        void applyColor_n(const WidgetColors<Colors...> cols, const WidgetElements<Elems...> elem, utils::_index<0>)
        {}
    template<typename... Colors, typename... Elems, size_t N>
        void applyColor_n(const WidgetColors<Colors...> cols, const WidgetElements<Elems...> elem, utils::_index<N>)
        {
            applyColor_n(cols, elem, utils::_index<N-1>());
            applyColor(std::get<N-1>(cols.colors), std::get<N-1>(elem.elements));
        }
    template<typename... Colors, typename... Elems>
        void applyColor(const WidgetColors<Colors...> cols, const WidgetElements<Elems...> elem)
        {
            applyColor_n(cols, elem, utils::_index<sizeof...(Colors)>());
        }

    template<typename Elem, typename Color>
        struct Widget : public Elem, Color
        {
            public:
                using ElementPreset = typename Elem::Preset;
                struct Preset
                {
                    Preset(const ElementPreset e, const Color c)
                        : elem(e)
                        , col(c)
                    {}

                    const ElementPreset elem;
                    const Color col;
                };
                using Data = typename Elem::Data;
                using Color::colors;

                Widget(Preset preset)
                    : Elem(Data(preset.elem))
                    , Color(preset.col)
                {
                    applyColor((Color)*this, (Elem)*this);
                }
        };

    template<typename... Elems>
        void moveWidget(Widget<Elems...> pWidget, glm::vec2& pV)
        {
            pWidget.move(pV);
        }

    template<typename... Elems>
        void resizeWidget(Widget<Elems...> pWidget, glm::vec2& pV)
        {
            pWidget.resize(pV);
        }
}
