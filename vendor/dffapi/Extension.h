#pragma once

#include "RwTypes.h"

struct gtaRwExtension
{
	gtaRwBool enabled;

	gtaRwExtension();

	void Enable();

	void Disable();
};