#pragma once
#include <cstddef>
#include "storage.h"

namespace gl
{
	void initPrimitiveVBO();

	extern Storage quadVBO;
	extern Storage quadEBO;
	extern Storage cubeVBO;
	extern Storage cubeEBO;
}
