#include "primitives.h"
#include "vao.h"

gl::Storage<glm::vec2> gl::quadVBO;
gl::Storage<unsigned int> gl::quadEBO;
gl::Storage<glm::vec3> gl::cubeVBO;
gl::Storage<unsigned int> gl::cubeEBO;

void gl::initPrimitives()
{
    glm::vec2 varr[4] = {
        glm::vec2( 0.0f, 0.0f ),
		glm::vec2( 1.0f, 0.0f ),
		glm::vec2( 0.0f, 1.0f ),
		glm::vec2( 1.0f, 1.0f )
	};

	unsigned int iarr[6] = {
		2, 0, 3, 0, 1, 3
	};
	quadVBO = Storage<glm::vec2>( "QuadVBO", 4, 0, &varr[0] );
	quadEBO = Storage<unsigned int>( "QuadEBO", 6, 0, &iarr[0] );

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
		0, 1, 2, 0, 2, 3,
		0, 4, 5, 0, 5, 1,
		0, 3, 4, 3, 7, 4,
		3, 6, 7, 3, 2, 6,
		1, 5, 6, 1, 6, 2,
		4, 6, 5, 4, 7, 6
	};

	cubeVBO = Storage<glm::vec3>( "CubeVBO", 8, 0, &cube_verts[0] );
	cubeEBO = Storage<unsigned int>( "CubeEBO", 36, 0, &cube_inds[0] );
}
