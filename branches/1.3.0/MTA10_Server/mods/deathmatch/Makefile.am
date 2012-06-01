ZIPDIR=$(srcdir)/../../../vendor/zip/
LUADIR=$(srcdir)/../../../vendor/lua
LUABDIR=$(builddir)/../../../vendor/lua

INCLUDES += -I$(srcdir)/logic/
INCLUDES += -I$(srcdir)/../../sdk/
INCLUDES += -I$(srcdir)/../../../Shared/sdk/
INCLUDES += -I$(srcdir)/../../../Shared/publicsdk/include/
INCLUDES += -I$(srcdir)/../../../vendor/ehs/
INCLUDES += -I$(srcdir)/../../../vendor/pme/
INCLUDES += -I$(srcdir)/../../../vendor/json-c/
INCLUDES += -I$(srcdir)/../../../vendor/bob_withers/
INCLUDES += -I$(srcdir)/../../../vendor/bochs/
INCLUDES += -I$(srcdir)/../../../Shared/animation/
INCLUDES += -I$(srcdir)/../../../Shared/mods/deathmatch/logic/
INCLUDES += -I$(ZIPDIR)/
INCLUDES += -I$(LUADIR)/src
INCLUDES += -I$(LUABDIR)/src
INCLUDES += $(SPARSEHASH_CFLAGS)
INCLUDES += $(SQLITE3_CFLAGS)
# pcre headers used by pme which is used by ehs
INCLUDES += $(PCRE_CFLAGS)

UTILS = \
	../../../Shared/animation/CEasingCurve.cpp \
	../../../Shared/animation/CPositionRotationAnimation.cpp \
	../../../Shared/mods/deathmatch/logic/CLatentReceiver.cpp \
	../../../Shared/mods/deathmatch/logic/CLatentSendQueue.cpp \
	../../../Shared/mods/deathmatch/logic/CLatentTransferManager.cpp \
	../../../Shared/mods/deathmatch/logic/CZoneNames.cpp \
	../../../Shared/mods/deathmatch/logic/Enums.cpp \
	../../../Shared/mods/deathmatch/logic/CIdArray.cpp \
	./utils/CHTTPClient.cpp ./utils/CHTTPRequest.cpp \
	./utils/CHTTPResponse.cpp \
	./utils/CTCPClientSocketImpl.cpp \
	./utils/CTCPImpl.cpp \
	./utils/CZipMaker.cpp \
	$(srcdir)/../../../vendor/bob_withers/base64.cpp \
	$(srcdir)/../../../vendor/bochs/bochs_internal/crc32.cpp \
	$(ZIPDIR)/ioapi.c \
	$(ZIPDIR)/mztools.c \
	$(ZIPDIR)/unzip.c \
	$(ZIPDIR)/zip.c

