/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CUtils.h
*  PURPOSE:     Util functions header
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Derek Abdine <>
*               Chris McArthur <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*****************************************************************************/

#ifndef __UTILS_H
#define __UTILS_H

#ifndef PI
#define PI 3.14159265358979323846264338327950f
#endif


const char* GetFilenameFromPath ( const char* szPath );

// Vector math
inline float DistanceBetweenPoints2D ( const CVector& vecPosition1, const CVector& vecPosition2 )
{
	float fDistanceX = vecPosition2.fX - vecPosition1.fX;
	float fDistanceY = vecPosition2.fY - vecPosition1.fY;

	return sqrt ( fDistanceX * fDistanceX + fDistanceY * fDistanceY );
}
inline float HorizontalAngleBetweenPoints3D ( const CVector &vecPosition1, const CVector &vecPosition2 )
{
    CVector zeroVec;
    zeroVec.fX = 0;
    zeroVec.fY = 0;
    zeroVec.fZ = 0;
    float fLenA = DistanceBetweenPoints2D( zeroVec, vecPosition1 );
    float fLenB = DistanceBetweenPoints2D( zeroVec, vecPosition2 );
    float fRad = acos((( vecPosition1.fX * vecPosition2.fX ) + ( vecPosition1.fY * vecPosition2.fY )) / (fLenA*fLenB));
    return fRad;
    //return (fRad*180/PI);
}
inline float DistanceBetweenPoints3D ( const CVector& vecPosition1, const CVector& vecPosition2 )
{
	float fDistanceX = vecPosition2.fX - vecPosition1.fX;
	float fDistanceY = vecPosition2.fY - vecPosition1.fY;
	float fDistanceZ = vecPosition2.fZ - vecPosition1.fZ;

	return sqrt ( fDistanceX * fDistanceX + fDistanceY * fDistanceY + fDistanceZ * fDistanceZ );
}

inline bool IsPointNearPoint2D ( const CVector& vecPosition1, const CVector& vecPosition2, float fDistance )
{
	float fDistanceX = vecPosition2.fX - vecPosition1.fX;
	float fDistanceY = vecPosition2.fY - vecPosition1.fY;

	return ( fDistanceX * fDistanceX + fDistanceY * fDistanceY <= fDistance * fDistance );
}

inline bool IsPointNearPoint3D ( const CVector& vecPosition1, const CVector& vecPosition2, float fDistance )
{
	float fDistanceX = vecPosition2.fX - vecPosition1.fX;
	float fDistanceY = vecPosition2.fY - vecPosition1.fY;
	float fDistanceZ = vecPosition2.fZ - vecPosition1.fZ;

	return ( fDistanceX * fDistanceX + fDistanceY * fDistanceY + fDistanceZ * fDistanceZ <= fDistance * fDistance );
}

inline float WrapAround ( float fValue, float fHigh )
{
    return fValue - ( fHigh * floor ( static_cast < float > ( fValue / fHigh ) ) );
}

// Radians to degrees and vice versa
inline float ConvertRadiansToDegrees ( float fRotation )
{
    return WrapAround ( static_cast < float > ( fRotation * 180.0f / PI + 360.0f ), 360.0f );
}

inline float ConvertRadiansToDegreesNoWrap ( float fRotation )
{
    return static_cast < float > ( fRotation * 180.0f / PI);
}

inline void ConvertRadiansToDegrees ( CVector2D& vecRotation )
{
    vecRotation.fX = ConvertRadiansToDegrees ( vecRotation.fX );
    vecRotation.fY = ConvertRadiansToDegrees ( vecRotation.fY );
}

inline void ConvertRadiansToDegreesNoWrap ( CVector2D& vecRotation )
{
    vecRotation.fX = ConvertRadiansToDegreesNoWrap ( vecRotation.fX );
    vecRotation.fY = ConvertRadiansToDegreesNoWrap ( vecRotation.fY );
}

inline void ConvertRadiansToDegrees ( CVector& vecRotation )
{
    vecRotation.fX = ConvertRadiansToDegrees ( vecRotation.fX );
    vecRotation.fY = ConvertRadiansToDegrees ( vecRotation.fY );
    vecRotation.fZ = ConvertRadiansToDegrees ( vecRotation.fZ );
}

inline void ConvertRadiansToDegreesNoWrap ( CVector& vecRotation )
{
    vecRotation.fX = ConvertRadiansToDegreesNoWrap ( vecRotation.fX );
    vecRotation.fY = ConvertRadiansToDegreesNoWrap ( vecRotation.fY );
    vecRotation.fZ = ConvertRadiansToDegreesNoWrap ( vecRotation.fZ );
}

