/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/ReportLogID.h
 *  PURPOSE:     Header for common definitions
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

enum class ReportLogID
{
    NONE = 0,

    WEAPON_FIRE_ANIM_CRASH_FIX_GROUP = 533,
    WEAPON_FIRE_ANIM_CRASH_FIX_INTERFACE = 534,

    CORRUPT_ANIM_ASSOC_GROUP_INTERFACE = 542,
    INVALID_ANIM_ASSOC_GROUP_INTERFACE = 543,
    ANIM_ASSOC_GROUP_INTERFACE_NULL_BLOCK = 544,

    ASSOC_GROUPS_ADDRESS_LOG = 567,

    ANIM_TRANSLATION_FAIL = 588,

    DOWNLOAD_SCRIPT_ERROR = 1002,
    INSTALL_MGR_CHANGEFROMADMIN = 1003,
    DEOBFUSCATION_LOADSTRING_FAIL = 1004,
    DEOBFUSCATION_LOAD_FAIL = 1005,

    RESOURCE_LOAD_FAIL = 1007,
    RESOURCE_NOT_READY_LOAD = 1008,
    RESOURCESTART_DELETE_FILE_FAIL = 1009,

    APPLY_DAMAGE_NET_FIX = 1010,

    HTTP_FILE_MISMATCH = 1011,
    HTTP_DOWNLOAD_ERROR = 1012,
    DOWNLOAD_FILE_ERROR = 1013,

    INSTALL_MGR_LAUNCH = 1041,
    LOADER_DOMAIN_FINISH = 1044,
    INSTALL_MGR_CONTINUE_EXCEPTION = 1045,
    INSTALL_MGR_LAUNCHGAME_EXCEPTION = 1046,
    INSTALL_MGR_LAUNCH_FINISH = 1060,
    INSTALL_MGR_SEQ_STATE = 1061, // CInstallManager::RestoreSequencerState
    INSTALL_MGR_GET_PATH = 1062,

    SEQUENCER_CALL_FUNC = 1070,
    SEQUENCER_LOG = 1071,

    VER_UPD_POLLDOWNLOAD_RESPONSE = 2005, // DoPollDownload: Downloaded X bytes
    VER_UPD_POLLDOWNLOAD_SUCCESS = 2007, // DoPollDownload: Downloaded

    INSTALL_MGR_INSTALL_SUCCESS = 2050,
    INSTALL_MGR_INSTALL_NEWS_SUCCESS = 2051,

    RESOURCESTART_FATAL_ERROR = 2081,

    VER_UPD_XML_SAVE_FAIL = 2501, // CXMLBuffer::SetFromBuffer: Could not save file
    VER_UPD_XML_CREATE_FAIL = 2502, // CXMLBuffer::SetFromBuffer: Could not CreateXML
    VER_UPD_XML_PARSE_FAIL = 2503, // CXMLBuffer::SetFromBuffer: Could not parse file
    VER_UPD_XML_MISSING_ROOT = 2504, // CXMLBuffer::SetFromBuffer: No root node in file

    INSTALL_MGR_AS_ADMIN = 3048,
    INSTALL_MGR_GTA_DLL = 3052, // _PrepareLaunchLocation: Copying

    GAME_PLAYER_TRANSGRESSION = 3100, // 

    DLL_DIR_FAIL = 3119, // DllDirectory path invalid
    CRASH_INFO = 3120,

    TROUBLE = 3200,

    NEWS_LAYOUT_FAIL = 3302, // CNewsBrowser::LoadLayout problem loading file
    NEWS_LAYOUT_IMAGESET_FAIL = 3303, // CNewsBrowser::LoadLayout: Problem with LoadImageset
    NEW_LAYOUT_LOAD_FAIL = 3304, // CNewsBrowser::LoadLayout: Problem with LoadLayout

    SERVER_IMPORTANT_MSG = 3400,

    ON_RESTART_CMD_FAIL = 4000, // OnRestartCommand disregarded due to version change
    ON_RESTART_CMD_RESULT = 4047,

    INSTALL_MGR_INSTALL_NEWS_FAIL = 4048,

    LOADER_DELETED_GTA_SETTINGS = 4053,

