/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/Utils.cpp
 *  PURPOSE:     Miscellaneous utility functions
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "Utils.h"

#ifndef MTA_CLIENT
    #include "CGame.h"
    #include "packets/CPlayerDisconnectedPacket.h"
    #include "net/SyncStructures.h"

    #include <sys/types.h>  // For stat().
    #include <sys/stat.h>   // For stat().

    #ifndef WIN32
        #include <sys/socket.h>
        #include <netinet/in.h>
        #include <arpa/inet.h>
    #endif
#endif

#ifdef MTA_CLIENT
unsigned int StripUnwantedCharacters(char* szText, unsigned char cReplace)
{
    // Replace any unwanted character with a space
    unsigned int uiReplaced = 0;
    char*        szTemp = szText;
    while (*szTemp != 0)
    {
        if (!IsVisibleCharacter(*szTemp))
        {
            *szTemp = cReplace;
            ++uiReplaced;
        }

        ++szTemp;
    }

    // Return how many characters we replaced
    return uiReplaced;
}

unsigned int StripControlCodes(char* szText, unsigned char cReplace)
{
    // Replace any unwanted character with a space
    unsigned int uiReplaced = 0;
    char*        szTemp = szText;
    while (*szTemp != 0)
    {
        if (IsControlCode(*szTemp) && *szTemp != '\n' && *szTemp != '\t')
        {
            *szTemp = cReplace;
            ++uiReplaced;
        }

        ++szTemp;
    }

    // Return how many characters we replaced
    return uiReplaced;
}

bool IsControlCode(unsigned char c)
{
    return c < 32;
}

bool IsValidFilePath(const char* szDir)
{
    if (szDir == NULL)
        return false;

    unsigned int  uiLen = strlen(szDir);
    unsigned char c, c_d;

    // iterate through the char array
    for (unsigned int i = 0; i < uiLen; i++)
    {
        c = szDir[i];                                          // current character
        c_d = (i < (uiLen - 1)) ? szDir[i + 1] : 0;            // one character ahead, if any
        if (!IsVisibleCharacter(c) || c == ':' || (c == '.' && c_d == '.') || (c == '\\' && c_d == '\\'))
            return false;
    }
    return true;
}

void ReplaceOccurrencesInString(std::string& s, const char* a, const char* b)
{
    int idx = 0;
    while ((idx = s.find_first_of(a, idx)) >= 0)
        s.replace(idx, 1, b);
}

void RaiseFatalError(unsigned int uiCode)
{
#ifdef MTA_DEBUG
    assert(0);
#endif

    // Populate the message and show the box
    SString strBuffer(_("Fatal error (%u). If this problem persists, please check out mtasa.com for support."), uiCode);
    SString strTroubleLink(SString("fatal-error&code=%d", uiCode));
    g_pCore->ShowErrorMessageBox(_("Fatal error") + _E("CD62"), strBuffer, strTroubleLink);

    // Request the mod unload
    AddReportLog(7108, SString("Game - RaiseFatalError %d", uiCode));
    g_pCore->GetModManager()->RequestUnload();
}

void RaiseProtocolError(unsigned int uiCode)
{
    // #ifdef MTA_DEBUG
    //     assert ( 0 );
    // #endif

    // Populate the message and show the box
    SString strBuffer(_("Protocol error (%u). If this problem persists, please check out mtasa.com for support."), uiCode);
    SString strTroubleLink(SString("protocol-error&code=%d", uiCode));
    g_pCore->ShowErrorMessageBox(_("Connection error") + _E("CD63"), strBuffer, strTroubleLink);            // Protocol error

    // Request the mod unload
    g_pCore->GetModManager()->RequestUnload();
    AddReportLog(7109, SString("Game - RaiseProtocolError %d", uiCode));
}

void RotateVector(CVector& vecLine, const CVector& vecRotation)
{
    // Rotate it along the X axis
    // [ 1     0        0    0 ]
    // [ 0   cos a   sin a   0 ]
    // [ 0  -sin a   cos a   0 ]

    float fLineY = vecLine.fY;
    vecLine.fY = cos(vecRotation.fX) * fLineY + sin(vecRotation.fX) * vecLine.fZ;
    vecLine.fZ = -sin(vecRotation.fX) * fLineY + cos(vecRotation.fX) * vecLine.fZ;

    // Rotate it along the Y axis
    // [ cos a   0   -sin a   0 ]
    // [   0     1     0      0 ]
    // [ sin a   0    cos a   0 ]

    float fLineX = vecLine.fX;
    vecLine.fX = cos(vecRotation.fY) * fLineX - sin(vecRotation.fY) * vecLine.fZ;
    vecLine.fZ = sin(vecRotation.fY) * fLineX + cos(vecRotation.fY) * vecLine.fZ;

    // Rotate it along the Z axis
    // [  cos a   sin a   0   0 ]
    // [ -sin a   cos a   0   0 ]
    // [    0       0     1   0 ]

    fLineX = vecLine.fX;
    vecLine.fX = cos(vecRotation.fZ) * fLineX + sin(vecRotation.fZ) * vecLine.fY;
    vecLine.fY = -sin(vecRotation.fZ) * fLineX + cos(vecRotation.fZ) * vecLine.fY;
}

