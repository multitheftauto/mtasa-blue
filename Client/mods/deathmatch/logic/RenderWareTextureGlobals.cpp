#include "StdInc.h"
#include <game/CRenderWare.h>

// Provide the expected RenderWare texture destructor pointer to deathmatch project
int (__cdecl* RwTextureDestroy)(RwTexture* texture) = reinterpret_cast<int(__cdecl*)(RwTexture*)>(0x007F3820);
