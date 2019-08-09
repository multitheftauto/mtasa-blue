#pragma once

#include "Extension.h"
#include "Stream.h"
#include "RwTypes.h"

struct gtaMaterialSpecMap : public gtaRwExtension
{
	gtaRwReal specularity;
	gtaRwChar textureName[24];

	gtaMaterialSpecMap();

	bool StreamWrite(gtaRwStream *stream);

	bool StreamRead(gtaRwStream *stream);
	
	unsigned int GetStreamSize();

	void Initialise(gtaRwReal Specularity, gtaRwChar *TextureName = NULL);

	void Destroy();
};