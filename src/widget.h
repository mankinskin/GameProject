#pragma once
#include "quad.h"
#include <tuple>
#include <utility>
#include "quadcolors.h"
#include <glm.hpp>
#include <vector>
#include <array>
#include "color.h"
#include "utils.h"
//-----Widgets
// This class is rather complex. It also does a little too many things in one place.
// However it provides a very efficient and stable, yet extremely flexible foundation for a class hierarchy
//
// To create a Widget composed of 2D quads you have to provide some data:
// - the initial data to be loaded into each element(quad) of the widget
// - the policies about how the element should pass an input command to its subelements

using gl::Color;

namespace gui {
    template<unsigned int> struct _index {};
    extern std::vector<glm::vec2> allWidgetPositions;
    extern std::vector<glm::vec2> allWidgetSizes;

    template<typename Wid>
        struct WidgetMovePolicy 
        {
            WidgetMovePolicy( std::array<glm::vec2, Wid::ELEMENT_COUNT> pPolicyMatrix )
                :matrix( pPolicyMatrix )
            {}
            std::array<glm::vec2, Wid::ELEMENT_COUNT> matrix;
        };

    template<typename Wid>
        struct WidgetResizePolicy 
        {
            WidgetResizePolicy( std::array<glm::vec4, Wid::ELEMENT_COUNT> pPolicyMatrix )
                :matrix( pPolicyMatrix ) 
            {}
            std::array<glm::vec4, Wid::ELEMENT_COUNT> matrix;
        };


    //stores a tuple of things that can be used in a matching Widget
    //to color each element respectively using the element's color member function
    template<class... Colors>
        struct WidgetColors 
        {
            WidgetColors( Colors... pColors )
                :colors( pColors... ) {}
            std::tuple<Colors...> colors;
        };

    template<typename... Elems>
        struct construct 
        {
            template<size_t... Ns, typename Head, typename... Rest>
                static constexpr const std::tuple<Rest...> drop_head_impl( std::index_sequence<Ns...> ns, 
                        std::tuple<Head, Rest...> tup )
                {
                    return std::tuple<Rest...>( std::get<Ns + 1u>( tup )... );
                }

            template<typename Head, typename... Rest>
                static constexpr const std::tuple<Rest...> drop_head( std::tuple<Head, Rest...> tup )
                {
                    return drop_head_impl( std::make_index_sequence<sizeof...(Rest)>(), tup );
                }

            template<typename Head>
            static constexpr const 
            std::tuple<Head> func_impl( std::tuple<typename Head::initer_t> initer )
            {
                return  std::tuple<Head>( { std::get<0>( initer ) } ); 
            }

            template<typename Head, typename Next, typename... Rest>
            static constexpr const 
            std::tuple<Head, Next, Rest...> 
            func_impl( std::tuple<typename Head::initer_t, typename Next::initer_t, typename Rest::initer_t...> initer )
            {
                std::tuple<Head> head( { std::get<0>( initer ) } ); 
                return std::tuple_cat( head, func_impl<Next, Rest...>( drop_head(initer) ) );
            }


            static constexpr const std::tuple<Elems...> func( std::tuple<typename Elems::initer_t...> initer )
            {
                return func_impl<Elems...>( initer );
            }
        };


    template<class ...Elems>
        class Widget 
        {
            public:
                static const unsigned int ELEMENT_COUNT = sizeof...( Elems );
                using initer_t = typename std::tuple<typename Elems::initer_t...>;

                Widget( const initer_t pIniter )
                    : elements( construct<Elems...>::func( pIniter ) )
                {
                    pos_index = allWidgetPositions.size();
                    size_index = allWidgetSizes.size();
                    allWidgetPositions.push_back( glm::vec2() );
                    allWidgetSizes.push_back( glm::vec2() );
                }
                template<unsigned int N>
                    auto element() const
                    {
                        return std::get<N>( elements );
                    }
                glm::vec2& get_pos() const
                {
                    return allWidgetPositions[pos_index];
                }
                glm::vec2& get_size() const
                {
                    return allWidgetSizes[size_index];
                }
                void set_pos( glm::vec2 pNewPos ) const
                {
                    set_all_pos<ELEMENT_COUNT>( _index<ELEMENT_COUNT>(), pNewPos );
                    allWidgetPositions[pos_index] = pNewPos;
                }
                template<unsigned int N>
                    void set_element_pos( glm::vec2 pNewPos ) const
                    {
                        std::get<N>( elements ).set_pos( pNewPos );
                    }

                void move( glm::vec2 pOffset ) const
                {
                    move_all<ELEMENT_COUNT>( pOffset );
                    allWidgetPositions[pos_index] += pOffset;
                }
                void move( float pOffsetX, float pOffsetY ) const
                {
                    move( glm::vec2( pOffsetX, pOffsetY ) );
                }
                template<unsigned int N>
                    void move_element( glm::vec2& pOffset ) const
                    {
                        std::get<N>( elements ).move( pOffset );//* move_policy.matrix[N] );
                    }

