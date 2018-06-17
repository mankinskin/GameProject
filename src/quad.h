#pragma once
#include <cstring>
#include <glm.hpp>
#include <vector>
#include <tuple>
#include "utils/id.h"
#include "storage.h"
#include "vao.h"
#include "color.h"

namespace gui 
{
    struct Quad 
    {
        Quad( glm::vec4 pData )
            :data( pData )
        {}

        glm::vec4 data;
        void move( const glm::vec2 pV ) const;
        void resize( const glm::vec2 pV ) const;
        void color( const gl::ColorID pColor ) const; 
    };
    using QuadID = utils::ID<Quad>;

    const unsigned int MAX_QUAD_COUNT = 10000;
    void setQuadPos( const QuadID pQuad, const glm::vec2 pPos );


    void initQuadBuffer();
    void updateQuadBuffer();
    extern gl::StreamStorage<glm::vec4> quadBuffer;
    extern size_t quadCount;
}
