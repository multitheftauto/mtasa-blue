#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#pragma pack(1)

// Proxy dll for winmm.dll (which gta_sa.exe loads first)

#ifdef DEBUG
    #define CORE_DLL "core_d.dll"
#else
    #define CORE_DLL "core.dll"
#endif

void InitRedirect();
void LoadMtaDll();

bool bDoneInit = false;
HINSTANCE hRealWinmm = 0;
HINSTANCE hCore = 0;
#define NUM_FUNCTIONS 193
FARPROC functionAddressList[NUM_FUNCTIONS] = {};
LPCSTR functionNameList[NUM_FUNCTIONS] = {
    "CloseDriver", "DefDriverProc", "DriverCallback", "DrvGetModuleHandle", "GetDriverModuleHandle", 
    "NotifyCallbackData", "OpenDriver", "PlaySound", "PlaySoundA", "PlaySoundW", 
    "SendDriverMessage", "WOW32DriverCallback", "WOW32ResolveMultiMediaHandle", "WOWAppExit", "aux32Message", 
    "auxGetDevCapsA", "auxGetDevCapsW", "auxGetNumDevs", "auxGetVolume", "auxOutMessage", 
    "auxSetVolume", "joy32Message", "joyConfigChanged", "joyGetDevCapsA", "joyGetDevCapsW", 
    "joyGetNumDevs", "joyGetPos", "joyGetPosEx", "joyGetThreshold", "joyReleaseCapture", 
    "joySetCapture", "joySetThreshold", "mci32Message", "mciDriverNotify", "mciDriverYield", 
    "mciExecute", "mciFreeCommandResource", "mciGetCreatorTask", "mciGetDeviceIDA", "mciGetDeviceIDFromElementIDA", 
    "mciGetDeviceIDFromElementIDW", "mciGetDeviceIDW", "mciGetDriverData", "mciGetErrorStringA", "mciGetErrorStringW", 
    "mciGetYieldProc", "mciLoadCommandResource", "mciSendCommandA", "mciSendCommandW", "mciSendStringA", 
    "mciSendStringW", "mciSetDriverData", "mciSetYieldProc", "mid32Message", "midiConnect", 
    "midiDisconnect", "midiInAddBuffer", "midiInClose", "midiInGetDevCapsA", "midiInGetDevCapsW", 
    "midiInGetErrorTextA", "midiInGetErrorTextW", "midiInGetID", "midiInGetNumDevs", "midiInMessage", 
    "midiInOpen", "midiInPrepareHeader", "midiInReset", "midiInStart", "midiInStop", 
    "midiInUnprepareHeader", "midiOutCacheDrumPatches", "midiOutCachePatches", "midiOutClose", "midiOutGetDevCapsA", 
    "midiOutGetDevCapsW", "midiOutGetErrorTextA", "midiOutGetErrorTextW", "midiOutGetID", "midiOutGetNumDevs", 
    "midiOutGetVolume", "midiOutLongMsg", "midiOutMessage", "midiOutOpen", "midiOutPrepareHeader", 
    "midiOutReset", "midiOutSetVolume", "midiOutShortMsg", "midiOutUnprepareHeader", "midiStreamClose", 
    "midiStreamOpen", "midiStreamOut", "midiStreamPause", "midiStreamPosition", "midiStreamProperty", 
    "midiStreamRestart", "midiStreamStop", "mixerClose", "mixerGetControlDetailsA", "mixerGetControlDetailsW", 
    "mixerGetDevCapsA", "mixerGetDevCapsW", "mixerGetID", "mixerGetLineControlsA", "mixerGetLineControlsW", 
    "mixerGetLineInfoA", "mixerGetLineInfoW", "mixerGetNumDevs", "mixerMessage", "mixerOpen", 
    "mixerSetControlDetails", "mmDrvInstall", "mmGetCurrentTask", "mmTaskBlock", "mmTaskCreate", 
    "mmTaskSignal", "mmTaskYield", "mmioAdvance", "mmioAscend", "mmioClose", 
    "mmioCreateChunk", "mmioDescend", "mmioFlush", "mmioGetInfo", "mmioInstallIOProcA", 
    "mmioInstallIOProcW", "mmioOpenA", "mmioOpenW", "mmioRead", "mmioRenameA", 
    "mmioRenameW", "mmioSeek", "mmioSendMessage", "mmioSetBuffer", "mmioSetInfo", 
    "mmioStringToFOURCCA", "mmioStringToFOURCCW", "mmioWrite", "mmsystemGetVersion", "mod32Message", 
    "mxd32Message", "sndPlaySoundA", "sndPlaySoundW", "tid32Message", "timeBeginPeriod", 
    "timeEndPeriod", "timeGetDevCaps", "timeGetSystemTime", "timeGetTime", "timeKillEvent", 
    "timeSetEvent", "waveInAddBuffer", "waveInClose", "waveInGetDevCapsA", "waveInGetDevCapsW", 
    "waveInGetErrorTextA", "waveInGetErrorTextW", "waveInGetID", "waveInGetNumDevs", "waveInGetPosition", 
    "waveInMessage", "waveInOpen", "waveInPrepareHeader", "waveInReset", "waveInStart", 
    "waveInStop", "waveInUnprepareHeader", "waveOutBreakLoop", "waveOutClose", "waveOutGetDevCapsA", 
    "waveOutGetDevCapsW", "waveOutGetErrorTextA", "waveOutGetErrorTextW", "waveOutGetID", "waveOutGetNumDevs", 
    "waveOutGetPitch", "waveOutGetPlaybackRate", "waveOutGetPosition", "waveOutGetVolume", "waveOutMessage", 
    "waveOutOpen", "waveOutPause", "waveOutPrepareHeader", "waveOutReset", "waveOutRestart", 
    "waveOutSetPitch", "waveOutSetPlaybackRate", "waveOutSetVolume", "waveOutUnprepareHeader", "waveOutWrite", 
    "wid32Message", "wod32Message", MAKEINTRESOURCE(2)
    };

