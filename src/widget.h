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
//- Widgets are used to control collections of 2D Quads in an object based manner
using gl::Color;

namespace gui {


    template<typename Wid>
        struct WidgetMovePolicy {
            WidgetMovePolicy() {	}
            WidgetMovePolicy( std::array<glm::vec2, Wid::ELEMENT_COUNT> pPolicyMatrix )
                :matrix( pPolicyMatrix ){}
            std::array<glm::vec2, Wid::ELEMENT_COUNT> matrix;
        };

    template<typename Wid>
        struct WidgetResizePolicy {
            WidgetResizePolicy() {	}
            WidgetResizePolicy( std::array<glm::vec4, Wid::ELEMENT_COUNT> pPolicyMatrix )
                :matrix( pPolicyMatrix ) {}
            std::array<glm::vec4, Wid::ELEMENT_COUNT> matrix;
        };


    template<typename Wid>
        struct WidgetSetup {
            WidgetSetup( typename Wid::initer_t pIniter, WidgetMovePolicy<Wid> pMovePolicy, WidgetResizePolicy<Wid> pResizePolicy )
                :move_policy( pMovePolicy ), resize_policy( pResizePolicy ), initer( pIniter ) {}

            typename Wid::initer_t initer;
            WidgetMovePolicy<Wid> move_policy;
            WidgetResizePolicy<Wid> resize_policy;
            operator Wid() {
                return Wid( initer, move_policy, resize_policy );
            }
        };
    template<>
        struct WidgetSetup<Quad> {
            WidgetSetup( glm::vec4 pIniter )
                :initer( pIniter ) {}

            glm::vec4 initer;
            operator Quad() {
                return Quad( initer );
            }
        };

    //stores a tuple of things that can be used in a matching Widget
    //to color each element respectively using the element's color member function
    template<class... Colors>
        struct WidgetColors {
            WidgetColors( Colors... pColors )
                :colors( pColors... ) {}
            std::tuple<Colors...> colors;
        };


    extern std::vector<glm::vec2> allWidgetPositions;
    extern std::vector<glm::vec2> allWidgetSizes;

    template<class ...Elems>
        class Widget {
            public:
                template<unsigned int> struct _index{};
                static const unsigned int ELEMENT_COUNT = sizeof...( Elems );
                template<unsigned int N, typename ...Initers>
                    struct extract_initers {
                        typedef typename std::tuple_element_t<N - 1, std::tuple<Elems...>>::initer_t element_initializer_t;
                        typedef typename extract_initers<N - 1, element_initializer_t, Initers...>::result result;
                    };
                template<typename ...Rest>
                    struct extract_initers<0, Rest...> {
                        typedef typename std::tuple<WidgetSetup<Rest>...> result;
                    };
                typedef typename std::tuple<WidgetSetup<Elems>...> initer_t;


                template<unsigned int N>
                    static std::tuple<Elems...> construct_all( _index<N>, typename Widget<Elems...>::initer_t pIniter ) {
                        std::tuple<Elems...> re = construct_all( _index<N-1>(), pIniter );
                        std::get<N - 1>( re ) = std::get<N - 1>( pIniter );
                        return re;
                    }
                static std::tuple<Elems...> construct_all( _index<0>, typename Widget<Elems...>::initer_t pIniter ) {
                    return std::tuple<Elems...>();
                }
                Widget() {	}
                Widget( Elems... pElements )
                    :elements( { pElements... } )
                    {	}
                Widget( std::tuple<Elems...> pElements )
                    :elements( pElements )
                {	}
                Widget( initer_t pIniter )
                    :elements( construct_all<ELEMENT_COUNT>( _index<ELEMENT_COUNT>(), pIniter ) )
                {	}
                Widget( typename Widget<Elems...>::initer_t pIniter, WidgetMovePolicy<Widget<Elems...>> pMovePolicy, WidgetResizePolicy<Widget<Elems...>> pResizePolicy )
                    :move_policy( pMovePolicy ), resize_policy( pResizePolicy ), elements( construct_all<ELEMENT_COUNT>( _index<ELEMENT_COUNT>(), pIniter ) )
                {
                    pos_index = allWidgetPositions.size();
                    size_index = allWidgetSizes.size();
                    allWidgetPositions.push_back( glm::vec2() );
                    allWidgetSizes.push_back( glm::vec2() );
                }
                template<unsigned int N>
                    auto element() {
                        return std::get<N>( elements );
                    }
                glm::vec2& get_pos() {
                    return allWidgetPositions[pos_index];
                }
                glm::vec2& get_size() {
                    return allWidgetSizes[size_index];
                }
                void set_pos( glm::vec2 pNewPos ) {
                    set_all_pos<ELEMENT_COUNT>( _index<ELEMENT_COUNT>(), pNewPos );
                    allWidgetPositions[pos_index] = pNewPos;
                }
                template<unsigned int N>
                    void set_element_pos( glm::vec2 pNewPos ) {
                        std::get<N>( elements ).set_pos( pNewPos );
                    }

                void move( glm::vec2 pOffset ) {
                    move_all<ELEMENT_COUNT>( pOffset );
                    allWidgetPositions[pos_index] += pOffset;
                }
                void move( float pOffsetX, float pOffsetY ) {
                    move( glm::vec2( pOffsetX, pOffsetY ) );
                }
                template<unsigned int N>
                    void move_element( glm::vec2& pOffset ) {
                        std::get<N>( elements ).move( pOffset * move_policy.matrix[N] );
                    }