                void move_to( glm::vec2 pNewPos ) const
                {
                    move_all<ELEMENT_COUNT>( pNewPos - allWidgetPositions[pos_index] );
                    allWidgetPositions[pos_index] = pNewPos;
                }
                template<unsigned int N>
                    void move_element_to( glm::vec2& pNewPos ) const
                    {
                        std::get<N>( elements ).move( 
                                ( pNewPos - allWidgetPositions[pos_index] ) );//* move_policy.matrix[N] );
                    }

                void resize( glm::vec2& pOffset ) const
                {
                    resize_all( _index<ELEMENT_COUNT>(), pOffset );
                }
                template<unsigned int N>
                    void resize_element( glm::vec2& pOffset ) const
                    {
                        std::get<N>( elements ).resize( 
                                pOffset );//* glm::vec2( resize_policy.matrix[N].z, resize_policy.matrix[N].w ) );
                    }

                template<typename WidColors, unsigned int I>
                    struct color_elements 
                    {
                        static void func( const std::tuple<Elems...>& pElems, WidColors& pColors )
                        {
                            color_elements<WidColors, I - 1>::func( pElems, pColors );
                            std::get<I - 1>( pElems ).color( std::get<I - 1>( pColors.colors ) );
                        }
                    };

                template<typename WidColors>
                    struct color_elements<WidColors, 0> 
                    {
                        static void func( const std::tuple<Elems...>& pElems, 
                                WidColors& pColors ) { }
                    };
                template<class... Colors>
                    void color( WidgetColors<Colors...> pColors ) const
                    {
                        static_assert( sizeof...( Colors ) == sizeof...( Elems ), 
                                "Color count does not match the element count of Widget" );
                        color_elements<WidgetColors<Colors...>, sizeof...( Elems )>::func( elements, pColors );
                    }

            private:
                const std::tuple<Elems...> elements;
                unsigned int pos_index;
                unsigned int size_index;
                //WidgetMovePolicy<Widget<Elems...>> move_policy;
                //WidgetResizePolicy<Widget<Elems...>> resize_policy;

                void move_all( glm::vec2& pOffset, _index<0> = _index<0>() ) const {}
                template<unsigned int N>
                    void move_all( glm::vec2& pOffset, _index<N> = _index<N>() ) const
                    {
                        move_all( pOffset, _index<N-1>() );
                        move_element<N - 1>( pOffset );//* ( glm::vec2 )move_policy.matrix[N] );
                    }

                void set_all_pos( _index<0>, glm::vec2& pNewPos ) const {}
                template<unsigned int N>
                    void set_all_pos( _index<N>, glm::vec2& pNewPos ) const
                    {                        
                        set_all_pos<N - 1>( _index<N-1>(), pNewPos );
                        set_element_pos<N - 1>( pNewPos );
                    }

                void resize_all( _index<0>, glm::vec2& pOffset ) const {}
                template<unsigned int N>
                    void resize_all( _index<N>, glm::vec2& pOffset ) const
                    {
                        resize_all<N - 1>( _index<N-1>(), pOffset );

                        move_element<N - 1>(  pOffset );//* glm::vec2( resize_policy.matrix[N - 1].x, resize_policy.matrix[N - 1].y ) );
                        resize_element<N - 1>( pOffset );//resize matrix is applied later to size
                    }
        };


    template<class Wid>
        void move_widget( Wid pWidget, glm::vec2& pOffset ) 
        {
            pWidget.move( pOffset );
        }
    template<class Wid, typename A, typename B>
        void move_widget( Wid pWidget, A pOffsetX, B pOffsetY ) 
        {
            pWidget.move( glm::vec2( pOffsetX, pOffsetY ) );
        }
    template<class Wid, unsigned int N>
        void move_widget_element( Wid pWidget, float pOffsetX, float pOffsetY ) 
        {
            pWidget.move.element<N>( glm::vec2( pOffsetX, pOffsetY ) );
        }
    template<class Wid>
        void resize_widget( Wid pWidget, glm::vec2& pOffset ) 
        {
            pWidget.resize( pOffset );
        }
    template<class Wid, typename A, typename B>
        void resize_widget( Wid pWidget, A pDirX, B pDirY ) 
        {
            pWidget.resize( glm::vec2( pDirX, pDirY ) );
        }

    //generate Widget with N Quads 
    template<unsigned int N, typename ...Quads>
        struct gen_quad_group //MAY BE SLOW TO COMPILE 
        {
            typedef typename gen_quad_group<N - 1, Quad, Quads...>::type type;
        };
    template<typename ...Quads>
        struct gen_quad_group<0, Quads...>//MAY BE SLOW TO COMPILE 
        {

            typedef Widget<Quads...> type;
        };

    template<unsigned int QuadCount>
        using QuadGroup = typename gen_quad_group<QuadCount>::type;


    //generate Colors for a Widget with N quads 
    template<unsigned int N, typename Color, typename ...Colors>
        struct gen_color_group 
        {
            typedef typename gen_color_group<N - 1, Color, Color, Colors...>::type type;
        };
    template<typename Color, typename ...Colors>
        struct gen_color_group<0, Color, Colors...> 
        {
            typedef WidgetColors<Colors...> type;
        };

    //ColorGroup is really of type WidgetColors
    template<unsigned int ColorCount, typename Color = Color>
        using ColorGroup = typename gen_color_group<ColorCount, Color>::type;
}

