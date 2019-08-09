#pragma once

#include "Extension.h"
#include "Memory.h"

gtaRwExtension::gtaRwExtension()
{
	gtaMemZero(this, sizeof(gtaRwExtension));
}

void gtaRwExtension::Enable()
{
	enabled = true;
}

void gtaRwExtension::Disable()
{
	enabled = false;
}