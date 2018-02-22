#pragma once
#include <vector>
#include <assimp\scene.h>
#include <glm\glm.hpp>
namespace mesh {



	struct Vertex {
		Vertex() {}
		Vertex(float pPosX, float pPosY, float pPosZ, float pNormX = 0.0f, float pNormY = 1.0f, float pNormZ = 0.0f, float pUVCoordS = 0.0f, float pUVCoordT = 0.0f)
			:pos(glm::vec3(pPosX, pPosY, pPosZ)), normal(glm::vec3(pNormX, pNormY, pNormZ)), uv(glm::vec2(pUVCoordS, pUVCoordT)) {}
		Vertex(glm::vec3 pPos, glm::vec3 pNormal = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2 pUV = glm::vec2(0.0f, 0.0f))
			:pos(pPos), normal(pNormal), uv(pUV) {}
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 uv;
	};
	struct Mesh {
		Mesh(size_t pVertexOffset, size_t pVertexCount, size_t pIndexOffset, size_t pIndexCount, size_t pMaterialIndex = 0)
			:vertexOffset(pVertexOffset), vertexCount(pVertexCount), indexOffset(pIndexOffset), indexCount(pIndexCount), materialIndex(pMaterialIndex), instanceOffset(0), instanceCount(0) {}
		size_t vertexOffset = 0;
		size_t vertexCount = 0;
		size_t indexOffset = 0;
		size_t indexCount = 0;
		size_t materialIndex = 0;

		size_t instanceOffset = 0;
		size_t instanceCount = 0;
	};


	void initMeshVAO();
	void initMeshShader();
	void setupMeshShader();
	void initMeshNormalShader();
	void setupMeshNormalShader();
	void initBlendMeshShader();
	void setupBlendMeshShader();
	size_t createMesh(size_t pIndexOffset, size_t pIndexCount, size_t pVertexOffset, size_t pVertexCount, size_t pMaterialIndex);
	size_t createMesh(std::vector<Vertex> pVertices, std::vector<size_t> pIndices, size_t pMaterialIndex);
	void addInstancesToMesh(size_t pMeshIndex, std::vector<size_t> pNodeIDs);

	void revalidateMeshNodeOffsets();
	void updateMeshBuffers();

	void renderMeshes();
	void renderMeshNormals();
	void renderBlendMeshes();

	void toggleNormals();
	void toggleCullFace();

	

	extern std::vector<size_t> opaqueMeshList;
	extern std::vector<size_t> blendMeshList;

	extern std::vector<size_t> allIndices;
	extern std::vector<Vertex> allStaticVertices;
	extern std::vector<Mesh> allMeshes;
	extern std::vector<size_t> allMeshInstancenode;
	extern bool draw_normals;
	extern bool cull_face;
	extern size_t meshShader;
	extern size_t blendMeshShader;
	extern size_t meshNormalShader;
	extern size_t meshVAO;
	extern size_t meshVBO;
	extern size_t meshIBO;
	extern size_t nodeIndexBuffer;
	extern size_t normalShaderProgram;
}