    VER_UPD_POLLDOWNLOAD_FAIL = 4002, // DoPollDownload: Regular fail
    VER_UPD_POLLDOWNLOAD_CHECKSUM_FAIL = 5003, // DoPollDownload: Checksum wrong
    VER_UPD_POLLDOWNLOAD_PATH_FAIL = 5004, // DoPollDownload: Unable to use the path
    VER_UPD_POLLDOWNLOAD_SAVE_FAIL = 5005, // DoPollDownload: Unable to save the file
    VER_UPD_POLLDOWNLOAD_SIGNATURE_FAIL = 5006, // DoPollDownload: Signature wrong
    VER_UPD_RESUMABLE_EXISTS = 5008, // Found existing file to resume download
    VER_UPD_POLLDOWNLOAD_URL_FAIL = 5007, // DoPollDownload: Fail for URL
    VER_UPD_RESUMABLE_NEW = 5009, // Save new file
    INSTALL_MGR_FAIL = 5049,
    CHECK_ON_RESTART_CMD_UNKNOWN = 5052, // CheckOnRestartCommand: Unknown restart command 
    FILEGEN_PATCH_FAIL = 5053, // IsPatchBase: Incorrect file & IsPatchDiff: Incorrect file
    LOADER_DELETE_GTA_SETTINGS_FAIL = 5054,
    LOADER_INSTALL_FAIL = 5055, // Install.cpp for Loader project
    VER_UPD_RESPONSE = 5061, // Processing patch file
    VER_UPD_STARTDOWNLOAD_CHECKSUM_FAIL = 5807, // StartDownload: Cached file reuse - Size correct, but md5 did not match
    VER_UPD_STARTDOWNLOAD_SIGNATURE_FAIL = 5808, // StartDownload: Cached file reuse - Size and md5 correct, but signature incorrect
    VER_UPD_STARTDOWNLOAD_SUCCESS = 5809, // StartDownload: Cached file reuse - Size, md5 and signature correct

    RTL_FOPEN_FAIL = 5321,

    VEHICLE_MODEL_FRAME_FIX = 5412,
    VEHICLE_MODEL_FRAME_FAIL = 5413,
    MODEL_NO_MODEL_INFO = 5414,
    MODEL_NO_COL_INFO = 5415,
    MODEL_NOT_LOADED = 5416,
    MODEL_BAD_ANIM_HIERARCHY = 5418,
    PED_NO_ANIM_HIERARCHY = 5421,

    REMOTE_WEAPON_SLOT_MISMATCH = 5430, // CClientPed::ValidateRemoteWeapons
    LOAD_ANIMATION_FAIL = 5431, // Failed to load animation
    REMOT_WEAPON_TYPE_MISMATCH = 5432, // DischargeWeapon adding missing weapon

    BULLETSYNC_DMG = 5501, // 2nd pass 1 for BulletSyncShot damage
    BULLETSYNC_NOWEAPON = 5502, // BulletSyncShot but not bBulletSyncWeapon
    BULLETSYNC_WEAPON_NOSYNC = 5503, // not BulletSyncShot but bBulletSyncWeapon

    OBJECT_MODEL_NO_COL_OR_INFO = 5601,

    LOADER_MODULE_LOAD_FAIL = 5711,
    LOADER_MODULE_MAIN_FAIL = 5714, // Failed to find DoWinMain in loader DLL
    LOADER_INVALID_DLL_PATH = 5716,
    LOADER_DLL_NOT_EXISTS = 5717,
    LOADER_DLL_INVALID_SIGNATURE = 5718,
    LOADER_LOADLIBRARY_FAIL = 5719,
    LOADER_PROXY_LAUNCH_LOG = 5720,
    LOADER_ELEVATED_PRIVILEGE = 5750,
    LOADER_EXCEPTION = 5751, // Exception in DoWinMain

    WAIT_FOR_SINGLE_OBJECT_TIMED_OUT = 6211,

    GTA_PRINTF = 6311,

    CHAT_RT_FAIL = 6532, // Chat rendertarget creation fail
    FORBODEN_PROGRAM_DETECTED = 6550,

    MODEL_INFO_LOAD_FAIL = 6641, // Blocking load fail
    MODEL_INFO_LOAD_SUCCESS = 6642, // Blocking load

    RESOURCE_FILE_MODIFY = 7059, // Resource file modifed by script

    CORE_NET_ERROR = 7100,
    CORE_QUIT = 7101,
    CORE_PROCESS_DETACH = 7102,

    LOADER_FINISH = 7104,
    GAME_KICKED = 7105,
    GAME_HTTPERROR = 7106,
    GAME_DISCONNECT = 7107,
    GAME_FATAL_ERROR = 7108,
    GAME_PROTOCOL_ERROR = 7109,
    GAME_CMD_DISCONNECT = 7110, // Game - COMMAND_Disconnect
    LOADER_ELEVATED_PRIVILEGE_REQUEST = 7115,

    VIDEO_ENUMSETTINGS_FAIL = 7340, // EnumDisplaySettings failed
    VIDEO_DISPLAYSETTINGS_FAIL = 7341, // ChangeDisplaySettingsEx failed

    GAME_FILECACHE_ROOT_NOSHARED = 7410, // CClientGame::SetFileCacheRoot - Not shared
    GAME_FILECACHE_ROOT_SHARED = 7411, // CClientGame::SetFileCacheRoot - Is shared
    GAME_FILECACHE_ROOT_INIT = 7412, // CClientGame::SetFileCacheRoot - Initial setting
    GAME_FILECACHE_ROOT_MODIFY = 7413, // CClientGame::SetFileCacheRoot - Change shared

