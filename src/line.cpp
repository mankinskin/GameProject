#include "line.h"
#include "shader.h"

const unsigned int MAX_LINE_VERTEX_COUNT = 500;
const unsigned int MAX_LINE_COUNT = 1000;
std::vector<gui::LineGroup> allLineGroups;
size_t lineCount = 0;
size_t lineVertexCount = 0;
size_t vertexPosCount = 0;
unsigned int lineVAO = 0;
gl::StreamStorage<glm::uvec2> lineVBO;
gl::StreamStorage<glm::uvec2> lineEBO;
gl::StreamStorage<glm::vec4> vertexPosBuffer;
unsigned int lineShader = 0;
std::vector<int> lineGroupFlags;


unsigned int gui::getLineCount() 
{
    return lineCount;
}

unsigned int gui::createLineGroup( unsigned int pLineOffset, 
        unsigned int pLineCount, int pFlag ) 
{
    allLineGroups.push_back( LineGroup( pLineOffset, pLineCount ) );
    lineGroupFlags.push_back( pFlag );
    return allLineGroups.size() - 1;
}

void gui::toggleLineGroup( unsigned int pLineGroup ) 
{
    lineGroupFlags[pLineGroup] = !lineGroupFlags[pLineGroup];
}

unsigned int gui::createLine( glm::vec4 pVertexAPos, unsigned int pColorIndexA, glm::vec4 pVertexBPos, unsigned int pColorIndexB ) 
{
    return createLine( createLineVertex( pVertexAPos, pColorIndexA ), createLineVertex( pVertexBPos, pColorIndexB ) );
}

unsigned int gui::createLine( glm::vec4 pVertexAPos, glm::vec4 pVertexBPos, unsigned int pColorIndex ) 
{
    return createLine( createLineVertex( pVertexAPos, pColorIndex ), createLineVertex( pVertexBPos, pColorIndex ) );
}

unsigned int gui::createLine( unsigned int pVertexA, unsigned int pVertexB ) 
{
	lineVBO[ lineCount ] = glm::uvec2( pVertexA, pVertexB );
    return lineCount++;
}

unsigned int gui::createLineVertex( glm::vec4 pPos, unsigned int pColorIndex ) 
{
    return createLineVertex( createLineVertexPosition( pPos ), pColorIndex );
}

unsigned int gui::createLineVertex( LinePointIt vertex, unsigned int pColorIndex ) 
{
    lineEBO[ lineVertexCount ] = glm::uvec2( vertex.index, pColorIndex );
    return lineVertexCount++;
}

gui::LinePointIt gui::createLineVertexPosition( glm::vec4 pPos ) 
{
    vertexPosBuffer[ vertexPosCount ] = pPos;
    return LinePointIt( vertexPosBuffer, vertexPosCount++ );
}

void gui::setLineColor( unsigned int pLineIndex, unsigned int pColorIndex ) 
{
    setLineVertexColor( pLineIndex, 0, pColorIndex );
    setLineVertexColor( pLineIndex, 1, pColorIndex );
}

unsigned int getLineVertexIndex( unsigned int pLineIndex, unsigned int pVertex )
{
    return *( unsigned int* )( &lineVBO[ pLineIndex ] + sizeof( unsigned int ) * pVertex );
}

void gui::setLineVertexColor( unsigned int pLineIndex, unsigned int pVertex, unsigned int pColorIndex )
{
    std::memcpy( &lineVBO[getLineVertexIndex( pLineIndex, pVertex )].y, &pColorIndex, sizeof( unsigned int ) );
}

void gui::initLineVAO() 
{
    glCreateVertexArrays( 1, &lineVAO );

    vertexPosBuffer = gl::StreamStorage<glm::vec4>( "LineVertexPosBuffer", MAX_LINE_VERTEX_COUNT, GL_MAP_WRITE_BIT );
    gl::setStorageTarget( vertexPosBuffer, GL_UNIFORM_BUFFER );

    lineVBO = gl::StreamStorage<glm::uvec2>( "LineIndexBuffer", MAX_LINE_VERTEX_COUNT, GL_MAP_WRITE_BIT );
    lineEBO = gl::StreamStorage<glm::uvec2>( "LineVertexIndexBuffer", MAX_LINE_VERTEX_COUNT, GL_MAP_WRITE_BIT );

    //gl::setVertexArrayVertexStorage( lineVAO, 0, lineVBO, sizeof( glm::uvec2 ) );
    glVertexArrayVertexBuffer( lineVAO, 0, lineVBO.ID, 0, sizeof( glm::uvec2 ) );
    //gl::setVertexAttrib( lineVAO, 0, 0, 2, GL_UNSIGNED_INT, 0 );

    glVertexArrayElementBuffer( lineVAO, lineEBO.ID );
}

void gui::updateLinePositions()
{
    //gl::uploadStorage( vertexPosBuffer, sizeof( glm::vec4 ) * allLinePointData.size(), &allLinePointData[0] );
}

void gui::updateLineBuffers()
{
    if ( !lineCount ) {
        return;
    }
        updateLinePositions();
        //gl::uploadStorage( lineVBO, sizeof( glm::uvec2 ) * allLineVertices.size(), &allLineVertices[0] );
        //gl::uploadStorage( lineEBO, sizeof( glm::uvec2 ) * allLines.size(), &allLines[0] );
}

void gui::renderLines()
{
    glBindVertexArray( lineVAO );
    glDepthFunc( GL_ALWAYS );
    glBlendFunc( GL_SRC_ALPHA, GL_DST_ALPHA );
    shader::use( lineShader );
    for ( unsigned int m = 0; m < allLineGroups.size(); ++m ) {
        if ( lineGroupFlags[m] ) {
            glDrawElements( GL_LINES, 
                    allLineGroups[m].lineCount * 2, 
                    GL_UNSIGNED_INT, 
                    ( unsigned int* )( allLineGroups[m].lineOffset * 2 * sizeof( unsigned int ) ) );
        }
    }
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glDepthFunc( GL_LESS );
    shader::unuse();
    glBindVertexArray( 0 );
}

void gui::initLineShader() 
{
    lineShader = shader::newProgram( "lineShader", 
            shader::createModule( "lineShader.vert" ), 
            shader::createModule( "lineShader.frag" ) );
    shader::addVertexAttribute( lineShader, "vertex", 0 );
}

void gui::setupLineShader() 
{
    shader::bindUniformBufferToShader( lineShader, gl::generalUniformBuffer, "GeneralUniformBuffer" );
    shader::bindUniformBufferToShader( lineShader, gl::colorBuffer, "ColorBuffer" );
    shader::bindUniformBufferToShader( lineShader, vertexPosBuffer, "PosBuffer" );
}
