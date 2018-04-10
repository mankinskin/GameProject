#include "mesh.h"
#include "gldebug.h"
#include "model.h"
#include "shader.h"
#include "entities.h"
#include "vao.h"
#include "lights.h"
#include "material.h"
#include "texture.h"
#include "framebuffer.h"


bool mesh::draw_normals = false;
bool mesh::cull_face = true;
std::vector<mesh::Mesh> mesh::allMeshes;
std::vector<unsigned int> mesh::allIndices;
std::vector<mesh::Vertex> mesh::allStaticVertices;
std::vector<unsigned int> mesh::allMeshInstancenode;
std::vector<unsigned int> mesh::opaqueMeshList;
std::vector<unsigned int> mesh::blendMeshList;
unsigned int mesh::meshShader = 0;
unsigned int mesh::blendMeshShader = 0;
unsigned int mesh::meshNormalShader = 0;
unsigned int mesh::meshVAO = 0;
gl::Storage<mesh::Vertex> mesh::meshVBO;
gl::Storage<unsigned int> mesh::meshIBO;
gl::StreamStorage<unsigned int> mesh::nodeIndexBuffer;
unsigned int mesh::normalShaderProgram = 0;

void mesh::initMeshVAO()
{
	glCreateVertexArrays( 1, &meshVAO );
	meshVBO = gl::Storage<Vertex>( "MeshVertexBuffer", 
			mesh::allStaticVertices.size(), 0, &mesh::allStaticVertices[0] );
	meshIBO = gl::Storage<unsigned int>( "MeshIndexBuffer", 
			mesh::allIndices.size(), 0, &mesh::allIndices[0] );
	nodeIndexBuffer = gl::StreamStorage<unsigned int>( "MeshNodeIndexBuffer", 
            model::MAX_MODELS*model::MAX_MESHES_PER_MODEL, GL_MAP_WRITE_BIT );
	glBindVertexArray( meshVAO );
	glVertexArrayElementBuffer( meshVAO, meshIBO.ID );
	glVertexArrayVertexBuffer( meshVAO, 0, meshVBO.ID, 0, sizeof( Vertex ) );
	glVertexArrayVertexBuffer( meshVAO, 1, nodeIndexBuffer.ID, 0, sizeof( Vertex ) );
	//gl::setVertexArrayVertexStorage( meshVAO, 1, nodeIndexBuffer, sizeof( unsigned int ) );

	//gl::setVertexAttrib( meshVAO, 0, 0, 3, GL_FLOAT, offsetof( Vertex, pos ) );
	//gl::setVertexAttrib( meshVAO, 0, 1, 3, GL_FLOAT, offsetof( Vertex, normal ) );
	//gl::setVertexAttrib( meshVAO, 0, 2, 2, GL_FLOAT, offsetof( Vertex, uv ) );

	//gl::setVertexAttrib( meshVAO, 1, 3, 1, GL_UNSIGNED_INT, 0 );
	glVertexArrayBindingDivisor( meshVAO, 1, 1 );
	glBindVertexArray( 0 );
}

void mesh::initMeshShader()
{
	meshShader = shader::newProgram( "meshShader", shader::createModule( "meshShader.vert" ), 
            shader::createModule( "meshShader.frag" ) );
	shader::addVertexAttribute( meshShader, "pos", 0 );
	shader::addVertexAttribute( meshShader, "normal", 1 );
	shader::addVertexAttribute( meshShader, "uv", 2 );
	shader::addVertexAttribute( meshShader, "transform", 3 );
}

void mesh::initMeshNormalShader()
{
	meshNormalShader = shader::newProgram( "meshNormalShader", shader::createModule( "meshNormalShader.vert" ), 
            shader::createModule( "meshNormalShader.geo" ), shader::createModule( "meshNormalShader.frag" ) );
	shader::addVertexAttribute( meshNormalShader, "pos", 0 );
	shader::addVertexAttribute( meshNormalShader, "normal", 1 );
	shader::addVertexAttribute( meshNormalShader, "transform", 3 );
}

