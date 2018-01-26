#pragma once
#include <glm/glm.hpp>
#include "Keys.h"
namespace app {
	namespace Input {
		
		struct MouseKeyEvent {//events thrown when a mouse key changes
			MouseKeyEvent()
				:key(-1) {}
			MouseKeyEvent(int pKey, KeyCondition pChange)
				:key(pKey), change(pChange) {}
			MouseKeyEvent(int pKey, int pAction)
				:key(pKey), change(KeyCondition(pAction)) {}
			int key;
			KeyCondition change;
		};
		inline bool operator==(MouseKeyEvent const & l, MouseKeyEvent const& r) {
			return l.key == r.key && l.change == r.change;
		}

		struct QuadEvent {//events thrown when entering or leaving quads with the cursor
			QuadEvent()
				:quad_index(0), enter(0) {}
			QuadEvent(size_t pQuad, int pEnter)
				:quad_index(pQuad), enter(pEnter) {}
			size_t quad_index;//0 if none
			int enter;
		};
		inline bool operator==(QuadEvent const & l, QuadEvent const& r) {
			return l.quad_index == r.quad_index && l.enter == r.enter;
		}
		class Mouse {//TODO

		};
		extern glm::vec2 relativeCursorPosition;
		extern glm::uvec2 absoluteCursorPosition;
		extern glm::vec2 cursorFrameDelta;
		void updateMouse();
		void resetMouse();
		void toggleCursor();
		void getMouseEvents();

		void mouseKey_Callback(GLFWwindow* window, int pKey, int pAction, int pMods);
		void cursorPosition_Callback(GLFWwindow* window, double pX, double pY);
		void cursorEnter_Callback(GLFWwindow * window, int pEntered);
		void scroll_Callback(GLFWwindow* window, double pX, double pY);
	}
}