    OBJECTS_LIMIT_REACHED = 7430, // CClientObjectManager reached limit

    NVIDIA_OPTIMUS_DETECTED = 7460, // NvAPI NvOptimusDetect
    NVIDIA_OPTIMUS_GPU_RESULT = 7461,

    CLOTHES_GEOM_REFS_BELOW = 7521,

    D3D_EFFECT_TEMPLATE_CREATED = 7544, // NewEffectTemplate
    LUA_UNDUMP_HASH_FAIL = 7555, // Unexpected undump hash for buffer

    TERMINATE_PROCESS_CHECKSERVICE = 8070, // Utils.cpp - TerminateProcess & CheckService

    ENTITY_ADD_PROTOCOL_ERROR = 8331, // RaiseEntityAddError
    ENTITY_ADD_ERROR_OFFSET = 8332, // RaiseEntityAddError

    TXD_RIGHTSIZE_ERROR = 8430,

    TASK_SIMPLE_FALLOUT_VFTBL = 8530, // IsTaskSimpleCarFallOutValid fail
    TASK_SIMPLE_FALLOUT_VEHPTR = 8531, // IsTaskSimpleCarFallOutValid invalid vehicle ptr

    PRELOAD_UPGRADE_FALLEN = 8544, // Report if LowestUnsafeUpgrade has fallen
    PRELOAD_UPGRADE_CACHE_CRASH = 8545, // Auto crash fix for crash in CModelCacheManagerImpl::PreLoad

    D3D_VERTEXBUFFER_FAIL = 8610, // CreateVertexBuffer fail
    D3D_INDEXBUFFER_FAIL = 8611, // CreateIndexBuffer fail
    D3D_TEXTURE_FAIL = 8612, // CreateTexture fail
    D3D_VERTEXDECL_FAIL = 8613, // CreateVertexDecl fail
    D3D_LOCK_VERTEXBUFFER_FAIL = 8620, // Lock VertexBuffer fail
    D3D_LOCK_VERTEXBUFFER_NULL = 8621, // Lock VertexBuffer result NULL
    D3D_LOCK_VERTEXBUFFER_OOR = 8622, // Offset out of range
    D3D_LOCK_VERTEXBUFFER_AV = 8623, // Access violation
    D3D_LOCK_INDEXBUFFER_FAIL = 8625, // Lock IndexBuffer fail
    D3D_LOCK_INDEXBUFFER_NULL = 8626, // Lock IndexBuffer result NULL
    D3D_CREATEDEVICE_OTHER_THREAD = 8627, // Check for calls to CreateDevice from other threads

    HOOKSYSTEM_ORIGINAL_BYTE_FAIL = 8423,

    PHYSICAL_MOVINGLIST_FIX = 8640,
    ENTITY_RENDER_NO_RWOBJECT = 8645,

    QUESTIONBOX_SHOW = 9100,

    QUERYWMI_FAIL = 9130,
    QUERYWMI_ENUMERATOR_FAIL = 9131,
    QUERYWMI_PCLSOBJ_FAIL = 9132,
    QUERYWMI_PCLSOBJ_RESULT = 9133,
    QUERYWMI_CONNECT_FAIL = 9135,
    QUERYWMI_PROXY_FAIL = 9136,
    QUERYWMI_QUERY_FAIL = 9137,

    DBGHELP_FAIL = 9201, // Could not load DBGHELP.DLL
    DBGHELP_MINIDUMPWRITEDUMP_FAIL = 9202, // Could not find MiniDumpWriteDump function in DBGHELP.DLL
    CREATE_DMP_FAIL = 9203, // CCrashDumpWriter::DumpMiniDump - Could not create core.dmp file
    DMP_MINIDUMPWRITEDUMP_FAIL = 9204, // MiniDumpWriteDump function in DBGHELP.DLL fail
    DISABLE_VOLUMETRIC_SHADOWS = 9205, // Auto crash fix if last crash was in volumetric shadow code
    INVALID_PARAMETER_COUNT = 9206, // CCrashDumpWriter invalid parameter (CCrashDumpWriter::UpdateCounters)

    LOADER_HANDLE_NOTUSED_MAINMENU = 9310,
    LOADER_HANDLE_NOTUSED_MAINMENU_CHANGE = 9311,
    LOADER_HANDLE_NOTUSED_MAINMENU_BORDERLESS_SET = 9312,
    LOADER_HANDLE_NOTUSED_MAINMENU_USER_NO = 9313,
    LOADER_HANDLE_NOTUSED_MAINMENU_NOT_FSCREEN = 9314,
    LOADER_HANDLE_NOTUSED_MAINMENU_BORDERLESS_IS = 9315,

    TXD_SHRUNK = 9400, // CClientTXD::GetFilenameToUse
    GRAPHICS_FONTRESOURCE_EX_FAIL = 9442, // Problem with AddFontResourceEx

    D3D_VERSION_FAIL = 9640, // D3DXCheckVersion FAILED
};
