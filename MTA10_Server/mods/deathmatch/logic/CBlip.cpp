/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CBlip.cpp
*  PURPOSE:     Blip entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CBlip::CBlip ( CElement* pParent, CXMLNode* pNode, CBlipManager* pBlipManager ) : CPerPlayerEntity ( pParent, pNode )
{
    // Init
    m_iType = CElement::BLIP;
    SetTypeName ( "blip" );
    m_pBlipManager = pBlipManager;
    m_ucSize = 2;
    m_ucIcon = 0;
    m_sOrdering = 0;
    m_fVisibleDistance = 99999.0f;

    // Add us to manager's list
    m_pBlipManager->m_List.push_back ( this );
}


CBlip::~CBlip ( void )
{
    // Remove us from the manager's list
    Unlink ();
}


void CBlip::Unlink ( void )
{
    // Remove us from the manager's list
    if ( m_pBlipManager->m_bRemoveFromList && !m_pBlipManager->m_List.empty() )
    {
        m_pBlipManager->m_List.remove ( this );
    }
}


bool CBlip::ReadSpecialData ( void )
{
    // Grab the "posX" data
    if ( !GetCustomDataFloat ( "posX", m_vecPosition.fX, true ) )
    {
        CLogger::ErrorPrintf ( "Bad/missing 'posX' attribute in <blip> (line %u)\n", m_uiLine );
        return false;
    }

    // Grab the "posY" data
    if ( !GetCustomDataFloat ( "posY", m_vecPosition.fY, true ) )
    {
        CLogger::ErrorPrintf ( "Bad/missing 'posY' attribute in <blip> (line %u)\n", m_uiLine );
        return false;
    }

    // Grab the "posZ" data
    if ( !GetCustomDataFloat ( "posZ", m_vecPosition.fZ, true ) )
    {
        CLogger::ErrorPrintf ( "Bad/missing 'posZ' attribute in <blip> (line %u)\n", m_uiLine );
        return false;
    }

    // Grab the "icon" data
    int iTemp;
    if ( GetCustomDataInt ( "icon", iTemp, true ) )
    {
        if ( CBlipManager::IsValidIcon ( iTemp ) )
        {
            m_ucIcon = static_cast < unsigned char > ( iTemp );
        }
        else
        {
            CLogger::ErrorPrintf ( "Bad 'icon' id specified in <blip> (line %u)\n", m_uiLine );
            return false;
        }
    }
    else
    {
        m_ucIcon = 0;
    }

    // Grab the "color" data
    char szColor [64];
    if ( GetCustomDataString ( "color", szColor, 64, true ) )
    {
        // Convert it to RGBA
        if ( !XMLColorToInt ( szColor, m_Color.R, m_Color.G, m_Color.B, m_Color.A ) )
        {
            CLogger::ErrorPrintf ( "Bad 'color' value specified in <blip> (line %u)\n", m_uiLine );
            return false;
        }
    }
    else
    {
        m_Color = SColorRGBA ( 0, 0, 255, 255 );
    }

    if ( GetCustomDataInt ( "dimension", iTemp, true ) )
        m_usDimension = static_cast < unsigned short > ( iTemp );

    // Grab the "ordering" data
    if ( GetCustomDataInt ( "ordering", iTemp, true ) )
    {
        if ( iTemp >= -32768 && iTemp <= 32767 )
        {
            m_sOrdering = static_cast < short > ( iTemp );
        }
        else
        {
            CLogger::ErrorPrintf ( "Bad 'ordering' id specified in <blip> (line %u)\n", m_uiLine );
            return false;
        }
    }
    else
    {
        m_sOrdering = 0;
    }

    return true;
}


const CVector & CBlip::GetPosition ( void )
{
    // Are we attached to something?
    if ( m_pAttachedTo ) GetAttachedPosition ( m_vecPosition );
    return m_vecPosition;
}


void CBlip::SetPosition ( const CVector& vecPosition )
{
    // Don't allow a change if we're attached to something
    if ( m_pAttachedTo ) return;
    
    // Is it any different to where we are now?
    if ( m_vecPosition != vecPosition )
    {
        // Remember our position vectors
        m_vecLastPosition = m_vecPosition;
        m_vecPosition = vecPosition;
    }
}


void CBlip::AttachTo ( CElement* pElement )
{
    CElement::AttachTo ( pElement );

    // If we have a new element, change our dimension to match it's
    if ( m_pAttachedTo )
    {
        SetDimension ( m_pAttachedTo->GetDimension () );
    }
}
