#ifndef QUAD_H
#define QUAD_H
#include <cstring>
#include <glm.hpp>
#include <vector>
#include "event.h"
#include <tuple>
#include "color.h"
#include "quadcolors.h"
#include "utils.h"

namespace gui {
	typedef glm::vec4 QuadData;

	extern std::vector<glm::vec4> allQuads;
	unsigned int createQuad( const float pPosX, const float pPosY, const float pWidth, const float pHeight );
	unsigned int createQuad( const glm::vec4 pQuad );

    struct Quad {
        using initer_t = glm::vec4;
        const unsigned int index;

        Quad( const unsigned int in )
            :index( in )
        {}
        Quad( const initer_t pIniter ) 
            :index( createQuad( pIniter ) )
        {}


        void set_pos( glm::vec2 pos ) const
        {
            std::memcpy( &allQuads[index - 1], &pos, sizeof( glm::vec2 ) );
        }
        void move( glm::vec2 dir ) const
        {
            allQuads[index-1] += glm::vec4( dir, 0.0f, 0.0f );
        }
        void resize( glm::vec2 dir ) const
        {
            allQuads[index-1] += glm::vec4( 0.0f, 0.0f, dir.x, -dir.y );
        }
        glm::vec2 get_pos() const
        {
            return glm::vec2( allQuads[index - 1].x, allQuads[index - 1].y );
        }
        glm::vec2 get_size() const
        {
            return glm::vec2( allQuads[index - 1].z, allQuads[index - 1].w );
        }

        template<unsigned int N>
            unsigned int element() const
            {
                return index;
            }
        unsigned int element() const
        {
            return index;
        }
    };

    void colorQuad( Quad pQuad, gl::ColorIt pColor );
    void moveQuad( const unsigned int pQuad, const glm::vec2 pOffset );
    void resizeQuad( const unsigned int pQuad, const glm::vec2 pOffset );
    void setQuadPos( const unsigned int pQuad, const glm::vec2 pPos );

    QuadData getQuadData( unsigned int pQuadIndex );
    template<typename A, typename B>
        void moveQuad( const unsigned int pQuad, A pOffsetX, B pOffsetY ) {
            allQuads[pQuad - 1] += glm::vec4( pOffsetX, pOffsetY, 
                    0.0f, 0.0f );
        }
    template<typename A, typename B>
        void setQuadPos( const unsigned int pQuad, A pPosX, B pPosY ) {
            allQuads[pQuad - 1].x = pPosX;
            allQuads[pQuad - 1].y = pPosY;
        }

    template<typename A, typename B>
        void resizeQuad( const unsigned int pQuad, A pOffsetX, B pOffsetY ) {
            allQuads[pQuad - 1] += glm::vec4( 0.0f, 0.0f, pOffsetX, pOffsetY );
        }
    void rasterQuadIndices();

    void initQuadIndexBuffer();
    void initQuadIndexShader();
    void setupQuadIndexShader();
    void readQuadIndexBuffer();
    void reserveQuads( const unsigned int pCount );
    unsigned int readQuadIndexMap( const unsigned int pPos );
    unsigned int readQuadIndexMap( const unsigned int pXPos, const unsigned int pYPos );
    float readQuadDepthMap( const unsigned int pPos );
    float readQuadDepthMap( const unsigned int pXPos, const unsigned int pYPos );
    void initQuadBuffer();
    void clearQuads();
    void updateQuadBuffer();

	extern unsigned int quadIndexBuffer;
    extern unsigned int quadIndexShader;
    extern unsigned int quadIndexVAO;
    extern unsigned int quadBuffer;

}
#endif //QUAD_H
