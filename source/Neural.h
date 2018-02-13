#pragma once
#include <vector>
namespace neural {

	void setup();

	struct RangeVision {

	};
	
	template<typename FocusType>
	struct ExtSense {
		

		size_t stream_neuron;
	};









	//A first version of a neural ai network. each agent has its own perception of its surrounding and acts depending on its internal state and those inputs
	//like with natural intelligence, these networks should reward selfish/survivalistic behaviour and should prevent useless or self-harmful behaviour
	
	struct Neuron {
		float charge;
	};
	

	struct Synapse {
		Synapse() {}
		Synapse(size_t pFrom, size_t pTo, float pInitialRating = 0.0f)
			:from(pFrom), to(pTo), rating(pInitialRating){}
		size_t from;
		size_t to;
		float rating;
	};

	struct Circuit {
		void step();
		void stepSynapse(size_t n, size_t s);

		std::vector<Neuron> neurons;
		std::vector<std::vector<Synapse>> synapses;
	};

	class Agent {
	public:
		size_t entity;//use this ID to get and set entity data
		Circuit circuit;
	};
}