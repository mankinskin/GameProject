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
#include "element.h"
#include "utils/array_utils.h"
#include "utils/tuple_utils.h"


//-----Widgets
// This class is rather complex. It also does a little too many things in one place.
// However it provides a very efficient and stable, yet extremely flexible foundation for a class hierarchy
//
// To create a Widget composed of 2D quads you have to provide some data:
// - the initial data to be loaded into each element(quad) of the widget
// - the policies about how the element should pass an input command to its subelements

namespace gui 
{
    template<size_t COUNT>
        struct Widget
        {
            public:
                using Colors = std::array<gl::ColorID, COUNT>;
                using MovePolicy = std::array<glm::vec2, COUNT>;
                using ResizePolicy = std::array<glm::vec4, COUNT>;
                using QuadPreset = std::array<Quad, COUNT>;
                using Quads = std::array<QuadID, COUNT>;

                struct Preset
                {
                    Preset( QuadPreset qs, Colors cs, 
                            MovePolicy mp, ResizePolicy rp )
                        :quads( qs ), 
                        colors( cs ),
                        movepolicy( mp ), 
                        resizepolicy( rp )
                    {
                    }
                    const QuadPreset quads;
                    const Colors colors;
                    const MovePolicy movepolicy;
                    const ResizePolicy resizepolicy;
                }; 
                const Quads quads;
                const Colors colors;
                const MovePolicy movepolicy;
                const ResizePolicy resizepolicy;

                Widget( Preset preset )
                    :quads( utils::convert_array<QuadID>( preset.quads ) ), 
                    colors( preset.colors ),
                    movepolicy( preset.movepolicy ), 
                    resizepolicy( preset.resizepolicy )
            {
                color( colors );
            }

                    Widget( QuadPreset qs, Colors cs, 
                            MovePolicy mp, ResizePolicy rp )
                        :quads( utils::convert_array<QuadID>( qs ) ), 
                        colors( cs ),
                        movepolicy( mp ), 
                        resizepolicy( rp )
            {
                color( colors );
            }

                void move_n( utils::_index<0> i, const glm::vec2 v ) const
                {
                }
                template<size_t N>
                    void move_n( utils::_index<N> i, const glm::vec2 v ) const
                    {
                        move_n( utils::_index<N-1>(), v );
                        quads[N - 1].move( v * movepolicy[N - 1] );
                    }
                void resize_n( utils::_index<0> i, const glm::vec2 v ) const
                {
                }
                template<size_t N>
                    void resize_n( utils::_index<N> i, const glm::vec2 v ) const
                    {
                        resize_n( utils::_index<N-1>(), v );
                        const glm::vec4& pol = resizepolicy[N - 1];
                        quads[N-1].move( v * glm::vec2( pol.x, pol.y ) );
                        quads[N-1].resize( v * glm::vec2( pol.z, pol.w ) );
                    }
                void color_n( utils::_index<0> i, Colors pColors ) const
                {
                }
                template<size_t N>
                    void color_n( utils::_index<N> i, Colors pColors ) const
                    {
                        color_n( utils::_index<N-1>(), pColors );
                        quads[N - 1].color( colors[N - 1] );
                    }
                void move( const glm::vec2 v ) const
                {
                    move_n( utils::_index<COUNT>(), v );   
                }
                void resize( const glm::vec2 v ) const
                {
                    resize_n( utils::_index<COUNT>(), v );   
                }
                void color( Colors pColors ) const
                {
                    color_n( utils::_index<COUNT>(), pColors );   
                }
        };

    template<size_t COUNT>
        void moveWidget( Widget<COUNT> pWidget, glm::vec2& pV )
        {
            pWidget.move( pV );
        }
    template<size_t COUNT>
        void resizeWidget( Widget<COUNT> pWidget, glm::vec2& pV )
        {
            pWidget.resize( pV );
        }
    template<size_t COUNT>
        void colorWidget( Widget<COUNT> pWidget, 
                typename Widget<COUNT>::Colors& pColors )
        {
            pWidget.color( pColors );
        }
}
