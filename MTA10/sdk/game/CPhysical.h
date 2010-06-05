/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CPhysical.h
*  PURPOSE:		Physical entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_PHYSICAL
#define __CGAME_PHYSICAL

#include "CEntity.h"

class CPhysical : public virtual CEntity
{
public:
    virtual             ~CPhysical ( void ) {};

	virtual CVector	*   GetMoveSpeed                ( CVector * vecMoveSpeed )=0;
	virtual CVector	*   GetTurnSpeed                ( CVector * vecTurnSpeed )=0;
	virtual VOID		SetMoveSpeed                ( CVector * vecMoveSpeed )=0;
	virtual VOID		SetTurnSpeed                ( CVector * vecTurnSpeed )=0;

    virtual float       GetMass                     ( void ) = 0;
    virtual void        SetMass                     ( float fMass ) = 0;
    virtual float       GetTurnMass                 ( void ) = 0;
    virtual void        SetTurnMass                 ( float fTurnMass ) = 0;
    virtual float       GetElasticity               ( void ) = 0;
    virtual void        SetElasticity               ( float fElasticity ) = 0;
    virtual float       GetBuoyancyConstant         ( void ) = 0;
    virtual void        SetBuoyancyConstant         ( float fBuoyancyConstant ) = 0;

	virtual VOID		ProcessCollision()=0;

    virtual float       GetDamageImpulseMagnitude   ( void ) = 0;
    virtual void        SetDamageImpulseMagnitude   ( float fMagnitude ) = 0;
    virtual CEntity*    GetDamageEntity             ( void ) = 0;
    virtual void        SetDamageEntity             ( CEntity* pEntity ) = 0;
    virtual void        ResetLastDamage             ( void ) = 0;

    virtual CEntity *   GetAttachedEntity           ( void ) = 0;
    virtual void        AttachEntityToEntity        ( CPhysical& Entity, const CVector& vecPosition, const CVector& vecRotation ) = 0;
    virtual void        DetachEntityFromEntity      ( float fUnkX, float fUnkY, float fUnkZ, bool bUnk )=0;
    virtual void        GetAttachedOffsets          ( CVector & vecPosition, CVector & vecRotation )=0;
    virtual void        SetAttachedOffsets          ( CVector & vecPosition, CVector & vecRotation )=0;

    virtual float       GetLighting                 ( void ) = 0;
    virtual void        SetLighting                 ( float fLighting ) = 0;

/*	virtual VOID		SetMass(FLOAT fMass)=0;
	virtual FLOAT		GetMass()=0;
	virtual VOID		SetTurnMass(FLOAT fTurnMass)=0;
	virtual FLOAT		GetTurnMass()=0;
	virtual VOID		SetMassMultiplier(FLOAT fMassMultiplier)=0;
	virtual FLOAT		GetMassMultiplier()=0;
	virtual VOID		SetAirResistance(FLOAT fAirResistance)=0;
	virtual FLOAT		GetAirResistance()=0;
	virtual VOID		SetElasticity(FLOAT fElasticity)=0;
	virtual FLOAT		GetElasticity()=0;
	virtual VOID		SetBuoyancyConstant(FLOAT fBuoyancyConstant)=0;
	virtual FLOAT		GetBuoyancyConstant()=0;
	virtual VOID		SetCenterOfMass(CVector * vecCenterOfMass)=0;
	virtual CVector	* GetCenterOfMass()=0;
	virtual BOOL		GetExtraHeavy()=0;
	virtual VOID		SetExtraHeavy(BOOL bExtraHeavy)=0;
	virtual BOOL		GetDoGravity()=0;
	virtual VOID		SetDoGravity(BOOL bDoGravity)=0;
	virtual BOOL		GetInfiniteMass()=0;
	virtual VOID		SetInfiniteMass(BOOL bInfiniteMass)=0;
	virtual BOOL		GetPositionFrozen()=0;
	virtual VOID		SetPositionFrozen(BOOL bPositionFrozen)=0;
	virtual BYTE		GetLastMaterialToHaveBeenStandingOn()=0;*/

};

#endif