BOOL WINAPI DllMain(HINSTANCE hInst,DWORD reason,LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        // Ensure this proxy dll forwards functions
        InitRedirect();
        // Load core
        LoadMtaDll();
    }
    if (reason == DLL_PROCESS_DETACH)
    {
        FreeLibrary(hCore);
        FreeLibrary(hRealWinmm);
    }
    return 1;
}

// Load MTA dll if it looks like we are in gta_sa.exe
void LoadMtaDll()
{
    TCHAR szFileName[MAX_PATH] = "";
    GetModuleFileName(NULL, szFileName, MAX_PATH);
    TCHAR* szFileNameTail = szFileName + strlen(szFileName) - 7;
    if (strcmp(szFileNameTail, "_sa.exe") == 0)
    {
        hCore = LoadLibrary(CORE_DLL);
    }
}

// Get addresses for functions in the real dll
void InitRedirect()
{
    if (!hRealWinmm)
    {
        wchar_t szResult[MAX_PATH * 2] = L"";
        SHGetFolderPathW(NULL, CSIDL_SYSTEM, NULL, 0, szResult);
        PathAppendW(szResult, L"winmm.dll");
        hRealWinmm = LoadLibraryW(szResult);
        if (!hRealWinmm)
            return;
    }

    if (!bDoneInit)
    {
        for (int i = 0; i < NUM_FUNCTIONS; i++)
        {
            functionAddressList[i] = GetProcAddress(hRealWinmm, functionNameList[i]);
        }
        bDoneInit = true;
    }
}

// Redirect each function to the real dll
#define ASM_REDIRECT(num) \
extern "C" __declspec(naked) void __stdcall __E__##num##__() \
{ \
    __asm { pushad } \
    if (!bDoneInit) \
        InitRedirect(); \
    __asm { popad } \
    __asm { jmp functionAddressList[num*4] } \
}

