#pragma once
#include <cstring>
#include <glm.hpp>
#include <vector>
#include <tuple>
#include "utils/itr.h"
#include "storage.h"
#include "vao.h"
#include "color.h"

namespace gui 
{
	struct Quad 
	{
		using Initer = glm::vec4;
		using Colors = gl::ColorIt;
		Quad( glm::vec4 pData ); 

		const unsigned int ID;
		void move( const glm::vec2 pV ) const;
		void resize( const glm::vec2 pV ) const;
		void color( const gl::ColorIt pColor ) const; 
	};

	const unsigned int MAX_QUAD_COUNT = 10000;
	void setQuadPos( const Quad pQuad, const glm::vec2 pPos );
	glm::vec4& getQuadData( const unsigned int pID );


	void initQuadBuffer();
	void updateQuadBuffer();
	extern gl::StreamStorage<glm::vec4> quadBuffer;
	extern size_t quadCount;
}
