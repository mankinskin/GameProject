#include "mesh.h"
#include "gldebug.h"
#include "model.h"
#include "shader.h"
#include "entity.h"
#include "vao.h"
#include "light.h"
#include "material.h"
#include "texture.h"
#include "framebuffer.h"


bool model::mesh::draw_normals = false;
bool model::mesh::cull_face = true;
std::vector<model::mesh::Mesh> model::mesh::allMeshes;
std::vector<unsigned int> model::mesh::allIndices;
std::vector<model::mesh::Vertex> model::mesh::allStaticVertices;
std::vector<unsigned int> model::mesh::allMeshInstancenode;
std::vector<unsigned int> model::mesh::opaqueMeshList;
std::vector<unsigned int> model::mesh::blendMeshList;
shader::Program model::mesh::meshShader;
shader::Program model::mesh::blendMeshShader;
shader::Program model::mesh::meshNormalShader;
gl::VAO model::mesh::meshVAO;
gl::Storage<model::mesh::Vertex> model::mesh::meshVBO;
gl::Storage<unsigned int> model::mesh::meshIBO;
gl::StreamStorage<unsigned int> model::mesh::nodeIndexBuffer;

void model::mesh::initMeshVAO()
{
  meshVBO = gl::Storage<Vertex>("MeshVertexBuffer",
	  allStaticVertices.size(), 0, &allStaticVertices[0]);
  meshIBO = gl::Storage<unsigned int>("MeshIndexBuffer",
	  allIndices.size(), 0, &allIndices[0]);
  nodeIndexBuffer = gl::StreamStorage<unsigned int>("MeshNodeIndexBuffer",
	  model::MAX_MODELS*model::MAX_MESHES_PER_MODEL, GL_MAP_WRITE_BIT);
  glBindVertexArray(meshVAO);
  glVertexArrayElementBuffer(meshVAO, meshIBO.ID);
  glVertexArrayVertexBuffer(meshVAO, 0, meshVBO.ID, 0, sizeof(Vertex));
  glVertexArrayVertexBuffer(meshVAO, 1, nodeIndexBuffer.ID, 0, sizeof(Vertex));
  //gl::setVertexArrayVertexStorage(meshVAO, 1, nodeIndexBuffer, sizeof(unsigned int));

  //gl::setVertexAttrib(meshVAO, 0, 0, 3, GL_FLOAT, offsetof(Vertex, pos));
  //gl::setVertexAttrib(meshVAO, 0, 1, 3, GL_FLOAT, offsetof(Vertex, normal));
  //gl::setVertexAttrib(meshVAO, 0, 2, 2, GL_FLOAT, offsetof(Vertex, uv));

  //gl::setVertexAttrib(meshVAO, 1, 3, 1, GL_UNSIGNED_INT, 0);
  glVertexArrayBindingDivisor(meshVAO, 1, 1);
  glBindVertexArray(0);
}

void model::mesh::initMeshShader()
{
  meshShader = shader::Program("meshShader", shader::Stage("meshShader.vert"),
	  shader::Stage("meshShader.frag"));
  meshShader.addVertexAttribute("pos", 0);
  meshShader.addVertexAttribute("normal", 1);
  meshShader.addVertexAttribute("uv", 2);
  meshShader.addVertexAttribute("transform", 3);
}

void model::mesh::initMeshNormalShader()
{
  meshNormalShader = shader::Program("meshNormalShader", shader::Stage("meshNormalShader.vert"),
	  shader::Stage("meshNormalShader.geo"), shader::Stage("meshNormalShader.frag"));
  meshNormalShader.addVertexAttribute("pos", 0);
  meshNormalShader.addVertexAttribute("normal", 1);
  meshNormalShader.addVertexAttribute("transform", 3);
}

void model::mesh::initBlendMeshShader()
{
  blendMeshShader = shader::Program("blendMeshShader", shader::Stage("blendMeshShader.vert"),
	  shader::Stage("blendMeshShader.frag"));
  blendMeshShader.addVertexAttribute("pos", 0);
  blendMeshShader.addVertexAttribute("normal", 1);
  blendMeshShader.addVertexAttribute("uv", 2);
  blendMeshShader.addVertexAttribute("transform", 3);
}

void model::mesh::setupMeshShader()
{
  meshShader.build();
  meshShader.bindUniformBuffer(gl::generalUniformBuffer, "GeneralUniformBuffer");
  meshShader.bindUniformBuffer(entities::entityMatrixBuffer, "NodeMatrixBuffer");
  meshShader.bindUniformBuffer(materialUBO, "MaterialBuffer");
}

void model::mesh::setupMeshNormalShader()
{
  meshNormalShader.build();
  meshNormalShader.bindUniformBuffer(gl::generalUniformBuffer, "GeneralUniformBuffer");
  meshNormalShader.bindUniformBuffer(entities::entityMatrixBuffer, "NodeMatrixBuffer");
}

void model::mesh::setupBlendMeshShader()
{
  blendMeshShader.bindUniformBuffer(materialUBO, "MaterialBuffer");
  blendMeshShader.bindUniformBuffer(lights::lightDataUBO, "LightDataBuffer");
  blendMeshShader.bindUniformBuffer(gl::generalUniformBuffer, "GeneralUniformBuffer");
  blendMeshShader.bindUniformBuffer(entities::entityMatrixBuffer, "NodeMatrixBuffer");
}

