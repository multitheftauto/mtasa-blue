#pragma message("Compiling precompiled header.\n")

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x500

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <mmsystem.h>
#include <winsock.h>

#include <list>
#include <map>
#include <set>
#include <vector>
#include <cstdio>
#include <cstring>

// SDK includes
#include <core/CCoreInterface.h>
#include <core/CExceptionInformation.h>
#include <xml/CXML.h>
#include <xml/CXMLNode.h>
#include <xml/CXMLFile.h>
#include <xml/CXMLAttributes.h>
#include <xml/CXMLAttribute.h>
#include <net/CNet.h>
#include <net/net_packetenums.h>
#include <game/CGame.h>
#include <CVector.h>
#include <CMatrix4.h>
#include <CQuat.h>
#include <ijsify.h>
#include <Common.h>

// Deathmatch includes
#include "Client.h"
#include "ClientCommands.h"
#include "CClient.h"
#include "HeapTrace.h"
#include "logic/CClientGame.h"
#include "logic/Packets.h"
#include "logic/CDeathmatchVehicle.h"
#include "logic/CResource.h"
#include "logic/CStaticFunctionDefinitions.h"
#include "logic/CSFX.h"

// Shared logic includes
#include <Utils.h>
#include <CClientCommon.h>
#include <CClient3DMarker.h>
#include <CClientCheckpoint.h>
#include <CClientCorona.h>
#include <CClientCRC32Hasher.h>
#include <CClientDFF.h>
#include <CClientDisplayGroup.h>
#include <CClientDummy.h>
#include <CClientEntity.h>
#include <CClientExplosionManager.h>
#include <CClientHandling.h>
#include <CClientPed.h>
#include <CClientPlayerClothes.h>
#include <CClientProjectileManager.h>
#include <CClientStreamSector.h>
#include <CClientStreamSectorRow.h>
#include <CClientTask.h>
#include <CClientTextDisplayBackgroundEffect.h>
#include <CClientTextDisplayFadeEffect.h>
#include <CClientTextDisplayGlowEffect.h>
#include <CClientTXD.h>
#include <CCustomData.h>
#include <CConfig.h>
#include <CDOMConfig.h>
#include <CElementArray.h>
#include <CLogger.h>
#include <CMapEventManager.h>
#include <CScriptFile.h>
#include <CWeaponNames.h>
#include <CVehicleNames.h>
#include <utils/CMD5Hasher.h>
#include <lua/CLuaCFunctions.h>
#include <lua/CLuaArguments.h>
#include <lua/CLuaMain.h>
#include <luadefs/CLuaDefs.h>
#include <luadefs/CLuaTaskDefs.h>
#include <luadefs/CLuaFxDefs.h>
#include <luadefs/CLuaFileDefs.h>
