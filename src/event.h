#pragma once
#include <vector>
#include "signal.h"
#include "gates.h"

namespace events {

	//EventSlots store a signature of their type to check it against their eventBuffer.
	//the eventBuffer is a vector which is filled by pushEvent
	//both EventSlot and pushEvent are templates. they can be used with any type which supports the comparision operator==

	void checkEvents();
	void clearEvents();

	void resetEvents();
	extern std::vector<bool> allEventStates;//shows the events which occurred each frame
	extern std::vector<void( * )()> eventCheckers;//checks all EventSlot templates
	extern std::vector<void( * )()> eventDestructors;//destroys all EventSlot templates
	extern std::vector<void( * )()> eventResetters;//clears all Event buffers

	template<class EventType>
	class EventSlot {
	public:
		EventSlot( unsigned int pStateIndex, EventType&& pEvent )
			:event_signature( pEvent ), stateIndex( pStateIndex )
		{		}
		static void reserve_slots( unsigned int pCount ) {
			allEventStates.reserve( allEventStates.size() + pCount );
			slots.reserve( slots.size() + pCount );
		}
		static unsigned int slot_count() {//instances of this type
			return slots.size();
		}
		static EventSlot<EventType>& get_slot( unsigned int index ) {
			return slots[index];
		}
		static void clear_slots() {
			slots.clear();
		}
		static void clear_events() {
			eventBuffer.clear();
		}
		static void checkEvents() {//throws the signals of the events in eventBuffer
			for ( unsigned int e = 0; e < EventSlot<EventType>::eventBuffer.size(); ++e ) {
				EventType& evnt = EventSlot<EventType>::eventBuffer[e];
				for ( unsigned int sl = 0; sl < EventSlot<EventType>::slots.size(); ++sl ) {
					EventSlot<EventType>& slot = EventSlot<EventType>::slots[sl];
					if ( evnt == slot.event_signature ) {//trigger signal
						slot.set();
					}
				}
			}
		}
		static std::vector<EventSlot<EventType>> slots;//all event slots of this type
		static std::vector<EventType> eventBuffer;
		void set() {
			allEventStates[stateIndex] = true;
		}
	private:
		EventType event_signature;
		unsigned int stateIndex;
	};


	template<class EventType>
	void pushEvent( EventType pEvent ) {
		EventSlot<EventType>::eventBuffer.push_back( pEvent );
	}
	struct Event {//event state source
		Event()
			:src( -1 ) {}
		Event( unsigned int pIndex )
			:src( pIndex ) {}
		unsigned int src;
		bool operator()() {
			return allEventStates[src];
		}
	};
	inline bool operator==( Event const & l, Event const& r ) {
		return l.src == r.src;
	}
	template<class EventType>
	Event createEvent( EventType pEvent ) {
		if ( !EventSlot<EventType>::slots.size() ) {
			eventCheckers.push_back( EventSlot<EventType>::checkEvents );
			eventDestructors.push_back( EventSlot<EventType>::clear_slots );
			eventResetters.push_back( EventSlot<EventType>::clear_events );
		}
		EventSlot<EventType>::slots.emplace_back( allEventStates.size(), std::forward<EventType>( pEvent ) );
		allEventStates.push_back( false );
		return Event( allEventStates.size() - 1 );
	}


	template<class EventType>
	std::vector<EventSlot<EventType>> EventSlot<EventType>::slots = std::vector<EventSlot<EventType>>();

	template<class EventType>
	std::vector<EventType> EventSlot<EventType>::eventBuffer = std::vector<EventType>();

	template<typename OnType, typename OffType = OnType>
	struct ButtonEvents {
		ButtonEvents(){}
		ButtonEvents( OnType pOn, OffType pOff )
			:on_evt( pOn ),
			off_evt( pOff ),
			hold_evt( gates::switch_gate<OnType, OffType>( on_evt, off_evt ) ),
			on( signals::createSignal( on_evt ) ),
			off( signals::createSignal( off_evt ) ),
			hold( signals::createSignal( hold_evt ) )
		{}
		OnType on_evt;
		OffType off_evt;
		gates::switch_gate<OnType, OffType> hold_evt;
		unsigned int on;
		unsigned int off;
		unsigned int hold;
	};
}