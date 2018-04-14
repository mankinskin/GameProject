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
        Quad( glm::vec4 pData ); 

        const unsigned int index;
    };

	const unsigned int MAX_QUAD_COUNT = 10000;
    void moveQuad( const Quad pQuad, const glm::vec2 pOffset );
    void moveQuadScaled( const Quad pQuad, const glm::vec2 pOffset, const glm::vec2 scale );
    void resizeQuad( const Quad pQuad, const glm::vec2 pOffset );
    void resizeQuadScaled( const Quad pQuad, const glm::vec2 pOffset, const glm::vec2 scale);
    void setQuadPos( const Quad pQuad, const glm::vec2 pPos );
    glm::vec4& getQuadData( Quad pQuad );


    void initQuadBuffer();
    void updateQuadBuffer();
    extern gl::StreamStorage<glm::vec4> quadBuffer;
    extern size_t quadCount;
}

