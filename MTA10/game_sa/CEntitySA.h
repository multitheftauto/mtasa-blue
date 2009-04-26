/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CEntitySA.h
*  PURPOSE:		Header file for base entity class
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
*               Kent Simon <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_ENTITY
#define __CGAMESA_ENTITY

#include "Common.h"
#include "COffsets.h"
#include <game/CEntity.h>
#include <CMatrix.h>
#include <CMatrix_Pad.h>
#include <CVector2D.h>
#include <CVector.h>

#define FUNC_GetDistanceFromCentreOfMassToBaseOfModel		0x536BE0

#define FUNC_SetRwObjectAlpha								0x5332C0
#define FUNC_SetOrientation									0x439A80

#define FUNC_CMatrix__ConvertToEulerAngles                    0x59A840
#define FUNC_CMatrix__ConvertFromEulerAngles                0x59AA40

#define FUNC_IsOnScreen										0x534540
#define FUNC_IsVisible                                      0x536BC0



// not in CEntity really
#define FUNC_RpAnimBlendClumpGetAssociation                 0x4D6870

// replace with enum from R*
#define STATUS_ABANDONED					4

class CEntitySAInterfaceVTBL
{	
public:
	DWORD SCALAR_DELETING_DESTRUCTOR;
	DWORD Add_CRect;
	DWORD Add;
	DWORD Remove;
	DWORD SetIsStatic;
	DWORD SetModelIndex;
	DWORD SetModelIndexNoCreate;
	DWORD CreateRwObject;
	DWORD DeleteRwObject;
	DWORD GetBoundRect;
	DWORD ProcessControl;
	DWORD ProcessCollision;
	DWORD ProcessShift;
	DWORD TestCollision;
	DWORD Teleport;
	DWORD SpecialEntityPreCollisionStuff;
	DWORD SpecialEntityCalcCollisionSteps;
	DWORD PreRender;
	DWORD Render;
	DWORD SetupLighting;
	DWORD RemoveLighting;
	DWORD FlagToDestroyWhenNextProcessed;
};


/** 
 * \todo Move CReferences (and others below?) into it's own file
 */
class CReferences
{
	CEntity		* pEntity;
};

class CSimpleTransformSAInterface
{
public:
	CVector							m_translate;
	FLOAT							m_heading;
};

class CPlaceableSAInterface
{
public:
	CSimpleTransformSAInterface		m_transform;
	CMatrix_Padded					* matrix;
};

class CEntitySAInterface;

class CEntitySAInterface
{
public:
	CEntitySAInterfaceVTBL		* vtbl;	// the virtual table
	
	CPlaceableSAInterface	Placeable; // 4

	RpClump		* m_pRwObject; // 24
	/********** BEGIN CFLAGS **************/
	unsigned long bUsesCollision : 1;			// does entity use collision
	unsigned long bCollisionProcessed : 1; 	// has object been processed by a ProcessEntityCollision function
	unsigned long bIsStatic : 1;				// is entity static
	unsigned long bHasContacted : 1;			// has entity processed some contact forces
	unsigned long bIsStuck : 1;				// is entity stuck
	unsigned long bIsInSafePosition : 1;		// is entity in a collision free safe position
	unsigned long bWasPostponed : 1;			// was entity control processing postponed
	unsigned long bIsVisible : 1;				//is the entity visible
	
	unsigned long bIsBIGBuilding : 1;			// Set if this entity is a big building
	unsigned long bRenderDamaged : 1;			// use damaged LOD models for objects with applicable damage
	unsigned long bStreamingDontDelete : 1;	// Dont let the streaming remove this 
	unsigned long bRemoveFromWorld : 1;		// remove this entity next time it should be processed
	unsigned long bHasHitWall : 1;				// has collided with a building (changes subsequent collisions)
	unsigned long bImBeingRendered : 1;		// don't delete me because I'm being rendered
	unsigned long bDrawLast :1;				// draw object last
	unsigned long bDistanceFade :1;			// Fade entity because it is far away
	
	unsigned long bDontCastShadowsOn : 1;		// Dont cast shadows on this object
	unsigned long bOffscreen : 1;				// offscreen flag. This can only be trusted when it is set to true
	unsigned long bIsStaticWaitingForCollision : 1; // this is used by script created entities - they are static until the collision is loaded below them
	unsigned long bDontStream : 1;				// tell the streaming not to stream me
	unsigned long bUnderwater : 1;				// this object is underwater change drawing order
	unsigned long bHasPreRenderEffects : 1;	// Object has a prerender effects attached to it
	unsigned long bIsTempBuilding : 1;			// whether or not the building is temporary (i.e. can be created and deleted more than once)
	unsigned long bDontUpdateHierarchy : 1;	// Don't update the aniamtion hierarchy this frame
	
	unsigned long bHasRoadsignText : 1;		// entity is roadsign and has some 2deffect text stuff to be rendered
	unsigned long bDisplayedSuperLowLOD : 1;
	unsigned long bIsProcObject : 1;			// set object has been generate by procedural object generator
	unsigned long bBackfaceCulled : 1;			// has backface culling on
	unsigned long bLightObject : 1;			// light object with directional lights
	unsigned long bUnimportantStream : 1;		// set that this object is unimportant, if streaming is having problems
	unsigned long bTunnel : 1;			// Is this model part of a tunnel
	unsigned long bTunnelTransition : 1;		// This model should be rendered from within and outside of the tunnel
	/********** END CFLAGS **************/

	WORD 		RandomSeed; //32
	WORD 		m_nModelIndex;//34
	CReferences	*pReferences; //36
	