void AttachedMatrix(const CMatrix& matrix, CMatrix& returnMatrix, const CVector& vecPosition, const CVector& vecRotation)
{
    returnMatrix = CMatrix(vecPosition, vecRotation) * matrix;
}

void LongToDottedIP(unsigned long ulIP, char* szDottedIP)
{
    in_addr in{};
    in.s_addr = ulIP;
    char* szTemp = inet_ntoa(in);
    if (szTemp)
    {
        strncpy(szDottedIP, szTemp, 22);
    }
    else
    {
        szDottedIP[0] = 0;
    }
}
#else

bool DoesDirectoryExist(const char* szPath)
{
#ifdef WIN32
    DWORD dwAtr = GetFileAttributes(szPath);
    if (dwAtr == INVALID_FILE_ATTRIBUTES)
        return false;
    return ((dwAtr & FILE_ATTRIBUTE_DIRECTORY) != 0);
#else
    struct stat Info;
    stat(szPath, &Info);
    return (S_ISDIR(Info.st_mode));
#endif
}

bool CheckNickProvided(const char* szNick)
{
    if (stricmp(szNick, "admin") == 0)
        return false;
    if (stricmp(szNick, "console") == 0)
        return false;
    if (stricmp(szNick, "server") == 0)
        return false;
    if (strchr(szNick, '`') != nullptr)
        return false;
    return true;
}

void ReplaceCharactersInString(char* szString, char cWhat, char cWith)
{
    // Loop through the string looking for what
    size_t sizeString = strlen(szString);
    for (size_t sizeIndex = 0; sizeIndex < sizeString; sizeIndex++)
    {
        // Replace matching character with the given replacement
        if (szString[sizeIndex] == cWhat)
            szString[sizeIndex] = cWith;
    }
}

void ReplaceOccurrencesInString(std::string& s, const char* a, const char* b)
{
    int idx = 0;
    while ((idx = s.find_first_of(a, idx)) >= 0)
        s.replace(idx, 1, b);
}

char* uppercase(char* s)
{
    for (unsigned int i = 0; i < strlen(s); i++)
    {
        s[i] = toupper(s[i]);
    }
    return s;
}

void stripString(char* szString)
{
    if (szString)
    {
        int          offset = 0;
        unsigned int i = 0;
        size_t       sizeString = strlen(szString);
        for (i = 0; i < sizeString; i++)
        {
            if (szString[i] < 32 || szString[i] > 126)
            {
                offset++;
            }
            else
            {
                szString[i - offset] = szString[i];
            }
        }
        szString[i - offset] = '\0';
    }
}

void stripControlCodes(char* szString)
{
    if (szString)
    {
        unsigned char*       pWrite = reinterpret_cast<unsigned char*>(szString);
        const unsigned char* pRead = pWrite;

        while (*pRead != '\0')
        {
            if (*pRead >= 32)
            {
                *pWrite = *pRead;
                ++pWrite;
            }
            ++pRead;
        }
        *pWrite = '\0';
    }
}

bool StringBeginsWith(const char* szText, const char* szBegins)
{
    if (szText && szBegins)
    {
        unsigned int uiTextLength = strlen(szText);
        unsigned int uiBeginsLength = strlen(szBegins);

        if (uiTextLength < uiBeginsLength)
        {
            return false;
        }

        for (unsigned int ui = 0; ui < uiBeginsLength; ui++)
        {
            if (szText[ui] != szBegins[ui])
            {
                return false;
            }
        }

        return true;
    }

    return false;
}

bool IsNumericString(const char* szString)
{
    char szSet[] = "-1234567890";
    return strspn(szString, szSet) == strlen(szString);
}

bool IsNumericString(const char* szString, size_t sizeString)
{
    char szSet[] = "-1234567890";
    return strspn(szString, szSet) == sizeString;
}

void DisconnectPlayer(CGame* pGame, CPlayer& Player, const char* szMessage)
{
    DisconnectPlayer(pGame, Player, CPlayerDisconnectedPacket::CUSTOM, szMessage);
}

void DisconnectPlayer(CGame* pGame, CPlayer& Player, CPlayerDisconnectedPacket::ePlayerDisconnectType eDisconnectType, const char* szMessage)
{
    if (Player.IsLeavingServer())
        return;

    // Send it to the disconnected player
    Player.Send(CPlayerDisconnectedPacket(eDisconnectType, szMessage));

    // Quit him
    pGame->QuitPlayer(Player);
}

