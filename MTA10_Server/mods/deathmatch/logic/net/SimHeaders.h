/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CSimPacket;
class CSimPlayerManager;

#include <sys/timeb.h>
#include "CNetBufferUtil.h"
#include "CNetBuffer.h"
#include "CSimPlayer.h"
#include "CSimPlayerManager.h"
#include "CSimPlayerPuresyncPacket.h"
#include "CSimVehiclePuresyncPacket.h"

extern CNetServer* g_pRealNetServer;
