#include "line.h"
#include "vao.h"
#include "shader.h"

const unsigned int MAX_LINE_VERTEX_COUNT = 4000;
const unsigned int MAX_LINE_COUNT = 4000;
std::vector<gui::LineGroup> allLineGroups;
std::vector<gl::Point3D> allLinePointData;
std::vector<glm::uvec2> allLineVertices;//stores a vertex as the indices to its data (pos & color)
std::vector<glm::uvec2> allLines;//stores each line as 2 indices to its vertices
unsigned int lineVAO = 0;
unsigned int lineVBO = 0;
unsigned int lineEBO = 0;
unsigned int vertexPosBuffer = 0;
unsigned int lineShader = 0;
std::vector<int> lineGroupFlags;


unsigned int gui::getLineCount() 
{
    return allLines.size();
}
unsigned int gui::createLineGroup(unsigned int pLineOffset, 
        unsigned int pLineCount, int pFlag) 
{
    allLineGroups.push_back(LineGroup(pLineOffset, pLineCount));
    lineGroupFlags.push_back(pFlag);
    return allLineGroups.size() - 1;
}
void gui::toggleLineGroup(unsigned int pLineGroup) 
{
    lineGroupFlags[pLineGroup] = !lineGroupFlags[pLineGroup];
}
unsigned int gui::createLine(glm::vec4 pVertexAPos, unsigned int pColorIndexA, glm::vec4 pVertexBPos, unsigned int pColorIndexB) 
{
    return createLine(createLineVertex(pVertexAPos, pColorIndexA), createLineVertex(pVertexBPos, pColorIndexB));
}
unsigned int gui::createLine(glm::vec4 pVertexAPos, glm::vec4 pVertexBPos, unsigned int pColorIndex) 
{
    return createLine(createLineVertex(pVertexAPos, pColorIndex), createLineVertex(pVertexBPos, pColorIndex));
}
unsigned int gui::createLine(unsigned int pVertexA, unsigned int pVertexB) 
{
    allLines.push_back(glm::uvec2(pVertexA, pVertexB));
    return allLines.size() - 1;
}

unsigned int gui::createLineVertex(glm::vec4 pPos, unsigned int pColorIndex) 
{
    return createLineVertex(createLineVertexPosition(pPos), pColorIndex);
}
unsigned int gui::createLineVertex(gui::Itr<gl::Point3D> vertex, unsigned int pColorIndex) 
{
    allLineVertices.push_back(glm::uvec2(vertex.index, pColorIndex));
    return allLineVertices.size() - 1;
}
gui::LinePointIt gui::createLineVertexPosition(glm::vec4 pPos) 
{
    allLinePointData.push_back(pPos);
    return LinePointIt(allLinePointData, allLinePointData.size()-1);
}
void gui::setLineColor(unsigned int pLineIndex, unsigned int pColorIndex) 
{
    setLineVertexColor(pLineIndex, 0, pColorIndex);
    setLineVertexColor(pLineIndex, 1, pColorIndex);
}
unsigned int getLineVertexIndex(unsigned int pLineIndex, unsigned int pVertex)
{
    return *(unsigned int*)(&allLines[pLineIndex] + sizeof(unsigned int)*pVertex);
}
void gui::setLineVertexColor(unsigned int pLineIndex, unsigned int pVertex, unsigned int pColorIndex)
{
    std::memcpy(&allLineVertices[getLineVertexIndex(pLineIndex, pVertex)].y, &pColorIndex, sizeof(unsigned int));
}
void gui::initLineVAO() 
{
    allLinePointData.reserve(MAX_LINE_VERTEX_COUNT);
    allLineVertices.reserve(MAX_LINE_COUNT);
    allLines.reserve(MAX_LINE_COUNT);
    glCreateVertexArrays(1, &lineVAO);

    vertexPosBuffer = vao::createStorage(sizeof(glm::vec4) * MAX_LINE_VERTEX_COUNT, 0, vao::MAP_PERSISTENT_FLAGS|GL_MAP_WRITE_BIT);
    vao::bindStorage(GL_UNIFORM_BUFFER, vertexPosBuffer);
    vao::createStream(vertexPosBuffer, GL_MAP_WRITE_BIT);


    lineVBO = vao::createStorage(sizeof(glm::uvec2) * MAX_LINE_VERTEX_COUNT, 0, vao::MAP_PERSISTENT_FLAGS | GL_MAP_WRITE_BIT);
    vao::createStream(lineVBO, GL_MAP_WRITE_BIT);

    lineEBO = vao::createStorage(sizeof(glm::uvec2) * MAX_LINE_VERTEX_COUNT, 0, vao::MAP_PERSISTENT_FLAGS | GL_MAP_WRITE_BIT);
    vao::createStream(lineEBO, GL_MAP_WRITE_BIT);

    vao::setVertexArrayVertexStorage(lineVAO, 0, lineVBO, sizeof(glm::uvec2));
    glVertexArrayVertexBuffer(lineVAO, 0, vao::getStorageID(lineVBO), 0, sizeof(glm::uvec2));
    vao::setVertexAttrib(lineVAO, 0, 0, 2, GL_UNSIGNED_INT, 0);

    glVertexArrayElementBuffer( lineVAO, vao::getStorageID( lineEBO ) );
}

void gui::updateLinePositions()
{
    vao::uploadStorage( vertexPosBuffer, sizeof(glm::vec4) * allLinePointData.size(), &allLinePointData[0] );
}
void gui::updateLineBuffers()
{
    if ( !allLines.size() ) {
        return;
    }
        updateLinePositions();
        vao::uploadStorage( lineVBO, sizeof(glm::uvec2) * allLineVertices.size(), &allLineVertices[0] );
        vao::uploadStorage( lineEBO, sizeof(glm::uvec2) * allLines.size(), &allLines[0] );
    
}

void gui::renderLines()
{
    glBindVertexArray( lineVAO );
    glDepthFunc( GL_ALWAYS );
    glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
    shader::use( lineShader );
    for ( unsigned int m = 0; m < allLineGroups.size(); ++m )
    {
        if ( lineGroupFlags[m] ){
            glDrawElements( GL_LINES, 
                    allLineGroups[m].lineCount * 2, 
                    GL_UNSIGNED_INT, 
                    (unsigned int*)( allLineGroups[m].lineOffset * 2 * sizeof(unsigned int) ) );
        }
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    shader::unuse();
    glBindVertexArray( 0 );
    glDepthFunc( GL_LESS );
}

void gui::initLineShader() 
{
    lineShader = shader::newProgram( "lineShader", 
            shader::createModule( "lineShader.vert" ), 
            shader::createModule( "lineShader.frag" ));
    shader::addVertexAttribute( lineShader, "vertex", 0 );
}
void gui::setupLineShader() 
{
    shader::bindUniformBufferToShader( lineShader, gl::generalUniformBuffer, "GeneralUniformBuffer" );
    shader::bindUniformBufferToShader( lineShader, gl::constColorBuffer, "ColorBuffer" );
    shader::bindUniformBufferToShader( lineShader, vertexPosBuffer, "PosBuffer" );
}
