#pragma once
#include "gates.h"
#include "utils/id.h"
#include <vector>
#include <tuple>

namespace signals
{
    namespace hidden
    {
        extern std::vector<void(*)()> eventCheckFuncs;
        extern std::vector<void(*)()> eventInstanceClearFuncs;

        template<typename Event>
            class EventListener
            {
                public:
                    using ID = utils::ID<EventListener<Event>>;
                    static constexpr typename ID::Container& all = ID::container;

                    EventListener(const Event pSignature)
                        : signature(pSignature)
                        , occurred(false)
                    {
                        (void)init;
                    }

                    static void pushEvent(const Event pEvent)
                    {
                        eventBuffer.push_back( pEvent );
                    }

                    static void clearBuffer()
                    {
                        eventBuffer.clear();
                    }

                    static void clear()
                    {
                        all.clear();
                    }

                    bool stat() const
                    {
                        return occurred;
                    }

                    static bool stat(size_t i)
                    {
                        return all[i].stat();
                    }

                    const Event signature;
                    bool occurred = false;

                    static void check()
                    {
                        for (EventListener<Event>& instance : all) {
                            instance.occurred = false;
                            for (const Event& event : eventBuffer) {
                                if(instance.signature == event) {
                                    instance.occurred = true;
                                    break;
                                }
                            }
                        }
                        clearBuffer();
                    }

                private:
                    static std::vector<Event> eventBuffer;
                    struct At_Init
                    {
                        At_Init()
                        {
                            eventCheckFuncs.push_back(&EventListener<Event>::check);
                            eventInstanceClearFuncs.push_back(&EventListener<Event>::clear);
                        }
                    };
                    static At_Init init;
            };

        template<typename Event>
            std::vector<Event> EventListener<Event>::eventBuffer = std::vector<Event>();

        template<typename Event>
            typename EventListener<Event>::At_Init EventListener<Event>::init;
    }

    template<typename Event>
        void pushEvent(const Event pEvent)
        {
            hidden::EventListener<Event>::pushEvent(pEvent);
        }

    template<typename Event>
        constexpr utils::ID<hidden::EventListener<Event>> eventsignal(const Event pEvent)
        {
            return utils::ID<hidden::EventListener<Event>>(pEvent);
        }

    void checkEvents();
    void clearEvents();
}