void mesh::initBlendMeshShader()
{
	blendMeshShader = shader::newProgram( "blendMeshShader", shader::createModule( "blendMeshShader.vert" ), 
            shader::createModule( "blendMeshShader.frag" ) );
	shader::addVertexAttribute( blendMeshShader, "pos", 0 );
	shader::addVertexAttribute( blendMeshShader, "normal", 1 );
	shader::addVertexAttribute( blendMeshShader, "uv", 2 );
	shader::addVertexAttribute( blendMeshShader, "transform", 3 );
}

void mesh::setupBlendMeshShader()
{
	shader::bindUniformBufferToShader( blendMeshShader, materialUBO, "MaterialBuffer" );
	shader::bindUniformBufferToShader( blendMeshShader, lights::lightDataUBO, "LightDataBuffer" );
	shader::bindUniformBufferToShader( blendMeshShader, gl::generalUniformBuffer, "GeneralUniformBuffer" );
	shader::bindUniformBufferToShader( blendMeshShader, entities::entityMatrixBuffer, "NodeMatrixBuffer" );
}

void mesh::renderMeshes()
{
	glBindVertexArray( meshVAO );
	shader::use( meshShader );

	for ( unsigned int m = 0; m < allMeshes.size(); ++m ) {
		mesh::Mesh& mesh = mesh::allMeshes[m];
		glActiveTexture( GL_TEXTURE0 );//amb
		glBindTexture( GL_TEXTURE_2D, mesh::allMaterialTextures[mesh.materialIndex].amb_tex );
		glActiveTexture( GL_TEXTURE1 );//diff
		glBindTexture( GL_TEXTURE_2D, mesh::allMaterialTextures[mesh.materialIndex].diff_tex );
		glActiveTexture( GL_TEXTURE2 );//spec
		glBindTexture( GL_TEXTURE_2D, mesh::allMaterialTextures[mesh.materialIndex].spec_tex );
		shader::setUniform( meshShader, "materialIndex", mesh.materialIndex );

		glDrawElementsInstancedBaseInstance( GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 
                ( void* )( mesh.indexOffset * sizeof( unsigned int ) ), 
                mesh.instanceCount, mesh.instanceOffset );
	}
	shader::unuse();
	glBindVertexArray( 0 );
}

void mesh::renderMeshNormals()
{
	if ( draw_normals ) {
		mesh::updateMeshBuffers();//TODO: make work without this
		glBindVertexArray( meshVAO );
		shader::use( meshNormalShader );
		for ( unsigned int m = 0; m < allMeshes.size(); ++m ) {
			mesh::Mesh& mesh = mesh::allMeshes[m];
			glDrawElementsInstancedBaseInstance( GL_POINTS, mesh.indexCount, GL_UNSIGNED_INT, 
                    ( void* )( mesh.indexOffset *sizeof( unsigned int ) ), 
                    mesh.instanceCount, mesh.instanceOffset );
		}
		shader::unuse();
		glBindVertexArray( 0 );
	}
}

void mesh::renderBlendMeshes()
{
	glBindVertexArray( meshVAO );
	shader::use( blendMeshShader );
	//glDepthMask( 0 );
	glDisable( GL_CULL_FACE );
	for ( unsigned int m = 0; m < allMeshes.size(); ++m ) {
		mesh::Mesh mesh = mesh::allMeshes[m];
		glActiveTexture( GL_TEXTURE0 );//amb
		glBindTexture( GL_TEXTURE_2D, mesh::allMaterialTextures[mesh.materialIndex].amb_tex );
		glActiveTexture( GL_TEXTURE1 );//diff
		glBindTexture( GL_TEXTURE_2D, mesh::allMaterialTextures[mesh.materialIndex].diff_tex );
		glActiveTexture( GL_TEXTURE2 );//spec
		glBindTexture( GL_TEXTURE_2D, mesh::allMaterialTextures[mesh.materialIndex].spec_tex );
		shader::setUniform( meshShader, "materialIndex", mesh.materialIndex );

		glDrawElementsInstancedBaseInstance( GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 
                ( void* )( mesh.indexOffset * sizeof( unsigned int ) ), 
                mesh.instanceCount, mesh.instanceOffset );
	}
	shader::unuse();
	glBindVertexArray( 0 );
	//glDepthMask( 1 );
	glEnable( GL_CULL_FACE );
}

