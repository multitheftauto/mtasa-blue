#pragma once

#include "Material.h"

struct gtaRwMaterialList
{
	gtaRwUInt32 materialCount;
	gtaRwInt32 *materialValues; // array of "-1"
	gtaRwMaterial *materials;
	
	gtaRwMaterialList();

	bool StreamRead(gtaRwStream *stream);

	bool StreamWrite(gtaRwStream *stream);

	unsigned int GetStreamSize();

	void Initialise(gtaRwUInt32 MaterialCount);

	void Destroy();
};