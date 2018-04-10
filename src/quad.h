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
#include "storage.h"

namespace gui {
	unsigned int createQuad( const float pPosX, const float pPosY, const float pWidth, const float pHeight );
	unsigned int createQuad( const glm::vec4 pQuad );

    struct Quad 
    {
        const unsigned int index;

        Quad( const unsigned int in )
            :index( in )
        {}
        Quad( glm::vec4 pData ) 
            :index( createQuad( pData ) )
        {}
    };

    void colorQuad( Quad pQuad, gl::ColorIt pColor );
    void moveQuad( const Quad pQuad, const glm::vec2 pOffset );
    void moveQuadScaled( const Quad pQuad, const glm::vec2 pOffset, const glm::vec2 scale );
    void resizeQuad( const Quad pQuad, const glm::vec2 pOffset );
    void resizeQuadScaled( const Quad pQuad, const glm::vec2 pOffset, const glm::vec2 scale);
    void setQuadPos( const Quad pQuad, const glm::vec2 pPos );
    glm::vec4& getQuadData( Quad pQuad );

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
    void updateQuadBuffer();

    extern unsigned int quadIndexShader;
    extern unsigned int quadIndexVAO;
	extern gl::StreamStorage<unsigned int> quadIndexBuffer;
    extern gl::StreamStorage<glm::vec4> quadBuffer;

}
#endif //QUAD_H
