#pragma once
#include <array>
#include <tuple>
#include <utility>
#include <vector>
#include <glm.hpp>
#include "color.h"
#include "quad.h"
#include "quadcolors.h"
#include "utils/itr.h"
#include "element.h"
#include "element_utils.h"


//-----Widgets
// This class is rather complex. It also does a little too many things in one place.
// However it provides a very efficient and stable, yet extremely flexible foundation for a class hierarchy
//
// To create a Widget composed of 2D quads you have to provide some data:
// - the initial data to be loaded into each element(quad) of the widget
// - the policies about how the element should pass an input command to its subelements

namespace gui 
{
    template<typename... Subs>
        struct Widget
        {
            public:
                static const size_t COUNT = sizeof...(Subs);
                using SubWidgets = utils::Element<Subs...>;
                using Colors = utils::Element<typename Subs::Colors...>;
                using MovePolicy = utils::gen_Element_t<glm::vec2, COUNT>;
                using ResizePolicy = utils::gen_Element_t<glm::vec4, COUNT>;
                using Initer = utils::Element<typename Subs::Initer...>;

                struct Preset
                {
                    Preset( Initer pIniter, Colors pColors, 
                            MovePolicy pMovePolicy, ResizePolicy pResizePolicy )
                        :initer( pIniter ), 
                        colors( pColors ),
                        movepolicy( pMovePolicy ), 
                        resizepolicy( pResizePolicy )
                    {
                    }
                    const Initer initer;
                    const Colors colors;
                    const MovePolicy movepolicy;
                    const ResizePolicy resizepolicy;
                }; 
                const SubWidgets subwidgets;
                const Colors colors;
                const MovePolicy movepolicy;
                const ResizePolicy resizepolicy;

                Widget( Preset pPreset )
                    :subwidgets( pPreset.initer ), 
                    colors( pPreset.colors ),
                    movepolicy( pPreset.movepolicy ), 
                    resizepolicy( pPreset.resizepolicy )
            {
                color( colors );
            }

                Widget( Initer pIniter, Colors pColors, 
                        MovePolicy pMovePolicy, ResizePolicy pResizePolicy )
                    :subwidgets( pIniter ), 
                    colors( pColors ),
                    movepolicy( pMovePolicy ), 
                    resizepolicy( pResizePolicy )
            {
                color( colors );
            }

                void move_n( utils::_index<0> i, const glm::vec2 pV ) const
                {
                }
                template<size_t N>
                    void move_n( utils::_index<N> i, const glm::vec2 pV ) const
                    {
                        move_n( utils::_index<N-1>(), pV );
                        std::get<N-1>( subwidgets.subelements ).move( pV * std::get<N - 1>( movepolicy.subelements ) );
                    }
                void resize_n( utils::_index<0> i, const glm::vec2 pV ) const
                {
                }
                template<size_t N>
                    void resize_n( utils::_index<N> i, const glm::vec2 pV ) const
                    {
                        resize_n( utils::_index<N-1>(), pV );
                        std::get<N-1>( subwidgets.subelements ).resize( pV * std::get<N - 1>( resizepolicy.subelements ) );
                    }
                void color_n( utils::_index<0> i, Colors pColors ) const
                {
                }
                template<size_t N>
                    void color_n( utils::_index<N> i, Colors pColors ) const
                    {
                        color_n( utils::_index<N-1>(), pColors );
                        std::get<N - 1>(subwidgets.subelements).color( std::get<N - 1>(pColors.subelements) );
                    }
                void move( const glm::vec2 pV ) const
                {
                    move_n( utils::_index<COUNT>(), pV );   
                }
                void resize( const glm::vec2 pV ) const
                {
                    resize_n( utils::_index<COUNT>(), pV );   
                }
                void color( Colors pColors ) const
                {
                    color_n( utils::_index<COUNT>(), pColors );   
                }
        };

    template<typename... Subs>
        void moveWidget( Widget<Subs...> pWidget, glm::vec2& pV )
        {
            pWidget.move( pV );
        }
    template<typename... Subs>
        void resizeWidget( Widget<Subs...> pWidget, glm::vec2& pV )
        {
            pWidget.resize( pV );
        }
    template<typename... Subs>
        void colorWidget( Widget<Subs...> pWidget, 
                typename Widget<Subs...>::Colors& pColors )
        {
            pWidget.color( pColors );
        }
}
