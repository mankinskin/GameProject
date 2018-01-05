#pragma once
#include "Quad.h"
#include <tuple>
#include "Colorings.h"
#include <glm\glm.hpp>
#include <initializer_list>
#include "GUI.h"
#include "../../Input/Event.h"
#include "../../Input/Mouse.h"
#include "Widget.h"
namespace gui {
	
	template<typename OnType, typename OffType = OnType>
	struct ButtonEvents {
		ButtonEvents(OnType pOn, OffType pOff)
			:on_evt(pOn),
			off_evt(pOff),
			hold_evt(app::Input::switch_gate<OnType, OffType>(on_evt, off_evt)),
			on(createSignal(on_evt)),
			off(createSignal(off_evt)),
			hold(app::Input::createSignal(hold_evt)) 
		{}
		OnType on_evt;
		OffType off_evt;
		app::Input::switch_gate<OnType, OffType> hold_evt;
		size_t on;
		size_t off;
		size_t hold;
	};

}

