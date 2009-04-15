INCLUDES += -I$(srcdir)/logic/
INCLUDES += -I$(srcdir)/../../sdk/
INCLUDES += -I$(srcdir)/../../../Shared/sdk/
INCLUDES += -I$(srcdir)/../../../Shared/publicsdk/include/
INCLUDES += -I$(srcdir)/../../../vendor/
INCLUDES += -I$(srcdir)/../../../vendor/pme/
INCLUDES += -I$(srcdir)/../../../vendor/json-c/

if PRECOMPILE_HEADERS
BUILT_SOURCES = StdInc.h.gch
AM_CXXFLAGS += -Winvalid-pch

StdInc.h.gch: StdInc.h
	-rm -f StdInc.h.gch
	$(CXX) -fPIC -DPIC $(DEFS) $(DEFAULT_INCLUDES) \
		$(AM_CXXFLAGS) $(CXXFLAGS) $(CPPFLAGS) \
		$(INCLUDES) -o StdInc.h.gch -xc++-header StdInc.h

clean-local:
	-rm -f StdInc.h.gch
endif

lib_LTLIBRARIES = \
	libdeathmatch.la

libdeathmatch_la_SOURCES = \
	Server.cpp \
	CServer.cpp \
	utils/base64.cpp \
	utils/CHTTPClient.cpp \
	utils/CTCPImpl.cpp \
	utils/CHTTPResponse.cpp \
	utils/zip/crc32.cpp \
	utils/zip/ioapi.c \
	utils/zip/mztools.c \
	utils/zip/unzip.c \
	utils/zip/zip.c \
	utils/CHTTPRequest.cpp \
	utils/CTCPClientSocketImpl.cpp \
	logic/CLogger.cpp \
	logic/CAccessControlListGroup.cpp \
	logic/CAccessControlList.cpp \
	logic/CClient.cpp \
	logic/CResource.cpp \
	logic/CScriptDebugging.cpp \
	logic/CSerialManager.cpp \
	logic/CPlayerManager.cpp \
	logic/CColShape.cpp \
	logic/CRemoteCalls.cpp \
	logic/CColCircle.cpp \
	logic/CConsoleClient.cpp \
	logic/CColRectangle.cpp \
	logic/CZoneNames.cpp \
	logic/CWeaponNames.cpp \
	logic/CColPolygon.cpp \
	logic/CObjectManager.cpp \
	logic/CStaticFunctionDefinitions.cpp \
	logic/CRadarAreaManager.cpp \
	logic/CBlip.cpp \
	logic/CVehicleUpgrades.cpp \
	logic/CMapEvent.cpp \
	logic/CVehicleManager.cpp \
	logic/CPedManager.cpp \
	logic/CResourceConfigItem.cpp \
	logic/CClock.cpp \
	logic/CBan.cpp \
	logic/CResourceClientFileItem.cpp \
	logic/CMapFiles.cpp \
	logic/CMD5Hasher.cpp \
	logic/CConsoleCommands.cpp \
	logic/CBanManager.cpp \
	logic/TaskNames.cpp \
	logic/CElementGroup.cpp \
	logic/CAccountManager.cpp \
	logic/CConsoleCommand.cpp \
	logic/ASE.cpp \
	logic/CElementDeleter.cpp \
	logic/Utils.cpp \
	logic/CVehicleColorManager.cpp \
	logic/CSettings.cpp \
	logic/CResourceScriptItem.cpp \
	logic/CElementIDs.cpp \
	logic/CRadarArea.cpp \
	logic/CResourceClientConfigItem.cpp \
	logic/CAccessControlListRight.cpp \
	logic/CDummy.cpp \
	logic/CCommandLineParser.cpp \
	logic/CResourceChecker.cpp \
	logic/CResourceDownloader.cpp \
	logic/CMapManager.cpp \
	logic/CHTTPD.cpp \
	logic/CResourceManager.cpp \
	logic/CCustomData.cpp \
	logic/luadefs/CLuaTextDefs.cpp \
	logic/luadefs/CLuaACLDefs.cpp \
	logic/luadefs/CLuaWorldDefs.cpp \
	logic/luadefs/CLuaCameraDefs.cpp \
	logic/luadefs/CLuaHTTPDefs.cpp \
	logic/luadefs/CLuaXMLDefs.cpp \
	logic/luadefs/CLuaPickupDefs.cpp \
	logic/luadefs/CLuaElementDefs.cpp \
	logic/luadefs/CLuaDefs.cpp \
	logic/luadefs/CLuaResourceDefs.cpp \
	logic/luadefs/CLuaHandlingDefs.cpp \
	logic/luadefs/CLuaFileDefs.cpp \
	logic/CTextDisplay.cpp \
	logic/CMainConfig.cpp \
	logic/CPlayerStats.cpp \
	logic/CPlayerClothes.cpp \
	logic/CPickup.cpp \
	logic/lua/CLuaFunctionDefinitions.cpp \
	logic/lua/LuaCommon.cpp \
	logic/lua/CLuaCFunctions.cpp \
	logic/lua/CLuaModuleManager.cpp \
	logic/lua/CLuaTimer.cpp \
	logic/lua/CLuaArguments.cpp \
	logic/lua/CLuaTimerManager.cpp \
	logic/lua/CLuaMain.cpp \
	logic/lua/CLuaArgument.cpp \
	logic/lua/CLuaManager.cpp \
	logic/CMarker.cpp \
	logic/CCommandFile.cpp \
	logic/CMarkerManager.cpp \
	logic/CConsole.cpp \
	logic/CRPCFunctions.cpp \
	logic/CRegisteredCommands.cpp \
	logic/CTeam.cpp \
	logic/CWhoWas.cpp \
	logic/CUnoccupiedVehicleSync.cpp \
	logic/CPed.cpp \
	logic/CVehicleDefaultColors.cpp \
	logic/CResourceFile.cpp \
	logic/CElement.cpp \
	logic/CPlayerTasks.cpp \
	logic/CPedSync.cpp \
	logic/CHandlingManager.cpp \
	logic/CTextItem.cpp \
	logic/CTeamManager.cpp \
	logic/CVehicleNames.cpp \
	logic/CVehicle.cpp \
	logic/CEvents.cpp \
	logic/CMapEventManager.cpp \
	logic/CColTube.cpp \
	logic/CGame.cpp \
	logic/CConnectHistory.cpp \
	logic/CConfig.cpp \
	logic/CPacketTranslator.cpp \
	logic/CPlayerCamera.cpp \
	logic/CAccessControlListManager.cpp \
	logic/CVehicleColor.cpp \
	logic/CColSphere.cpp \
	logic/CXMLConfig.cpp \
	logic/CResourceMapItem.cpp \
	logic/CScriptFile.cpp \
	logic/CPickupManager.cpp \
	logic/CBlipManager.cpp \
	logic/CMapFile.cpp \
	logic/CResourceHTMLItem.cpp \
	logic/CPlayerTextManager.cpp \
	logic/CRegistry.cpp \
	logic/CBlendedWeather.cpp \
	logic/CResourceClientScriptItem.cpp \
	logic/CPerPlayerEntity.cpp \
	logic/CAccount.cpp \
	logic/CHandling.cpp \
	logic/CColCuboid.cpp \
	logic/CPad.cpp \
	logic/CGroups.cpp \
	logic/CColManager.cpp \
	logic/CObject.cpp \
	logic/CPlayer.cpp \
	logic/CKeyBinds.cpp \
	logic/CWater.cpp \
	logic/CWaterManager.cpp \
	logic/packets/CPlayerListPacket.cpp \
	logic/packets/CConsoleEchoPacket.cpp \
	logic/packets/CCustomDataPacket.cpp \
	logic/packets/CVehicleDamageSyncPacket.cpp \
	logic/packets/CPedSyncPacket.cpp \
	logic/packets/CCameraSyncPacket.cpp \
	logic/packets/CPlayerChangeNickPacket.cpp \
	logic/packets/CPickupHitConfirmPacket.cpp \
	logic/packets/CDebugEchoPacket.cpp \
	logic/packets/CCommandPacket.cpp \
	logic/packets/CPlayerWastedPacket.cpp \
	logic/packets/CLuaEventPacket.cpp \
	logic/packets/CResourceStopPacket.cpp \
	logic/packets/CMapInfoPacket.cpp \
	logic/packets/CProjectileSyncPacket.cpp \
	logic/packets/CPlayerPingsPacket.cpp \
	logic/packets/CDetonateSatchelsPacket.cpp \
	logic/packets/CPlayerTimeoutPacket.cpp \
	logic/packets/CPlayerQuitPacket.cpp \
	logic/packets/CVehicleSpawnPacket.cpp \
	logic/packets/CVehicleTrailerPacket.cpp \
	logic/packets/CEntityAddPacket.cpp \
	logic/packets/CPlayerPuresyncPacket.cpp \
	logic/packets/CExplosionSyncPacket.cpp \
	logic/packets/CUnoccupiedVehicleStartSyncPacket.cpp \
	logic/packets/CVoiceDataPacket.cpp \
	logic/packets/CPlayerClothesPacket.cpp \
	logic/packets/CPlayerJoinPacket.cpp \
	logic/packets/CPlayerJoinDataPacket.cpp \
	logic/packets/CPedStartSyncPacket.cpp \
	logic/packets/CResourceStartPacket.cpp \
	logic/packets/CEntityRemovePacket.cpp \
	logic/packets/CVehicleInOutPacket.cpp \
	logic/packets/CUnoccupiedVehicleSyncPacket.cpp \
	logic/packets/CPickupHideShowPacket.cpp \
	logic/packets/CPlayerSpawnPacket.cpp \
	logic/packets/CKeysyncPacket.cpp \
	logic/packets/CPlayerStatsPacket.cpp \
	logic/packets/CVehiclePuresyncPacket.cpp \
	logic/packets/CPlayerDisconnectedPacket.cpp \
	logic/packets/CReturnSyncPacket.cpp \
	logic/packets/CFireSyncPacket.cpp \
	logic/packets/CPlayerConnectCompletePacket.cpp \
	logic/packets/CServerTextItemPacket.cpp \
	logic/packets/CChatEchoPacket.cpp \
	logic/packets/CPlayerJoinCompletePacket.cpp \
	logic/packets/CPacket.cpp \
	logic/packets/CLuaPacket.cpp \
	StdInc.cpp

libdeathmatch_la_LIBADD = \
	@PCRE_LIBS@ \
	-lehs \
	-lpme \
	-lplatform \
	-lsqlite3 \
	-llua \
	-ljson-c

libdeathmatch_la_LDFLAGS = \
	-Wl,-export-dynamic \
	../../../libs/ASEQuerySDK.o \
	-L../../sdk/ \
	-L../../../vendor/json-c/ \
	-L../../../vendor/ehs/ \
	-L../../../vendor/pme/ \
	-L../../../libs/ \
	-L../../../libs/lua \
	-L/usr/lib