LOGIC = \
./logic/ASE.cpp \
./logic/CAccessControlList.cpp \
./logic/CAccessControlListGroup.cpp \
./logic/CAccessControlListManager.cpp \
./logic/CAccessControlListRight.cpp \
./logic/CAccount.cpp \
./logic/CAccountManager.cpp \
./logic/CBan.cpp \
./logic/CBanManager.cpp \
./logic/CBlendedWeather.cpp \
./logic/CBlip.cpp \
./logic/CBlipManager.cpp \
./logic/CBuildingRemovalManager.cpp \
./logic/CBuildingRemoval.cpp \
./logic/CCameraSpatialDatabase.cpp \
./logic/CClient.cpp \
./logic/CClock.cpp \
./logic/CColCircle.cpp \
./logic/CColCuboid.cpp \
./logic/CColManager.cpp \
./logic/CColPolygon.cpp \
./logic/CColRectangle.cpp \
./logic/CColShape.cpp \
./logic/CColSphere.cpp \
./logic/CColTube.cpp \
./logic/CCommandFile.cpp \
./logic/CCommandLineParser.cpp \
./logic/CConfig.cpp \
./logic/CConnectHistory.cpp \
./logic/CConsole.cpp \
./logic/CConsoleClient.cpp \
./logic/CConsoleCommand.cpp \
./logic/CConsoleCommands.cpp \
./logic/CCustomData.cpp \
./logic/CDummy.cpp \
./logic/CDatabaseConnectionSqlite.cpp \
./logic/CDatabaseJobQueue.cpp \
./logic/CDatabaseManager.cpp \
./logic/CDatabaseTypeMySql.cpp \
./logic/CDatabaseTypeSqlite.cpp \
./logic/CDynamicLibrary.cpp \
./logic/CElement.cpp \
./logic/CElementDeleter.cpp \
./logic/CElementGroup.cpp \
./logic/CElementIDs.cpp \
./logic/CElementRefManager.cpp \
./logic/CEvents.cpp \
./logic/CGame.cpp \
./logic/CGroups.cpp \
./logic/CHandlingEntry.cpp \
./logic/CHandlingManager.cpp \
./logic/CHTTPD.cpp \
./logic/CKeyBinds.cpp \
./logic/CLanBroadcast.cpp \
./logic/CLightsyncManager.cpp \
./logic/CLogger.cpp \
./logic/CMainConfig.cpp \
./logic/CMapEvent.cpp \
./logic/CMapEventManager.cpp \
./logic/CMapManager.cpp \
./logic/CMarker.cpp \
./logic/CMarkerManager.cpp \
./logic/CObject.cpp \
./logic/CObjectManager.cpp \
./logic/CObjectSync.cpp \
./logic/CPacketTranslator.cpp \
./logic/CPad.cpp \
./logic/CPed.cpp \
./logic/CPedManager.cpp \
./logic/CPedSync.cpp \
./logic/CPerfStat.BandwidthReduction.cpp \
./logic/CPerfStat.BandwidthUsage.cpp \
./logic/CPerfStat.DebugInfo.cpp \
./logic/CPerfStat.DebugTable.cpp \
./logic/CPerfStat.FunctionTiming.cpp \
./logic/CPerfStat.LibMemory.cpp \
./logic/CPerfStat.LuaMemory.cpp \
./logic/CPerfStat.LuaTiming.cpp \
./logic/CPerfStat.PacketUsage.cpp \
./logic/CPerfStat.ServerInfo.cpp \
./logic/CPerfStat.ServerTiming.cpp \
./logic/CPerfStat.SqliteTiming.cpp \
./logic/CPerfStatManager.cpp \
./logic/CPerPlayerEntity.cpp \
./logic/CPickup.cpp \
./logic/CPickupManager.cpp \
./logic/CPlayer.cpp \
./logic/CPlayerCamera.cpp \
./logic/CPlayerClothes.cpp \
./logic/CPlayerManager.cpp \
./logic/CPlayerStats.cpp \
./logic/CPlayerTasks.cpp \
./logic/CPlayerTextManager.cpp \
./logic/CRPCFunctions.cpp \
./logic/CRadarArea.cpp \
./logic/CRadarAreaManager.cpp \
./logic/CRegisteredCommands.cpp \
./logic/CRegistry.cpp \
./logic/CRegistryManager.cpp \
./logic/CRemoteCalls.cpp \
./logic/CResource.cpp \
./logic/CResource.AclRequest.cpp \
./logic/CResourceChecker.cpp \
./logic/CResourceClientConfigItem.cpp \
./logic/CResourceClientFileItem.cpp \
./logic/CResourceClientScriptItem.cpp \
./logic/CResourceConfigItem.cpp \
./logic/CResourceDownloader.cpp \
./logic/CResourceFile.cpp \
./logic/CResourceHTMLItem.cpp \
./logic/CResourceManager.cpp \
./logic/CResourceMapItem.cpp \
./logic/CResourceScriptItem.cpp \
./logic/CScriptDebugging.cpp \
./logic/CScriptFile.cpp \
./logic/CSerialManager.cpp \
./logic/CSettings.cpp \
./logic/CSpatialDatabase.cpp \
./logic/CStaticFunctionDefinitions.cpp \
./logic/CTeam.cpp \
./logic/CTeamManager.cpp \
./logic/CTextDisplay.cpp \
./logic/CTextItem.cpp \
./logic/CUnoccupiedVehicleSync.cpp \
./logic/CVehicle.cpp \
./logic/CVehicleColorManager.cpp \
./logic/CVehicleDefaultColors.cpp \
./logic/CVehicleManager.cpp \
./logic/CVehicleNames.cpp \
./logic/CVehicleUpgrades.cpp \
./logic/CWater.cpp \
./logic/CWaterManager.cpp \
./logic/CWeaponNames.cpp \
./logic/CWeaponStat.cpp \
./logic/CWeaponStatManager.cpp \
./logic/CWhoWas.cpp \
./logic/CXMLConfig.cpp \
./logic/TaskNames.cpp \
./logic/Utils.cpp \
./logic/lua/CLuaArgument.cpp \
./logic/lua/CLuaArguments.cpp \
./logic/lua/CLuaCFunctions.cpp \
./logic/lua/CLuaFunctionDefinitions.cpp \
./logic/lua/CLuaFunctionParseHelpers.cpp \
./logic/lua/CLuaFunctionRef.cpp \
./logic/lua/CLuaMain.cpp \
./logic/lua/CLuaManager.cpp \
./logic/lua/CLuaModuleManager.cpp \
./logic/lua/CLuaTimer.cpp \
./logic/lua/CLuaTimerManager.cpp \
./logic/lua/LuaCommon.cpp \
./logic/lua/LuaUtils.cpp \
./logic/luadefs/CLuaACLDefs.cpp \
./logic/luadefs/CLuaCameraDefs.cpp \
./logic/luadefs/CLuaDefs.cpp \
./logic/luadefs/CLuaElementDefs.cpp \
./logic/luadefs/CLuaFileDefs.cpp \
./logic/luadefs/CLuaHandlingDefs.cpp \
./logic/luadefs/CLuaHTTPDefs.cpp \
./logic/luadefs/CLuaPickupDefs.cpp \
./logic/luadefs/CLuaResourceDefs.cpp \
./logic/luadefs/CLuaTextDefs.cpp \
./logic/luadefs/CLuaVoiceDefs.cpp \
./logic/luadefs/CLuaWorldDefs.cpp \
./logic/luadefs/CLuaXMLDefs.cpp \
./logic/packets/CCameraSyncPacket.cpp \
./logic/packets/CChatEchoPacket.cpp \
./logic/packets/CCommandPacket.cpp \
./logic/packets/CConsoleEchoPacket.cpp \
./logic/packets/CCustomDataPacket.cpp \
./logic/packets/CDebugEchoPacket.cpp \
./logic/packets/CDetonateSatchelsPacket.cpp \
./logic/packets/CEntityAddPacket.cpp \
./logic/packets/CEntityRemovePacket.cpp \
./logic/packets/CExplosionSyncPacket.cpp \
./logic/packets/CFireSyncPacket.cpp \
./logic/packets/CElementRPCPacket.cpp \
./logic/packets/CKeysyncPacket.cpp \
./logic/packets/CBulletsyncPacket.cpp \
./logic/packets/CLightsyncPacket.cpp \
./logic/packets/CLuaEventPacket.cpp \
./logic/packets/CLuaPacket.cpp \
./logic/packets/CMapInfoPacket.cpp \
./logic/packets/CObjectStartSyncPacket.cpp \
./logic/packets/CObjectSyncPacket.cpp \
./logic/packets/CPacket.cpp \
./logic/packets/CPedStartSyncPacket.cpp \
./logic/packets/CPedSyncPacket.cpp \
./logic/packets/CPedWastedPacket.cpp \
./logic/packets/CPickupHideShowPacket.cpp \
./logic/packets/CPickupHitConfirmPacket.cpp \
./logic/packets/CPlayerChangeNickPacket.cpp \
./logic/packets/CPlayerClothesPacket.cpp \
./logic/packets/CPlayerConnectCompletePacket.cpp \
./logic/packets/CPlayerDiagnosticPacket.cpp \
./logic/packets/CPlayerDisconnectedPacket.cpp \
./logic/packets/CPlayerJoinCompletePacket.cpp \
./logic/packets/CPlayerJoinDataPacket.cpp \
./logic/packets/CPlayerJoinPacket.cpp \
./logic/packets/CPlayerListPacket.cpp \
./logic/packets/CPlayerPuresyncPacket.cpp \
./logic/packets/CPlayerModInfoPacket.cpp \
./logic/packets/CPlayerScreenShotPacket.cpp \
./logic/packets/CPlayerQuitPacket.cpp \
./logic/packets/CPlayerSpawnPacket.cpp \
./logic/packets/CPlayerStatsPacket.cpp \
./logic/packets/CPlayerTransgressionPacket.cpp \
./logic/packets/CPlayerTimeoutPacket.cpp \
./logic/packets/CPlayerWastedPacket.cpp \
./logic/packets/CProjectileSyncPacket.cpp \
./logic/packets/CResourceClientScriptsPacket.cpp \
./logic/packets/CResourceStartPacket.cpp \
./logic/packets/CResourceStopPacket.cpp \
./logic/packets/CReturnSyncPacket.cpp \
./logic/packets/CServerTextItemPacket.cpp \
./logic/packets/CUpdateInfoPacket.cpp \
./logic/packets/CUnoccupiedVehiclePushPacket.cpp \
./logic/packets/CUnoccupiedVehicleStartSyncPacket.cpp \
./logic/packets/CUnoccupiedVehicleSyncPacket.cpp \
./logic/packets/CVehicleDamageSyncPacket.cpp \
./logic/packets/CVehicleInOutPacket.cpp \
./logic/packets/CVehiclePuresyncPacket.cpp \
./logic/packets/CVehicleResyncPacket.cpp \
./logic/packets/CVehicleSpawnPacket.cpp \
./logic/packets/CVehicleTrailerPacket.cpp \
./logic/packets/CVoiceDataPacket.cpp \
./logic/packets/CVoiceEndPacket.cpp \
./logic/net/CNetBuffer.cpp \
./logic/net/CNetBufferWatchDog.cpp \
./logic/net/CSimControl.cpp \
./logic/net/CSimPlayerManager.cpp \
./logic/net/CSimPlayerPuresyncPacket.cpp \
./logic/net/CSimVehiclePuresyncPacket.cpp \
./logic/net/CSimKeysyncPacket.cpp \
./logic/net/CSimBulletsyncPacket.cpp

