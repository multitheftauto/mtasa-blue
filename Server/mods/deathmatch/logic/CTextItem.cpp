/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CTextItem.cpp
*  PURPOSE:     Text display item class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

static unsigned long ulUniqueId = 0;

CTextItem::CTextItem ( const char* szText, const CVector2D& vecPosition, eTextPriority Priority, const SColor color, float fScale, unsigned char ucFormat, unsigned char ucShadowAlpha )
{
    m_uiScriptID = CIdArray::PopUniqueId ( this, EIdClass::TEXT_ITEM );
    // Assign us an unique ID
    m_ulUniqueId = ulUniqueId++;

    // Copy the text
    m_strText = szText ? szText : "";

    // Assign the properties
    m_vecPosition = vecPosition;
    m_Color = color;
    m_fScale = fScale;
    m_ucFormat = ucFormat;
    m_Priority = Priority;
    m_ucShadowAlpha = ucShadowAlpha;
    m_bDeletable = false;
}


CTextItem::CTextItem ( const CTextItem& TextItem )
{
    m_uiScriptID = CIdArray::PopUniqueId ( this, EIdClass::TEXT_ITEM );
    // Copy the tex
    m_strText = TextItem.m_strText;

    // Copy over the properties
    m_ulUniqueId = TextItem.m_ulUniqueId;
    m_vecPosition = TextItem.m_vecPosition;
    m_Color = TextItem.m_Color;
    m_fScale = TextItem.m_fScale;
    m_ucFormat = TextItem.m_ucFormat;
    m_Priority = TextItem.m_Priority;
    m_ucShadowAlpha = TextItem.m_ucShadowAlpha;
    m_bDeletable = TextItem.m_bDeletable;
}


bool CTextItem::operator= ( const CTextItem& TextItem )
{
    // Copy the new text
    m_strText = TextItem.m_strText;

    // Copy the properties
    m_ulUniqueId = TextItem.m_ulUniqueId;
    m_vecPosition = TextItem.m_vecPosition;
    m_Color = TextItem.m_Color;
    m_fScale = TextItem.m_fScale;
    m_ucFormat = TextItem.m_ucFormat;
    m_Priority = TextItem.m_Priority;
    m_ucShadowAlpha = TextItem.m_ucShadowAlpha;
    m_bDeletable = TextItem.m_bDeletable;
    m_Observers.clear ();

    return true;
}


CTextItem::~CTextItem ( )
{
    CIdArray::PushUniqueId ( this, EIdClass::TEXT_ITEM, m_uiScriptID );
    // Tell all our observers about it
    m_bDeletable = true;
    NotifyObservers ();
}


void CTextItem::AddObserver ( CTextDisplay* pObserver )
{
    // Remove them first if they're already added (easier than checking if they are and only marginally slower)
    m_Observers.remove ( pObserver );

    // Add it
    m_Observers.push_back ( pObserver );
}


void CTextItem::NotifyObservers ( void )
{
    // Update all our observers
    list < CTextDisplay* > ::iterator iter = m_Observers.begin ();
    for ( ; iter != m_Observers.end (); iter++ )
    {
        (*iter)->Update ( this );
    }
}


void CTextItem::SetText ( const char* szText )
{
    // If the text has changed
    if ( m_strText != szText )
    {
        // Update
        m_strText = szText;

        // Tell the text displays
        NotifyObservers ();
    }
}


void CTextItem::SetPosition ( const CVector2D& vecPosition )
{
    // If the position has changed
    if ( m_vecPosition != vecPosition )
    {
        // Update
        m_vecPosition = vecPosition;

        // Tell the text displays
        NotifyObservers ();
    }
}


void CTextItem::SetColor ( const SColor color )
{
    // If the color has changed
    if ( m_Color != color )
    {
        // Set it
        m_Color = color;

        // Tell the text displays
        NotifyObservers ();
    }
}


void CTextItem::SetScale ( float fScale )
{
    // If the scale has changed
    if ( m_fScale != fScale )
    {
        // Set it
        m_fScale = fScale;

        // Tell the text displays
        NotifyObservers();
    }
}
