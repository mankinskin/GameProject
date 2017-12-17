#pragma once
namespace app {
	namespace Input {

		struct SignalSlot {
			SignalSlot(int pRemain = 0, int pStartWith = 0)
				:signaled(pStartWith), remain(pRemain)
			{}
			int signaled;
			int remain;
		};

		struct SignalEvent {
			SignalEvent(size_t pSignalIndex, size_t pSignaled)
				:signal(pSignalIndex), state(pSignaled) {}
			size_t signal;//index
			size_t state;//on or off
		};

		inline bool operator==(SignalEvent& lhs, SignalEvent& rhs) {
			return lhs.signal == rhs.signal && lhs.state == rhs.state;
		}
		void checkEvents();
		void clearEvents();
		void clearSignals();
		void resetSignals();
		extern std::vector<SignalSlot> allSignals;

		size_t createSignal(int pRemain = 0, int pStartWith = 0);
		template<class EventType>
		struct EventSignal {
			EventSignal() {}
			EventSignal(EventType pEvent, size_t pSignalIndex = createSignal(), int pSetTo = 1)
				:signal_index(pSignalIndex)
			{	}

			static void reserve(size_t pCount) {
				EventInternal::EventSlot<EventType>::reserve_slots(pCount);
			}
			size_t index() {
				return signal_index;
			}

		private:
			size_t signal_index;
		};
		template<class EventType>
		void pushEvent(EventType pEvent) {
			EventInternal::EventSlot<EventType>::eventBuffer.push_back(pEvent);
		}

		template<class EventType>
		size_t createEvent(EventType&& pEvent, size_t pSignalIndex = createSignal(), int pSetTo = 1) {
			using namespace EventInternal;
			EventInternal::EventSlot<EventType>::createNew(pEvent, pSignalIndex, pSetTo);
			return pSignalIndex;
		}
		namespace EventInternal {
			template<class EventType>
			class EventSlot {
			public:
				EventSlot(EventType pEvent, size_t pSignalIndex, int pSetTo = 1)
					:event_signature(pEvent), signalIndex(pSignalIndex), set_to(pSetTo) {}

				static void createNew(EventType& pEvent, size_t& pSignalIndex, int& pSetTo) {
					slots.emplace_back(pEvent, pSignalIndex, pSetTo);
				}
				static void reserve_slots(size_t pCount) {
					allSignals.reserve(allSignals.size() + pCount);
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
								slot.setSignal();
								EventSlot<SignalEvent>::eventBuffer.emplace_back(slot.signalIndex, slot.set_to);//an event that this signal got triggered
							}
						}
					}
					EventSlot<EventType>::eventBuffer.clear();
				}
				static std::vector<EventSlot<EventType>> slots;//all event slots of this type
				static std::vector<EventType> eventBuffer;
				void setSignal() {
					allSignals[signalIndex].signaled = set_to;
				}
			private:
				int set_to = 1;//set signal to this
				EventType event_signature;
				size_t signalIndex;
				struct initializer {
					initializer() {
						eventCheckers.push_back(EventSlot<EventType>::checkEvents);
						eventDestructors.push_back(EventSlot<EventType>::clear_slots);
					}
				};
				const initializer ini = initializer();
			};
			template<class EventType>
			std::vector<EventSlot<EventType>> EventSlot<EventType>::slots = std::vector<EventSlot<EventType>>();

			template<class EventType>
			std::vector<EventType> EventSlot<EventType>::eventBuffer = std::vector<EventType>();



			extern std::vector<void(*)()> eventCheckers;//checks all EventSlot templates
			extern std::vector<void(*)()> eventDestructors;//destroys all EventSlot templates

		}//END INTERNAL


	}
}