                void move_to( glm::vec2 pNewPos ) {
                    move_all<ELEMENT_COUNT>( pNewPos - allWidgetPositions[pos_index] );
                    allWidgetPositions[pos_index] = pNewPos;
                }
                template<unsigned int N>
                    void move_element_to( glm::vec2& pNewPos ) {
                        std::get<N>( elements ).move( ( pNewPos - allWidgetPositions[pos_index] ) * move_policy.matrix[N] );
                    }

                void resize( glm::vec2& pOffset ) {
                    resize_all( _index<ELEMENT_COUNT>(), pOffset );
                }
                template<unsigned int N>
                    void resize_element( glm::vec2& pOffset ) {
                        std::get<N>( elements ).resize( pOffset * glm::vec2( resize_policy.matrix[N].z, resize_policy.matrix[N].w ) );
                    }

                template<typename WidColors, unsigned int I>
                    struct color_elements {
                        static void func( std::tuple<Elems...>& pElems, WidColors& pColors ) {
                            color_elements<WidColors, I - 1>::func( pElems, pColors );
                            std::get<I - 1>( pElems ).color( std::get<I - 1>( pColors.colors ) );
                        }
                    };

                template<typename WidColors>
                    struct color_elements<WidColors, 0> {
                        static void func( std::tuple<Elems...>& pElems, WidColors& pColors ) {				}
                    };
                template<class... Colors>
                    void color( WidgetColors<Colors...> pColors ) {
                        static_assert( sizeof...( Colors ) == sizeof...( Elems ), "Color count does not match the element count of Widget" );
                        color_elements<WidgetColors<Colors...>, sizeof...( Elems )>::func( elements, pColors );
                    }

            private:

                std::tuple<Elems...> elements;
                unsigned int pos_index;
                unsigned int size_index;
                WidgetMovePolicy<Widget<Elems...>> move_policy;
                WidgetResizePolicy<Widget<Elems...>> resize_policy;

                void move_all( glm::vec2& pOffset, _index<0> = _index<0>() ) {}
                template<unsigned int N>
                    void move_all( glm::vec2& pOffset, _index<N> = _index<N>() ) {//move all elements
                        move_all( pOffset, _index<N-1>() );
                        move_element<N - 1>( pOffset );//* ( glm::vec2 )move_policy.matrix[N] );
                    }


                void set_all_pos( _index<0>, glm::vec2& pNewPos ) {}
                template<unsigned int N>
                    void set_all_pos( _index<N>, glm::vec2& pNewPos ) {//move all elements
                        set_all_pos<N - 1>( _index<N-1>(), pNewPos );
                        set_element_pos<N - 1>( pNewPos );
                    }

                void resize_all( _index<0>, glm::vec2& pOffset ) {}
                template<unsigned int N>
                    void resize_all( _index<N>, glm::vec2& pOffset ) {//move all elements
                        resize_all<N - 1>( _index<N-1>(), pOffset );

                        move_element<N - 1>( pOffset * glm::vec2( resize_policy.matrix[N - 1].x, resize_policy.matrix[N - 1].y ) );
                        resize_element<N - 1>( pOffset );//resize matrix is applied later to size
                    }
        };


    template<class Wid>
        void move_widget( Wid pWidget, glm::vec2& pOffset ) {
            pWidget.move( pOffset );
        }
    template<class Wid, typename A, typename B>
        void move_widget( Wid pWidget, A pOffsetX, B pOffsetY ) {
            pWidget.move( glm::vec2( pOffsetX, pOffsetY ) );
        }
    template<class Wid, unsigned int N>
        void move_widget_element( Wid pWidget, float pOffsetX, float pOffsetY ) {
            pWidget.move.element<N>( glm::vec2( pOffsetX, pOffsetY ) );
        }
    template<class Wid>
        void resize_widget( Wid pWidget, glm::vec2& pOffset ) {
            pWidget.resize( pOffset );
        }
    template<class Wid, typename A, typename B>
        void resize_widget( Wid pWidget, A pDirX, B pDirY ) {
            pWidget.resize( glm::vec2( pDirX, pDirY ) );
        }

    //generate Widget with N Quads 
    template<unsigned int N, typename ...Quads>
        struct gen_quad_group {//MAY BE SLOW TO COMPILE

            typedef typename gen_quad_group<N - 1, Quad, Quads...>::type type;
        };
    template<typename ...Quads>
        struct gen_quad_group<0, Quads...> {//MAY BE SLOW TO COMPILE

            typedef Widget<Quads...> type;
        };

    template<unsigned int QuadCount>
        using QuadGroup = typename gen_quad_group<QuadCount>::type;


    //generate Colors for a Widget with N quads 
    template<unsigned int N, typename Color, typename ...Colors>
        struct gen_color_group {

            typedef typename gen_color_group<N - 1, Color, Color, Colors...>::type type;
        };
    template<typename Color, typename ...Colors>
        struct gen_color_group<0, Color, Colors...> {

            typedef WidgetColors<Colors...> type;
        };

    //ColorGroup is really of type WidgetColors
    template<unsigned int ColorCount, typename Color = Color>
        using ColorGroup = typename gen_color_group<ColorCount, Color>::type;

}