if PRECOMPILE_HEADERS
BUILT_SOURCES = StdInc.h.gch
AM_CXXFLAGS += -Winvalid-pch

StdInc.h.gch:
	$(LIBTOOL) --tag=CXX $(deathmatch_la_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --mode=compile \
		$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
		$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)  \
		-o deathmatch_la-StdInc.h.lo $(srcdir)/StdInc.h \
		-MT deathmatch_la-StdInc.h.gch -MD -MP -MF $(DEPDIR)/deathmatch_la-StdInc.h.Tpo

	mv -f $(DEPDIR)/deathmatch_la-StdInc.h.Tpo $(DEPDIR)/deathmatch_la-StdInc.h.Plo
	mv -f .libs/deathmatch_la-StdInc.h.o $@

include ./$(DEPDIR)/deathmatch_la-StdInc.h.Plo

clean-local:
	-rm -f StdInc.h.gch
endif

libdir = $(outputdir)/mods/deathmatch
lib_LTLIBRARIES = deathmatch.la
deathmatch_la_LIBTOOLFLAGS = --tag=disable-static
deathmatch_la_SOURCES = \
	Server.cpp \
	CServer.cpp \
	$(UTILS) \
	$(LOGIC)

deathmatch_la_LIBADD = \
	../../sdk/libplatform.la \
	$(top_builddir)/vendor/ehs/libehs.la \
	$(LUABDIR)/src/liblua.la \
	$(top_builddir)/vendor/json-c/libjson.la \
	$(SPARSEHASH_LIBS) \
	$(SQLITE3_LIBS)

deathmatch_la_LDFLAGS = \
	-module -avoid-version
