#include "Neural.h"
using namespace neural;

Agent agent;
size_t NEURON_COUNT = 100;
size_t MAX_SYNAPSES = 40;
void neural::setup() {

	agent.circuit.neurons = std::vector<Neuron>(NEURON_COUNT);
	agent.circuit.synapses.resize(NEURON_COUNT, std::vector<Synapse>(MAX_SYNAPSES));
	for (size_t n = 0; n < (NEURON_COUNT); ++n) {
		for (size_t s = 0; s < MAX_SYNAPSES; ++s) {
			agent.circuit.synapses[n][s] = Synapse(n, (n+s+1)%NEURON_COUNT, 0.0f);
		}
	}


}

void neural::Circuit::stepSynapse(size_t n, size_t s) {

}
void neural::Circuit::step() {
	for (size_t n = 0; n < neurons.size(); ++n) {
		
		
		//transmit to links
		for (size_t s = 0; s < synapses.size(); ++s) {
			agent.circuit.stepSynapse(n, s);
		}
	}
}
void traverse() {
	agent.circuit.step();
}