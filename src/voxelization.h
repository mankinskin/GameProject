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
	template<unsigned int LevelCount>
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
	extern unsigned int volumeImage;
	extern unsigned int backVolumeImage;//Back buffer for volume rendering
	extern unsigned int voxelizationShader;
}
