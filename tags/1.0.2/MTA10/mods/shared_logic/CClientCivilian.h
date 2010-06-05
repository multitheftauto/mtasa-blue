/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientCivilian.h
*  PURPOSE:     Civilian ped entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*****************************************************************************/

class CClientCivilian;

#ifndef __CCLIENTCIVILIAN_H
#define __CCLIENTCIVILIAN_H

#ifdef COMPILE_FOR_VC
    #include <game/CCivilianPed.h>
#else if COMPILE_FOR_SA
    #include <game/CCivilianPed.h>
    #include <game/TaskGoTo.h>
    #include <game/TaskCarAccessories.h>
#endif

#include "CClientCommon.h"
#include "CClientEntity.h"

class CClientCivilian : public CClientEntity
{
    friend class CClientCivilianManager;

public:
                                    ~CClientCivilian        ( void );
    
    void                            Unlink                  ( void );
    
    inline eClientEntityType        GetType                 ( void ) const                      { return CCLIENTCIVILIAN; };

	inline CCivilianPed*		    GetGameCivilian			( void )							{ return m_pCivilianPed; }

    inline void                     GetPosition             ( CVector& vecPosition ) const      { vecPosition = *m_pCivilianPed->GetPosition (); };
    inline void                     SetPosition             ( const CVector& vecPosition )      { m_pCivilianPed->SetPosition ( const_cast < CVector* > ( &vecPosition ) ); };
    int                             GetRotation             ( void );
    void                            GetRotation             ( CVector& vecRotation ) const;
    void                            SetRotation             ( const CVector& vecRotation );
    void                            SetRotation             ( int iRotation );

    inline void                     ModelRequestCallback    ( unsigned short usModelID )        {};

    float                           GetDistanceFromCentreOfMassToBaseOfModel ( void );

    bool                            GetMatrix               ( CMatrix& Matrix ) const;
    bool                            SetMatrix               ( const CMatrix& Matrix );

   	void                            GetMoveSpeed            ( CVector& vecMoveSpeed ) const;
    void                            SetMoveSpeed            ( const CVector& vecMoveSpeed );
	void                            GetTurnSpeed            ( CVector& vecTurnSpeed ) const;
	void                            SetTurnSpeed            ( const CVector& vecTurnSpeed );

    bool                            IsVisible               ( void );
    void                            SetVisible              ( bool bVisible );

    float                           GetHealth               ( void ) const;
    void                            SetHealth               ( float fHealth );

    int                             GetModelID              ( void );
    void                            SetModelID              ( int iModelID );

private:
                                    CClientCivilian         ( class CClientManager* pManager, ElementID ID, int iPedModel );
								    CClientCivilian			( class CClientManager* pManager, ElementID ID, CCivilianPed* pCivilianPed );

    void                            Dump                    ( FILE* pFile, bool bDumpDetails, unsigned int uiIndex );

    void                            Create                  ( void );
    void                            Destroy                 ( void );
    void                            ReCreate                ( void );

    class CClientCivilianManager*   m_pCivilianManager;

	int							    m_iModel;

    CCivilianPed*                   m_pCivilianPed;
};

#endif
