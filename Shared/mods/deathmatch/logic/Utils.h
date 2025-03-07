/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/Utils.h
 *  PURPOSE:     Miscellaneous utility functions
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#ifdef MTA_CLIENT
extern CLocalizationInterface* g_pLocalization;
#else
    #include <CVector.h>
    #include <CVector2D.h>
    #include <net/bitstream.h>
    #include <cmath>
    #include "CCommon.h"
    #include "CPad.h"
    #include "packets/CPlayerDisconnectedPacket.h"
#endif

class CMatrix;

// Vector math
inline float DistanceBetweenPoints2D(const CVector& vecPosition1, const CVector& vecPosition2)
{
    float fDistanceX = vecPosition2.fX - vecPosition1.fX;
    float fDistanceY = vecPosition2.fY - vecPosition1.fY;

    return sqrt(fDistanceX * fDistanceX + fDistanceY * fDistanceY);
}

// Vector math
inline float DistanceBetweenPoints2D(const CVector2D& vecPosition1, const CVector2D& vecPosition2)
{
    float fDistanceX = vecPosition2.fX - vecPosition1.fX;
    float fDistanceY = vecPosition2.fY - vecPosition1.fY;

    return sqrt(fDistanceX * fDistanceX + fDistanceY * fDistanceY);
}

inline float HorizontalAngleBetweenPoints3D(const CVector& vecPosition1, const CVector& vecPosition2)
{
    CVector zeroVec;
    zeroVec.fX = 0;
    zeroVec.fY = 0;
    zeroVec.fZ = 0;
    float fLenA = DistanceBetweenPoints2D(zeroVec, vecPosition1);
    float fLenB = DistanceBetweenPoints2D(zeroVec, vecPosition2);
    float fRad = acos(((vecPosition1.fX * vecPosition2.fX) + (vecPosition1.fY * vecPosition2.fY)) / (fLenA * fLenB));
    return fRad;
    // return (fRad*180/PI);
}

inline float AngleBetweenPoints2D(const CVector& vecPosition1, const CVector& vecPosition2)
{
    float fRad = (PI * 2) - atan2((vecPosition2.fX - vecPosition1.fX), (vecPosition2.fY - vecPosition1.fY));
    // Clamp it to -PI .. PI
    if (fRad < -PI)
    {
        do
        {
            fRad += PI * 2.0f;
        } while (fRad < -PI);
    }
    else if (fRad > PI)
    {
        do
        {
            fRad -= PI * 2.0f;
        } while (fRad > PI);
    }
    return fRad;
}

inline float DistanceBetweenPoints3D(const CVector& vecPosition1, const CVector& vecPosition2)
{
    float fDistanceX = vecPosition2.fX - vecPosition1.fX;
    float fDistanceY = vecPosition2.fY - vecPosition1.fY;
    float fDistanceZ = vecPosition2.fZ - vecPosition1.fZ;

    return sqrt(fDistanceX * fDistanceX + fDistanceY * fDistanceY + fDistanceZ * fDistanceZ);
}

inline bool IsPointNearPoint2D(const CVector& vecPosition1, const CVector& vecPosition2, float fDistance)
{
    float fDistanceX = vecPosition2.fX - vecPosition1.fX;
    float fDistanceY = vecPosition2.fY - vecPosition1.fY;

    return (fDistanceX * fDistanceX + fDistanceY * fDistanceY <= fDistance * fDistance);
}

inline bool IsPointNearPoint3D(const CVector& vecPosition1, const CVector& vecPosition2, float fDistance)
{
    float fDistanceX = vecPosition2.fX - vecPosition1.fX;
    float fDistanceY = vecPosition2.fY - vecPosition1.fY;
    float fDistanceZ = vecPosition2.fZ - vecPosition1.fZ;

    return (fDistanceX * fDistanceX + fDistanceY * fDistanceY + fDistanceZ * fDistanceZ <= fDistance * fDistance);
}

#ifndef MTA_CLIENT
bool  CheckNickProvided(const char* szNick);
float DistanceBetweenPoints2D(const CVector& vecPosition1, const CVector& vecPosition2);
float DistanceBetweenPoints2D(const CVector2D& vecPosition1, const CVector2D& vecPosition2);
float DistanceBetweenPoints3D(const CVector& vecPosition1, const CVector& vecPosition2);
bool  IsPointNearPoint2D(const CVector& vecPosition1, const CVector& vecPosition2, float fDistance);
bool  IsPointNearPoint3D(const CVector& vecPosition1, const CVector& vecPosition2, float fDistance);

void ReplaceCharactersInString(char* szString, char cWhat, char cWith);
void ReplaceOccurrencesInString(std::string& s, const char* a, const char* b);

bool DoesDirectoryExist(const char* szPath);

