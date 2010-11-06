#pragma message("Compiling precompiled header.\n")

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x500

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <mmsystem.h>
#include <winsock.h>

#include <algorithm>
#include <list>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <vector>
#include <cstdio>
#include <cstring>

// SDK includes
#include "SharedUtil.h"
#include "SString.h"
#include <core/CCoreInterface.h>
#include <core/CExceptionInformation.h>
#include <xml/CXML.h>
#include <xml/CXMLNode.h>
#include <xml/CXMLFile.h>
#include <xml/CXMLAttributes.h>
#include <xml/CXMLAttribute.h>
#include <net/CNet.h>
#include <net/packetenums.h>
#include <game/CGame.h>
#include <CVector.h>
#include <CMatrix4.h>
#include <CQuat.h>
#include <CSphere.h>
#include <CBox.h>
#include <ijsify.h>
#include <Common.h>

// Shared logic includes
#include <Utils.h>
#include <CClientCommon.h>
#include <CClientManager.h>
#include <CClient3DMarker.h>
#include <CClientCheckpoint.h>
#include <CClientColShape.h>
#include <CClientColCircle.h>
#include <CClientColCuboid.h>
#include <CClientColSphere.h>
#include <CClientColRectangle.h>
#include <CClientColPolygon.h>
#include <CClientColTube.h>
#include <CClientCorona.h>
#include <CClientCRC32Hasher.h>
#include <CClientDFF.h>
#include <CClientDisplayGroup.h>
#include <CClientDummy.h>
#include <CClientEntity.h>
#include <CClientSpatialDatabase.h>
#include <CClientExplosionManager.h>
#include <CClientHandling.h>
#include <CClientPed.h>
#include <CClientPlayerClothes.h>
#include <CClientProjectileManager.h>
#include <CClientStreamSector.h>
#include <CClientStreamSectorRow.h>
#include <CClientTask.h>
#include <CClientTXD.h>
#include <CClientWater.h>
#include <CClientIFP.h>
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

// Deathmatch includes
#include "Client.h"
#include "ClientCommands.h"
#include "CClient.h"
#include "CEvents.h"
#include "HeapTrace.h"
#include "logic/CClientGame.h"
#include "net/Packets.h"
#include "logic/CDeathmatchVehicle.h"
#include "logic/CResource.h"
#include "logic/CStaticFunctionDefinitions.h"
#include "../../version.h"
