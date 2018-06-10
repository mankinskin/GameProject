#include "shader.h"
#include "gl.h"
#include "gldebug.h"
#include "debug.h"
#include "camera.h"
#include "mesh.h"
#include "light.h"
#include "quadcolors.h"
#include "line.h"
#include "shaderfile.h"
#include "quad.h"
#include "voxelization.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <gtc/type_ptr.hpp>

unsigned int shader::currentShaderProgram;

std::vector<shader::Program> shader::allPrograms;
std::unordered_map<std::string, unsigned int> shader::programLookup;
std::vector<shader::Shader> shader::allShaders;
std::unordered_map<std::string, unsigned int> shader::shaderLookup;


void shader::Loader::buildShaderPrograms()
{
	compileAndLink();
}

std::string extractStageString( std::string filename ) 
{
    unsigned int begin = filename.find_first_of('.') + 1;
    unsigned int end = filename.find_last_of( '.' );
	return filename.substr( begin, end );
}

int setShaderType( shader::Shader& module, std::string stagetype )
{
	if ( stagetype ==  "vert" ) {
		module.ID = glCreateShader( GL_VERTEX_SHADER );
		module.type = shader::ShaderType::Vertex;
	}
	else if ( stagetype ==  "geo" ) {
		module.ID = glCreateShader( GL_GEOMETRY_SHADER );
		module.type = shader::ShaderType::Geometry;
	}
	else if ( stagetype ==  "frag" ) {
		module.ID = glCreateShader( GL_FRAGMENT_SHADER );
		module.type = shader::ShaderType::Fragment;
	}
	else if ( stagetype ==  "comp" ) {
		module.ID = glCreateShader( GL_COMPUTE_SHADER );
		module.type = shader::ShaderType::Compute;
	}
	else {
		debug::pushError( "\nShader::loadShader(): invalid shader file name " + module.fileName + "!\nHas to include '.vert', '.frag', '.geo' or '.comp'! Got: " + stagetype, debug::Error::Fatal );
		return 1;
	}
    return 0;
}
void compileShaderSource( shader::Shader& module )
{
    using namespace shader::Loader;
	std::ifstream moduleFile;
	moduleFile.open( SHADER_DIR + module.fileName + ".txt" );
	if ( moduleFile.fail() ) {
		debug::pushError( "Failed to compile shader: Could not open " + SHADER_DIR + module.fileName + ".txt" + "!\n", debug::Error::Fatal );
		return;
	}

	module.content = static_cast<std::stringstream const&>( std::stringstream() << moduleFile.rdbuf() ).str();
	const char* content = module.content.c_str();
	int length = module.content.length();
	glShaderSource( module.ID, 1, &content, &length );
	glCompileShader( module.ID );
	int success = 0;
	glGetShaderiv( module.ID, GL_COMPILE_STATUS, &success );
	if ( success == GL_FALSE ) {
		int maxLength = 0;
		glGetShaderiv( module.ID, GL_INFO_LOG_LENGTH, &maxLength );
		std::vector<char> errorLog( maxLength );
		glGetShaderInfoLog( module.ID, maxLength, &maxLength, &errorLog[0] );
		debug::pushError( "\n!!! Failed to compile " + module.fileName + "\nOpenGL Error Log: " + std::string( &( errorLog[0] ) ) + "\n", debug::Error::Fatal );
		return;
	}
}
void shader::Loader::compileShader( unsigned int pShaderIndex )
{
	Shader& mod = allShaders[pShaderIndex];
	printf( "Shaderfile: Compiling %s\n", mod.fileName.c_str() );
    std::string moduleStageType = extractStageString( mod.fileName ); 
    if( setShaderType( mod, moduleStageType ) ){
        return;
    }   
    compileShaderSource( mod );
	allShaders[pShaderIndex] = mod;
}

void shader::Loader::linkProgram( unsigned int pProgramIndex )
{
	Program& program = allPrograms[pProgramIndex];
	printf( "Shaderfile: Linking %s\n", program.name.c_str() );
	if( program.shaderCount ) {
		if ( program.type == ProgramType::Compute ) {
			program.stages[0] = allShaders[program.stages[0]].ID;
			glAttachShader( program.ID, program.stages[0] );
		} 
		else {
			for ( unsigned int i = 0; i < program.shaderCount; ++i ) {
				program.stages[i] = allShaders[program.stages[i]].ID;
				glAttachShader( ( GLuint )program.ID, ( GLuint )program.stages[i] );
			}
		}
		glLinkProgram( program.ID );
		GLint success = 0;
		glGetProgramiv( program.ID, GL_LINK_STATUS, &success );

		if ( success == GL_FALSE ) {
			GLint maxLength = 0;
			glGetProgramiv( program.ID, GL_INFO_LOG_LENGTH, &maxLength );
			std::vector<char> errorLog( maxLength );
			glGetProgramInfoLog( program.ID, maxLength, &maxLength, &errorLog[0] );
			glDeleteProgram( program.ID );
			debug::pushError( "!!!/nError when linking program: " + program.name + " /nopenGL Error Log: " + &( errorLog[0] ), debug::Error::Fatal );
		}

		for ( unsigned int i = 0; i < program.shaderCount; ++i ) {
			glDetachShader( program.ID, program.stages[i] );
		}
	} 
	else {
		puts( "Shaderfile: Error: Unable to link program: Program contains no shaders." );
	}
}

void shader::Loader::compileAndLink()
{
	for ( unsigned int s = 0; s < allShaders.size(); ++s ) {
		compileShader( s );
	}
	for ( unsigned int p = 0; p < allPrograms.size(); ++p ) {
		linkProgram( p );
	}
}
unsigned int shader::createShader( std::string pFileName )
{
    unsigned int index = allShaders.size();
    shaderLookup.insert( std::pair<std::string, unsigned int>( pFileName, index ) );
    allShaders.push_back( Shader( pFileName ) );
    return index;
}

