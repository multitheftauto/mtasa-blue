/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CObjectSA.h
*  PURPOSE:     Header file for object entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "CVehicleSA.h"
#include "CPedSA.h"

#ifndef __CGAMESA_OBJECT
#define __CGAMESA_OBJECT

#include <game/CObject.h>
#include "CPhysicalSA.h"


#define FUNC_CObject_Create             0x5A1F60
#define FUNC_CObject_Explode            0x5A1340

class CObjectInfo
{
public:
	float fMass;
	float fTurnMass;
	float fAirResistance;
	float fElasticity;
	float fBuoyancy;
	float fUprootLimit;
	float fColDamageMultiplier;
	uint8 ucColDamageEffect;
	uint8 ucSpecialColResponseCase;
	uint8 ucCameraAvoidObject;
	uint8 ucCausesExplosion;
	uint8 ucFxType;
	uint8 pad1[3];
	CVector vecFxOffset;
	void* pFxSystem; // ret from CParticleData::GetDataFromName
	float fSmashMultiplier;
	CVector vecBreakVelocity;
	float fBreakVelocityRand;
	uint32 uiGunBreakMode;
	uint32 uiSparksOnImpact;
};
// TODO: Find out correct size
//C_ASSERT(sizeof(CObjectInfo) == 0x50);

class CObjectSAInterface : public CPhysicalSAInterface
{
	void* pObjectList; // 312
	uint8 pad1; // 316
	uint8 pad2; // 317
	uint16 pad3; // 318
	
	// flags
	uint32 b0x01 : 1; // 320
	uint32 b0x02 : 1;
	uint32 b0x04 : 1;
	uint32 b0x08 : 1;
	uint32 b0x10 : 1;
	uint32 b0x20 : 1;
	uint32 b0x40 : 1;
	uint32 b0x80 : 1;

	uint32 b0x100 : 1; // 321
	uint32 b0x200 : 1;
	uint32 b0x400 : 1;
	uint32 b0x800 : 1;
	uint32 b0x1000 : 1;
	uint32 b0x2000 : 1;
	uint32 b0x4000 : 1;
	uint32 b0x8000 : 1;

	uint32 b0x10000 : 1; // 322
	uint32 b0x20000 : 1;
	uint32 b0x40000 : 1;
	uint32 b0x80000 : 1;
	uint32 b0x100000 : 1;
	uint32 b0x200000 : 1;
	uint32 b0x400000 : 1;
	uint32 b0x800000 : 1;

	uint32 b0x1000000 : 1; // 323
	uint32 b0x2000000 : 1;
	uint32 b0x4000000 : 1;
	uint32 b0x8000000 : 1;
	uint32 b0x10000000 : 1;
	uint32 b0x20000000 : 1;
	uint32 b0x40000000 : 1;
	uint32 b0x80000000 : 1;

	uint8 ucColDamageEffect; // 324
	uint8 pad4; // 325
	uint8 pad5; // 326
	uint8 pad6; // 327
	uint8 pad7; // 328
	uint8 pad8; // 329
	uint16 pad9; // 330
	uint8 pad10; // 332
	uint8 pad11; // 333
	uint8 pad12; // 334
	uint8 pad13; // 335
	uint32 uiObjectRemovalTime; // 336
	uint32 pad14; // 340
	uint32 pad15; // 344
	float fScale; // 348
	CObjectInfo* pObjectInfo; // 352
	uint32 pad16; // 356
	uint16 pad17; // 360
	uint16 pad18; // 362
	uint32 pad19; // 364
	uint32 pad20; // 368
	uint32 pad21; // 372
	uint32 pad22; // 376
};
C_ASSERT(sizeof(CObjectSAInterface) == 0x17C);

class CObjectSA : public virtual CObject, public virtual CPhysicalSA
{
private:
    unsigned char               m_ucAlpha;
    bool                        m_bIsAGangTag;

public:
                                CObjectSA           ( CObjectSAInterface * objectInterface );
                                CObjectSA           ( DWORD dwModel, bool bBreakable );
                                ~CObjectSA          ( void );

    inline CObjectSAInterface * GetObjectInterface  ( void )    { return ( CObjectSAInterface * ) GetInterface (); }

    void                        Explode             ( void );
    void                        Break               ( void );
    void                        SetScale            ( float fScale );
    void                        SetHealth           ( float fHealth );
    float                       GetHealth           ( void );
    void                        SetModelIndex       ( unsigned long ulModel );

    inline void                 SetAlpha            ( unsigned char ucAlpha ) { m_ucAlpha = ucAlpha; }
    inline unsigned char        GetAlpha            ( ) { return m_ucAlpha; }

    bool                        IsAGangTag          ( ) const { return m_bIsAGangTag; }
private:
    void                        CheckForGangTag     ( );
};

/*
#define COBJECTSA_DEFINED
#define COBJECTSAINTERFACE_DEFINED
*/

#endif
