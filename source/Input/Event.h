#pragma once
namespace app {
	namespace Input {

		void checkEvents();
		void clearEvents();

		void resetEvents();

		template<class EventType>
		class EventSlot {
		public:
			EventSlot(size_t pStateIndex, EventType pEvent)
				:event_signature(pEvent), stateIndex(pStateIndex)
			{}
			static void reserve_slots(size_t pCount) {
				allEventStates.reserve(allEventStates.size() + pCount);
				slots.reserve(slots.size() + pCount);
			}
			static size_t slot_count() {//instances of this type
				return slots.size();
			}
			static EventSlot<EventType>& get_slot(size_t index) {
				return slots[index];
			}
			static void clear_slots() {
				slots.clear();
			}
			static void checkEvents() {//throws the signals of the events in eventBuffer
				for (size_t e = 0; e < EventSlot<EventType>::eventBuffer.size(); ++e) {
					EventType& evnt = EventSlot<EventType>::eventBuffer[e];
					for (size_t sl = 0; sl < EventSlot<EventType>::slots.size(); ++sl) {
						EventSlot<EventType>& slot = EventSlot<EventType>::slots[sl];
						if (evnt == slot.event_signature) {//trigger signal
							slot.set();
						}
					}
				}
				EventSlot<EventType>::eventBuffer.clear();
			}
			static std::vector<EventSlot<EventType>> slots;//all event slots of this type
			static std::vector<EventType> eventBuffer;
			void set() {
				allEventStates[stateIndex] = true;
			}
		private:
			EventType event_signature;
			size_t stateIndex;
			struct initializer {
				initializer() {
					eventCheckers.push_back(EventSlot<EventType>::checkEvents);
					eventDestructors.push_back(EventSlot<EventType>::clear_slots);
				}
			};
			const initializer ini = initializer();
		};
		template<class EventType>
		void pushEvent(EventType pEvent) {
			EventSlot<EventType>::eventBuffer.push_back(pEvent);
		}

		template<class EventType>
		size_t createEvent(EventType&& pEvent) {
			EventSlot<EventType>::slots.emplace_back(allEventStates.size(), pEvent);
			allEventStates.push_back(false);
			return allEventStates.size()-1;
		}

		template<class EventType>
		std::vector<EventSlot<EventType>> EventSlot<EventType>::slots = std::vector<EventSlot<EventType>>();

		template<class EventType>
		std::vector<EventType> EventSlot<EventType>::eventBuffer = std::vector<EventType>();


		extern std::vector<bool> allEventStates;//shows the events which occurred each frame
		extern std::vector<void(*)()> eventCheckers;//checks all EventSlot templates
		extern std::vector<void(*)()> eventDestructors;//destroys all EventSlot templates

	}
}