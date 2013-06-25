/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CAspectRatioConverter.h
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

//
// Convert positions of items that align with GTA Hud elements
//
class CAspectRatioConverter
{
public:
    ZERO_ON_NEW

    // Call once
    void Init( float ViewportHeight )
    {
        m_fToRelMul = 2 / ViewportHeight;
        m_fFromRelMul = ViewportHeight / 2.f;
    }

    // Call once a frame to update Target Ratio Value
    void Pulse( float fTargetRatioValue )
    {
        m_fTargetRatioValue = fTargetRatioValue;
        m_fTargetBase = 1 - 0.36 * fTargetRatioValue;
        m_bValidRatios = ( m_fSourceRatioValue != 0 ) && ( m_fTargetRatioValue != 0 );
    }

    // Call when Source Ratio Value changes
    void SetSourceRatioValue( float fSourceRatioValue )
    {
        m_fSourceRatioValue = fSourceRatioValue;
        m_bValidRatios = ( m_fSourceRatioValue != 0 ) && ( m_fTargetRatioValue != 0 );
        if ( m_bValidRatios )
        {
            m_fSourceBase = 1 - 0.36 * m_fSourceRatioValue;
            m_fConvertScale = m_fTargetRatioValue / m_fSourceRatioValue;
        }
    }

    float GetSourceRatioValue( void ) const
    {
        return m_fSourceRatioValue;
    }

    // Temp suspend conversion. i.e. When F11 map is displayed
    void SetSuspended( bool bSuspended )
    {
        m_bSuspended = bSuspended;
    }

    bool IsEnabled( void ) const
    {
        return ( m_bValidRatios && !m_bSuspended );
    }

    // Convert a y screen position
    float ConvertPositionForAspectRatio( const float fY ) const
    {
        if ( !IsEnabled() )
            return fY;

        // To screen center relative
        float fRelY = 1 - fY * m_fToRelMul;

        // From source ratio to target ratio
        float fNewRelY = ( abs( fRelY ) - m_fSourceBase ) * m_fConvertScale + m_fTargetBase;
        fNewRelY *= fRelY < 0 ? -1 : 1;

        // From screen center relative
        float fNewY = ( 1 - fNewRelY ) * m_fFromRelMul;
        return fNewY;
    }

    // Convert a y screen position and length (In-place)
    void ConvertSideForAspectRatio( float* pfY, float* pfHeight ) const
    {
        if ( !IsEnabled() )
            return;

        float fNewY = ConvertPositionForAspectRatio( *pfY );
        float fNewBottom = ConvertPositionForAspectRatio( *pfY + *pfHeight );
        *pfHeight = fNewBottom - fNewY;
        *pfY = fNewY;
    }

protected:
    bool m_bValidRatios;
    bool m_bSuspended;
    float m_fSourceRatioValue;
    float m_fTargetRatioValue;
    float m_fToRelMul;
    float m_fFromRelMul;
    float m_fTargetBase;
    float m_fSourceBase;
    float m_fConvertScale;
};
