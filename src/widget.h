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

    template<typename... Elems>
        struct Widget : public signals::ButtonSignals<Elems...>
        {
            public:
                constexpr static const size_t COUNT = sizeof...(Elems);
                using SubPresets = std::tuple<typename Elems::Preset...>;

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
                const MovePolicy movepolicy;
                const ResizePolicy resizepolicy;

                Widget(Preset preset)
                    : signals::ButtonSignals<Elems...>(utils::convert_tuple<Elems...>(preset.subpresets))
                    , movepolicy(preset.movepolicy)
                    , resizepolicy(preset.resizepolicy)
                {}


                using signals::ButtonSignals<Elems...>::elements;

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
        };


    template<typename Elem>
    struct Widget<Elem> : public signals::ButtonSignals<Elem>
    {
        public:
            using Preset = typename Elem::Preset;

            Widget(Preset preset)
                : signals::ButtonSignals<QuadID>(utils::makeID(preset))
            {}
            using signals::ButtonSignals<QuadID>::elem;
            void move(const glm::vec2 v) const
            {
                elem->move(v);
            }

            void resize(const glm::vec2 v) const
            {
                elem->resize(v);
            }
    };

    template<typename Color>
    struct QuadWidget : public Widget<QuadID>
    {
        public:
            struct Preset
            {
                Preset(typename Widget<QuadID>::Preset q, Color c)
                    : quad(q)
                    , color(c)
                {}
                typename Widget<QuadID>::Preset quad;
                Color color;
            };

            Color color;

            QuadWidget(Preset preset)
                : Widget<QuadID>(preset.quad)
                , color(preset.color)
            {
                colorQuad(elem, color);
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
