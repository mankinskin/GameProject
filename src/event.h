#pragma once

namespace signals
{
    template<typename ObjectType, typename StateType>
        struct Event
        {
            Event(ObjectType o, StateType s)
                : obj(o)
                , state(s)
            {}
            ObjectType obj;
            StateType state;
        };

    template<typename ObjectType, typename StateType>
    inline bool operator==(const Event<ObjectType, StateType>& l, const Event<ObjectType, StateType>& r)
    {
        return l.obj == r.obj && l.state == r.state;
    }

    extern std::vector<void(*)()> signalClearFuncs;
    extern std::vector<void(*)()> eventCheckFuncs;
    extern std::vector<void(*)()> eventInstanceClearFuncs;

    template<typename E>
        class EventListener
        {
            public:
                using ID = utils::ID<EventListener<E>>;
                static constexpr typename ID::Container& all = ID::container;
                constexpr EventListener(const E pSignature)
                    : signature(pSignature)
                    , occurred(false)
                {
                    initialize();
                }

                static void pushEvent(const E& pEvent)
                {
                    eventBuffer.emplace_back(pEvent);
                }

                bool stat() const
                {
                    return occurred;
                }

                static bool stat(size_t i)
                {
                    return all[i].stat();
                }

                static void clear()
                {
                    all.clear();
                }

                const EventListener* operator->() const
                {
                    this;
                }

                const E signature;
                bool occurred = false;

            private:
                static std::vector<E> eventBuffer;
                static void check()
                {
                    for (EventListener<E>& instance : all) {
                        instance.occurred = false;
                        for (const E& event : EventListener<E>::eventBuffer) {
                            if(instance.signature == event) {
                                instance.occurred = true;
                                break;
                            }
                        }
                    }
                    eventBuffer.clear();
                }
                static void initialize()
                {
                    static At_Init init;
                }

                struct At_Init
                {
                    constexpr At_Init()
                    {
                        eventCheckFuncs.push_back(&check);
                        signalClearFuncs.push_back(&clear);
                    }
                };
        };

    template<typename E>
        std::vector<E> EventListener<E>::eventBuffer = std::vector<E>();

    template<typename ObjectType, typename StateType>
        constexpr const utils::ID<EventListener<Event<ObjectType, StateType>>> ifEvent(const ObjectType pEvent, const StateType pState)
        {
            return utils::makeID(EventListener<Event<ObjectType, StateType>>(Event<ObjectType, StateType>(pEvent, pState)));
        }
    template<typename E>
        constexpr const utils::ID<EventListener<E>> ifEvent(const E pEvent)
        {
            return utils::makeID(EventListener<E>(pEvent));
        }

    template<typename E>
        void pushEvent(const E pEvent)
        {
            EventListener<E>::pushEvent(pEvent);
        }

    template<typename ObjectType, typename StateType>
        void pushEvent(const ObjectType pObject, const StateType pState)
        {
            EventListener<Event<ObjectType, StateType>>::pushEvent(Event<ObjectType, StateType>(pObject, pState));
        }
}
