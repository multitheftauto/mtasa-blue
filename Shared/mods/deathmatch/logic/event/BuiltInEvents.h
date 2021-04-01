#pragma once

#include "Event.h"

struct BuiltInEvents
{
#define DECLARE_EVENT(name, id) inline static const BuiltInEvent name{#name, BuiltInEvent::ID::id}

	DECLARE_EVENT(OnElementDataChange, ON_ELEMENT_DATA_CHANGE);

#undef DECLARE_EVENT
	inline static constexpr auto COUNT{ static_cast<size_t>(BuiltInEvent::ID::COUNT) };
};
