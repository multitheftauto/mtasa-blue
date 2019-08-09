#pragma once

#include "Extension.h"
#include "Stream.h"
#include "RwTypes.h"

struct gtaRwTextureAnisot : public gtaRwExtension
{
	gtaRwUInt32 anisotropyLevel;

	gtaRwTextureAnisot();

	gtaRwBool StreamWrite(gtaRwStream *Stream);

	gtaRwBool StreamRead(gtaRwStream *Stream);
	
	gtaRwUInt32 GetStreamSize();

	void Initialise(gtaRwUInt32 AnisotropyLevel);

	void Destroy();
};