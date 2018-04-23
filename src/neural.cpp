#include "Neural.h"
using namespace neural;

Agent agent;
unsigned int NEURON_COUNT = 100;
unsigned int MAX_SYNAPSES = 40;
void neural::setup() {

	agent.circuit.neurons = std::vector<Neuron>( NEURON_COUNT );
	agent.circuit.synapses.resize( NEURON_COUNT, std::vector<Synapse>( MAX_SYNAPSES ) );
	for ( unsigned int n = 0; n < ( NEURON_COUNT ); ++n ) {
		for ( unsigned int s = 0; s < MAX_SYNAPSES; ++s ) {
			agent.circuit.synapses[n][s] = Synapse( n, ( n+s+1 )%NEURON_COUNT, 0.0f );
		}
	}


}

void neural::Circuit::stepSynapse( unsigned int n, unsigned int s ) {

}
void neural::Circuit::step() {
	for ( unsigned int n = 0; n < neurons.size(); ++n ) {
		
		
		//transmit to links
		for ( unsigned int s = 0; s < synapses.size(); ++s ) {
			agent.circuit.stepSynapse( n, s );
		}
	}
}
void traverse() {
	agent.circuit.step();
}