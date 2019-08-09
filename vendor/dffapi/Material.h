#pragma once

#include "Texture.h"
#include "UVAnim.h"
#include "EnvMap.h"
#include "SpecMap.h"
#include "MatFX.h"
#include "NormalMap.h"

struct gtaRwMaterial
{
	gtaRwInt32 flags;   // Material flags - unused currently - for future expansion
	gtaRwRGBA color;    // Color of material
	gtaRwInt32 unused;  // Not used
	gtaRwBool textured; // Are we textured?
	gtaRwSurfaceProperties surfaceProps; // Surface properties
	gtaRwTexture texture;
	struct {
		gtaRwMaterialUVAnim uvAnim;
		gtaRwMaterialMatFX matFx;
		gtaRwMaterialNormalMap normalMap;
		gtaMaterialEnvMap envMap;
		gtaMaterialSpecMap specMap;
	} Extension;

	gtaRwMaterial();

	bool StreamRead(gtaRwStream *stream);

	bool StreamWrite(gtaRwStream *stream);

	unsigned int GetStreamSize();

	void Initialise(gtaRwUInt8 Red, gtaRwUInt8 Green, gtaRwUInt8 Blue, gtaRwUInt8 Alpha, gtaRwBool Textured, gtaRwReal Ambient, gtaRwReal Diffuse);

	void Destroy();
};