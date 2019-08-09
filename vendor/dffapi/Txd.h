#pragma once

#include "StdInc.h"
#include "TextureNative.h"

struct gtaRwTexDictionary
{
	gtaRwUInt32 version;
	gtaRwUInt32 build;

	gtaRwUInt16 numTextures;
	gtaRwUInt8 deviceId;
	gtaRwInt8 _zero;
	gtaRwTextureNative *textures;

	void Initialise(gtaRwUInt16 NumTextures);

	void Destroy();

	gtaRwTexDictionary();

	gtaRwBool StreamRead(gtaRwStream *stream);

	gtaRwBool StreamWrite(gtaRwStream *stream);

	gtaRwUInt32 GetStreamSize();
};