char* uppercase(char* s);
void  stripString(char* szString);
void  stripControlCodes(char* szString);
bool  StringBeginsWith(const char* szText, const char* szBegins);
bool  IsNumericString(const char* szString);
bool  IsNumericString(const char* szString, size_t sizeString);

void DisconnectPlayer(class CGame* pGame, class CPlayer& Player, const char* szMessage);
void DisconnectPlayer(class CGame* pGame, class CPlayer& Player, CPlayerDisconnectedPacket::ePlayerDisconnectType eDisconnectType, const char* szMessage = "");
void DisconnectPlayer(class CGame* pGame, class CPlayer& Player, CPlayerDisconnectedPacket::ePlayerDisconnectType eDisconnectType, time_t BanDuration,
                      const char* szMessage = "");
void DisconnectConnectionDesync(class CGame* pGame, class CPlayer& Player, unsigned int uiCode);

bool InitializeSockets();
bool CleanupSockets();

double GetRandomDouble();
int    GetRandom(int iLow, int iHigh);

bool IsValidFilePath(const char* szPath);
bool IsValidFilePath(const char* szDir, size_t length);
bool IsValidOrganizationPath(const char* szPath);
#endif

inline float WrapAround(float fValue, float fHigh)
{
    return fValue - (fHigh * floor(static_cast<float>(fValue / fHigh)));
}

// Radians to degrees and vice versa
inline float ConvertRadiansToDegrees(float fRotation)
{
    return WrapAround(static_cast<float>(fRotation * 180.0f / PI + 360.0f), 360.0f);
}

inline float ConvertRadiansToDegreesNoWrap(float fRotation)
{
    return static_cast<float>(fRotation * 180.0f / PI);
}

inline void ConvertRadiansToDegrees(CVector2D& vecRotation)
{
    vecRotation.fX = ConvertRadiansToDegrees(vecRotation.fX);
    vecRotation.fY = ConvertRadiansToDegrees(vecRotation.fY);
}

inline void ConvertRadiansToDegreesNoWrap(CVector2D& vecRotation)
{
    vecRotation.fX = ConvertRadiansToDegreesNoWrap(vecRotation.fX);
    vecRotation.fY = ConvertRadiansToDegreesNoWrap(vecRotation.fY);
}

inline void ConvertRadiansToDegrees(CVector& vecRotation)
{
    vecRotation.fX = ConvertRadiansToDegrees(vecRotation.fX);
    vecRotation.fY = ConvertRadiansToDegrees(vecRotation.fY);
    vecRotation.fZ = ConvertRadiansToDegrees(vecRotation.fZ);
}

inline void ConvertRadiansToDegreesNoWrap(CVector& vecRotation)
{
    vecRotation.fX = ConvertRadiansToDegreesNoWrap(vecRotation.fX);
    vecRotation.fY = ConvertRadiansToDegreesNoWrap(vecRotation.fY);
    vecRotation.fZ = ConvertRadiansToDegreesNoWrap(vecRotation.fZ);
}

inline float ConvertDegreesToRadians(float fRotation)
{
    return WrapAround(static_cast<float>(fRotation * PI / 180.0f + 2 * PI), static_cast<float>(2 * PI));
}

inline float ConvertDegreesToRadiansNoWrap(float fRotation)
{
    return static_cast<float>(fRotation * PI / 180.0f);
}

inline void ConvertDegreesToRadians(CVector2D& vecRotation)
{
    vecRotation.fX = ConvertDegreesToRadians(vecRotation.fX);
    vecRotation.fY = ConvertDegreesToRadians(vecRotation.fY);
}

inline void ConvertDegreesToRadiansNoWrap(CVector2D& vecRotation)
{
    vecRotation.fX = ConvertDegreesToRadiansNoWrap(vecRotation.fX);
    vecRotation.fY = ConvertDegreesToRadiansNoWrap(vecRotation.fY);
}

