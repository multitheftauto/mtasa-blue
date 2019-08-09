#pragma once

#include "Extension.h"
#include "Stream.h"
#include "RwTypes.h"

struct gtaMaterialEnvMap : public gtaRwExtension
{
	gtaRwReal scaleX;
	gtaRwReal scaleY;
	gtaRwReal transSclX;
	gtaRwReal transSclY;
	gtaRwReal shininess;
	gtaRwInt32 texture; // always NULL

	gtaMaterialEnvMap();

	bool StreamWrite(gtaRwStream *stream);

	bool StreamRead(gtaRwStream *stream);
	
	unsigned int GetStreamSize();

	void Initialise(gtaRwReal ScaleX, gtaRwReal ScaleY, gtaRwReal TransSclX, gtaRwReal TransSclY, gtaRwReal Shininess);

	void Destroy();
};