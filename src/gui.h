#pragma once
#include "gl.h"
#include <string>
#include <unordered_map>
namespace gui 
{
	void init();
	void initWidgets();
	extern glm::vec2 pixel_size;
	glm::vec2 pixel_round( glm::vec2 pIn );
	float pixel_round_x( float pIn );
	float pixel_round_y( float pIn );
	
}

