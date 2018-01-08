#pragma once
#include "Quad.h"
#include <tuple>
#include "Colorings.h"
#include <glm\glm.hpp>
#include <initializer_list>
#include "GUI.h"
#include "../../Input/Event.h"
#include "../../Input/EventGates.h"
#include "../../Input/Signal.h"
#include "../../Input/Mouse.h"
#include "Widget.h"
namespace gui {
	
	template<typename OnType, typename OffType = OnType>
	struct ButtonEvents {
		ButtonEvents(OnType pOn, OffType pOff)
			:on_evt(pOn),
			off_evt(pOff),
			hold_evt(events::switch_gate<OnType, OffType>(on_evt, off_evt)),
			on(signals::createSignal(on_evt)),
			off(signals::createSignal(off_evt)),
			hold(signals::createSignal(hold_evt))
		{}
		OnType on_evt;
		OffType off_evt;
		events::switch_gate<OnType, OffType> hold_evt;
		size_t on;
		size_t off;
		size_t hold;
	};

}

