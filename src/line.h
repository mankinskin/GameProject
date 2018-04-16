#pragma once

#include <array>
#include <vector>
#include <glm.hpp>
#include "gl.h"
#include "color.h"
#include "utils.h"
#include "storage.h"

namespace gui
{
    const unsigned int MAX_LINE_VERTEX_COUNT = 200;
    const unsigned int MAX_LINE_VERTEX_COLOR_COUNT = 400;
    const unsigned int MAX_LINE_COUNT = 200;
    typedef utils::Itr<glm::vec4, std::array<glm::vec4, MAX_LINE_VERTEX_COUNT>> LinePointIt;
    const int DEFAULT_LINE_GROUP_FLAGS = 1;

    struct LineGroup {
        LineGroup( unsigned int pLineOffset, unsigned int pLineCount, 
                int pFlags = DEFAULT_LINE_GROUP_FLAGS );
        unsigned int lineOffset;
        unsigned int lineCount;
        int flags;
    };

    struct LineVertex{
        LinePointIt vertex;
        gl::ColorIt color;
    };

    void toggleLineGroup( unsigned int pLineGroup );
    unsigned int getLineCount();

    unsigned int createLineGroup( unsigned int pLineOffset, 
            unsigned int pLineCount, int pFlags = DEFAULT_LINE_GROUP_FLAGS );
    unsigned int createLine( glm::vec4 pVertexAPos, glm::vec4 pVertexBPos );
    unsigned int createLine( unsigned int pVertexA, unsigned int pVertexB );
    unsigned int createLineVertex( glm::vec4 pPos );

    void colorLine( unsigned int pLineIndex, unsigned int pColorIndex );
    void colorLineVertex( unsigned int pVertex, unsigned int pColorIndex );

    void initLineVAO();
    void updateLinePositions();
    void updateLineColors();
    void renderLines();
    void initLineShader();
    void updateLineBuffer();
    void setupLineShader();
}

