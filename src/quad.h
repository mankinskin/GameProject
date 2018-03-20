#ifndef QUAD_H
#define QUAD_H
#include <cstring>
#include <glm.hpp>
#include <vector>
#include "event.h"
#include <tuple>
#include "quadcolors.h"
#include "utils.h"

namespace gui {
	typedef glm::vec4 QuadData;

	extern std::vector<glm::vec4> allQuads;
	unsigned int createQuad( float pPosX, float pPosY, float pWidth, float pHeight );
	unsigned int createQuad( glm::vec4 pQuad );

	template<class ColorType>
	void colorQuad( unsigned int pQuadID, utils::Itr<ColorType> pColor );

    struct Quad {
        typedef glm::vec4 initer_t;
        Quad() :index( -1 ) {}
        Quad( unsigned int pIndex ) :index( pIndex ) {}
        Quad( initer_t pIniter ) :index( createQuad( pIniter ) ) {}
        Quad operator=( unsigned int i ) { return Quad( i ); }


        unsigned int index;
        //--
        void set_pos( glm::vec2 pos ) {
            std::memcpy( &allQuads[index - 1], &pos, sizeof( glm::vec2 ) );
        }
        void move( glm::vec2 dir ) {
            allQuads[index-1] += glm::vec4( dir, 0.0f, 0.0f );
        }
        void resize( glm::vec2 dir ) {
            allQuads[index-1] += glm::vec4( 0.0f, 0.0f, dir.x, -dir.y );
        }
        glm::vec2 get_pos() {
            return glm::vec2( allQuads[index - 1].x, allQuads[index - 1].y );
        }
        glm::vec2 get_size() {
            return glm::vec2( allQuads[index - 1].z, allQuads[index - 1].w );
        }

        template<class ColorType>
            void color( utils::Itr<ColorType> pColor ) {
                colorQuad( index, pColor );
            }
        //--
        template<unsigned int N>
            unsigned int element() {
                return index;
            }
        unsigned int element() {
            return index;
        }

    };

    void moveQuad( unsigned int pQuad, glm::vec2 pOffset );
    void resizeQuad( unsigned int pQuad, glm::vec2 pOffset );
    void setQuadPos( unsigned int pQuad, glm::vec2 pPos );

    template<typename A, typename B>
        void moveQuad( unsigned int pQuad, A pOffsetX, B pOffsetY ) {
            allQuads[pQuad - 1] += glm::vec4( pOffsetX, pOffsetY, 0.0f, 0.0f );
        }
    template<typename A, typename B>
        void setQuadPos( unsigned int pQuad, A pPosX, B pPosY ) {
            allQuads[pQuad - 1].x = pPosX;
            allQuads[pQuad - 1].y = pPosY;
        }

    template<typename A, typename B>
        void resizeQuad( unsigned int pQuad, A pOffsetX, B pOffsetY ) {
            allQuads[pQuad - 1] += glm::vec4( 0.0f, 0.0f, pOffsetX, pOffsetY );
        }
    void rasterQuadIndices();

    void initQuadIndexBuffer();
    void initQuadIndexShader();
    void setupQuadIndexShader();
    void readQuadIndexBuffer();
    void reserveQuads( unsigned int pCount );
    unsigned int readQuadIndexMap( unsigned int pPos );
    unsigned int readQuadIndexMap( unsigned int pXPos, unsigned int pYPos );
    float readQuadDepthMap( unsigned int pPos );
    float readQuadDepthMap( unsigned int pXPos, unsigned int pYPos );
    void initQuadBuffer();
    void clearQuads();
    void updateQuadBuffer();

	extern unsigned int quadIndexBuffer;
    extern unsigned int quadIndexShader;
    extern unsigned int quadIndexVAO;
    extern unsigned int quadBuffer;

}
template<typename ColorType>
void gui::colorQuad( unsigned int pQuadID, utils::Itr<ColorType> pColor ) 
{
    printf( "Coloring Quad %u with color %u\n", pQuadID, pColor.index );
    quadColors[pQuadID-1] = pColor.index;
}
#endif //QUAD_H
