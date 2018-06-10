#include "shaderfile.h"
#include "shader.h"
#include "debug.h"
#include <fstream>
#include <sstream>
#include "gldebug.h"

#define DEFAULT_SHADER_DIRECTORY "shaders//"

std::string shader::Loader::SHADER_DIR = DEFAULT_SHADER_DIRECTORY;



void shader::Loader::buildShaderPrograms()
{
	compileAndLink();
}

void shader::Loader::setShaderDirectory( std::string& pDirectory )
{
	SHADER_DIR = pDirectory;
}

void shader::Loader::resetShaderDirectory()
{
	SHADER_DIR = DEFAULT_SHADER_DIRECTORY;
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