unsigned int shader::newProgram( std::string pProgramName, unsigned int pVertexShaderIndex, unsigned int pFragmentShaderIndex )
{
    unsigned int index = createProgram( pProgramName );
    allPrograms[index].type = ProgramType::Basic;
    allPrograms[index].stages[0] = pVertexShaderIndex;
    allPrograms[index].stages[1] = pFragmentShaderIndex;
    allPrograms[index].shaderCount = 2;
    return allPrograms[index].ID;
}

unsigned int shader::newProgram( std::string pProgramName, unsigned int pVertexShaderIndex, unsigned int pFragmentShaderIndex, unsigned int pGeometryShaderIndex )
{
    unsigned int index = createProgram( pProgramName );
    allPrograms[index].type = ProgramType::Geometry;
    allPrograms[index].stages[0] = pVertexShaderIndex;
    allPrograms[index].stages[1] = pFragmentShaderIndex;
    allPrograms[index].stages[2] = pGeometryShaderIndex;
    allPrograms[index].shaderCount = 3;
    return allPrograms[index].ID;
}

unsigned int shader::newProgram( std::string pProgramName, unsigned int pComputeShaderIndex )
{
    unsigned int index = createProgram( pProgramName );
    allPrograms[index].type = ProgramType::Compute;
    allPrograms[index].stages[0] = pComputeShaderIndex;
    allPrograms[index].shaderCount = 1;
    return allPrograms[index].ID;
}

unsigned int shader::createProgram( std::string pProgramName )
{
    Program program;
    program.ID = glCreateProgram();
    program.name = pProgramName;
    programLookup.insert( std::pair<std::string, unsigned int>( pProgramName, program.ID ) );
    allPrograms.push_back( program );
    return allPrograms.size() - 1;
}

void shader::use( unsigned int pID )
{
    currentShaderProgram = pID;
    glUseProgram( pID );
}

void shader::use( std::string pProgramName )
{
    use( programLookup.find( pProgramName )->second );
}

void shader::unuse()
{
    currentShaderProgram = {};
    glUseProgram( 0 );
}

void shader::addVertexAttribute( unsigned int pProgramID, std::string pAttributeName, unsigned int pAttributeIndex )
{
    //binds a in variable symbol out of a shader to an attribute index
    glBindAttribLocation( pProgramID, pAttributeIndex, pAttributeName.c_str() );
}

void shader::addVertexAttribute( std::string pProgramName, std::string pAttributeName, unsigned int pAttributeIndex )
{
    auto it = programLookup.find( pProgramName.c_str() );
    if ( it == programLookup.end() ) {
        debug::pushError( "addVertexAttribute():/nCould not find shader Program " + pProgramName + "!/n" );
        return;
    }
    addVertexAttribute( it->second, pAttributeName, pAttributeIndex );
}

void shader::setUniform( unsigned int pProgram, std::string pUniformName, int pValue ) {
    glUniform1i( glGetUniformLocation( pProgram, pUniformName.c_str() ), pValue );
}
void shader::setUniform( unsigned int pProgram, std::string pUniformName, unsigned int pValue ) {
    glUniform1ui( glGetUniformLocation( pProgram, pUniformName.c_str() ), pValue );
}
void shader::setUniform( unsigned int pProgram, std::string pUniformName, float pValue ) {
    glUniform1f( glGetUniformLocation( pProgram, pUniformName.c_str() ), pValue );
}
void shader::setUniform( unsigned int pProgram, std::string pUniformName, glm::vec3 pValue ) {
    glUniform3f( glGetUniformLocation( pProgram, pUniformName.c_str() ), pValue.x, pValue.y, pValue.z );
}
void shader::setUniform( unsigned int pProgram, std::string pUniformName, glm::vec4 pValue ) {
    glUniform4f( glGetUniformLocation( pProgram, pUniformName.c_str() ), pValue.x, pValue.y, pValue.z, pValue.w );
}
void shader::setUniform( unsigned int pProgram, std::string pUniformName, glm::uvec4 pValue ) {
    glUniform4ui( glGetUniformLocation( pProgram, pUniformName.c_str() ), pValue.x, pValue.y, pValue.z, pValue.w );
}
void shader::setUniform( unsigned int pProgram, std::string pUniformName, glm::uvec3 pValue ) {
    glUniform3ui( glGetUniformLocation( pProgram, pUniformName.c_str() ), pValue.x, pValue.y, pValue.z );
}
void shader::setUniform( unsigned int pProgram, std::string pUniformName, glm::ivec4 pValue ) {
    glUniform4i( glGetUniformLocation( pProgram, pUniformName.c_str() ), pValue.x, pValue.y, pValue.z, pValue.w );
}
void shader::setUniform( unsigned int pProgram, std::string pUniformName, glm::ivec3 pValue ) {
    glUniform3i( glGetUniformLocation( pProgram, pUniformName.c_str() ), pValue.x, pValue.y, pValue.z );
}
void shader::setUniform( unsigned int pProgram, std::string pUniformName, glm::mat4 pValue, bool pTranspose ) {
    glUniformMatrix4fv( glGetUniformLocation( currentShaderProgram, pUniformName.c_str() ), 1, pTranspose, glm::value_ptr( pValue ) );
}
void shader::setUniform( unsigned int pProgram, std::string pUniformName, glm::mat3 pValue, bool pTranspose ) {
    glUniformMatrix3fv( glGetUniformLocation( pProgram, pUniformName.c_str() ), 1, pTranspose, glm::value_ptr( pValue ) );
}