inline void ConvertDegreesToRadians(CVector& vecRotation)
{
    vecRotation.fX = ConvertDegreesToRadians(vecRotation.fX);
    vecRotation.fY = ConvertDegreesToRadians(vecRotation.fY);
    vecRotation.fZ = ConvertDegreesToRadians(vecRotation.fZ);
}
inline CVector GetExtrapolatedSpeed(const CVector& vecOne, unsigned long ul_time_1, const CVector& vecTwo, unsigned long ul_time_2)
{
    CVector       vecSpeed;
    unsigned long dt = ul_time_2 - ul_time_1;
    vecSpeed.fX = (vecTwo.fX - vecOne.fX) / dt;
    vecSpeed.fY = (vecTwo.fY - vecOne.fY) / dt;
    vecSpeed.fZ = (vecTwo.fZ - vecOne.fZ) / dt;
    return vecSpeed;
}
inline void ConvertDegreesToRadiansNoWrap(CVector& vecRotation)
{
    vecRotation.fX = ConvertDegreesToRadiansNoWrap(vecRotation.fX);
    vecRotation.fY = ConvertDegreesToRadiansNoWrap(vecRotation.fY);
    vecRotation.fZ = ConvertDegreesToRadiansNoWrap(vecRotation.fZ);
}
inline float GetOffsetDegrees(float a, float b)
{
    float c = (b > a) ? b - a : 0.0f - (a - b);
    if (c > 180.0f)
        c = 0.0f - (360.0f - c);
    else if (c <= -180.0f)
        c = (360.0f + c);
    return c;
}
inline float GetOffsetRadians(float a, float b)
{
    float c = (b > a) ? b - a : 0.0f - (a - b);
    if (c > PI)
        c = 0.0f - (2 * PI - c);
    else if (c <= -PI)
        c = (2 * PI + c);
    return c;
}

// Assuming fValue is the result of a difference calculation, calculate
// the shortest positive distance after wrapping
inline float GetSmallestWrapUnsigned(float fValue, float fHigh)
{
    float fWrapped = fValue - (fHigh * floor(static_cast<float>(fValue / fHigh)));
    if (fWrapped > fHigh / 2)
        fWrapped = fHigh - fWrapped;
    return fWrapped;
}

void RotateVector(CVector& vecLine, const CVector& vecRotation);

#ifdef MTA_CLIENT
// Misc utility functions
unsigned int StripUnwantedCharacters(char* szText, unsigned char cReplace = ' ');
unsigned int StripControlCodes(char* szText, unsigned char cReplace = ' ');
bool         IsControlCode(unsigned char c);
bool         IsValidFilePath(const char* szDir);
bool         IsValidFilePath(const char* szDir, size_t length);
void         ReplaceOccurrencesInString(std::string& s, const char* a, const char* b);

void RaiseFatalError(unsigned int uiCode);
void RaiseProtocolError(unsigned int uiCode);

void AttachedMatrix(const CMatrix& matrix, CMatrix& returnMatrix, const CVector& vecPosition, const CVector& vecRotation);

unsigned int GetRandom(unsigned int uiLow, unsigned int uiHigh);
double       GetRandomDouble();

SString GetDataUnit(unsigned long long ullInput);

// Utility network functions
void LongToDottedIP(unsigned long ulIP, char* szDottedIP);

bool BitStreamReadUsString(class NetBitStreamInterface& bitStream, SString& strOut);
#else

bool ReadSmallKeysync(CControllerState& ControllerState, NetBitStreamInterface& BitStream);
void WriteSmallKeysync(const CControllerState& ControllerState, NetBitStreamInterface& BitStream);
bool ReadFullKeysync(CControllerState& ControllerState, NetBitStreamInterface& BitStream);
void WriteFullKeysync(const CControllerState& ControllerState, NetBitStreamInterface& BitStream);
void ReadCameraOrientation(const CVector& vecBasePosition, NetBitStreamInterface& BitStream, CVector& vecOutCamPosition, CVector& vecOutCamFwd);

// Validation funcs
bool IsNickValid(const char* szNick);
bool IsNametagValid(const char* szNick);

// Network funcs
SString LongToDottedIP(unsigned long ulIP);

inline SString SQLEscape(const SString& strEscapeString, bool bSingleQuotes, bool bDoubleQuotes)
{
    SString strParsedQuery = "";
    for (unsigned int k = 0; k < strEscapeString.length(); k++)
    {
        if (bSingleQuotes && strEscapeString[k] == '\'')
            strParsedQuery += '\'';
        if (bDoubleQuotes && strEscapeString[k] == '\"')
            strParsedQuery += '\"';

        strParsedQuery += strEscapeString[k];
    }
    return strParsedQuery;
}
#endif

inline bool IsVisibleCharacter(unsigned char c)
{
    // 32..126 are visible characters
    return c >= 32 && c <= 126;
}

unsigned int HexToInt(const char* szHex);
bool         XMLColorToInt(const char* szColor, unsigned long& ulColor);
bool         XMLColorToInt(const char* szColor, unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue, unsigned char& ucAlpha);

// Maths utility functions
CVector ConvertEulerRotationOrder(const CVector& a_vRotation, eEulerRotationOrder a_eSrcOrder, eEulerRotationOrder a_eDstOrder);

// Clear list of object pointers
template <class T>
void DeletePointersAndClearList(T& elementList)
{
    T cloneList = elementList;
    elementList.clear();

    typename T::const_iterator iter = cloneList.begin();
    for (; iter != cloneList.end(); iter++)
    {
        delete *iter;
    }
}
