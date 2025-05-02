/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

class CSimPacket;
class CSimPlayerManager;

#include "SharedUtil.Thread.h"
#include "CNetBufferWatchDog.h"
#include "CNetBuffer.h"
#include "CSimPlayer.h"
#include "CSimPlayerManager.h"
#include "CSimPlayerPuresyncPacket.h"
#include "CSimVehiclePuresyncPacket.h"
#include "CSimKeysyncPacket.h"
#include "CSimBulletsyncPacket.h"
#include "CSimPedTaskPacket.h"

extern CNetServer* g_pRealNetServer;
