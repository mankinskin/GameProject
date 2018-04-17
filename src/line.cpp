#include "line.h"
#include "shader.h"
#include "vao.h"

std::vector<gui::LineGroup> allLineGroups;

std::array<glm::uvec2, gui::MAX_LINE_COUNT> allLines;
unsigned int lineCount = 0;
std::array<glm::uvec2, gui::MAX_LINE_VERTEX_COUNT> allLineVertices;
unsigned int lineVertexCount = 0;
std::array<glm::vec4, gui::MAX_LINE_VERTEX_POSITION_COUNT> allLineVertexPositions;
unsigned int lineVertexPositionCount = 0;

gl::StreamStorage<glm::uvec2> lineBuffer;
gl::StreamStorage<glm::uvec2> lineVertexBuffer;
gl::StreamStorage<glm::vec4> lineVertexPositionBuffer;

gl::VAO lineVAO;
unsigned int lineShader = 0;

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

unsigned int gui::createLine( unsigned int pVertexA, unsigned int pVertexB ) 
{
	allLines[ lineCount ] = glm::uvec2( pVertexA, pVertexB );
    return lineCount++;
}

unsigned int gui::createLineVertexPos( glm::vec4 pPos ) 
{
    allLineVertexPositions[ lineVertexPositionCount ] = pPos;
    return lineVertexPositionCount++;
}

unsigned int getLineVertex( unsigned int pLineIndex, unsigned int pVertex )
{
    return allLines[ pLineIndex ][ pVertex ];
}

unsigned int gui::createLineVertex( unsigned int pPos, unsigned int pColor )
{
    allLineVertices[ lineVertexCount ] = glm::uvec2( pPos, pColor );
	return lineVertexCount++;
}

void gui::setLineVertexColor( unsigned int pVertex, unsigned int pColor )
{
	allLineVertices[ pVertex ].y = pColor;
}

void gui::colorLine( unsigned int pLine, unsigned int pColor )
{
	setLineVertexColor( getLineVertex( pLine, 0 ), pColor );
	setLineVertexColor( getLineVertex( pLine, 1 ), pColor );
}

void gui::initLineVAO() 
{
    lineBuffer = gl::StreamStorage<glm::uvec2>( "LineBuffer", 
            MAX_LINE_COUNT, GL_MAP_WRITE_BIT );

    lineVertexPositionBuffer = gl::StreamStorage<glm::vec4>( "LineVertexPositonBuffer", 
            MAX_LINE_VERTEX_POSITION_COUNT, GL_MAP_WRITE_BIT );
    lineVertexPositionBuffer.setTarget( GL_UNIFORM_BUFFER );

    lineVertexBuffer = gl::StreamStorage<glm::uvec2>( "LineVertexBuffer", 
            MAX_LINE_VERTEX_COUNT, GL_MAP_WRITE_BIT );

    lineVAO = gl::VAO( "lineVAO" );
    lineVAO.vertexBuffer( 0, lineVertexBuffer, sizeof(unsigned int) * 2);
    lineVAO.vertexAttrib( 0, 0, 2, GL_UNSIGNED_INT, 0 );
	lineVAO.elementBuffer( lineBuffer );
}

void gui::updateLinePositions()
{
    gl::uploadStorage( lineVertexPositionBuffer, sizeof( glm::vec4 ) * lineVertexPositionCount, &allLineVertexPositions[0] );
}

void gui::updateLineColors()
{
    gl::uploadStorage( lineVertexBuffer, sizeof( glm::uvec2 ) * lineVertexCount, &allLineVertices[0] );
}

void gui::updateLineBuffer()
{
    gl::uploadStorage( lineBuffer, sizeof( glm::uvec2 ) * lineCount, &allLines[0] );
}

void gui::renderLines()
{
    lineVAO.bind();
    glDepthFunc( GL_ALWAYS );
    glBlendFunc( GL_SRC_ALPHA, GL_DST_ALPHA );
    shader::use( lineShader );
    for ( unsigned int g = 0; g < allLineGroups.size(); ++g ) {
        const LineGroup& lineGroup = allLineGroups[g];
        if ( lineGroup.flags ) {
            glDrawElements( GL_LINES, 
                    lineGroup.lineCount * 2, 
					GL_UNSIGNED_INT,
                    (void*)( lineGroup.lineOffset * 2 * sizeof(unsigned int) ) ); 
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
    shader::bindUniformBufferToShader( lineShader, lineVertexPositionBuffer, "PosBuffer" );
}
