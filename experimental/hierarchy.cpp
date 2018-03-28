#include <vector>
#include <tuple>

#include "hierarchy.h"

template<typename T>
struct Itr
{
    Itr( std::vector<T>& Container, size_t Index )
        :container( Container ), index( Index )
        {}
    std::vector<T>& container;
    size_t index;
    T& operator*()
    {
        return container[index];
    }
};

struct vec4
{
    vec4( float X, float Y, float Z, float W )
        :x( X ), y( Y ), z( Z ), w( W )
    {}
    float x, y, z, w;
};

std::vector<vec4> allColors;

struct Color
{
    Color( vec4 pColor )
        :index( allColors, allColors.size() )
    {
       allColors.push_back( pColor ); 
    }
    Color( size_t i )
        :index( allColors, i )
    {}
    Itr<vec4> index;

    vec4& operator*()
    {
        return *index;
    }
};

std::vector<size_t> allQuads;

struct Quad
{
    Quad( size_t pQuad )
        :index( allQuads, allQuads.size() )
    {
        printf("Creating %d\n", pQuad );
        allQuads.push_back( pQuad ); 
    }
    Itr<size_t> index;

    size_t& operator*()
    {
        return *index;
    }
};


int main()
{
    Color white( vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );
    Color black( vec4( 0.0f, 0.0f, 0.0f, 1.0f ) ); 

    Hierarchy<size_t, size_t> double_quad( (size_t)1, (size_t)2 );
    Hierarchy<size_t, size_t> double_quad_two( size_t(3), size_t(4) );
    Hierarchy<Hierarchy<Quad, Quad>, Hierarchy<Quad, Quad>> quad_tree( double_quad, double_quad_two );

    return 0;
}
