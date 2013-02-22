/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientCivilian.cpp
*  PURPOSE:     Civilian ped entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*****************************************************************************/

#include <StdInc.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

CClientCivilian::CClientCivilian ( CClientManager* pManager, ElementID ID, int iModel ) : ClassInit ( this ), CClientEntity ( ID )
{
    // Initialize members
    m_pManager = pManager;
    m_pCivilianManager = pManager->GetCivilianManager ();
    m_iModel = iModel;

    m_pCivilianPed = NULL;

    // Add this civilian to the civilian list
    m_pCivilianManager->AddToList ( this );

    // Show us by default
    Create ();
}


CClientCivilian::CClientCivilian ( CClientManager* pManager, ElementID ID, CCivilianPed * pCivilianPed ) : ClassInit ( this ), CClientEntity ( ID )
{
    // Initialize members
    m_pManager = pManager;
    m_pCivilianManager = pManager->GetCivilianManager ();
    m_iModel = 0;

    // Store the given civilian. Also make sure we set our pointer in its stored pointer data.
    m_pCivilianPed = pCivilianPed;
    if ( m_pCivilianPed )
    {
        m_pCivilianPed->SetStoredPointer ( this );
    }

    // Add our pointer to the manager list
    m_pCivilianManager->AddToList ( this );
}


CClientCivilian::~CClientCivilian( void )
{
    // Reset the stored pointer in the civilian ped if neccessary
    if ( m_pCivilianPed )
    {
        m_pCivilianPed->SetStoredPointer ( NULL );
    }

    // Destroy the civilian
    Destroy ();

    // Remove us from the civilian list
    Unlink ();
}


void CClientCivilian::Unlink ( void )
{
    m_pCivilianManager->RemoveFromList ( this );
}


int CClientCivilian::GetRotation ( void )
{
    int iRotation = 0;
    double dRotation;

    CMatrix pMat;
    m_pCivilianPed->GetMatrix ( &pMat );

    float fX = pMat.vFront.fX;
    float fY = pMat.vFront.fY;
    dRotation = acos ( fY );

    if (fX <= 0)
    {
        iRotation = (int)(dRotation * 57.29577951);
    }
    else
    {
        iRotation = 360 - (int)(dRotation * 57.29577951);
    }

    return iRotation;
}


void CClientCivilian::GetRotationRadians ( CVector& vecRotation ) const
{
//    vecRotation = m_vecRotation;
}


void CClientCivilian::SetRotationRadians ( const CVector& vecRotation )
{
    if ( m_pCivilianPed )
    {
        m_pCivilianPed->SetOrientation ( vecRotation.fX, vecRotation.fY, vecRotation.fZ );
    }

//    m_vecRotation = vecRotation;
}


void CClientCivilian::SetRotation ( int iRotation )
{
    // Convert
    float fX = static_cast < float > ( iRotation ) / 57.29577951f;

    // Eventually apply it to the vehicle
    if ( m_pCivilianPed )
    {
        m_pCivilianPed->SetOrientation ( fX, 0, 0 );
    }

    // Store the rotation vector
//    m_vecRotation = CVector ( fX, 0, 0 );

    CMatrix pMat;
    m_pCivilianPed->GetMatrix ( &pMat );

    m_pCivilianPed->SetMatrix ( &pMat );
}


float CClientCivilian::GetDistanceFromCentreOfMassToBaseOfModel ( void )
{
    if ( m_pCivilianPed )
    {
        return m_pCivilianPed->GetDistanceFromCentreOfMassToBaseOfModel ();
    }
   return 0.0f;;
}


bool CClientCivilian::GetMatrix ( CMatrix& Matrix ) const
{
    m_pCivilianPed->GetMatrix ( &Matrix );
    return true;
}


bool CClientCivilian::SetMatrix ( const CMatrix& Matrix )
{
    m_pCivilianPed->SetMatrix ( const_cast < CMatrix* > ( &Matrix ) );
    return true;
}


void CClientCivilian::GetMoveSpeed ( CVector& vecMoveSpeed ) const
{
    m_pCivilianPed->GetMoveSpeed ( &vecMoveSpeed );
}


void CClientCivilian::SetMoveSpeed ( const CVector& vecMoveSpeed )
{
    m_pCivilianPed->SetMoveSpeed ( const_cast < CVector* > ( &vecMoveSpeed ) );
}


void CClientCivilian::GetTurnSpeed ( CVector& vecTurnSpeed ) const
{
    m_pCivilianPed->GetTurnSpeed ( &vecTurnSpeed );
}


void CClientCivilian::SetTurnSpeed ( const CVector& vecTurnSpeed )
{
    m_pCivilianPed->SetTurnSpeed ( const_cast < CVector* > ( &vecTurnSpeed ) );
}


bool CClientCivilian::IsVisible ( void )
{
    return m_pCivilianPed->IsVisible ();
}


void CClientCivilian::SetVisible ( bool bVisible )
{
    m_pCivilianPed->SetVisible ( bVisible );
    m_pCivilianPed->SetUsesCollision ( bVisible );
}


float CClientCivilian::GetHealth ( void ) const
{
    return m_pCivilianPed->GetHealth ();
}


void CClientCivilian::SetHealth ( float fHealth )
{
    // Set the health
    m_pCivilianPed->SetHealth ( fHealth );
}

int CClientCivilian::GetModelID ( void )
{
    //return m_iVehicleModel;
    return 0;
}


void CClientCivilian::SetModelID ( int iModelID )
{

}


void CClientCivilian::Dump ( FILE* pFile, bool bDumpDetails, unsigned int uiIndex )
{

}

void CClientCivilian::Create ( void )
{

}

void CClientCivilian::Destroy ( void )
{
 
}


void CClientCivilian::ReCreate ( void )
{
    // Re-create the vehicle
    Destroy ();
    Create ();
}
