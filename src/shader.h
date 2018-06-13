#pragma once
#include "gl.h"
#include "storage.h"
#include <GL/glew.h>
#include <string>
#include <unordered_map>
#include <glm.hpp>
#include <typeinfo>
#include <gtc/type_ptr.hpp>
#include <array>

namespace shader 
{
    extern std::string SHADER_DIR;
    void setShaderDirectory( std::string& pDirectory );
    void resetShaderDirectory();

    struct Stage
    {
        Stage( std::string pFilename );
        Stage()
            :type( 0 )
        {}
        GLenum type;
        GLuint ID;
        std::string filename;
        std::string content;
        void compile();
    };


    struct Program 
    {
        Program()
            : ID( 0 )
              , stageCount( 0 )
        {}

        Program( std::string pName, const Stage& vert, const Stage& frag )
            : ID( glCreateProgram() )
              , name( pName )
              , stageCount( 0 )
        {
            addStage( vert );
            addStage( frag );
        }
        Program( std::string pName, const Stage& vert, const Stage& geo, const Stage& frag )
            : ID( glCreateProgram() )
              , name( pName )
              , stageCount( 0 )
        {
            addStage( vert );
            addStage( geo );
            addStage( frag );
        }
        Program( std::string pName, const Stage& comp )
            : ID( glCreateProgram() )
              , name( pName )
              , stageCount( 0 )
        {
            addStage( comp );
        }

        Program& operator=( const Program& other )
        {
            ID = other.ID;
            name = other.name;
            stages = other.stages;
            stageCount = other.stageCount;
            return *this;
        }
        static const unsigned int MAX_POSSIBLE_STAGES = 3;
        GLuint ID;
        std::string name;
        std::array<Stage, MAX_POSSIBLE_STAGES> stages;
        unsigned int stageCount;

        void addStage( const Stage& );
        void link();
        void build();
        void use() const;
        static void unuse();
        void addVertexAttribute( std::string, unsigned int );
        void setUniform( std::string pUniformName, int pValue );
        void setUniform( std::string pUniformName, unsigned int pValue );
        void setUniform( std::string pUniformName, float pValue );
        void setUniform( std::string pUniformName, glm::vec3 pValue );
        void setUniform( std::string pUniformName, glm::vec4 pValue ); 
        void setUniform( std::string pUniformName, glm::uvec4 pValue );
        void setUniform( std::string pUniformName, glm::uvec3 pValue );
        void setUniform( std::string pUniformName, glm::ivec4 pValue );
        void setUniform( std::string pUniformName, glm::ivec3 pValue );
        void setUniform( std::string pUniformName, glm::mat4 pValue, bool pTranspose );
        void setUniform( std::string pUniformName, glm::mat3 pValue, bool pTranspose );
        template<typename T>
            void bindUniformBuffer( const gl::Storage<T>& pStorage, std::string pBlockName )
            {
                int blockIndex = glGetUniformBlockIndex( ID, pBlockName.c_str() );
                if ( blockIndex < 0 ) {
                    debug::pushError( "invalid uniform block name " + pBlockName + "!" );
                    return;
                }
                glUniformBlockBinding( ID, blockIndex, pStorage.binding );
            }
    };
}