void model::mesh::renderMeshes()
{
  glBindVertexArray(meshVAO);
  meshShader.use();

  for (unsigned int m = 0; m < allMeshes.size(); ++m) {
	model::mesh::Mesh& mesh = model::mesh::allMeshes[m];
	glActiveTexture(GL_TEXTURE0);//amb
	glBindTexture(GL_TEXTURE_2D, MaterialTextures::all[mesh.materialIndex].amb_tex);
	glActiveTexture(GL_TEXTURE1);//diff
	glBindTexture(GL_TEXTURE_2D, MaterialTextures::all[mesh.materialIndex].diff_tex);
	glActiveTexture(GL_TEXTURE2);//spec
	glBindTexture(GL_TEXTURE_2D, MaterialTextures::all[mesh.materialIndex].spec_tex);
	meshShader.setUniform("materialIndex", mesh.materialIndex);

	glDrawElementsInstancedBaseInstance(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT,
		(void*)(mesh.indexOffset * sizeof(unsigned int)),
		mesh.instanceCount, mesh.instanceOffset);
  }
  shader::Program::unuse();
  glBindVertexArray(0);
}

void model::mesh::renderMeshNormals()
{
  if (draw_normals) {
	updateMeshBuffers();	//TODO: make work without this
	glBindVertexArray(meshVAO);
	meshNormalShader.use();
	for (unsigned int m = 0; m < allMeshes.size(); ++m) {
	  Mesh& mesh = allMeshes[m];
	  glDrawElementsInstancedBaseInstance(GL_POINTS, mesh.indexCount, GL_UNSIGNED_INT,
		  (void*)(mesh.indexOffset * sizeof(unsigned int)),
		  mesh.instanceCount, mesh.instanceOffset);
	}
	shader::Program::unuse();
	glBindVertexArray(0);
  }
}

void model::mesh::renderBlendMeshes()
{
  glBindVertexArray(meshVAO);
  blendMeshShader.use();
  //glDepthMask(0);
  glDisable(GL_CULL_FACE);
  for (unsigned int m = 0; m < allMeshes.size(); ++m) {
	Mesh mesh = allMeshes[m];
	glActiveTexture(GL_TEXTURE0);//amb
	glBindTexture(GL_TEXTURE_2D, MaterialTextures::all[mesh.materialIndex].amb_tex);
	glActiveTexture(GL_TEXTURE1);//diff
	glBindTexture(GL_TEXTURE_2D, MaterialTextures::all[mesh.materialIndex].diff_tex);
	glActiveTexture(GL_TEXTURE2);//spec
	glBindTexture(GL_TEXTURE_2D, model::mesh::MaterialTextures::all[mesh.materialIndex].spec_tex);
	meshShader.setUniform("materialIndex", mesh.materialIndex);

	glDrawElementsInstancedBaseInstance(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT,
		(void*)(mesh.indexOffset * sizeof(unsigned int)),
		mesh.instanceCount, mesh.instanceOffset);
  }
  shader::Program::unuse();
  glBindVertexArray(0);
  //glDepthMask(1);
  glEnable(GL_CULL_FACE);
}

void model::mesh::updateMeshBuffers()
{
  if (allMeshInstancenode.size()) {
	//gl::uploadStorage(nodeIndexBuffer, sizeof(unsigned int)*allMeshInstancenode.size(),
	//&allMeshInstancenode[0]);
  }
}

void model::mesh::addInstancesToMesh(unsigned int pMeshIndex, std::vector<unsigned int> pNodeIDs)
{
  Mesh& msh = allMeshes[pMeshIndex];
  if (msh.instanceCount == 0) {
	msh.instanceOffset = allMeshInstancenode.size();
  }
  msh.instanceCount += pNodeIDs.size();
  allMeshInstancenode.insert(allMeshInstancenode.begin() + msh.instanceOffset,
	  pNodeIDs.begin(), pNodeIDs.end());
}

void model::mesh::revalidateMeshNodeOffsets()
{
  unsigned int offs = 0;
  for (unsigned int msh = 0; msh < allMeshes.size(); ++msh) {
	allMeshes[msh].instanceOffset = offs;
	offs += allMeshes[msh].instanceCount;
  }
}

unsigned int model::mesh::createMesh(unsigned int pIndexOffset, unsigned int pIndexCount,
	unsigned int pVertexOffset, unsigned int pVertexCount, unsigned int pMaterialIndex)
{
  allMeshes.emplace_back(pIndexOffset, pIndexCount, pVertexOffset, pVertexCount, pMaterialIndex);
  return allMeshes.size() - 1;
}

unsigned int model::mesh::createMesh(std::vector<Vertex> pVertices,
	std::vector<unsigned int> pIndices, unsigned int pMaterialIndex)
{
  unsigned int indexOffset = allIndices.size();
  unsigned int indexCount = pIndices.size();
  unsigned int vertexOffset = allStaticVertices.size();
  unsigned int msh = createMesh(indexOffset, indexCount, vertexOffset, pVertices.size(), pMaterialIndex);

  allIndices.resize(allIndices.size() + indexCount);
  for (unsigned int i = 0; i < pIndices.size(); ++i) {
	allIndices[indexOffset + i] = pIndices[i] + vertexOffset;
  }
  allStaticVertices.insert(allStaticVertices.end(), pVertices.begin(), pVertices.end());
  return msh;
}

void model::mesh::toggleNormals()
{
  draw_normals = !draw_normals;
}
void model::mesh::toggleCullFace()
{
  if (cull_face) {
	glDisable(GL_CULL_FACE);
  }
  else {
	glEnable(GL_CULL_FACE);
  }
  cull_face = !cull_face;
}

