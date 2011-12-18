/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRegisteredCoronaSA.h
*  PURPOSE:     Header file for registered corona entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_REGISTEREDCORONA
#define __CGAMESA_REGISTEREDCORONA

#include <game/CRegisteredCorona.h>

class CEntitySAInterface;

#define ARRAY_CORONAS               0xC3E058 // ##SA##

class CRegisteredCoronaSAInterface // coronas are 104 bytes long, and theres 56 of them
{
public:
    CVector     Coordinates;            // Where is it exactly.
    DWORD       Identifier;             // Should be unique for each corona. Address or something (0 = empty)
    RwTexture   * pTex;                 // Pointer to the actual texture to be rendered
    FLOAT       Size;                   // How big is this fellow
    FLOAT       NormalAngle;            // Is corona normal (if relevant) facing the camera?
    FLOAT       Range;                  // How far away is this guy still visible
    FLOAT       PullTowardsCam;         // How far away is the z value pulled towards camera.
    FLOAT       HeightAboveGround;      // Stired so that we don't have to do a ProcessVerticalLine every frame
                                        // The following fields are used for trails behind coronas (glowy lights)
    FLOAT       FadeSpeed;              // The speed the corona fades in and out ##SA##
    BYTE        Red, Green, Blue;       // Rendering colour.
    BYTE        Intensity;              // 255 = full
    BYTE        FadedIntensity;         // Intensity that lags behind the given intenisty and fades out if the LOS is blocked
    BYTE        RegisteredThisFrame;    // Has this guy been registered by game code this frame
    BYTE        FlareType;              // What type of flare to render
    BYTE        ReflectionType;         // What type of reflection during wet weather
    BYTE        LOSCheck : 1;           // Do we check the LOS or do we render at the right Z value
    BYTE        OffScreen : 1;          // Set by the rendering code to be used by the update code
    BYTE        JustCreated;            // If this guy has been created this frame we won't delete it (It hasn't had the time to get its OffScreen cleared) ##SA removed from packed byte ##
    BYTE        NeonFade : 1;           // Does the guy fade out when closer to cam
    BYTE        OnlyFromBelow : 1;      // This corona is only visible if the camera is below it. ##SA##
    BYTE        bHasValidHeightAboveGround : 1;
    BYTE        WhiteCore : 1;          // This corona rendered with a small white core.
    BYTE        bIsAttachedToEntity : 1;
    
    CEntitySAInterface  * pEntityAttachedTo;
};

/**
 * \todo Add support for setting WhiteCore, OnlyFromBelow and AttachedToEntity
 */
class CRegisteredCoronaSA : public CRegisteredCorona
{
private:
    CRegisteredCoronaSAInterface * internalInterface;
public:
                    CRegisteredCoronaSA(CRegisteredCoronaSAInterface * coronaInterface);
    CVector         * GetPosition();
    VOID            SetPosition(CVector * vector);
    FLOAT           GetSize();
    VOID            SetSize(FLOAT fSize);
    FLOAT           GetRange();
    VOID            SetRange(FLOAT fRange);
    FLOAT           GetPullTowardsCamera();
    VOID            SetPullTowardsCamera(FLOAT fPullTowardsCamera);
    VOID            SetColor(BYTE Red, BYTE Green, BYTE Blue, BYTE Alpha);
    VOID            SetTexture(RwTexture * texture);
    VOID            SetTexture(eCoronaType texture);
    BYTE            GetFlareType();
    VOID            SetFlareType(BYTE fFlareType);
    inline DWORD    GetIdentifier() { return internalInterface->Identifier; }
    DWORD           GetID();
    VOID            Init(DWORD Identifier);
    VOID            Disable();
    VOID            Refresh();
};

#endif
