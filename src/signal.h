#pragma once
#include <stddef.h>
#include <vector>

namespace signals {

	//Signals are simply a uniform way of transmitting signals between Events and functors
	//Functors store indices of signals to check whether they should invoke or not
	//Signals use Events (or Gates) as sources for their state

	void clearSignals();
	void resetSignals();
	void checkSignals();

	
	extern std::vector<bool> allSignals;
	extern std::vector<void(*)()> signalCheckers;//checks all Signal templates
	extern std::vector<void(*)()> signalDestructors;//destroys all Signal templates

	template<typename Source>
	struct Signal {
		Signal(unsigned int pSignal, Source pSource)
			:signal(pSignal), src(pSource) {
		}

		static void check_slots() {
			for (unsigned int s = 0; s < slots.size(); ++s) {
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

		unsigned int signal;
		Source src;
	};

	struct Source {
		Source(bool pSrc)
			:src(pSrc) {
		}
		bool operator()() {
			return src;
		}
	private:
		bool src;
	};

	template<typename Source>
	std::vector<Signal<Source>> Signal<Source>::slots = std::vector<Signal<Source>>();

	template<typename Source>
	unsigned int createSignal(Source pSource, bool pStartAs = false) {
		if (!Signal<Source>::slots.size()) {
			signalCheckers.push_back(Signal<Source>::check_slots);
			signalDestructors.push_back(Signal<Source>::clear);
		}
		Signal<Source>::slots.emplace_back(Signal<Source>(allSignals.size(), std::forward<Source>(pSource)));
		allSignals.push_back(pStartAs);
		return allSignals.size() - 1;
	}

}
