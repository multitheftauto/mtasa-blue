/// \file
/// \brief \b [Internal] A container class for a list of RPCNodes
///
/// \ingroup RAKNET_RPC
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __RPC_MAP
#define __RPC_MAP

#include "RakMemoryOverride.h"
#include "RPCNode.h"
#include "DS_List.h"
#include "RakNetTypes.h"
#include "Export.h"

/// \ingroup RAKNET_RPC 
/// \internal
/// \brief A container class for a list of RPCNodes
struct RAK_DLL_EXPORT RPCMap
{
public:
	RPCMap();
	~RPCMap();
	void Clear(void);
    RPCNode *GetNodeFromIndex(RPCIndex index);
	RPCNode *GetNodeFromFunctionName(const char *uniqueIdentifier);
	RPCIndex GetIndexFromFunctionName(const char *uniqueIdentifier);
	void AddIdentifierWithFunction(const char *uniqueIdentifier, void *functionPointer, bool isPointerToMember);
	void AddIdentifierAtIndex(const char *uniqueIdentifier, RPCIndex insertionIndex);
	void RemoveNode(const char *uniqueIdentifier);
protected:
	DataStructures::List<RPCNode *> rpcSet;
};

#endif

