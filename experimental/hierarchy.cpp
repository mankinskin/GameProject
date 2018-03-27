#include <vector>
#include <tuple>

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
    using initer_t = vec4;
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

std::vector<vec4> allQuads;

struct Quad
{
    using initer_t = vec4;
    Quad( vec4 pQuad )
        :index( allQuads, allQuads.size() )
    {
       allQuads.push_back( pQuad ); 
    }
    Quad( size_t i )
        :index( allQuads, i )
    {}
    Itr<vec4> index;

    vec4& operator*()
    {
        return *index;
    }
};

template<typename... Elems>
struct Hierarchy
{
    using initer_t = std::tuple<typename Elems::initer_t...>;
    Hierarchy( const initer_t pIniter )
        :elements( pIniter )
    {}
    std::tuple<Elems...> elements;
};


int main()
{
    Color white( vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );
    Color black( vec4( 0.0f, 0.0f, 0.0f, 1.0f ) ); 
    
    Hierarchy<Quad, Quad>::initer_t double_quad( { vec4( 0.0f, 0.0f, 1.0f, 1.0f ), vec4( 0.1f, 0.1f, 0.9f, 0.9f ) } );
    Hierarchy<Hierarchy<Quad, Quad>, Hierarchy<Quad, Quad>> quad_tree( { double_quad, double_quad } );
    
    return 0;
}
