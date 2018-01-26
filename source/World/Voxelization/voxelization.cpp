#include "../../Global/stdafx.h"
#include "voxelization.h"
#include "../../Graphics/BaseGL/Shader/Shader.h"
#include "../../Graphics/BaseGL/texture.h"
#include "../../Graphics/Mesh/Mesh.h"
#include "../../Graphics/Mesh/Material.h"
#include "../../Graphics/Model/Node.h"
glm::uvec3 voxelization::frustum_size = glm::ivec3(200, 200, 200);
size_t voxelization::voxelizationShader = 0;
size_t voxelization::volumeImage = 0;

void voxelization::init() {
	voxelizationShader = shader::newProgram("voxelizationShader", shader::createModule("voxelizationShader.vert"), shader::createModule("voxelizationShader.frag"));
	shader::addVertexAttribute(voxelizationShader, "pos", 0);
	shader::addVertexAttribute(voxelizationShader, "normal", 1);
	shader::addVertexAttribute(voxelizationShader, "transform", 3);


	//glm::vec4 volume[sx * sy * sz];
	//std::fill(&volume[0], &volume[sx * sy * sz], glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	glGenTextures(1, &volumeImage);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, volumeImage);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F, frustum_size.x, frustum_size.y, frustum_size.z, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTextureParameteri(volumeImage, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(volumeImage, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(volumeImage, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTextureParameteri(volumeImage, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glGenerateMipmap(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, 0);
}

void voxelization::voxelizeMeshes()
{
	float zoom = 1.0f;
	glViewport(0, 0, frustum_size.x*zoom, frustum_size.y*zoom);

	shader::use(voxelizationShader);
	glBindVertexArray(mesh::meshVAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, volumeImage);
	glBindImageTexture(0, volumeImage, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	for (size_t m = 0; m < mesh::allMeshes.size(); ++m) {
		mesh::Mesh& mesh = mesh::allMeshes[m];
		glDrawElementsInstancedBaseInstance(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, (void*)(mesh.indexOffset * sizeof(size_t)), mesh.instanceCount, mesh.instanceOffset);
	}
	
	glBindImageTexture(0, 0, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	glBindTexture(GL_TEXTURE_3D, 0);
	glBindVertexArray(0);
	shader::unuse();

	glViewport(0, 0, gl::screenWidth, gl::screenHeight);
}

void voxelization::setupShader()
{
	shader::bindUniformBufferToShader(voxelizationShader, gl::generalUniformBuffer, "GeneralUniformBuffer");
	shader::bindUniformBufferToShader(voxelizationShader, node::nodeMatrixBuffer, "NodeMatrixBuffer");
}

void voxelization::clearVolumeTexture()
{
	glBindTexture(GL_TEXTURE_3D, volumeImage);
	glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, frustum_size.x, frustum_size.y, frustum_size.z, GL_RGBA, GL_FLOAT, nullptr);
	glBindTexture(GL_TEXTURE_3D, 0);
}
