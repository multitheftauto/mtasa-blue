#pragma once

#include "Extension.h"
#include "Stream.h"
#include "RwTypes.h"

#define rpHANIMSTREAMCURRENTVERSION 0x100
// Flags for gtaRwHAnimNode
#define rpHANIMPOPPARENTMATRIX      0x01
#define rpHANIMPUSHPARENTMATRIX     0x02

enum gtaRwHAnimHierarchyFlag
{
    // creation flags
    rpHANIMHIERARCHYSUBHIERARCHY =              0x01, // This hierarchy is a sub-hierarchy
    rpHANIMHIERARCHYNOMATRICES =                0x02, // This hierarchy has no local matrices
    // update flags
    rpHANIMHIERARCHYUPDATEMODELLINGMATRICES = 0x1000, // This hierarchy updates modeling matrices
    rpHANIMHIERARCHYUPDATELTMS =              0x2000, // This hierarchy updates LTMs
    rpHANIMHIERARCHYLOCALSPACEMATRICES =      0x4000, // This hierarchy calculates matrices in a space
};

struct gtaRwHAnimNode
{
	gtaRwInt32 nodeID;    // User defined ID for this node
	gtaRwInt32 nodeIndex; // Array index of node
	union{
		gtaRwInt32 flags;     // Matrix push/pop flags
		struct{
			gtaRwInt32 popParentMatrix : 1;
			gtaRwInt32 pushParentMatrix : 1;
		};
	};

	gtaRwHAnimNode();

	void Initialise(gtaRwInt32 NodeID, gtaRwInt32 NodeIndex, gtaRwInt32 Flags);

	void Destroy();
};

struct gtaRwFrameHAnim : public gtaRwExtension
{
	gtaRwUInt32 hAnimVersion;
	gtaRwInt32 id;
	gtaRwUInt32 numNodes; // Number of nodes in the hierarchy
	union{
		gtaRwUInt32 flags; // Flags for the hierarchy
		struct{
			gtaRwUInt32 subHierarchy : 1;
			gtaRwUInt32 noMatrices : 1;
			gtaRwUInt32 unused : 10;
			gtaRwUInt32 updateModellingMatrices : 1;
			gtaRwUInt32 updateLTMs : 1;
			gtaRwUInt32 localSpaceMatrices : 1;
		};
	};
	gtaRwUInt32 keyFrameSize;
	gtaRwHAnimNode *nodes; // Array of node information (push/pop flags etc)

	gtaRwFrameHAnim();

	void Initialise(gtaRwInt32 Id);

	void Initialise(gtaRwInt32 Id, gtaRwUInt32 NumNodes, gtaRwUInt32 Flags, gtaRwUInt32 KeyFrameSize);

	void Destroy();

	bool StreamWrite(gtaRwStream *stream);

	bool StreamRead(gtaRwStream *stream);
	
	unsigned int GetStreamSize();
};