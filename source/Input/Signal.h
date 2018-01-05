#pragma once
#include <vector>

namespace app {
	namespace Input {
		void initSignals();
		void clearSignals();
		void resetSignals();
		void checkSignals();

		//Gates are used to control when a Functor should call. The signals of events (allEventStates) are used as inputs and decay down to a single signal determining if the functor calls.
		//Gates are implemented as templates and can be nested inside each other
		//o-L o-R
		// \&/-op
		//  o-result

		template<typename L, typename R>
		struct and_gate {
			and_gate(L pA, R pB) :a(std::forward<L>(pA)), b(std::forward<R>(pB)) {}
			L a;
			R b;
			bool operator()() {
				return a() && b();
			}
		};
		template<typename L, typename R>
		inline bool operator==(and_gate<L, R> const & l, and_gate<L, R> const& r) {
			return l.a == r.a && l.b == r.b;
		}
		template<typename L, typename R>
		struct or_gate {
			or_gate(L pA, R pB) :a(std::forward<L>(pA)), b(std::forward<R>(pB)) {}
			L a;
			R b;
			bool operator()() {
				return a() || b();
			}
		};
		template<typename L, typename R>
		inline bool operator==(or_gate<L, R> const & l, or_gate<L, R> const& r) {
			return l.a == r.a && l.b == r.b;
		}
		template<typename L, typename R>
		struct xor_gate {
			xor_gate(L pA, R pB) :a(std::forward<L>(pA)), b(std::forward<R>(pB)) {}
			L a;
			R b;
			bool operator()() {
				return !a() != !b();
			}
		};
		template<typename L, typename R>
		inline bool operator==(xor_gate<L, R> const & l, xor_gate<L, R> const& r) {
			return l.a == r.a && l.b == r.b;
		}
		template<typename L, typename R>
		struct nor_gate {
			nor_gate(L pA, R pB) :a(std::forward<L>(pA)), b(std::forward<R>(pB)) {}
			L a;
			R b;
			bool operator()() {
				return !a() && !b();
			}
		};
		template<typename L, typename R>
		inline bool operator==(nor_gate<L, R> const & l, nor_gate<L, R> const& r) {
			return l.a == r.a && l.b == r.b;
		}
		template<typename L, typename R>
		struct nand_gate {
			nand_gate(L pA, R pB) :a(std::forward<L>(pA)), b(std::forward<R>(pB)) {}
			L a;
			R b;
			bool operator()() {
				return !(a() && b());
			}
		};
		template<typename L, typename R>
		inline bool operator==(nand_gate<L, R> const & l, nand_gate<L, R> const& r) {
			return l.a == r.a && l.b == r.b;
		}

		template<typename Source>
		struct toggle_gate {
			toggle_gate(Source pSource, bool pStartAs = false) :src(std::forward<Source>(pSource)), state(pStartAs) {}
			Source src;
			bool state;
			bool operator()() {
				if (src()) {//toggle if src() == true
					src();
					state = !state;
				}
				return state;
			}
		};
		template<typename Source>
		inline bool operator==(toggle_gate<Source> const & l, toggle_gate<Source> const& r) {
			return l.src == r.src && l.state == r.state;
		}
		template<typename Source>
		struct clicker_gate {//fires once when the input changes
			clicker_gate(Source pSource, bool pStartWith = false) :src(std::forward<Source>(pSource)), last_state(pStartWith) {}
			Source src;
			bool last_state;
			bool operator()() {
				if (src() != last_state) {//fire if src() != last_state
					last_state = !last_state;
					return true;
				}
				return false;
			}
		};
		template<typename Source>
		inline bool operator==(clicker_gate<Source> const & l, clicker_gate<Source> const& r) {
			return l.src == r.src && l.last_state == r.last_state;
		}
		template<typename On, typename Off>
		struct switch_gate {//has internal state and two inputs each either turning the state true or false
			switch_gate(){}
			switch_gate(On pOnSource, Off pOffSource, bool pStartWith = false) :on_src(std::forward<On>(pOnSource)), off_src(std::forward<Off>(pOffSource)), state(pStartWith) {}
			On on_src;
			Off off_src;
			bool state;
			bool operator()() {
				//on_src can switch state on and off_src can switch state off
				bool on = on_src();
				bool off = off_src();
				if (on != off) {
					state = on;
				}
				//state = state + (on_src() & !state) + ()%2;

				return state;
			}
		};
		template<typename On, typename Off>
		inline bool operator==(switch_gate<On, Off> const & l, switch_gate<On, Off> const& r) {
			return l.on_src == r.on_src && l.off_src == r.off_src && l.state == r.state;
		}
		template<typename Source>
		struct not_gate {//has internal state and two inputs each either turning the state true or false
			not_gate(Source pSource) :src(std::forward<Source>(pSource)) {}
			Source src;
			bool operator()() {
				return !src();
			}
		};
		template<typename Source>
		inline bool operator==(not_gate<Source> const & l, not_gate<Source> const& r) {
			return l.src == r.src;
		}
		extern std::vector<bool> allSignals;
		extern std::vector<void(*)()> signalCheckers;//checks all Signal templates
		extern std::vector<void(*)()> signalDestructors;//destroys all Signal templates

		template<typename Source>
		struct Signal {
			Signal(size_t pSignal, Source pSource)
				:signal(pSignal), src(pSource) {
			}

			static void check_slots(){
				for (size_t s = 0; s < slots.size(); ++s) {
					slots[s].check();
				}
			}
			void check(){
				allSignals[signal] = src();
			}
			static void clear() {
				slots.clear();
			}
			static std::vector<Signal<Source>> slots;
		private:

			size_t signal;
			Source src;
		};
		
		
		template<typename Source>
		std::vector<Signal<Source>> Signal<Source>::slots = std::vector<Signal<Source>>();
		
		template<typename Source>
		size_t createSignal(Source pSource, bool pStartAs = false) {
			if(!Signal<Source>::slots.size()) {
				signalCheckers.push_back(Signal<Source>::check_slots);
				signalDestructors.push_back(Signal<Source>::clear);
			}
			Signal<Source>::slots.emplace_back(Signal<Source>(allSignals.size(), std::forward<Source>(pSource)));
			allSignals.push_back(pStartAs);
			return allSignals.size() - 1;
		}

	}
}