#pragma once
#include <vector>
#include <stddef.h>

namespace neural
{

  void setup();

  struct RangeVision
  {

  };

  template<typename FocusType>
	struct ExtSense
	{


	  unsigned int stream_neuron;
	};


  //A first version of a neural ai network. each agent has its own perception of its surrounding and acts depending on its internal state and those inputs
  //like with natural intelligence, these networks should reward selfish/survivalistic behaviour and should prevent useless or self-harmful behaviour

  struct Neuron
  {
	float charge;
  };


  struct Synapse
  {
	Synapse()
	{}
	Synapse(unsigned int pFrom, unsigned int pTo, float pInitialRating = 0.0f)
	  : from(pFrom)
	  , to(pTo)
	  , rating(pInitialRating)
	{}
	unsigned int from;
	unsigned int to;
	float rating;
  };

  struct Circuit
  {
	void step();
	void stepSynapse(unsigned int n, unsigned int s);

	std::vector<Neuron> neurons;
	std::vector<std::vector<Synapse>> synapses;
  };

  class Agent
  {
	public:
	  unsigned int entity;//use this ID to get and set entity data
	  Circuit circuit;
  };
}
