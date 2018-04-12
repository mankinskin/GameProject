#pragma once
#include <cstring>
#include <glm.hpp>
#include <vector>
#include <tuple>
#include "utils.h"
#include "storage.h"
#include "vao.h"

namespace gui 
{
    struct Quad 
    {
        Quad( const unsigned int in );
        Quad( glm::vec4 pData ); 

        const unsigned int index;
    };

	const unsigned int MAX_QUAD_COUNT = 10000;
	unsigned int createQuad( const float pPosX, const float pPosY, 
            const float pWidth, const float pHeight );
	unsigned int createQuad( const glm::vec4 pQuadData );
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
    extern gl::VAO quadIndexVAO;
	extern gl::StreamStorage<unsigned int> quadIndexBuffer;
    extern gl::StreamStorage<glm::vec4> quadBuffer;
    extern size_t quadCount;
}

