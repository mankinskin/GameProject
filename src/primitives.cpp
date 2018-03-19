#include "primitives.h"
#include "vao.h"

#include <glm.hpp>

unsigned int gl::quadVBO = 0;
unsigned int gl::quadEBO = 0;
unsigned int gl::cubeVBO = 0;
unsigned int gl::cubeEBO = 0;

void gl::initPrimitiveVBO()
{
	/*        Colored-Quad
		  2---3     0,0------1,0
		  |   |      |        |
		  |   |      |        |
		  |   |      |        |
		  0---1     0,1------1,1
		  */
	float varr[8] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
	};

	unsigned int iarr[6] = {
		2, 0, 1, 1, 3, 2
	};
	quadVBO = vao::createStorage( "QuadVBO", sizeof( float ) * 4 * 2, &varr[0], 0 );
	quadEBO = vao::createStorage( "QuadEBO", sizeof( unsigned int ) * 6, &iarr[0], 0 );

	float cube_width = 1.0f;
	glm::vec3 cube_verts[8] = {
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( cube_width, 0.0f, 0.0f ),
		glm::vec3( cube_width, 0.0f, cube_width ),
		glm::vec3( 0.0f, 0.0f, cube_width ),

		glm::vec3( 0.0f, cube_width , 0.0f ),
		glm::vec3( cube_width, cube_width, 0.0f ),
		glm::vec3( cube_width , cube_width, cube_width ),
		glm::vec3( 0.0f, cube_width, cube_width )
	};

	unsigned int cube_inds[36] = {
		0, 1, 2,
		0, 2, 3,

		0, 4, 5,
		0, 5, 1,

		0, 3, 4,
		3, 7, 4,

		3, 6, 7,
		3, 2, 6,

		1, 5, 6,
		1, 6, 2,

		4, 6, 5,
		4, 7, 6
	};
	cubeVBO = vao::createStorage( "CubeVBO", sizeof( glm::vec3 ) * 8, &cube_verts[0], 0 );
	cubeEBO = vao::createStorage( "CubeEBO", sizeof( unsigned int ) * 36, &cube_inds[0], 0 );
}
