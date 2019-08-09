#pragma once

#include "Extension.h"
#include "Stream.h"
#include "RwTypes.h"
#include "StdInc.h"

struct gtaRwMaterialUVAnim : public gtaRwExtension
{
	union{
		gtaRwUInt32 animSlotsMap;
		struct{
			gtaRwUInt32 animSlots1Used : 1;
			gtaRwUInt32 animSlots2Used : 1;
			gtaRwUInt32 animSlots3Used : 1;
			gtaRwUInt32 animSlots4Used : 1;
			gtaRwUInt32 animSlots5Used : 1;
			gtaRwUInt32 animSlots6Used : 1;
			gtaRwUInt32 animSlots7Used : 1;
			gtaRwUInt32 animSlots8Used : 1;
		};
	};
	gtaRwChar *animNames;
	gtaRwUInt32 numAnimNames;
	gtaRwUInt32 currentSlot;

	gtaRwMaterialUVAnim();

	void Initialise(gtaRwUInt32 NumSlots);

	void SetupAnim(gtaRwUInt32 Slot, gtaRwChar *AnimName);

	void Destroy();

	bool StreamWrite(gtaRwStream *stream);

	bool StreamRead(gtaRwStream *stream);
	
	unsigned int GetStreamSize();
};