void DisconnectPlayer(CGame* pGame, CPlayer& Player, CPlayerDisconnectedPacket::ePlayerDisconnectType eDisconnectType, time_t BanDuration,
                      const char* szMessage)
{
    if (Player.IsLeavingServer())
        return;

    Player.Send(CPlayerDisconnectedPacket(eDisconnectType, BanDuration, szMessage));
    pGame->QuitPlayer(Player);
}

void DisconnectConnectionDesync(CGame* pGame, CPlayer& Player, unsigned int uiCode)
{
    if (Player.IsLeavingServer())
        return;

    // Send message to the disconnected player
    Player.Send(CPlayerDisconnectedPacket(CPlayerDisconnectedPacket::CONNECTION_DESYNC, SString("(%u)", uiCode)));

    // Quit him
    pGame->QuitPlayer(Player, CClient::QUIT_CONNECTION_DESYNC);
}

bool InitializeSockets()
{
#ifdef WIN32
    WSADATA wsaData;
    if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR)
    {
        WSACleanup();
        return false;
    }
#endif
    return true;
}

bool CleanupSockets()
{
#ifdef WIN32
    WSACleanup();
#endif
    return true;
}
#endif

double GetRandomDouble()
{
    return static_cast<double>(rand()) / (static_cast<double>(RAND_MAX) + 1.0);
}

int GetRandom(int iLow, int iHigh)
{
    double dLow = static_cast<double>(iLow);
    double dHigh = static_cast<double>(iHigh);

    return static_cast<int>(floor((dHigh - dLow + 1.0) * GetRandomDouble())) + iLow;
}
#ifdef MTA_CLIENT
SString GetDataUnit(unsigned long long ullInput)
{
    // Convert it to a float
    float fInput = static_cast<float>(ullInput);

    // Bytes per sec?
    if (fInput < 1024)
    {
        return SString("%u B", (uint)ullInput);
    }

    // Kilobytes per sec?
    fInput /= 1024;
    if (fInput < 1024)
    {
        return SString("%.2f kB", fInput);
    }

    // Megabytes per sec?
    fInput /= 1024;
    if (fInput < 1024)
    {
        return SString("%.2f MB", fInput);
    }

    // Gigabytes per sec?
    fInput /= 1024;
    if (fInput < 1024)
    {
        return SString("%.2f GB", fInput);
    }

    // Terrabytes per sec?
    fInput /= 1024;
    if (fInput < 1024)
    {
        return SString("%.2f TB", fInput);
    }

    // Unknown
    SString strUnknown = "X";
    return strUnknown;
}

#ifdef MTA_DEBUG
struct ReleaseVirtualMemory
{
    HANDLE process;

    ReleaseVirtualMemory(HANDLE process_) : process(process_) {}

    void operator()(void* p) const noexcept
    {
        if (p)
            VirtualFreeEx(process, p, 0, MEM_RELEASE);
    }
};

using VirtualMemoryScope = std::unique_ptr<void, ReleaseVirtualMemory>;

