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
    const unsigned int MAX_LINE_VERTEX_COUNT = 400;
    const unsigned int MAX_LINE_VERTEX_POSITION_COUNT = 200;
    const unsigned int MAX_LINE_COUNT = 200;
    const int DEFAULT_LINE_GROUP_FLAGS = 1;

    struct LineGroup {
        LineGroup( unsigned int pLineOffset, unsigned int pLineCount, 
                int pFlags = DEFAULT_LINE_GROUP_FLAGS );
        unsigned int lineOffset;
        unsigned int lineCount;
        int flags;
    };

    void toggleLineGroup( unsigned int pLineGroup );
    unsigned int createLineGroup( unsigned int pLineOffset, 
            unsigned int pLineCount, int pFlags = DEFAULT_LINE_GROUP_FLAGS );
    //struct LineVertex{
    //    LinePointIt vertex;
    //    gl::ColorIt color;
    //};


    unsigned int createLineVertexPos( glm::vec4 pPos );
    unsigned int createLineVertex( unsigned int pVertex, unsigned int pColor = 0 );
    unsigned int createLine( unsigned int pVertexA, unsigned int pVertexB );
	void colorLine( unsigned int pLine, unsigned int pColor );
	void setLineVertexColor( unsigned int pVertex, unsigned int pColor );


    unsigned int getLineCount();
    void initLineVAO();
    void updateLinePositions();
    void updateLineColors();
    void renderLines();
    void initLineShader();
    void updateLineBuffer();
    void setupLineShader();
}

