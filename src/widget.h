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
    template<typename Color>
    struct QuadWidget
    {
        public:
            using SignalType = utils::ID<signals::EventListener<QuadEvent>>;

            struct Preset
            {
                Preset(Quad q, Color c)
                    : quad(q)
                    , color(c)
                {}
                Quad quad;
                Color color;
            };

            QuadID quad;
            Color color;

            QuadWidget(Preset preset)
                : quad(utils::makeID(preset.quad))
                , color(preset.color)
            {
                colorQuad(quad, color);
            }

            const SignalType event(bool enter) const
            {
                return signals::ifEvent(QuadEvent(quad, enter));
            }

            void move(const glm::vec2 v) const
            {
                quad->move(v);
            }

            void resize(const glm::vec2 v) const
            {
                quad->resize(v);
            }
    };

    template<typename... Elems>
        struct Widget
        {
            public:
                using SignalType = signals::SignalListener<signals::And,
                      signals::SignalListener<signals::Or, typename Elems::SignalType...>,
                      signals::SignalListener<signals::Nor, typename Elems::SignalType...>>;
                constexpr static const size_t COUNT = sizeof...(Elems);
                using SubPresets = std::tuple<typename Elems::Preset...>;

                using Elements = std::tuple<Elems...>;
                using MovePolicy = std::array<glm::vec2, COUNT>;
                using ResizePolicy = std::array<glm::vec4, COUNT>;

                struct Preset
                {
                    Preset(SubPresets qs, MovePolicy mp, ResizePolicy rp)
                        : subpresets(qs)
                        , movepolicy(mp)
                        , resizepolicy(rp)
                    {}
                    const SubPresets subpresets;
                    const MovePolicy movepolicy;
                    const ResizePolicy resizepolicy;
                };
                const Elements elements;
                const MovePolicy movepolicy;
                const ResizePolicy resizepolicy;

                Widget(Preset preset)
                    : elements(utils::convert_tuple<Elems...>(preset.subpresets))
                    , movepolicy(preset.movepolicy)
                    , resizepolicy(preset.resizepolicy)
                {}

                template<size_t... Ns>
                    const SignalType gen_event(bool enter, std::index_sequence<Ns...>) const
                    {
                        return ifAll(ifAny(std::get<Ns>(elements).event(enter)...), ifNone(std::get<Ns>(elements).event(!enter)...));
                    }
                constexpr const SignalType event(bool enter) const
                {
                    return gen_event(enter, std::make_index_sequence<sizeof...(Elems)>());
                }

                void move_n(utils::_index<0> i, const glm::vec2 v) const
                {}
                template<size_t N>
                    void move_n(utils::_index<N> i, const glm::vec2 v) const
                    {
                        move_n(utils::_index<N-1>(), v);
                        std::get<N-1>(elements).move(v * movepolicy[N-1]);
                    }
                void resize_n(utils::_index<0> i, const glm::vec2 v) const
                {}
                template<size_t N>
                    void resize_n(utils::_index<N> i, const glm::vec2 v) const
                    {
                        resize_n(utils::_index<N-1>(), v);
                        const glm::vec4& pol = resizepolicy[N-1];
                        std::get<N-1>(elements).move(v * glm::vec2(pol.x, pol.y));
                        std::get<N-1>(elements).resize(v * glm::vec2(pol.z, pol.w));
                    }
                void move(const glm::vec2 v) const
                {
                    move_n(utils::_index<COUNT>(), v);
                }
                void resize(const glm::vec2 v) const
                {
                    resize_n(utils::_index<COUNT>(), v);
                }
                template<size_t N>
                    constexpr auto getElement() const
                    {
                        return std::get<N>(elements);
                    }
        };

    //template<size_t COUNT>
    //    void moveWidget(Widget<COUNT> pWidget, glm::vec2& pV)
    //    {
    //        pWidget.move(pV);
    //    }
    //template<size_t COUNT>
    //    void resizeWidget(Widget<COUNT> pWidget, glm::vec2& pV)
    //    {
    //        pWidget.resize(pV);
    //    }
    //template<size_t COUNT>
    //    void colorWidget(Widget<COUNT> pWidget,
    //            typename Widget<COUNT>::Colors& pColors)
    //    {
    //        //pWidget.color(pColors);
    //    }
}