bool RemoteLoadLibrary(HANDLE hProcess, const char* szLibPath)
{
    if (!szLibPath || !szLibPath[0])
        return false;

    HMODULE kernel32 = GetModuleHandleA("kernel32");

    if (!kernel32)
        return false;

    // Allocate memory in the remote process for the library path
    size_t libraryPathSize = strlen(szLibPath) + 1;
    void*  remoteLibraryPath = VirtualAllocEx(hProcess, nullptr, libraryPathSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (!remoteLibraryPath)
        return false;

    VirtualMemoryScope remoteMemory(remoteLibraryPath, ReleaseVirtualMemory{hProcess});

    // Write the DLL library path to the remote allocation
    DWORD byteswritten = 0;
    WriteProcessMemory(hProcess, remoteLibraryPath, static_cast<LPCVOID>(szLibPath), libraryPathSize, &byteswritten);

    if (byteswritten != libraryPathSize)
        return false;

    // Start a remote thread executing LoadLibraryA exported from Kernel32. Passing the
    // remotely allocated path buffer as an argument to that thread (and also to LoadLibraryA)
    // will make the remote process load the DLL into it's userspace (giving the DLL full
    // access to the game executable).
    HANDLE remoteThread = CreateRemoteThread(
        hProcess, nullptr, 0, static_cast<LPTHREAD_START_ROUTINE>(static_cast<void*>(GetProcAddress(kernel32, "LoadLibraryA"))), remoteLibraryPath, 0, nullptr);

    if (!remoteThread)
        return false;

    // We wait for the created remote thread to finish executing. When it's done, the DLL
    // is loaded into the game's userspace, and we can destroy the thread-handle.
    WaitForSingleObject(remoteThread, INFINITE);
    return true;
}

#endif
#else
bool IsValidFilePath(const char* szDir)
{
    if (szDir == NULL)
        return false;

    unsigned int uiLen = strlen(szDir);

    if (uiLen > 0 && szDir[uiLen - 1] == '/')            // will return false if ending with an invalid character, mainly used for linux (#6871)
        return false;

    unsigned char c, c_d;

    // iterate through the char array
    for (unsigned int i = 0; i < uiLen; i++)
    {
        c = szDir[i];                                          // current character
        c_d = (i < (uiLen - 1)) ? szDir[i + 1] : 0;            // one character ahead, if any
        if (!IsVisibleCharacter(c) || c == ':' || (c == '.' && c_d == '.') || (c == '\\' && c_d == '\\'))
            return false;
    }
    return true;
}

bool IsValidOrganizationPath(const char* szDir)
{
    if (szDir == NULL)
        return false;

    unsigned int uiLen = strlen(szDir);

    if (uiLen > 0 && szDir[uiLen - 1] == '/')            // will return false if ending with an invalid character, mainly used for linux (#6871)
        return false;

    unsigned char c;
    bool          bInsideBraces = false;

    // iterate through the char array
    for (unsigned int i = 0; i < uiLen; i++)
    {
        c = szDir[i];            // current character

        // Enforce braces around visible letters
        if (!bInsideBraces && IsVisibleCharacter(c) && c != '[' && c != ']' && c != '/' && c != '\\')
            return false;

        if (c == '[')
        {
            if (bInsideBraces)
                return false;            // Duplicate braces (e.g. "[hel[lo]world]")
            else
                bInsideBraces = true;
        }
        else if (c == ']')
        {
            if (!bInsideBraces)
                return false;            // Ending brace without opening brace (e.g. "hello]")
            else
                bInsideBraces = false;
        }
        else if (c == '/' || c == '\\')
        {
            if (bInsideBraces)
                return false;            // Slash within braches (e.g. "[hell/o]")
        }
    }

    // Make sure we ended with closed braces
    return !bInsideBraces;
}
#endif

unsigned int HexToInt(const char* szHex)
{
    unsigned int value = 0;

    if (szHex[0] == '0' && (szHex[0] == 'x' || szHex[0] == 'X'))
    {
        szHex += 2;
    }

    while (*szHex != 0)
    {
        char c = *szHex++;

        value <<= 4;

        if (c >= 'A' && c <= 'F')
        {
            value += c - 'A' + 10;
        }
        else if (c >= 'a' && c <= 'f')
        {
            value += c - 'a' + 10;
        }
        else
        {
            value += c - '0';
        }
    }

    return value;
}

bool XMLColorToInt(const char* szColor, unsigned long& ulColor)
{
    const char validHexChars[] = "0123456789ABCDEFabcdef";

    unsigned int uiLength = strlen(szColor);
    // Make sure it starts with #
    if (szColor[0] == '#')
    {
        // #RGB #RGBA #RRGGBB #RRGGBBAA
        if (uiLength >= 4 && szColor[4] == 0)
        {
            // Make a RRGGBBAA string
            unsigned char szTemp[12]{};
            szTemp[0] = 'F';
            szTemp[1] = 'F';
            szTemp[2] = szColor[3];
            szTemp[3] = szColor[3];
            szTemp[4] = szColor[2];
            szTemp[5] = szColor[2];
            szTemp[6] = szColor[1];
            szTemp[7] = szColor[1];
            szTemp[8] = 0;

            // Make sure it only contains 0-9 and A-F
            if (strspn((const char*)szTemp, validHexChars) == 8)
            {
                ulColor = HexToInt((const char*)szTemp);
                return true;
            }
        }
        else if (uiLength >= 5 && szColor[5] == 0)
        {
            // Make a RRGGBBAA string
            unsigned char szTemp[12]{};
            szTemp[0] = szColor[4];
            szTemp[1] = szColor[4];
            szTemp[2] = szColor[3];
            szTemp[3] = szColor[3];
            szTemp[4] = szColor[2];
            szTemp[5] = szColor[2];
            szTemp[6] = szColor[1];
            szTemp[7] = szColor[1];
            szTemp[8] = 0;

            // Make sure it only contains 0-9 and A-F
            if (strspn((const char*)szTemp, validHexChars) == 8)
            {
                ulColor = HexToInt((const char*)szTemp);
                return true;
            }
        }
        else if (uiLength >= 7 && szColor[7] == 0)
        {
            // Make a RRGGBBAA string
            unsigned char szTemp[12]{};
            szTemp[0] = 'F';
            szTemp[1] = 'F';
            szTemp[2] = szColor[5];
            szTemp[3] = szColor[6];
            szTemp[4] = szColor[3];
            szTemp[5] = szColor[4];
            szTemp[6] = szColor[1];
            szTemp[7] = szColor[2];
            szTemp[8] = 0;

            // Make sure it only contains 0-9 and A-F
            if (strspn((const char*)szTemp, validHexChars) == 8)
            {
                ulColor = HexToInt((const char*)szTemp);
                return true;
            }
        }
        else if (uiLength >= 9 && szColor[9] == 0)
        {
            // Copy the string without the pre-#
            unsigned char szTemp[12]{};
            szTemp[0] = szColor[7];
            szTemp[1] = szColor[8];
            szTemp[2] = szColor[5];
            szTemp[3] = szColor[6];
            szTemp[4] = szColor[3];
            szTemp[5] = szColor[4];
            szTemp[6] = szColor[1];
            szTemp[7] = szColor[2];
            szTemp[8] = 0;

            // Make sure it only contains 0-9 and A-F
            if (strspn((const char*)szTemp, validHexChars) == 8)
            {
                ulColor = HexToInt((const char*)szTemp);
                return true;
            }
        }
    }

    return false;
}

bool XMLColorToInt(const char* szColor, unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue, unsigned char& ucAlpha)
{
    // Convert it to an integer first
    unsigned long ulColor;
    if (!XMLColorToInt(szColor, ulColor))
    {
        return false;
    }

    // Convert it to red, green, blue and alpha
    ucRed = static_cast<unsigned char>(ulColor);
    ucGreen = static_cast<unsigned char>(ulColor >> 8);
    ucBlue = static_cast<unsigned char>(ulColor >> 16);
    ucAlpha = static_cast<unsigned char>(ulColor >> 24);
    return true;
}

#ifdef MTA_CLIENT
//
// Safely read a ushort sized string from a NetBitStreamInterface
//
bool BitStreamReadUsString(class NetBitStreamInterface& bitStream, SString& strOut)
{
    bool bResult = false;

    // Read out the string length
    unsigned short usLength;
    if (bitStream.Read(usLength))
    {
        // Allocate a buffer and read the string into it
        char* szValue = new char[usLength + 1];
        // String with a length of zero is considered a success
        if (!usLength || bitStream.Read(szValue, usLength))
        {
            // Put it into us
            szValue[usLength] = 0;
            strOut = szValue;
            bResult = true;
        }

        // Delete the buffer
        delete[] szValue;
    }

    // Clear output on fail
    if (!bResult)
        strOut = "";

    return bResult;
}
#else
bool ReadSmallKeysync(CControllerState& ControllerState, NetBitStreamInterface& BitStream)
{
    SSmallKeysyncSync keys;
    if (!BitStream.Read(&keys))
        return false;

    // Put the result into the controllerstate
    ControllerState.LeftShoulder1 = keys.data.bLeftShoulder1;
    ControllerState.RightShoulder1 = keys.data.bRightShoulder1;
    short sButtonSquare = keys.data.bButtonSquare ? 255 : 0;
    short sButtonCross = keys.data.bButtonCross ? 255 : 0;
    if (BitStream.Can(eBitStreamVersion::AnalogControlSync_AccelBrakeReverse))
    {
        if (keys.data.ucButtonSquare != 0)
            sButtonSquare = (short)keys.data.ucButtonSquare;            // override controller state with analog data if present

        if (keys.data.ucButtonCross != 0)
            sButtonCross = (short)keys.data.ucButtonCross;            // override controller state with analog data if present
    }
    ControllerState.ButtonSquare = sButtonSquare;
    ControllerState.ButtonCross = sButtonCross;
    ControllerState.ButtonCircle = keys.data.bButtonCircle;
    ControllerState.ButtonTriangle = keys.data.bButtonTriangle;
    ControllerState.ShockButtonL = keys.data.bShockButtonL;
    ControllerState.m_bPedWalk = keys.data.bPedWalk;
    if (BitStream.Version() >= 0x2C)
    {
        ControllerState.LeftStickX = keys.data.sLeftStickX;
        ControllerState.LeftStickY = keys.data.sLeftStickY;
    }

    return true;
}

void WriteSmallKeysync(const CControllerState& ControllerState, NetBitStreamInterface& BitStream)
{
    SSmallKeysyncSync keys;
    keys.data.bLeftShoulder1 = (ControllerState.LeftShoulder1 != 0);                   // Action / Secondary-Fire
    keys.data.bRightShoulder1 = (ControllerState.RightShoulder1 != 0);                 // Aim-Weapon / Handbrake
    keys.data.bButtonSquare = (ControllerState.ButtonSquare != 0);                     // Jump / Reverse
    keys.data.bButtonCross = (ControllerState.ButtonCross != 0);                       // Sprint / Accelerate
    keys.data.bButtonCircle = (ControllerState.ButtonCircle != 0);                     // Fire // Fire
    keys.data.bButtonTriangle = (ControllerState.ButtonTriangle != 0);                 // Enter/Exit/Special-Attack / Enter/exit
    keys.data.bShockButtonL = (ControllerState.ShockButtonL != 0);                     // Crouch / Horn
    keys.data.bPedWalk = (ControllerState.m_bPedWalk != 0);                            // Walk / -
    keys.data.ucButtonSquare = (unsigned char)ControllerState.ButtonSquare;            // Reverse
    keys.data.ucButtonCross = (unsigned char)ControllerState.ButtonCross;              // Accelerate
    keys.data.sLeftStickX = ControllerState.LeftStickX;
    keys.data.sLeftStickY = ControllerState.LeftStickY;

    // Write it
    BitStream.Write(&keys);
}

bool ReadFullKeysync(CControllerState& ControllerState, NetBitStreamInterface& BitStream)
{
    // Read the keysync
    SFullKeysyncSync keys;
    if (!BitStream.Read(&keys))
        return false;

    // Put the result into the controllerstate
    ControllerState.LeftShoulder1 = keys.data.bLeftShoulder1;
    ControllerState.RightShoulder1 = keys.data.bRightShoulder1;
    short sButtonSquare = keys.data.bButtonSquare ? 255 : 0;
    short sButtonCross = keys.data.bButtonCross ? 255 : 0;
    if (BitStream.Can(eBitStreamVersion::AnalogControlSync_AccelBrakeReverse))
    {
        if (keys.data.ucButtonSquare != 0)
            sButtonSquare = (short)keys.data.ucButtonSquare;            // override controller state with analog data if present

        if (keys.data.ucButtonCross != 0)
            sButtonCross = (short)keys.data.ucButtonCross;            // override controller state with analog data if present
    }
    ControllerState.ButtonSquare = sButtonSquare;
    ControllerState.ButtonCross = sButtonCross;
    ControllerState.ButtonCircle = keys.data.bButtonCircle;
    ControllerState.ButtonTriangle = keys.data.bButtonTriangle;
    ControllerState.ShockButtonL = keys.data.bShockButtonL;
    ControllerState.m_bPedWalk = keys.data.bPedWalk;
    ControllerState.LeftStickX = keys.data.sLeftStickX;
    ControllerState.LeftStickY = keys.data.sLeftStickY;

    return true;
}

void WriteFullKeysync(const CControllerState& ControllerState, NetBitStreamInterface& BitStream)
{
    // Put the controllerstate bools into a key byte
    SFullKeysyncSync keys;
    keys.data.bLeftShoulder1 = (ControllerState.LeftShoulder1 != 0);
    keys.data.bRightShoulder1 = (ControllerState.RightShoulder1 != 0);
    keys.data.bButtonSquare = (ControllerState.ButtonSquare != 0);
    keys.data.bButtonCross = (ControllerState.ButtonCross != 0);
    keys.data.bButtonCircle = (ControllerState.ButtonCircle != 0);
    keys.data.bButtonTriangle = (ControllerState.ButtonTriangle != 0);
    keys.data.bShockButtonL = (ControllerState.ShockButtonL != 0);
    keys.data.bPedWalk = (ControllerState.m_bPedWalk != 0);
    keys.data.ucButtonSquare = (unsigned char)ControllerState.ButtonSquare;
    keys.data.ucButtonCross = (unsigned char)ControllerState.ButtonCross;
    keys.data.sLeftStickX = ControllerState.LeftStickX;
    keys.data.sLeftStickY = ControllerState.LeftStickY;

    // Write it
    BitStream.Write(&keys);
}

void ReadCameraOrientation(const CVector& vecBasePosition, NetBitStreamInterface& BitStream, CVector& vecOutCamPosition, CVector& vecOutCamFwd)
{
    //
    // Read rotations
    //
    const float         fPI = 3.14159265f;
    SFloatAsBitsSync<8> rotation(-fPI, fPI, false);

    BitStream.Read(&rotation);
    float fCamRotZ = rotation.data.fValue;

    BitStream.Read(&rotation);
    float fCamRotX = rotation.data.fValue;

    // Remake direction
    float fCosCamRotX = cos(fCamRotX);
    vecOutCamFwd.fX = fCosCamRotX * sin(fCamRotZ);
    vecOutCamFwd.fY = fCosCamRotX * cos(fCamRotZ);
    vecOutCamFwd.fZ = sin(fCamRotX);

    //
    // Read offset
    //

    // Lookup table used when sending
    struct
    {
        uint  uiNumBits;
        float fRange;
    } bitCountTable[4] = {
        {3, 4.0f},                // 3 bits is +-4        12 bits total
        {5, 16.0f},               // 5 bits is +-16       18 bits total
        {9, 256.0f},              // 9 bits is +-256      30 bits total
        {14, 8192.0f},            // 14 bits is +-8192    45 bits total
    };
    // Read flag
    bool bUseAbsolutePosition = false;
    BitStream.ReadBit(bUseAbsolutePosition);

    // Read table look up index for num of bits
    uchar idx = 0;
    BitStream.ReadBits((char*)&idx, 2);

    const uint  uiNumBits = bitCountTable[idx].uiNumBits;
    const float fRange = bitCountTable[idx].fRange;

    // Read each component
    SFloatAsBitsSyncBase position(uiNumBits, -fRange, fRange, false);

    CVector vecUsePosition;
    BitStream.Read(&position);
    vecUsePosition.fX = position.data.fValue;

    BitStream.Read(&position);
    vecUsePosition.fY = position.data.fValue;

    BitStream.Read(&position);
    vecUsePosition.fZ = position.data.fValue;

    // Remake position
    if (bUseAbsolutePosition)
        vecOutCamPosition = vecUsePosition;
    else
        vecOutCamPosition = vecBasePosition - vecUsePosition;
}

bool IsNametagValid(const char* szNick)
{
    // Grab the size of the nick. Check that it's not to long or short
    size_t sizeNick = MbUTF8ToUTF16(szNick).size();
    if (sizeNick < MIN_PLAYER_NAMETAG_LENGTH || sizeNick > MAX_PLAYER_NAMETAG_LENGTH)
    {
        return false;
    }

    // Check that each character is valid (visible characters exluding space)
    unsigned char ucTemp;
    for (size_t i = 0; i < sizeNick; i++)
    {
        ucTemp = szNick[i];
        if (ucTemp < 32)
        {
            return false;
        }
    }

    // nametag is valid, return true
    return true;
}

bool IsNickValid(const char* szNick)
{
    // Grab the size of the nick. Check that it's within the player
    size_t sizeNick = strlen(szNick);
    if (sizeNick < MIN_PLAYER_NICK_LENGTH || sizeNick > MAX_PLAYER_NICK_LENGTH)
    {
        return false;
    }

    // Check that each character is valid (visible characters exluding space)
    unsigned char ucTemp;
    for (size_t i = 0; i < sizeNick; i++)
    {
        ucTemp = szNick[i];
        if (ucTemp < 33 || ucTemp > 126)
        {
            return false;
        }
    }

    // Nickname is valid, return true
    return true;
}

void RotateVector(CVector& vecLine, const CVector& vecRotation)
{
    // Rotate it along the X axis
    // [ 1     0        0    0 ]
    // [ 0   cos a   sin a   0 ]
    // [ 0  -sin a   cos a   0 ]

    float fLineY = vecLine.fY;
    vecLine.fY = cos(vecRotation.fX) * fLineY + sin(vecRotation.fX) * vecLine.fZ;
    vecLine.fZ = -sin(vecRotation.fX) * fLineY + cos(vecRotation.fX) * vecLine.fZ;

    // Rotate it along the Y axis
    // [ cos a   0   -sin a   0 ]
    // [   0     1     0      0 ]
    // [ sin a   0    cos a   0 ]

    float fLineX = vecLine.fX;
    vecLine.fX = cos(vecRotation.fY) * fLineX - sin(vecRotation.fY) * vecLine.fZ;
    vecLine.fZ = sin(vecRotation.fY) * fLineX + cos(vecRotation.fY) * vecLine.fZ;

    // Rotate it along the Z axis
    // [  cos a   sin a   0   0 ]
    // [ -sin a   cos a   0   0 ]
    // [    0       0     1   0 ]

    fLineX = vecLine.fX;
    vecLine.fX = cos(vecRotation.fZ) * fLineX + sin(vecRotation.fZ) * vecLine.fY;
    vecLine.fY = -sin(vecRotation.fZ) * fLineX + cos(vecRotation.fZ) * vecLine.fY;
}

SString LongToDottedIP(unsigned long ulIP)
{
    in_addr in;
    in.s_addr = ulIP;
    return inet_ntoa(in);
}
#endif

// RX(theta)
// | 1              0               0       |
// | 0              c(theta)    -s(theta)   |
// | 0              s(theta)    c(theta)    |

// RY(theta)
// | c(theta)       0               s(theta)    |
// | 0              1               0           |
// | -s(theta)  0               c(theta)        |

// RZ(theta)
// | c(theta)       -s(theta)   0               |
// | s(theta)       c(theta)    0               |
// | 0              0               1           |

// ZXY = RZ(z).RX(x).RY(y)
// | c(y)*c(z)-s(x)*s(y)*s(z)       -c(x)*s(z)                          s(x)*c(y)*s(z)+s(y)*c(z)        |
// | c(y)*s(z)+s(x)*s(y)*c(z)       c(x)*c(z)                               s(y)*s(z)-s(x)*c(y)*c(z)    |
// | -c(x)*s(y)                             s(x)                                        c(x)*c(y)       |

// ZYX = RZ(z).RY(y).RX(x)
// | c(y)*c(z)                              s(x)*s(y)*c(z)-c(x)*s(z)        s(x)*s(z)+c(x)*s(y)*c(z)    |
// | c(y)*s(z)                              s(x)*s(y)*s(z)+c(x)*c(z)        c(x)*s(y)*s(z)-s(x)*c(z)    |
// | -s(y)                                      s(x)*c(y)                               c(x)*c(y)       |

CVector euler_ZXY_to_ZYX(const CVector& a_vZXY)
{
    CVector vZXY(a_vZXY);
    ConvertDegreesToRadiansNoWrap(vZXY);            // NoWrap for this conversion since it's used for cos/sin only

    float cx = cos(vZXY.fX);
    float sx = sin(vZXY.fX);
    float cy = cos(vZXY.fY);
    float sy = sin(vZXY.fY);
    float cz = cos(vZXY.fZ);
    float sz = sin(vZXY.fZ);

    CVector vZYX;

    // ZYX (unknown)     => A = s(x)*c(y)    /  c(x)*c(y)   = t(x)
    // ZXY (known)       => A = s(x)         /   c(x)*c(y)
    vZYX.fX = atan2(sx, cx * cy);

    // ZYX (unknown)     => B = c(y)*s(z)                /   c(y)*c(z)                   = t(z)
    // ZXY (known)       => B = c(y)*s(z)+s(x)*s(y)*c(z) /   c(y)*c(z)-s(x)*s(y)*s(z)
    vZYX.fZ = atan2(cy * sz + sx * sy * cz, cy * cz - sx * sy * sz);

    // ZYX (unknown)     => C = -s(y)
    // ZXY (known)       => C = -c(x)*s(y)
    // Isn't asin not as good as atan2 ? solution tried with atan2 doesn't work that well though
    vZYX.fY = asin(cx * sy);

    ConvertRadiansToDegrees(vZYX);

    return vZYX;
}

CVector euler_ZYX_to_ZXY(const CVector& a_vZYX)
{
    CVector vZYX(a_vZYX);
    ConvertDegreesToRadiansNoWrap(vZYX);            // NoWrap for this conversion since it's used for cos/sin only

    float cx = cos(vZYX.fX);
    float sx = sin(vZYX.fX);
    float cy = cos(vZYX.fY);
    float sy = sin(vZYX.fY);
    float cz = cos(vZYX.fZ);
    float sz = sin(vZYX.fZ);

    CVector vZXY;

    // ZXY (unknown)     => A = -c(x)*s(z)               /   c(x)*c(z)                   => t(z) = -A
    // ZYX (known)       => A = s(x)*s(y)*c(z)-c(x)*s(z) /   s(x)*s(y)*s(z)+c(x)*c(z)
    vZXY.fZ = atan2(-(sx * sy * cz - cx * sz), sx * sy * sz + cx * cz);

    // ZXY (unknown)     => B = -c(x)*s(y)   /       c(x)*c(y) => t(y) = -B
    // ZYX (known)       => B =  -s(y)       /       c(x)*c(y)
    vZXY.fY = atan2(sy, cx * cy);

    // ZXY (unknown)     => C = s(x)
    // ZYX (known)       => C =  s(x)*c(y)
    // Isn't asin not as good as atan2 ? solution tried with atan2 doesn't work that well though
    vZXY.fX = asin(sx * cy);

    ConvertRadiansToDegrees(vZXY);
    return vZXY;
}

CVector ConvertEulerRotationOrder(const CVector& a_vRotation, eEulerRotationOrder a_eSrcOrder, eEulerRotationOrder a_eDstOrder)
{
    if (a_eSrcOrder == a_eDstOrder || a_eSrcOrder == EULER_DEFAULT || a_eDstOrder == EULER_DEFAULT)
    {
        return a_vRotation;
    }

    if (a_eSrcOrder == EULER_ZXY && a_eDstOrder == EULER_ZYX)
    {
        return euler_ZXY_to_ZYX(a_vRotation);
    }
    else if (a_eSrcOrder == EULER_ZYX && a_eDstOrder == EULER_ZXY)
    {
        return euler_ZYX_to_ZXY(a_vRotation);
    }
    else if (a_eSrcOrder == EULER_MINUS_ZYX)
    {
        CVector vZYX;
        vZYX.fX = -a_vRotation.fX;
        vZYX.fY = -a_vRotation.fY;
        vZYX.fZ = -a_vRotation.fZ;
        return ConvertEulerRotationOrder(vZYX, EULER_ZYX, a_eDstOrder);
    }
    else if (a_eDstOrder == EULER_MINUS_ZYX)
    {
        CVector vZYX = ConvertEulerRotationOrder(a_vRotation, a_eSrcOrder, EULER_ZYX);
        vZYX.fX = -vZYX.fX;
        vZYX.fY = -vZYX.fY;
        vZYX.fZ = -vZYX.fZ;
        return vZYX;
    }
    else
    {
        return a_vRotation;
    }
}
