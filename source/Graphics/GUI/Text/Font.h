#pragma once
#include <glm\glm.hpp>

namespace gui {
	namespace text {

		void initFontShader();
		void initFontVAO();
		void updateCharStorage();
		void clearCharStorage();
		void revalidateFontStringIndices();
	}
}
