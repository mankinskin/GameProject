#include "line.h"
#include "shader.h"
#include "vao.h"

std::vector<gui::LineGroup> allLineGroups;

std::array<glm::uvec2, gui::MAX_LINE_COUNT> allLines;
unsigned int lineCount = 0;
std::array<glm::uvec2, gui::MAX_LINE_VERTEX_COLOR_COUNT> allLineVertexColors;
unsigned int lineVertexColorCount = 0;
std::array<glm::vec4, gui::MAX_LINE_VERTEX_COUNT> allLineVertices;
unsigned int lineVertexCount = 0;

gl::StreamStorage<glm::uvec2> lineBuffer;
gl::StreamStorage<glm::uvec2> lineVertexColorBuffer;
gl::StreamStorage<glm::vec4> lineVertexBuffer;

gl::VAO lineVAO;
unsigned int lineShader = 0;

// How line shading works
//
// lines are connections between dots
// you create a dot (lineVertexPosition) using createLineVertex()
// you connect these dots using createLine(), passing it the indices to two dots
// you color a line using colorLine
//
// TO-DO:
// the design still contains a lot of ambiguity. It assigns a color to a vertex position by 
// creating an index pair containing first the index to a vertex position and second a color.
// this creates a coloredVertex, which should be used to form the lines. right now the lines are referencing 
// vertex positions, but this can not include color information, which is necessary for any shading
//
// Instead lines should reference coloredVertices (index pairs). the shader will use the lineBuffer as 
// elementBuffer and coloredVertexBuffer as vertexBuffer. 

unsigned int gui::getLineCount() 
{
    return lineCount;
}

gui::LineGroup::LineGroup( unsigned int pLineOffset, 
        unsigned int pLineCount, int pFlags )
			:lineOffset( pLineOffset ), lineCount( pLineCount ), flags( 1 )
{
}

unsigned int gui::createLineGroup( unsigned int pLineOffset, 
        unsigned int pLineCount, int pFlags )
{
    allLineGroups.push_back( LineGroup( pLineOffset, pLineCount, pFlags ) );
    return allLineGroups.size() - 1;
}

void gui::toggleLineGroup( unsigned int pLineGroup ) 
{
    allLineGroups[pLineGroup].flags = !allLineGroups[pLineGroup].flags;
}

unsigned int gui::createLine( glm::vec4 pVertexAPos, glm::vec4 pVertexBPos ) 
{
    return createLine( createLineVertex( pVertexAPos ), 
            createLineVertex( pVertexBPos ) );
}

unsigned int gui::createLine( unsigned int pVertexA, unsigned int pVertexB ) 
{
	allLines[ lineCount ] = glm::uvec2( pVertexA, pVertexB );
    return lineCount++;
}

unsigned int gui::createLineVertex( glm::vec4 pPos ) 
{
    allLineVertices[ lineVertexCount ] = pPos;
    return lineVertexCount++;
}

unsigned int getLineVertexIndex( unsigned int pLineIndex, unsigned int pVertex )
{
    return allLines[ pLineIndex ][ pVertex ];
}

void gui::colorLine( unsigned int pLineIndex, unsigned int pColorIndex ) 
{
    colorLineVertex( getLineVertexIndex( pLineIndex, 0 ), pColorIndex );
    colorLineVertex( getLineVertexIndex( pLineIndex, 1 ), pColorIndex );
}

void gui::colorLineVertex( unsigned int pVertex, unsigned int pColorIndex )
{
    allLineVertexColors[ lineVertexColorCount++ ] = glm::uvec2( pVertex, pColorIndex );
}

void gui::initLineVAO() 
{
    lineVertexBuffer = gl::StreamStorage<glm::vec4>( "LineVertexPosBuffer", 
            MAX_LINE_VERTEX_COUNT, GL_MAP_WRITE_BIT );
    lineVertexBuffer.setTarget( GL_UNIFORM_BUFFER );

    lineVertexColorBuffer = gl::StreamStorage<glm::uvec2>( "LineVertexColorBuffer", 
            MAX_LINE_VERTEX_COLOR_COUNT, GL_MAP_WRITE_BIT );

    lineVAO = gl::VAO( "lineVAO" );
    lineVAO.vertexBuffer( 0, lineVertexColorBuffer, sizeof(unsigned int) * 2);
    lineVAO.vertexAttrib( 0, 0, 2, GL_UNSIGNED_INT, 0 );
}

void gui::updateLinePositions()
{
    gl::uploadStorage( lineVertexBuffer, sizeof( glm::vec4 ) * lineVertexCount, &allLineVertices[0] );
}

void gui::updateLineColors()
{
    gl::uploadStorage( lineVertexColorBuffer, sizeof( glm::uvec2 ) * lineVertexColorCount, &allLineVertexColors[0] );
}

void gui::renderLines()
{
    lineVAO.bind();
    glDepthFunc( GL_ALWAYS );
    glBlendFunc( GL_SRC_ALPHA, GL_DST_ALPHA );
    shader::use( lineShader );
    for ( unsigned int g = 0; g < allLineGroups.size(); ++g ) {
        const LineGroup& lineGroup = allLineGroups[g];
        printf("Drawing Line Group: %u %u\n", lineGroup.lineOffset, lineGroup.lineCount );
        if ( lineGroup.flags ) {
            glDrawArrays( GL_LINES, 
                    lineGroup.lineOffset * 2, 
                    lineGroup.lineCount * 2); 
        }
    }
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glDepthFunc( GL_LESS );
    shader::unuse();
    lineVAO.unbind();
}

void gui::initLineShader() 
{
    puts( "Creating lineShader..." );
    lineShader = shader::newProgram( "lineShader", 
            shader::createModule( "lineShader.vert" ), 
            shader::createModule( "lineShader.frag" ) );
    shader::addVertexAttribute( lineShader, "vertexColor", 0 );
}

void gui::setupLineShader() 
{
    puts( "Binding lineShader..." );
    shader::bindUniformBufferToShader( lineShader, gl::generalUniformBuffer, "GeneralUniformBuffer" );
    shader::bindUniformBufferToShader( lineShader, gl::colorBuffer, "ColorBuffer" );
    shader::bindUniformBufferToShader( lineShader, lineVertexBuffer, "PosBuffer" );
}
