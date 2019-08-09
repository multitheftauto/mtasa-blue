#pragma once

#include "Extension.h"
#include "Stream.h"
#include "RwTypes.h"
#include "StdInc.h"
#include "Texture.h"

struct gtaRwMatFXBump
{
	gtaRwReal bumpiness;
	gtaRwBool hasTexture;
	gtaRwTexture texture;
	gtaRwBool hasBumpedTexture;
	gtaRwTexture bumpedTexture;

	gtaRwMatFXBump();

	bool StreamWrite(gtaRwStream *stream);

	bool StreamRead(gtaRwStream *stream);
	
	unsigned int GetStreamSize();
};

struct gtaRwMatFXEnv
{
	gtaRwReal coefficient;
	gtaRwBool useFrameBufferAlpha;
	gtaRwBool hasTexture;
	gtaRwTexture texture;

	gtaRwMatFXEnv();

	bool StreamWrite(gtaRwStream *stream);

	bool StreamRead(gtaRwStream *stream);
	
	unsigned int GetStreamSize();
};

struct gtaRwMatFXDual
{
	gtaRwUInt32 srcBlendMode;
	gtaRwUInt32 dstBlendMode;
	gtaRwBool hasTexture;
	gtaRwTexture texture;

	gtaRwMatFXDual();

	bool StreamWrite(gtaRwStream *stream);

	bool StreamRead(gtaRwStream *stream);
	
	unsigned int GetStreamSize();
};

struct gtaRwMatFxEffect
{
	gtaRwUInt32 type;
	gtaRwMatFXBump bump;
	gtaRwMatFXEnv env;
	gtaRwMatFXDual dual;

	void SetupNull();

	void SetupBump(gtaRwReal Bumpiness, gtaRwBool HasTexture, gtaRwBool HasBumpedTexture);

	void SetupEnv(gtaRwReal Coefficient, gtaRwBool UseFrameBufferAlpha, gtaRwBool HasTexture);

	void SetupDual(gtaRwUInt32 SrcBlendMode, gtaRwUInt32 DstBlendMode, gtaRwBool HasTexture);

	void SetupUvTransform();

	bool StreamWrite(gtaRwStream *stream);

	bool StreamRead(gtaRwStream *stream);
	
	unsigned int GetStreamSize();
};

struct gtaRwMaterialMatFX : public gtaRwExtension
{
	gtaRwUInt32 effectType;
	gtaRwMatFxEffect effect1;
	gtaRwMatFxEffect effect2; // if second effect is not used, it must be effect with rpMATFXEFFECTNULL type

	gtaRwMaterialMatFX();

	void Initialise(RpMatFXMaterialFlags EffectType);

	void Destroy();

	bool StreamWrite(gtaRwStream *stream);

	bool StreamRead(gtaRwStream *stream);
	
	unsigned int GetStreamSize();
};

struct gtaRwAtomicMatFx : public gtaRwExtension
{
	gtaRwUInt32 matFxEnabled;

	gtaRwAtomicMatFx();

	gtaRwBool StreamWrite(gtaRwStream *Stream);

	gtaRwBool StreamRead(gtaRwStream *Stream);
	
	gtaRwUInt32 GetStreamSize();

	void Initialise(gtaRwUInt32 MatFxEnabled);

	void Destroy();
};