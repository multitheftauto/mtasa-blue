/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CEntity.h
*  PURPOSE:     Base entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_ENTITY
#define __CGAME_ENTITY

#include "Common.h"
#include <CMatrix.h>
#include <CVector.h>

#include <windows.h>

struct RpClump;

enum eEntityType
{
    ENTITY_TYPE_NOTHING,
    ENTITY_TYPE_BUILDING,
    ENTITY_TYPE_VEHICLE,
    ENTITY_TYPE_PED,
    ENTITY_TYPE_OBJECT,
    ENTITY_TYPE_DUMMY,
    ENTITY_TYPE_NOTINPOOLS
};

enum eEntityStatus
{
    STATUS_PLAYER,
    STATUS_PLAYER_PLAYBACKFROMBUFFER,
    STATUS_SIMPLE,
    STATUS_PHYSICS,
    STATUS_ABANDONED,
    STATUS_WRECKED,
    STATUS_TRAIN_MOVING,
    STATUS_TRAIN_NOT_MOVING,
    STATUS_HELI,
    STATUS_PLANE,
    STATUS_REMOTE_CONTROLLED,
    STATUS_PLAYER_DISABLED,
    STATUS_TRAILER, // ToDo: Check this. I can't reproduce that this is set for trailers
    STATUS_SIMPLE_TRAILER // Same here
};

enum eRenderModeValueType
{
    RMODE_VOID,
    RMODE_BOOLEAN,
    RMODE_FLOAT,
    RMODE_INT
};

struct rModeResult
{
    inline rModeResult( void )
    {
        successful = false;
    }

    inline rModeResult( bool result )
    {
        successful = result;
    }

    inline rModeResult( bool result, const char *failureReason )
    {
        successful = result;
        debugMsg = failureReason;
    }
    
    bool successful;
    std::string debugMsg;
};

enum eEntityRenderMode
{
    ENTITY_RMODE_LIGHTING,
    ENTITY_RMODE_LIGHTING_AMBIENT,
    ENTITY_RMODE_LIGHTING_DIRECTIONAL,
    ENTITY_RMODE_LIGHTING_POINT,
    ENTITY_RMODE_LIGHTING_SPOT,
    ENTITY_RMODE_LIGHTING_MATERIAL,
    ENTITY_RMODE_REFLECTION,
    ENTITY_RMODE_ALPHACLAMP,
    ENTITY_RMODE_MAX
};

typedef bool (__cdecl*entityReferenceCallback_t)( class CEntity *entity );
typedef void (__cdecl*gameEntityPreRenderCallback_t)( void );
typedef void (__cdecl*gameEntityRenderCallback_t)( void );
typedef void (__cdecl*gameEntityRenderUnderwaterCallback_t)( void );
typedef void (__cdecl*gameEntityPostProcessCallback_t)( void );


class CEntity
{
public:
    virtual                             ~CEntity ( void ) {};

    //  virtual VOID                        SetModelAlpha ( int iAlpha )=0;
    virtual class CEntitySAInterface *  GetInterface()=0;
    virtual VOID                        SetPosition(float fX, float fY, float fZ)=0;
    virtual VOID                        SetPosition ( CVector * vecPosition )=0;
    virtual VOID                        Teleport ( float fX, float fY, float fZ )=0;
    virtual VOID                        ProcessControl ( void )=0;
    virtual VOID                        SetupLighting ( )=0;
    virtual VOID                        Render ( )=0;
    virtual VOID                        SetOrientation ( float fX, float fY, float fZ )=0;
    virtual VOID                        FixBoatOrientation ( void ) = 0;
    virtual CVector                     * GetPosition (  )=0;
    virtual CMatrix                     * GetMatrix ( CMatrix * matrix )=0;
    virtual VOID                        SetMatrix ( CMatrix * matrix )=0;
    virtual WORD                        GetModelIndex ()=0;
    virtual eEntityType                 GetEntityType ()=0;
    virtual FLOAT                       GetDistanceFromCentreOfMassToBaseOfModel()=0;
    virtual VOID                        SetEntityStatus( eEntityStatus bStatus )=0;
    virtual eEntityStatus               GetEntityStatus( )=0;
    virtual bool                        IsOnScreen ()=0;
    virtual bool                        IsFullyVisible ()=0;
    virtual void                        SetUnderwater ( bool bUnderwater )=0;
    virtual bool                        GetUnderwater ( void )=0;

    virtual RpClump *                   GetRpClump () = 0;

    virtual bool                        IsVisible ( void ) = 0;
    virtual void                        SetVisible ( bool bVisible ) = 0;

    virtual rModeResult                 SetEntityRenderModeBool ( eEntityRenderMode rMode, bool value ) = 0;
    virtual rModeResult                 SetEntityRenderModeInt ( eEntityRenderMode rMode, int value ) = 0;
    virtual rModeResult                 SetEntityRenderModeFloat ( eEntityRenderMode rMode, float value ) = 0;

    virtual rModeResult                 GetEntityRenderModeBool ( eEntityRenderMode rMode, bool& value ) const = 0;
    virtual rModeResult                 GetEntityRenderModeInt ( eEntityRenderMode rMode, int& value ) const = 0;
    virtual rModeResult                 GetEntityRenderModeFloat ( eEntityRenderMode rMode, float& value ) const = 0;

    virtual rModeResult                 ResetEntityRenderMode ( eEntityRenderMode rMode ) = 0;

    virtual VOID                        SetDoNotRemoveFromGameWhenDeleted ( bool bDoNotRemoveFromGame )=0;
    virtual VOID                        SetUsesCollision(BOOL bUsesCollision)=0;
    virtual BOOL                        IsBackfaceCulled(VOID)=0;
    virtual VOID                        SetBackfaceCulled(BOOL bBackfaceCulled)=0;
    virtual BOOL                        IsStatic(VOID)=0;
    virtual VOID                        SetStatic(BOOL bStatic)=0;
    virtual VOID                        SetAlpha(DWORD dwAlpha)=0;

    virtual VOID                        MatrixConvertFromEulerAngles ( float fX, float fY, float fZ, int iUnknown )=0;
    virtual VOID                        MatrixConvertToEulerAngles ( float * fX, float * fY, float * fZ, int iUnknown )=0;
    virtual bool                        IsPlayingAnimation ( char * szAnimName )=0;

    virtual void*                       GetStoredPointer    ( void ) = 0;
    virtual void                        SetStoredPointer    ( void* pPointer ) = 0;

    virtual BYTE                        GetAreaCode ( void ) = 0;
    virtual void                        SetAreaCode ( BYTE areaCode ) = 0;

    virtual bool                        IsStaticWaitingForCollision ( void ) = 0;
    virtual void                        SetStaticWaitingForCollision ( bool bStatic ) = 0;
};

#endif
