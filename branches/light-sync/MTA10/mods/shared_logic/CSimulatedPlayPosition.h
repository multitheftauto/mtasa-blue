/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:
*  PURPOSE:
*  DEVELOPERS:  me'n'caz
*
*****************************************************************************/


#define DEFAULT_SOUND_LENGTH (100)

//
// Keeps track of the play position when the sound is out of range
//
// Based on Caz's smart seek code
//
class CSimulatedPlayPosition
{
    bool        m_bLoop;
    double      m_dLength;
    bool        m_bValid;
    float       m_fPlaybackSpeed;
    bool        m_bPaused;
    double      m_dSavedPosition;
    CTickCount  m_SavedTickcount;
public:

    ///////////////////////////////////////////////////////
    // Validity
    bool IsValid ( void ) const
    {
        return m_bValid;
    }

    void SetValid ( bool bValid )
    {
        m_bValid = bValid;
    }


    ///////////////////////////////////////////////////////
    // Fixed settings which effect the simulated position
    void SetLooped ( bool bLoop )
    {
        m_bLoop = bLoop;
    }

    void SetLength ( double dLength )
    {
        m_dLength = dLength;
    }


    ///////////////////////////////////////////////////////
    // Variable settings which effect the simulated position
    void SetPlaybackSpeed ( float fSpeed )
    {
        // Catch up
        SetPlayPositionNow ( GetPlayPositionNow () );
        // Then change
        m_fPlaybackSpeed = fSpeed;
    }

    void SetPaused ( bool bPaused )
    {
        // Catch up
        SetPlayPositionNow ( GetPlayPositionNow () );
        // Then change
        m_bPaused = bPaused;
    }


    ///////////////////////////////////////////////////////
    // Set the actual play position
    void SetPlayPositionNow ( double dPosition )
    {
        m_dSavedPosition = dPosition;
        m_SavedTickcount = CTickCount::Now ();
    }


    ///////////////////////////////////////////////////////
    // Get the simulated play position
    double GetPlayPositionNow ( void )
    {
        // If paused, then no time compensation is needed
        if ( m_bPaused )
            return m_dSavedPosition;

        CTickCount deltaTicks = CTickCount::Now () - m_SavedTickcount;
        double dDeltaSeconds = deltaTicks.ToDouble () / 1000.0;
        double dNewPosition = m_dSavedPosition + dDeltaSeconds * m_fPlaybackSpeed;

        // Ensure length is a valid value
        double dUseLength = m_dLength;
        if ( dUseLength <= 0 )
            dUseLength = DEFAULT_SOUND_LENGTH;

        if ( m_bLoop )
            dNewPosition = mod ( dNewPosition, dUseLength );
        else
            dNewPosition = Clamp ( 0.0, dNewPosition, dUseLength );

        return dNewPosition;
    }

    ///////////////////////////////////////////////////////
    // Check if play position has reached the end
    bool IsFinished ( void )
    {
        if ( !m_bLoop && m_dLength > 0 )
            return GetPlayPositionNow () == m_dLength;
        return false;
    }


    // Magical code from the internete
    double mod(double x, double y) { return x - y * floor(x / y); }
};
