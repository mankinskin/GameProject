#pragma once
#include "storage.h"
#include "vao.h"
#include "shader.h"

namespace gui 
{
    void rasterQuadIndices();

    void initQuadIndexBuffer();
    void initQuadIndexShader();
    void setupQuadIndexShader();
    void readQuadIndexBuffer();
    unsigned int readQuadIndexMap( const unsigned int pPos );
    unsigned int readQuadIndexMap( const unsigned int pXPos, const unsigned int pYPos );
    float readQuadDepthMap( const unsigned int pPos );
    float readQuadDepthMap( const unsigned int pXPos, const unsigned int pYPos );

    extern shader::Program quadIndexShader;
    extern gl::VAO quadIndexVAO;
    extern gl::StreamStorage<unsigned int> quadIndexBuffer;
}