ASM_REDIRECT(0) // CloseDriver
ASM_REDIRECT(1) // DefDriverProc
ASM_REDIRECT(2) // DriverCallback
ASM_REDIRECT(3) // DrvGetModuleHandle
ASM_REDIRECT(4) // GetDriverModuleHandle
ASM_REDIRECT(5) // NotifyCallbackData
ASM_REDIRECT(6) // OpenDriver
ASM_REDIRECT(7) // PlaySound
ASM_REDIRECT(8) // PlaySoundA
ASM_REDIRECT(9) // PlaySoundW
ASM_REDIRECT(10) // SendDriverMessage
ASM_REDIRECT(11) // WOW32DriverCallback
ASM_REDIRECT(12) // WOW32ResolveMultiMediaHandle
ASM_REDIRECT(13) // WOWAppExit
ASM_REDIRECT(14) // aux32Message
ASM_REDIRECT(15) // auxGetDevCapsA
ASM_REDIRECT(16) // auxGetDevCapsW
ASM_REDIRECT(17) // auxGetNumDevs
ASM_REDIRECT(18) // auxGetVolume
ASM_REDIRECT(19) // auxOutMessage
ASM_REDIRECT(20) // auxSetVolume
ASM_REDIRECT(21) // joy32Message
ASM_REDIRECT(22) // joyConfigChanged
ASM_REDIRECT(23) // joyGetDevCapsA
ASM_REDIRECT(24) // joyGetDevCapsW
ASM_REDIRECT(25) // joyGetNumDevs
ASM_REDIRECT(26) // joyGetPos
ASM_REDIRECT(27) // joyGetPosEx
ASM_REDIRECT(28) // joyGetThreshold
ASM_REDIRECT(29) // joyReleaseCapture
ASM_REDIRECT(30) // joySetCapture
ASM_REDIRECT(31) // joySetThreshold
ASM_REDIRECT(32) // mci32Message
ASM_REDIRECT(33) // mciDriverNotify
ASM_REDIRECT(34) // mciDriverYield
ASM_REDIRECT(35) // mciExecute
ASM_REDIRECT(36) // mciFreeCommandResource
ASM_REDIRECT(37) // mciGetCreatorTask
ASM_REDIRECT(38) // mciGetDeviceIDA
ASM_REDIRECT(39) // mciGetDeviceIDFromElementIDA
ASM_REDIRECT(40) // mciGetDeviceIDFromElementIDW
ASM_REDIRECT(41) // mciGetDeviceIDW
ASM_REDIRECT(42) // mciGetDriverData
ASM_REDIRECT(43) // mciGetErrorStringA
ASM_REDIRECT(44) // mciGetErrorStringW
ASM_REDIRECT(45) // mciGetYieldProc
ASM_REDIRECT(46) // mciLoadCommandResource
ASM_REDIRECT(47) // mciSendCommandA
ASM_REDIRECT(48) // mciSendCommandW
ASM_REDIRECT(49) // mciSendStringA
ASM_REDIRECT(50) // mciSendStringW
ASM_REDIRECT(51) // mciSetDriverData
ASM_REDIRECT(52) // mciSetYieldProc
ASM_REDIRECT(53) // mid32Message
ASM_REDIRECT(54) // midiConnect
ASM_REDIRECT(55) // midiDisconnect
ASM_REDIRECT(56) // midiInAddBuffer
ASM_REDIRECT(57) // midiInClose
ASM_REDIRECT(58) // midiInGetDevCapsA
ASM_REDIRECT(59) // midiInGetDevCapsW
ASM_REDIRECT(60) // midiInGetErrorTextA
ASM_REDIRECT(61) // midiInGetErrorTextW
ASM_REDIRECT(62) // midiInGetID
ASM_REDIRECT(63) // midiInGetNumDevs
ASM_REDIRECT(64) // midiInMessage
ASM_REDIRECT(65) // midiInOpen
ASM_REDIRECT(66) // midiInPrepareHeader
ASM_REDIRECT(67) // midiInReset
ASM_REDIRECT(68) // midiInStart
ASM_REDIRECT(69) // midiInStop
ASM_REDIRECT(70) // midiInUnprepareHeader
ASM_REDIRECT(71) // midiOutCacheDrumPatches
ASM_REDIRECT(72) // midiOutCachePatches
ASM_REDIRECT(73) // midiOutClose
ASM_REDIRECT(74) // midiOutGetDevCapsA
ASM_REDIRECT(75) // midiOutGetDevCapsW
ASM_REDIRECT(76) // midiOutGetErrorTextA
ASM_REDIRECT(77) // midiOutGetErrorTextW
ASM_REDIRECT(78) // midiOutGetID
ASM_REDIRECT(79) // midiOutGetNumDevs
ASM_REDIRECT(80) // midiOutGetVolume
ASM_REDIRECT(81) // midiOutLongMsg
ASM_REDIRECT(82) // midiOutMessage
ASM_REDIRECT(83) // midiOutOpen
ASM_REDIRECT(84) // midiOutPrepareHeader
ASM_REDIRECT(85) // midiOutReset
ASM_REDIRECT(86) // midiOutSetVolume
ASM_REDIRECT(87) // midiOutShortMsg
ASM_REDIRECT(88) // midiOutUnprepareHeader
ASM_REDIRECT(89) // midiStreamClose
ASM_REDIRECT(90) // midiStreamOpen
ASM_REDIRECT(91) // midiStreamOut
ASM_REDIRECT(92) // midiStreamPause
ASM_REDIRECT(93) // midiStreamPosition
ASM_REDIRECT(94) // midiStreamProperty
ASM_REDIRECT(95) // midiStreamRestart
ASM_REDIRECT(96) // midiStreamStop
ASM_REDIRECT(97) // mixerClose
ASM_REDIRECT(98) // mixerGetControlDetailsA
ASM_REDIRECT(99) // mixerGetControlDetailsW
ASM_REDIRECT(100) // mixerGetDevCapsA
ASM_REDIRECT(101) // mixerGetDevCapsW
ASM_REDIRECT(102) // mixerGetID
ASM_REDIRECT(103) // mixerGetLineControlsA
ASM_REDIRECT(104) // mixerGetLineControlsW
ASM_REDIRECT(105) // mixerGetLineInfoA
ASM_REDIRECT(106) // mixerGetLineInfoW
ASM_REDIRECT(107) // mixerGetNumDevs
ASM_REDIRECT(108) // mixerMessage
ASM_REDIRECT(109) // mixerOpen
ASM_REDIRECT(110) // mixerSetControlDetails
ASM_REDIRECT(111) // mmDrvInstall
ASM_REDIRECT(112) // mmGetCurrentTask
ASM_REDIRECT(113) // mmTaskBlock
ASM_REDIRECT(114) // mmTaskCreate
ASM_REDIRECT(115) // mmTaskSignal
ASM_REDIRECT(116) // mmTaskYield
ASM_REDIRECT(117) // mmioAdvance
ASM_REDIRECT(118) // mmioAscend
ASM_REDIRECT(119) // mmioClose
ASM_REDIRECT(120) // mmioCreateChunk
ASM_REDIRECT(121) // mmioDescend
ASM_REDIRECT(122) // mmioFlush
ASM_REDIRECT(123) // mmioGetInfo
ASM_REDIRECT(124) // mmioInstallIOProcA
ASM_REDIRECT(125) // mmioInstallIOProcW
ASM_REDIRECT(126) // mmioOpenA
ASM_REDIRECT(127) // mmioOpenW
ASM_REDIRECT(128) // mmioRead
ASM_REDIRECT(129) // mmioRenameA
ASM_REDIRECT(130) // mmioRenameW
ASM_REDIRECT(131) // mmioSeek
ASM_REDIRECT(132) // mmioSendMessage
ASM_REDIRECT(133) // mmioSetBuffer
ASM_REDIRECT(134) // mmioSetInfo
ASM_REDIRECT(135) // mmioStringToFOURCCA
ASM_REDIRECT(136) // mmioStringToFOURCCW
ASM_REDIRECT(137) // mmioWrite
ASM_REDIRECT(138) // mmsystemGetVersion
ASM_REDIRECT(139) // mod32Message
ASM_REDIRECT(140) // mxd32Message
ASM_REDIRECT(141) // sndPlaySoundA
ASM_REDIRECT(142) // sndPlaySoundW
ASM_REDIRECT(143) // tid32Message
ASM_REDIRECT(144) // timeBeginPeriod
ASM_REDIRECT(145) // timeEndPeriod
ASM_REDIRECT(146) // timeGetDevCaps
ASM_REDIRECT(147) // timeGetSystemTime
ASM_REDIRECT(148) // timeGetTime
ASM_REDIRECT(149) // timeKillEvent
ASM_REDIRECT(150) // timeSetEvent
ASM_REDIRECT(151) // waveInAddBuffer
ASM_REDIRECT(152) // waveInClose
ASM_REDIRECT(153) // waveInGetDevCapsA
ASM_REDIRECT(154) // waveInGetDevCapsW
ASM_REDIRECT(155) // waveInGetErrorTextA
ASM_REDIRECT(156) // waveInGetErrorTextW
ASM_REDIRECT(157) // waveInGetID
ASM_REDIRECT(158) // waveInGetNumDevs
ASM_REDIRECT(159) // waveInGetPosition
ASM_REDIRECT(160) // waveInMessage
ASM_REDIRECT(161) // waveInOpen
ASM_REDIRECT(162) // waveInPrepareHeader
ASM_REDIRECT(163) // waveInReset
ASM_REDIRECT(164) // waveInStart
ASM_REDIRECT(165) // waveInStop
ASM_REDIRECT(166) // waveInUnprepareHeader
ASM_REDIRECT(167) // waveOutBreakLoop
ASM_REDIRECT(168) // waveOutClose
ASM_REDIRECT(169) // waveOutGetDevCapsA
ASM_REDIRECT(170) // waveOutGetDevCapsW
ASM_REDIRECT(171) // waveOutGetErrorTextA
ASM_REDIRECT(172) // waveOutGetErrorTextW
ASM_REDIRECT(173) // waveOutGetID
ASM_REDIRECT(174) // waveOutGetNumDevs
ASM_REDIRECT(175) // waveOutGetPitch
ASM_REDIRECT(176) // waveOutGetPlaybackRate
ASM_REDIRECT(177) // waveOutGetPosition
ASM_REDIRECT(178) // waveOutGetVolume
ASM_REDIRECT(179) // waveOutMessage
ASM_REDIRECT(180) // waveOutOpen
ASM_REDIRECT(181) // waveOutPause
ASM_REDIRECT(182) // waveOutPrepareHeader
ASM_REDIRECT(183) // waveOutReset
ASM_REDIRECT(184) // waveOutRestart
ASM_REDIRECT(185) // waveOutSetPitch
ASM_REDIRECT(186) // waveOutSetPlaybackRate
ASM_REDIRECT(187) // waveOutSetVolume
ASM_REDIRECT(188) // waveOutUnprepareHeader
ASM_REDIRECT(189) // waveOutWrite
ASM_REDIRECT(190) // wid32Message
ASM_REDIRECT(191) // wod32Message
ASM_REDIRECT(192) // ___XXX___193