void mesh::updateMeshBuffers()
{
	if ( allMeshInstancenode.size() ) {
		//gl::uploadStorage( nodeIndexBuffer, sizeof( unsigned int )*allMeshInstancenode.size(), 
                //&allMeshInstancenode[0] );
	}
}

void mesh::setupMeshShader()
{
	shader::bindUniformBufferToShader( meshShader, gl::generalUniformBuffer, "GeneralUniformBuffer" );
	shader::bindUniformBufferToShader( meshShader, entities::entityMatrixBuffer, "NodeMatrixBuffer" );
	shader::bindUniformBufferToShader( meshShader, materialUBO, "MaterialBuffer" );
}

void mesh::setupMeshNormalShader()
{
	shader::bindUniformBufferToShader( meshNormalShader, gl::generalUniformBuffer, "GeneralUniformBuffer" );
	shader::bindUniformBufferToShader( meshNormalShader, entities::entityMatrixBuffer, "NodeMatrixBuffer" );
}


void mesh::addInstancesToMesh( unsigned int pMeshIndex, std::vector<unsigned int> pNodeIDs )
{
	Mesh& msh = allMeshes[pMeshIndex];
	if ( msh.instanceCount == 0 ) {
		msh.instanceOffset = allMeshInstancenode.size();
	}
	msh.instanceCount += pNodeIDs.size();
	allMeshInstancenode.insert( allMeshInstancenode.begin() + msh.instanceOffset, 
            pNodeIDs.begin(), pNodeIDs.end() );
}

void mesh::revalidateMeshNodeOffsets()
{
	unsigned int offs = 0;
	for ( unsigned int msh = 0; msh < allMeshes.size(); ++msh ) {
		allMeshes[msh].instanceOffset = offs;
		offs += allMeshes[msh].instanceCount;
	}
}

unsigned int mesh::createMesh( unsigned int pIndexOffset, unsigned int pIndexCount, 
        unsigned int pVertexOffset, unsigned int pVertexCount, unsigned int pMaterialIndex )
{
	allMeshes.emplace_back( pIndexOffset, pIndexCount, pVertexOffset, pVertexCount, pMaterialIndex );
	return allMeshes.size() - 1;
}

unsigned int mesh::createMesh( std::vector<Vertex> pVertices, 
        std::vector<unsigned int> pIndices, unsigned int pMaterialIndex )
{
	unsigned int indexOffset = allIndices.size();
	unsigned int indexCount = pIndices.size();
	unsigned int vertexOffset = allStaticVertices.size();
	unsigned int msh = createMesh( indexOffset, indexCount, vertexOffset, pVertices.size(), pMaterialIndex );

	allIndices.resize( allIndices.size() + indexCount );
	for ( unsigned int i = 0; i < pIndices.size(); ++i ) {
		allIndices[indexOffset + i] = pIndices[i] + vertexOffset;
	}
	allStaticVertices.insert( allStaticVertices.end(), pVertices.begin(), pVertices.end() );
	return msh;
}

void mesh::toggleNormals()
{
	draw_normals = !draw_normals;
}
void mesh::toggleCullFace()
{
	if ( cull_face ) {
		glDisable( GL_CULL_FACE );
	}
	else {
		glEnable( GL_CULL_FACE );
	}
	cull_face = !cull_face;
}
