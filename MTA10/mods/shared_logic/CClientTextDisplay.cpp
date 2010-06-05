/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTextDisplay.cpp
*  PURPOSE:     Client text display base class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

float CClientTextDisplay::m_fGlobalScale = 1.0f;

CClientTextDisplay::CClientTextDisplay ( CClientDisplayManager* pDisplayManager, int ID, bool bIs3DText ): CClientDisplay ( pDisplayManager, ID )
{
    // Init
    m_bIs3DText = bIs3DText;

    m_szCaption = NULL;
    m_fScale = 1;
    m_ulFormat = 0;
    m_bVisible = true;
}


CClientTextDisplay::~CClientTextDisplay ( void )
{   
    // Delete all effects applied to us
    RemoveAllEffects ();

    // Delete our caption
    if ( m_szCaption )
        delete [] m_szCaption;
}


void CClientTextDisplay::ApplyEffect ( CClientTextDisplayEffect* pTextDisplayEffect )
{
    if ( pTextDisplayEffect )
    {
        pTextDisplayEffect->SetTextDisplay ( this );
        m_EffectList.push_back ( pTextDisplayEffect );
    }
}


void CClientTextDisplay::RemoveEffect ( CClientTextDisplayEffect* pTextDisplayEffect )
{
    delete pTextDisplayEffect;
}


void CClientTextDisplay::RemoveAllEffects ( void )
{
    list < CClientTextDisplayEffect* > ::iterator iter;
    for ( iter = m_EffectList.begin (); iter != m_EffectList.end (); iter++ )
    {
        delete *iter;
    }

    m_EffectList.clear ();
}


char* CClientTextDisplay::GetCaption ( char* szBuffer, size_t sizeBuffer )
{
    if (sizeBuffer <= 0)
        return NULL;

    if ( m_szCaption )
    {
        strncpy ( szBuffer, m_szCaption, sizeBuffer );
        szBuffer[sizeBuffer-1] = '\0';
        return szBuffer;
    }
    else
    {
        szBuffer[0] = '\0';
        return szBuffer;
    }

    return NULL;
}


void CClientTextDisplay::SetCaption ( const char* szCaption )
{
    if ( m_szCaption )
    {
        delete [] m_szCaption;
        m_szCaption = NULL;
    }

    if ( szCaption )
    {
        m_szCaption = new char [ strlen ( szCaption ) + 1 ];
        strcpy ( m_szCaption, szCaption );
    }
}


void CClientTextDisplay::SetPosition ( const CVector &vecPosition )
{
	m_vecPosition = vecPosition;
}


void CClientTextDisplay::Render ( bool bPulseEffects )
{
    // Pulse the text effects
    if ( bPulseEffects )
    {
        list < CClientTextDisplayEffect* > ::iterator iter;
        for ( iter = m_EffectList.begin (); iter != m_EffectList.end (); )
        {
            (*iter)->DoPulse ();
            if ( (*iter)->CanDeleteMe () && !(*iter)->GetNeverDelete () )
            {
				// Delete the object
                delete *iter;

				// Remove from the list
				iter = m_EffectList.erase ( iter );
            }
            else
            {
                // Increment iterator
                ++iter;
            }
        }
    }

    // If we're visible
    if ( m_bVisible && m_szCaption )
    {
        // Render
        if ( m_bIs3DText )
        {
            g_pCore->GetGraphics ()->DrawText3DA ( m_vecPosition.fX, m_vecPosition.fY, m_vecPosition.fZ, m_rgbaColor, m_fScale * m_fGlobalScale, "%s", m_szCaption );
        }
        else
        {			
            CVector2D vecResolution = g_pCore->GetGUI ()->GetResolution ();
			unsigned int uiX = unsigned int ( m_vecPosition.fX * vecResolution.fX );
			unsigned int uiY = unsigned int ( m_vecPosition.fY * vecResolution.fY );
			if ( m_ulFormat & DT_BOTTOM )
				m_ulFormat |= DT_SINGLELINE;

            unsigned int uiShadowOffset = Max ( 1, (int)(m_fScale * m_fGlobalScale) );
            SColorRGBA rgbaShadowColor( 0, 0, 0, m_rgbaColor.A * m_ucShadowAlpha / 255 );

			if ( g_pCore->GetGraphics ()->GetCEGUIUsed () )
			{
                if ( rgbaShadowColor.A > 0 )
                    g_pCore->GetGraphics ()->DrawTextCEGUI ( uiX - 17 + uiShadowOffset, uiY + uiShadowOffset, uiX - 17 + uiShadowOffset, uiY + uiShadowOffset, rgbaShadowColor, m_szCaption, m_fScale * m_fGlobalScale, m_ulFormat | DT_NOCLIP );
			    g_pCore->GetGraphics ()->DrawTextCEGUI ( uiX - 17, uiY, uiX - 17, uiY, m_rgbaColor, m_szCaption, m_fScale * m_fGlobalScale, m_ulFormat | DT_NOCLIP );
			}
			else
			{				
                if ( rgbaShadowColor.A > 0 )
                    g_pCore->GetGraphics ()->DrawText ( uiX + uiShadowOffset, uiY + uiShadowOffset, uiX + uiShadowOffset, uiY + uiShadowOffset, rgbaShadowColor, m_szCaption, m_fScale * m_fGlobalScale, m_fScale * m_fGlobalScale, m_ulFormat | DT_NOCLIP );        
                g_pCore->GetGraphics ()->DrawText ( uiX, uiY, uiX, uiY, m_rgbaColor, m_szCaption, m_fScale * m_fGlobalScale, m_fScale * m_fGlobalScale, m_ulFormat | DT_NOCLIP );        
			}
		}
    }
}

void CClientTextDisplay::SetColor ( RGBA rgbaColor )
{
	m_rgbaColor = rgbaColor;
}

void CClientTextDisplay::SetColorAlpha ( unsigned char ucAlpha )
{
    SColor color = GetColor ();
    color.A = ucAlpha;
    SetColor ( color );
}

void CClientTextDisplay::SetShadowAlpha ( unsigned char ucShadowAlpha )
{
    m_ucShadowAlpha = ucShadowAlpha;
}

void CClientTextDisplay::SetScale ( float fScale )
{
    m_fScale = fScale;
}


void CClientTextDisplay::SetFormat ( unsigned long ulFormat )
{
    m_ulFormat = ulFormat;
}

void CClientTextDisplay::SetVisible ( bool bVisible )
{
    CClientDisplay::SetVisible ( bVisible );
}
