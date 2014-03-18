/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CEntitySA.renderutils.h
*  PURPOSE:     Entity render utilities
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _ENTITY_RENDER_UTILITIES_
#define _ENTITY_RENDER_UTILITIES_

namespace EntityRender
{
    bool __cdecl    AreScreenEffectsEnabled         ( void );
    void __cdecl    SetupNightVisionLighting        ( void );
    void __cdecl    SetupInfraRedLighting           ( void );

    void __cdecl    NormalizeDayNight               ( void );
    void __cdecl    RestoreDayNight                 ( void );

    void            SetGlobalDrawDistanceScale      ( float scale );
    float           GetGlobalDrawDistanceScale      ( void );
    float           GetGlobalDrawDistance           ( void );

    float           CalculateFadingAlphaEx          ( CBaseModelInfoSAInterface *info, const CEntitySAInterface *entity, float camDistance, float camFarClip, float sectorDivide, float useDist );
    float           CalculateFadingAlpha            ( CBaseModelInfoSAInterface *info, const CEntitySAInterface *entity, float camDistance, float camFarClip );
    float           GetComplexCameraEntityDistance  ( const CEntitySAInterface *entity, const CVector& camPos );

    void __cdecl    DisableSecondLighting           ( void );
    void __cdecl    SetupPostProcessLighting        ( void );

    float           GetEntityCameraDistance         ( CEntitySAInterface *entity );
};

#endif //_ENTITY_RENDER_UTILITIES_