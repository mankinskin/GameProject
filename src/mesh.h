#pragma once
#include <vector>
#include <assimp/scene.h>
#include <glm.hpp>
#include "storage.h"
#include "vao.h"
#include "shader.h"


namespace mesh 
{
    struct Vertex 
    {
        Vertex() 
        {}
        Vertex(float pPosX, float pPosY, float pPosZ, 
                float pNormX = 0.0f, float pNormY = 1.0f, float pNormZ = 0.0f, 
                float pUVCoordS = 0.0f, float pUVCoordT = 0.0f)
            :pos(glm::vec3(pPosX, pPosY, pPosZ)), 
            normal(glm::vec3(pNormX, pNormY, pNormZ)), 
            uv(glm::vec2(pUVCoordS, pUVCoordT)) 
        {}
        Vertex(glm::vec3 pPos, 
                glm::vec3 pNormal = glm::vec3(0.0f, 1.0f, 0.0f), 
                glm::vec2 pUV = glm::vec2(0.0f, 0.0f))
            :pos(pPos), normal(pNormal), uv(pUV) 
        {}
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    struct Mesh 
    {
        Mesh(unsigned int pVertexOffset, unsigned int pVertexCount, 
                unsigned int pIndexOffset, unsigned int pIndexCount, unsigned int pMaterialIndex = 0)
            :vertexOffset(pVertexOffset), vertexCount(pVertexCount), 
            indexOffset(pIndexOffset), indexCount(pIndexCount), 
            materialIndex(pMaterialIndex), instanceOffset(0), instanceCount(0) 
        {}
        unsigned int vertexOffset = 0;
        unsigned int vertexCount = 0;
        unsigned int indexOffset = 0;
        unsigned int indexCount = 0;
        unsigned int materialIndex = 0;

        unsigned int instanceOffset = 0;
        unsigned int instanceCount = 0;
    };


    void initMeshVAO();
    void initMeshShader();
    void setupMeshShader();
    void initMeshNormalShader();
    void setupMeshNormalShader();
    void initBlendMeshShader();
    void setupBlendMeshShader();
    unsigned int createMesh(unsigned int pIndexOffset, unsigned int pIndexCount, unsigned int pVertexOffset, 
            unsigned int pVertexCount, unsigned int pMaterialIndex);
    unsigned int createMesh(std::vector<Vertex> pVertices, std::vector<unsigned int> pIndices, 
            unsigned int pMaterialIndex);
    void addInstancesToMesh(unsigned int pMeshIndex, std::vector<unsigned int> pNodeIDs);

    void revalidateMeshNodeOffsets();
    void updateMeshBuffers();

    void renderMeshes();
    void renderMeshNormals();
    void renderBlendMeshes();

    void toggleNormals();
    void toggleCullFace();

    extern std::vector<unsigned int> opaqueMeshList;
    extern std::vector<unsigned int> blendMeshList;

    extern std::vector<unsigned int> allIndices;
    extern std::vector<Vertex> allStaticVertices;
    extern std::vector<Mesh> allMeshes;
    extern std::vector<unsigned int> allMeshInstancenode;
    extern bool draw_normals;
    extern bool cull_face;
    extern shader::Program meshShader;
    extern shader::Program blendMeshShader;
    extern shader::Program meshNormalShader;
    extern gl::VAO meshVAO;
    extern gl::Storage<Vertex> meshVBO;
    extern gl::Storage<unsigned int> meshIBO;
    extern gl::StreamStorage<unsigned int> nodeIndexBuffer;
}