	DWORD		* m_pLastRenderedLink; // 	CLink<CEntity*>* m_pLastRenderedLink; +40
	
	WORD m_nScanCode;			// 44
	BYTE m_iplIndex;			// used to define which IPL file object is in +46
	BYTE m_areaCode;			// used to define what objects are visible at this point +47
	
	// LOD shit
	CEntitySAInterface * m_pLod; // 48
	// num child higher level LODs
	BYTE numLodChildren; // 52
	// num child higher level LODs that have been rendered
	signed char numLodChildrenRendered; // 53

	//********* BEGIN CEntityInfo **********//
	BYTE nType : 3; // what type is the entity				// 54 (2 == Vehicle)
	BYTE nStatus : 5;				// control status		// 54
	//********* END CEntityInfo **********//

	//58-66 padded
	BYTE pad[8];
    BYTE nImmunities;
    BYTE pad2 [ 1 ];

    /* IMPORTANT: KEEP "pad" in CVehicle UP-TO-DATE if you add something here (or eventually pad someplace else) */
};

class CEntitySA : public virtual CEntity
{
    friend COffsets;
public:
                                CEntitySA           ( void ) { m_pStoredPointer = NULL; m_ulArrayID = 0; };

	CEntitySAInterface*         m_pInterface;

	DWORD						internalID;
//	VOID						SetModelAlpha ( int iAlpha );

	inline CEntitySAInterface * GetInterface() { return m_pInterface; };
	VOID						SetInterface( CEntitySAInterface * intInterface ) { m_pInterface = intInterface; };

	VOID						SetPosition ( float fX, float fY, float fZ );
	VOID						Teleport ( float fX, float fY, float fZ );
	VOID						ProcessControl ( void );
	VOID						SetupLighting ( );
	VOID						Render ( );
	VOID						SetOrientation ( float fX, float fY, float fZ );
    VOID                        FixBoatOrientation ( void );        // eAi you might want to rename this
	VOID						SetPosition ( CVector * vecPosition );
    VOID                        SetRoll ( CVector * vecRoll );
    VOID                        SetDirection ( CVector * vecDir );
    VOID                        SetWas ( CVector * vecWas );

	void						SetUnderwater ( bool bUnderwater );
	bool						GetUnderwater ( void );

	CVector						* GetPosition (  );
	CMatrix						* GetMatrix ( CMatrix * matrix ) const;
	VOID						SetMatrix ( CMatrix * matrix );
	WORD						GetModelIndex ();
	eEntityType					GetEntityType ();
	bool						IsOnScreen ();

    bool                        IsVisible ( void );
    void                        SetVisible ( bool bVisible );

    BYTE                        GetAreaCode ( void );
    void                        SetAreaCode ( BYTE areaCode );

	FLOAT						GetDistanceFromCentreOfMassToBaseOfModel();
	/**
	 * \todo Find enum for SetEntityStatus
	 */
	VOID						SetEntityStatus( eEntityStatus bStatus );
	eEntityStatus				GetEntityStatus( );

    RwFrame *                   GetFrameFromId ( int id );
    RwMatrix *                  GetLTMFromId ( int id );

	RpClump *					GetRpClump ();


	BOOL						BeingDeleted; // to prevent it trying to delete twice
	BOOL						DoNotRemoveFromGame; // when deleted, if this is true, it won't be removed from the game

	VOID						SetDoNotRemoveFromGameWhenDeleted ( bool bDoNotRemoveFromGame ) { DoNotRemoveFromGame = bDoNotRemoveFromGame; };
    BOOL                        IsStatic(VOID)                          { return m_pInterface->bIsStatic; }
    VOID						SetStatic(BOOL bStatic)					{ m_pInterface->bIsStatic		= bStatic; };
	VOID						SetUsesCollision(BOOL bUsesCollision)	{ m_pInterface->bUsesCollision	= bUsesCollision;};
	VOID						SetAlpha(DWORD dwAlpha);

    VOID                        MatrixConvertFromEulerAngles ( float fX, float fY, float fZ, int iUnknown );
    VOID                        MatrixConvertToEulerAngles ( float * fX, float * fY, float * fZ, int iUnknown );

    bool                        IsPlayingAnimation ( char * szAnimName );

    void*                       GetStoredPointer    ( void )                { return m_pStoredPointer; };
    void                        SetStoredPointer    ( void* pPointer )      { m_pStoredPointer = pPointer; };

    bool                        IsStaticWaitingForCollision ( void )        { return m_pInterface->bIsStaticWaitingForCollision; }
    void                        SetStaticWaitingForCollision ( bool bStatic ) { m_pInterface->bIsStaticWaitingForCollision = bStatic; }

    void                        GetImmunities   ( bool & bNoClip, bool & bFrozen, bool & bBulletProof, bool & bFlameProof, bool & bUnk, bool & bUnk2, bool & bCollisionProof, bool & bExplosionProof );

    inline unsigned long        GetArrayID      ( void ) { return m_ulArrayID; }
    inline void                 SetArrayID      ( unsigned long ulID ) { m_ulArrayID = ulID; }

private:
    static unsigned long        FUNC_CClumpModelInfo__GetFrameFromId;
    static unsigned long        FUNC_RwFrameGetLTM;

    unsigned long               m_ulArrayID;

/*	VOID						InitFlags()
	{
		//this->GetInterface()->bIsStaticWaitingForCollision = true;
		this->GetInterface()->nStatus = 4;
		
		DWORD dwThis = (DWORD)this->GetInterface();

		DWORD dwFunc = 0x41D000;
		_asm
		{
			push	dwThis
			call	dwFunc
			pop		eax
		}

	};*/

    void*                       m_pStoredPointer;
};

#endif