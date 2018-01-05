#include "../Global/stdafx.h"
#include "stdafx.h"
#include "Mesh.h"
#include "../GlobalGL/glDebug.h"
#include "../Model/Model.h"
#include "../BaseGL/shader/shader.h"
#include "../Model/Node.h"
#include "../BaseGL/VAO.h"
#include "../Lighting/Lights.h"
#include "Material.h"

size_t mesh::meshShader = 0;
size_t mesh::blendMeshShader = 0;
size_t mesh::meshNormalShader = 0;
bool mesh::draw_normals = false;
bool mesh::cull_face = true;
std::vector<mesh::Mesh> mesh::allMeshes;
std::vector<size_t> mesh::allIndices;
std::vector<mesh::Vertex> mesh::allStaticVertices;
std::vector<size_t> mesh::allMeshInstancenode;
std::vector<size_t> mesh::opaqueMeshList;
std::vector<size_t> mesh::blendMeshList;
size_t mesh::meshVAO = 0;
size_t mesh::meshVBO = 0;
size_t mesh::meshIBO = 0;

size_t mesh::nodeIndexBuffer = 0;
size_t mesh::normalShaderProgram = 0;

void mesh::initMeshVAO()
{
	glCreateVertexArrays(1, &meshVAO);
	meshVBO = vao::createStorage(sizeof(Vertex)*mesh::allStaticVertices.size(), &mesh::allStaticVertices[0], 0);
	meshIBO = vao::createStorage(sizeof(size_t)*mesh::allIndices.size(), &mesh::allIndices[0], 0);
	nodeIndexBuffer = vao::createStorage(sizeof(size_t)*node::MAX_NODES * model::MAX_MODELS, nullptr, GL_MAP_WRITE_BIT | vao::MAP_PERSISTENT_FLAGS);
	vao::createStream(nodeIndexBuffer, GL_MAP_WRITE_BIT);
	glBindVertexArray(meshVAO);
	glVertexArrayElementBuffer(meshVAO, meshIBO + 1);
	glVertexArrayVertexBuffer(meshVAO, 0, meshVBO + 1, 0, sizeof(Vertex));
	glVertexArrayVertexBuffer(meshVAO, 1, nodeIndexBuffer + 1, 0, sizeof(Vertex));
	vao::setVertexArrayVertexStorage(meshVAO, 1, nodeIndexBuffer, sizeof(size_t));

	vao::setVertexAttrib(meshVAO, 0, 0, 3, GL_FLOAT, offsetof(Vertex, pos));
	vao::setVertexAttrib(meshVAO, 0, 1, 3, GL_FLOAT, offsetof(Vertex, normal));
	vao::setVertexAttrib(meshVAO, 0, 2, 2, GL_FLOAT, offsetof(Vertex, uv));

	vao::setVertexAttrib(meshVAO, 1, 3, 1, GL_UNSIGNED_INT, 0);
	glVertexArrayBindingDivisor(meshVAO, 1, 1);
	glBindVertexArray(0);
}

void mesh::initMeshShader()
{
	meshShader = shader::newProgram("meshShader", shader::createModule("meshShader.vert"), shader::createModule("meshShader.frag"));
	shader::addVertexAttribute(meshShader, "pos", 0);
	shader::addVertexAttribute(meshShader, "normal", 1);
	shader::addVertexAttribute(meshShader, "uv", 2);
	shader::addVertexAttribute(meshShader, "transform", 3);
}

void mesh::initMeshNormalShader()
{
	meshNormalShader = shader::newProgram("meshNormalShader", shader::createModule("meshNormalShader.vert"), shader::createModule("meshNormalShader.geo"), shader::createModule("meshNormalShader.frag"));
	shader::addVertexAttribute(meshNormalShader, "pos", 0);
	shader::addVertexAttribute(meshNormalShader, "normal", 1);
	shader::addVertexAttribute(meshNormalShader, "transform", 3);
}

void mesh::initBlendMeshShader()
{
	blendMeshShader = shader::newProgram("blendMeshShader", shader::createModule("blendMeshShader.vert"), shader::createModule("blendMeshShader.frag"));
	shader::addVertexAttribute(blendMeshShader, "pos", 0);
	shader::addVertexAttribute(blendMeshShader, "normal", 1);
	shader::addVertexAttribute(blendMeshShader, "uv", 2);
	shader::addVertexAttribute(blendMeshShader, "transform", 3);
}

void mesh::setupBlendMeshShader()
{
	shader::bindUniformBufferToShader(blendMeshShader, materialUBO, "MaterialBuffer");
	shader::bindUniformBufferToShader(blendMeshShader, lighting::lightDataUBO, "LightDataBuffer");
	shader::bindUniformBufferToShader(blendMeshShader, gl::generalUniformBuffer, "GeneralUniformBuffer");
	shader::bindUniformBufferToShader(blendMeshShader, node::nodeMatrixBuffer, "NodeMatrixBuffer");
}

void mesh::renderMeshes()
{
	glBindVertexArray(meshVAO);
	shader::use(meshShader);

	for (size_t m = 0; m < allMeshes.size(); ++m) {
		mesh::Mesh& mesh = mesh::allMeshes[m];
		glActiveTexture(GL_TEXTURE0);//amb
		glBindTexture(GL_TEXTURE_2D, mesh::allMaterialTextures[mesh.materialIndex].amb_tex);
		glActiveTexture(GL_TEXTURE1);//diff
		glBindTexture(GL_TEXTURE_2D, mesh::allMaterialTextures[mesh.materialIndex].diff_tex);
		glActiveTexture(GL_TEXTURE2);//spec
		glBindTexture(GL_TEXTURE_2D, mesh::allMaterialTextures[mesh.materialIndex].spec_tex);
		shader::setUniform(meshShader, "materialIndex", mesh.materialIndex);

		glDrawElementsInstancedBaseInstance(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, (void*)(mesh.indexOffset * sizeof(size_t)), mesh.instanceCount, mesh.instanceOffset);
	}
	shader::unuse();
	glBindVertexArray(0);

	glDebug::getGLError("render(Meshes)");
}

