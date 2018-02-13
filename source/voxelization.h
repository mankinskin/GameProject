#pragma once
#include <vector>
#include <glm\glm.hpp>

namespace voxelization {


	//voxel octree
	//averages the data of the mesh at an arbitary position
	struct TreeNode 
	{
		glm::vec3 normal;
	};
	template<size_t LevelCount>
	struct Octree
	{
		std::vector<TreeNode> nodes[LevelCount];
	};
	void init();
	void voxelizeMeshes();
	void setupShader();
	void clearVolumeTexture();
	void swapVolumeBuffers();

	extern glm::uvec3 frustum_size;
	extern glm::mat4 projectionMatrix;
	extern size_t volumeImage;
	extern size_t backVolumeImage;//Back buffer for volume rendering
	extern size_t voxelizationShader;
}