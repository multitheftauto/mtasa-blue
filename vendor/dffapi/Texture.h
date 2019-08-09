#pragma once

#include "Anisot.h"
#include "RwTypes.h"
#include "Stream.h"
#include "String.h"
#include "SkyMipmap.h"

struct gtaRwTexture
{
	gtaRwUInt8 filtering;
	gtaRwUInt8 uAddressing : 4;
	gtaRwUInt8 vAddressing : 4;
	gtaRwUInt8 generateMipMaps : 1;
	gtaRwUInt8 padding;
	gtaRwString name;
	gtaRwString maskName;
	struct{
		gtaRwTextureSkyMipmap skyMipmap;
		gtaRwTextureAnisot anisot;
	} Extension;

	gtaRwTexture();

	bool StreamRead(gtaRwStream *stream);

	bool StreamWrite(gtaRwStream *stream);

	unsigned int GetStreamSize();

	void Initialise(gtaRwTextureFilterMode Filtering, gtaRwTextureAddressMode UAddressing, gtaRwTextureAddressMode VAddressing,
		gtaRwBool AutoMipMapping, gtaRwChar *Name, gtaRwChar *MaskName);

	void Destroy();
};