void mesh::renderMeshNormals()
{
	if (draw_normals) {
		mesh::updateMeshBuffers();//TODO: make work without this
		glBindVertexArray(meshVAO);
		shader::use(meshNormalShader);
		glDebug::getGLError("render(MeshNormals)1");
		for (size_t m = 0; m < allMeshes.size(); ++m) {
			mesh::Mesh& mesh = mesh::allMeshes[m];
			glDrawElementsInstancedBaseInstance(GL_POINTS, mesh.indexCount, GL_UNSIGNED_INT, (void*)(mesh.indexOffset *sizeof(size_t)), mesh.instanceCount, mesh.instanceOffset);
		}
		glDebug::getGLError("render(MeshNormals)2");
		shader::unuse();
		glBindVertexArray(0);
	}
	glDebug::getGLError("render(MeshNormals)");
}

void mesh::renderBlendMeshes()
{
	glBindVertexArray(meshVAO);
	shader::use(blendMeshShader);
	//glDepthMask(0);
	glDisable(GL_CULL_FACE);
	for (size_t m = 0; m < allMeshes.size(); ++m) {
		mesh::Mesh& mesh = mesh::allMeshes[m];
		glActiveTexture(GL_TEXTURE0);//amb
		glBindTexture(GL_TEXTURE_2D, mesh::allMaterialTextures[mesh.materialIndex].amb_tex);
		glActiveTexture(GL_TEXTURE1);//diff
		glBindTexture(GL_TEXTURE_2D, mesh::allMaterialTextures[mesh.materialIndex].diff_tex);
		glActiveTexture(GL_TEXTURE2);//spec
		glBindTexture(GL_TEXTURE_2D, mesh::allMaterialTextures[mesh.materialIndex].spec_tex);
		shader::setUniform(meshShader, "materialIndex", (size_t&)mesh.materialIndex);

		glDrawElementsInstancedBaseInstance(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, (void*)(mesh.indexOffset * sizeof(size_t)), mesh.instanceCount, mesh.instanceOffset);
	}
	shader::unuse();
	glBindVertexArray(0);
	//glDepthMask(1);
	glEnable(GL_CULL_FACE);
	glDebug::getGLError("render(Meshes)");
}

void mesh::updateMeshBuffers()
{
	if (allMeshInstancenode.size()) {
		vao::uploadStorage(nodeIndexBuffer, sizeof(size_t)*allMeshInstancenode.size(), &allMeshInstancenode[0]);
	}
}

void mesh::setupMeshShader()
{
	shader::bindUniformBufferToShader(meshShader, gl::generalUniformBuffer, "GeneralUniformBuffer");
	shader::bindUniformBufferToShader(meshShader, node::nodeMatrixBuffer, "NodeMatrixBuffer");
	shader::bindUniformBufferToShader(meshShader, materialUBO, "MaterialBuffer");
}

void mesh::setupMeshNormalShader()
{
	shader::bindUniformBufferToShader(meshNormalShader, gl::generalUniformBuffer, "GeneralUniformBuffer");
	shader::bindUniformBufferToShader(meshNormalShader, node::nodeMatrixBuffer, "NodeMatrixBuffer");
}


void mesh::addInstancesToMesh(size_t pMeshIndex, std::vector<size_t> pNodeIDs)
{
	Mesh& msh = allMeshes[pMeshIndex];
	if (msh.instanceCount == 0) {
		msh.instanceOffset = allMeshInstancenode.size();
	}
	msh.instanceCount += pNodeIDs.size();
	allMeshInstancenode.insert(allMeshInstancenode.begin() + msh.instanceOffset, pNodeIDs.begin(), pNodeIDs.end());
}

void mesh::revalidateMeshNodeOffsets()
{
	size_t offs = 0;
	for (size_t msh = 0; msh < allMeshes.size(); ++msh) {
		allMeshes[msh].instanceOffset = offs;
		offs += allMeshes[msh].instanceCount;
	}
}

size_t mesh::createMesh(size_t pIndexOffset, size_t pIndexCount, size_t pVertexOffset, size_t pVertexCount, size_t pMaterialIndex)
{
	allMeshes.emplace_back(pIndexOffset, pIndexCount, pVertexOffset, pVertexCount, pMaterialIndex);
	return allMeshes.size() - 1;
}

size_t mesh::createMesh(std::vector<Vertex> pVertices, std::vector<size_t> pIndices, size_t pMaterialIndex)
{
	size_t indexOffset = allIndices.size();
	size_t indexCount = pIndices.size();
	size_t vertexOffset = allStaticVertices.size();
	size_t ret_ = createMesh(indexOffset, indexCount, vertexOffset, pVertices.size(), pMaterialIndex);

	allIndices.resize(allIndices.size() + indexCount);
	for (size_t i = 0; i < pIndices.size(); ++i) {
		allIndices[indexOffset + i] = pIndices[i] + vertexOffset;
	}
	allStaticVertices.insert(allStaticVertices.end(), pVertices.begin(), pVertices.end());
	return ret_;
}

void mesh::toggleNormals()
{
	draw_normals = !draw_normals;
}
void mesh::toggleCullFace()
{
	if (cull_face) {
		glDisable(GL_CULL_FACE);
	}
	else {
		glEnable(GL_CULL_FACE);
	}
	cull_face = !cull_face;
}


