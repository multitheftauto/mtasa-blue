#ifndef __ROUTER_INTERFACE_H
#define __ROUTER_INTERFACE_H

#include "Export.h"
#include "RakNetTypes.h"

/// On failed directed sends, RakNet can call an alternative send function to use.
class RAK_DLL_EXPORT RouterInterface
{
public:
	RouterInterface() {}
	virtual ~RouterInterface() {}

	virtual bool Send( const char *data, BitSize_t bitLength, PacketPriority priority, PacketReliability reliability, char orderingChannel, SystemAddress systemAddress )=0;
};

#endif
