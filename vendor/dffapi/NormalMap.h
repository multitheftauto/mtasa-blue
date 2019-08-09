#pragma once

#include "Extension.h"
#include "Stream.h"
#include "RwTypes.h"
#include "Texture.h"

struct gtaRwMaterialNormalMap : public gtaRwExtension
{
	union{
		gtaRwUInt32 flags;
		struct{
			gtaRwUInt32 normalMapUsed : 1; // do we use this section at all?
			gtaRwUInt32 notUsed : 3; // this field is not used
			gtaRwUInt32 envMapUsed : 1; // do we use env map
			gtaRwUInt32 envMapModulate : 1; // combine bump and env (bump * env)
		};
	};
	gtaRwTexture normalMapTexture; // normal map texture
	float envMapCoefficient; // env map intensity
	gtaRwTexture envMapTexture; // env map texture

	gtaRwMaterialNormalMap();

	bool StreamWrite(gtaRwStream *stream);

	bool StreamRead(gtaRwStream *stream);
	
	unsigned int GetStreamSize();

	void Initialise(gtaRwBool NormalMapUsed, gtaRwBool EnvMapUsed, gtaRwReal EnvMapCoefficient, gtaRwBool EnvMapModulate);

	void Destroy();
};