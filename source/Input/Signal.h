#pragma once
#include "Event.h"
#include <vector>

namespace app {
	namespace Input {
		void clearSignals();
		void resetSignals();
		void checkSignals();

		//Gates are used to control when a Functor should call. The signals of events (allEventStates) are used as inputs and decay down to a single signal determining if the functor calls.
		//Gates are implemented as templates and can be nested inside each other
		//o-L o-R
		// \&/-op
		//  o-result

		template<typename L, typename R>
		struct and_gate{
			and_gate(L pA, R pB) :a(pA), b(pB) {}
			L a;
			R b;
			bool operator()() {
				return a() && b();
			}
		};
		template<typename L, typename R>
		struct or_gate {
			or_gate(L pA, R pB) :a(pA), b(pB) {}
			L a;
			R b;
			bool operator()() {
				return a() || b();
			}
		};

		template<typename L, typename R>
		struct xor_gate {
			xor_gate(L pA, R pB) :a(pA), b(pB) {}
			L a;
			R b;
			bool operator()() {
				return !a() != !b();
			}
		};

		template<typename L, typename R>
		struct nor_gate {
			nor_gate(L pA, R pB) :a(pA), b(pB) {}
			L a;
			R b;
			bool operator()() {
				return !a() && !b();
			}
		};

		template<typename L, typename R>
		struct nand_gate {
			nand_gate(L pA, R pB) :a(pA), b(pB) {}
			L a;
			R b;
			bool operator()() {
				return !(a() && b());
			}
		};


		template<typename Source>
		struct toggle_gate {
			toggle_gate(Source pSource) :src(pSource) {}
			Source src;
			bool state;
			bool operator()() {
				if (src()) {//toggle if src() == true
					state = !state;
				}
				return state;
			}
		};

		struct EventSource {//event state source
			EventSource(size_t pIndex)
				:src(pIndex){}
			size_t src;
			bool operator()() {
				return allEventStates[src];
			}
		};

		template<typename Source>
		struct Signal {
			Signal(size_t pSignal, Source pSource):signal(pSignal), src(pSource){}

			
			static void check_slots() {
				for (size_t s = 0; s < slots.size(); ++s) {
					slots[s].check();
				}
			}
			void check() {
				allSignals[signal] = src();
			}
			static void clear() {
				slots.clear();
			}
			static std::vector<Signal<Source>> slots;
		private:
			size_t signal;
			Source src;
			struct initializer {
				initializer() {
					signalCheckers.push_back(Signal<Source>::check_slots);
					signalDestructors.push_back(Signal<Source>::clear);
				}
			};
			const initializer ini = initializer();

		};
		template<typename Source>
		std::vector<Signal<Source>> Signal<Source>::slots;
		
		template<typename Source>
		size_t createSignal(Source pSource, bool pStartAs = false) {
			Signal<Source>::slots.emplace_back(allSignals.size(), pSource);
			allSignals.push_back(pStartAs);
			return allSignals.size() - 1;
		}
		extern std::vector<bool> allSignals;
		extern std::vector<void(*)()> signalCheckers;//checks all Signal templates
		extern std::vector<void(*)()> signalDestructors;//destroys all Signal templates
	}
}