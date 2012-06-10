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
#include "RenderWare.h"

#include <windows.h>

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
    STATUS_PLAYER_REMOTE,
    STATUS_PLAYER_DISABLED,
    STATUS_TRAILER,
    STATUS_SIMPLE_TRAILER
};

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
    virtual RwFrame *                   GetFrameFromId ( int id )=0;
    virtual RwMatrix *                  GetLTMFromId ( int id )=0;

    virtual RpClump *                   GetRpClump () = 0;

    virtual bool                        IsVisible ( void ) = 0;
    virtual void                        SetVisible ( bool bVisible ) = 0;

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

    virtual unsigned long               GetArrayID ( void ) = 0;
};

#endif
