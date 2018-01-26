#pragma once
#include "../Graphics/BaseGL/ContextWindow.h"
namespace app {
	namespace Input {

		class KeyCondition {
		public:
			KeyCondition()
				:action(0) // 0 = release, 1 = press
			{}
			KeyCondition(int pAction)
				:action(pAction)
			{}
			int action;
		};
		inline bool operator==(KeyCondition const & l, KeyCondition const& r) {
			return l.action == r.action;
		}
		inline bool operator!=(KeyCondition const & l, KeyCondition const& r) {
			return l.action != r.action;
		}

		class KeyEvent {
		public:
			KeyEvent()
				:key(-1), change(KeyCondition()) {}
			KeyEvent(int pKey, KeyCondition pChange)
				:key(pKey), change(pChange) {}
			KeyEvent(int pKey, int pAction, int pMods)
				:key(pKey), change(KeyCondition(pAction)) {}

			int key;
			KeyCondition change;
		};

		inline bool operator==(KeyEvent const & l, KeyEvent const& r) {
			return l.key == r.key && l.change == r.change;
		}
		struct KeySignal {
			KeySignal(){}
			KeySignal(int pKey);
			size_t hold;
			size_t press;
			size_t release;
		};

		void reserveKeySignals(size_t pCount);
		void key_Callback(GLFWwindow* window, int pKey, int pScancode, int pAction, int pMods);
		void char_Callback(GLFWwindow* window, size_t pCodepoint);
	}
}