inline float ConvertDegreesToRadians ( float fRotation )
{
    return WrapAround ( static_cast < float > ( fRotation * PI / 180.0f + 2*PI ), static_cast < float > ( 2*PI ) );
}

inline float ConvertDegreesToRadiansNoWrap ( float fRotation )
{
    return static_cast < float > ( fRotation * PI / 180.0f );
}

inline void ConvertDegreesToRadians ( CVector2D& vecRotation )
{
    vecRotation.fX = ConvertDegreesToRadians ( vecRotation.fX );
    vecRotation.fY = ConvertDegreesToRadians ( vecRotation.fY );
}

inline void ConvertDegreesToRadiansNoWrap ( CVector2D& vecRotation )
{
    vecRotation.fX = ConvertDegreesToRadiansNoWrap ( vecRotation.fX );
    vecRotation.fY = ConvertDegreesToRadiansNoWrap ( vecRotation.fY );
}

inline void ConvertDegreesToRadians ( CVector& vecRotation )
{
    vecRotation.fX = ConvertDegreesToRadians ( vecRotation.fX );
    vecRotation.fY = ConvertDegreesToRadians ( vecRotation.fY );
    vecRotation.fZ = ConvertDegreesToRadians ( vecRotation.fZ );
}
inline CVector Extrapolate(const CVector &vecOld, const CVector &vecSpeed, long ul_delta_t )
{
    CVector vecRet;
    float fDelta_t = static_cast < float > ( ul_delta_t );
    vecRet.fX = vecOld.fX + ( vecSpeed.fX*fDelta_t );
    vecRet.fY = vecOld.fY + ( vecSpeed.fY*fDelta_t );
    vecRet.fZ = vecOld.fZ + ( vecSpeed.fZ*fDelta_t );
    return vecRet;
}
inline CVector GetExtrapolatedSpeed(const CVector &vecOne, unsigned long ul_time_1, const CVector &vecTwo, unsigned long ul_time_2)
{
    CVector vecSpeed;
    unsigned long dt = ul_time_2 - ul_time_1;
    vecSpeed.fX = ( vecTwo.fX - vecOne.fX )/dt;
    vecSpeed.fY = ( vecTwo.fY - vecOne.fY )/dt;
    vecSpeed.fZ = ( vecTwo.fZ - vecOne.fZ )/dt;
    return vecSpeed;
}
inline void ConvertDegreesToRadiansNoWrap ( CVector& vecRotation )
{
    vecRotation.fX = ConvertDegreesToRadiansNoWrap ( vecRotation.fX );
    vecRotation.fY = ConvertDegreesToRadiansNoWrap ( vecRotation.fY );
    vecRotation.fZ = ConvertDegreesToRadiansNoWrap ( vecRotation.fZ );
}
inline float GetOffsetDegrees ( float a, float b )
{
    float c = ( b > a ) ? b - a : 0.0f - ( a - b );
    if ( c > 180.0f )
        c = 0.0f - ( 360.0f - c );
    else if ( c <= -180.0f )
        c = ( 360.0f + c );
    return c;
}

bool            DoesFileExist               ( const char* szFilename );

// Misc utility functions
char*           ReplaceAnyStringOccurrence  ( char* szBuffer, const char* szWhat, const char* szWith, size_t sizeMax );
unsigned int    StripUnwantedCharacters     ( char* szText, unsigned char cReplace = ' ' );
bool            IsWantedCharacter           ( unsigned char c );
bool            IsValidFilePath	            ( const char* szDir );
void			ReplaceOccurrencesInString  ( std::string &s, const char *a, const char *b );

void            RaiseFatalError             ( unsigned int uiCode );
void            RaiseProtocolError          ( unsigned int uiCode );

void            RotateVector                ( CVector& vecLine, const CVector& vecRotation );

unsigned int    GetRandom                   ( unsigned int uiLow, unsigned int uiHigh );
double          GetRandomDouble             ( void );
float           GetRandomFloat              ( void );

unsigned int    HashString                  ( const char* szString );

SString         GetDataUnit                 ( unsigned int uiInput );

unsigned int    HexToInt                    ( const char* szHex );
bool            XMLColorToInt               ( const char* szColor, unsigned long& ulColor );
bool            XMLColorToInt               ( const char* szColor, unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue, unsigned char& ucAlpha );

// Utility network functions
void            LongToDottedIP              ( unsigned long ulIP, char* szDottedIP );

bool            BitStreamReadUsString       ( class NetBitStreamInterface& bitStream, SString& strOut );

void            MakeSureDirExists           ( const char* szPath );


// for debug
#ifdef MTA_DEBUG
HMODULE RemoteLoadLibrary(HANDLE hProcess, const char* szLibPath);
#endif

#endif
