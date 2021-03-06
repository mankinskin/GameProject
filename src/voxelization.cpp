#include "voxelization.h"
#include "shader.h"
#include "texture.h"
#include "mesh.h"
#include "material.h"
#include "nodes.h"
#include <gtc/matrix_transform.hpp>
#include "viewport.h"

glm::uvec3 voxelization::frustum_size = glm::ivec3(200, 200, 200);
static shader::Program voxelizationShader;
unsigned int voxelization::volumeImage = 0;
unsigned int voxelization::backVolumeImage = 0;
glm::mat4 voxelization::projectionMatrix;
glm::vec4* volume = new glm::vec4[200*200*200];

void voxelization::init()
{
  voxelizationShader = shader::Program("voxelizationShader", shader::Stage("voxelizationShader.vert"), shader::Stage("voxelizationShader.frag"));
  voxelizationShader.addVertexAttribute("pos", 0);
  voxelizationShader.addVertexAttribute("normal", 1);
  voxelizationShader.addVertexAttribute("transform", 3);

  projectionMatrix = glm::perspective(70.0, 1.0, 1.0, 200.0);

  std::fill(&volume[0], &volume[frustum_size.x * frustum_size.y * frustum_size.z], glm::vec4(0.0f, 1.0f, 0.0f, 0.01f));

  glGenTextures(1, &volumeImage);
  glGenTextures(1, &backVolumeImage);
  glActiveTexture(GL_TEXTURE0);

  glBindTexture(GL_TEXTURE_3D, volumeImage);
  glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F, frustum_size.x, frustum_size.y, frustum_size.z, 0, GL_RGBA, GL_FLOAT, &volume[0]);
  glTextureParameteri(volumeImage, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTextureParameteri(volumeImage, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTextureParameteri(volumeImage, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTextureParameteri(volumeImage, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glGenerateMipmap(GL_TEXTURE_3D);

  glBindTexture(GL_TEXTURE_3D, backVolumeImage);
  glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F, frustum_size.x, frustum_size.y, frustum_size.z, 0, GL_RGBA, GL_FLOAT, &volume[0]);
  glTextureParameteri(backVolumeImage, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTextureParameteri(backVolumeImage, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTextureParameteri(backVolumeImage, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTextureParameteri(backVolumeImage, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glGenerateMipmap(GL_TEXTURE_3D);
  glBindTexture(GL_TEXTURE_3D, 0);

  //delete[] volume;
}

void voxelization::voxelizeMeshes()
{
  float fitx = 1.0f;
  float fity = 1.0f;
  glViewport(0, 0, frustum_size.x*fitx, frustum_size.y*fity);
  //glViewport(0, 0, gl::Viewport::current->width, gl::Viewport::current->height);
  voxelizationShader.use();
  glBindVertexArray(model::mesh::meshVAO);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_3D, volumeImage);
  glBindImageTexture(0, volumeImage, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
  for (unsigned int m = 0; m < model::mesh::allMeshes.size(); ++m) {
	model::mesh::Mesh& mesh = model::mesh::allMeshes[m];
	glDrawElementsInstancedBaseInstance(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, (void*)(mesh.indexOffset * sizeof(unsigned int)), mesh.instanceCount, mesh.instanceOffset);
  }

  glBindImageTexture(0, 0, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
  glBindTexture(GL_TEXTURE_3D, 0);
  glBindVertexArray(0);
  shader::Program::unuse();
  //swapVolumeBuffers();
  glViewport(0, 0, gl::Viewport::current->width, gl::Viewport::current->height);
}

void voxelization::setupShader()
{
  voxelizationShader.bindUniformBuffer(gl::generalUniformBuffer, "GeneralUniformBuffer");
  voxelizationShader.bindUniformBuffer(nodes::nodeBuffer, "NodeMatrixBuffer");
}

void voxelization::clearVolumeTexture()
{
  glBindTexture(GL_TEXTURE_3D, volumeImage);
  glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, frustum_size.x, frustum_size.y, frustum_size.z, GL_RGBA, GL_FLOAT, &volume[0]);
  glBindTexture(GL_TEXTURE_3D, 0);
}

void voxelization::swapVolumeBuffers()
{
  unsigned int old_front = volumeImage;
  volumeImage = backVolumeImage;
  backVolumeImage = old_front;
}

