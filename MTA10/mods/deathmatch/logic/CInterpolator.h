/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CInterpolator.h
*  PURPOSE:     Header for interpolator class
*  DEVELOPERS:  Kent Simon <>
*               Christian Myhre Lundheim <>
*               Jax <>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

#include "Utils.h"

template < class T, unsigned int uiMaxElements = 64 >
class CInterpolator
{
protected:
    struct VecMap
    {
        unsigned long   m_ulTime;
        T               data;
    };

public:
    CInterpolator       ( )
    {
        Clear ();
    }

    ~CInterpolator      ( )
    {
        Clear ();
    }

    void                Push                ( const T& newData, unsigned long ulTime )
    {
        unsigned int uiIndex = Index ( m_uiEndIdx + 1 );
        m_nodes [ m_uiEndIdx ].data = newData;
        m_nodes [ m_uiEndIdx ].m_ulTime = ulTime;
        m_uiEndIdx = uiIndex;

        if ( Size () < uiMaxElements )
            ++m_uiSize;
        else
            m_uiStartIdx = Index ( m_uiStartIdx + 1 );
    }

    void                Pop                 ( )
    {
        if ( Size () > 0 )
        {
            m_uiStartIdx = Index ( m_uiStartIdx + 1 );
            --m_uiSize;
        }
    }

    bool                Evaluate            ( unsigned long ulTime, T* output )
    {
        if ( Size () == 0 )
            return false;

        // Time later than newest point, so use the newest point
        if ( ulTime >= m_nodes [ Index ( m_uiEndIdx - 1 ) ].m_ulTime )
        {
            *output = m_nodes [ Index ( m_uiEndIdx - 1 ) ].data;
        }
        // Time earlier than oldest point, so use the oldest point
        else if ( ulTime <= m_nodes [ m_uiStartIdx ].m_ulTime )
        {
            *output = m_nodes [ m_uiStartIdx ].data;
        }
        else
        {
            // Find the two points either side and interpolate
            unsigned int uiCurrent = Index ( m_uiStartIdx + 1 );
            for ( ; uiCurrent != m_uiEndIdx; uiCurrent = Index ( uiCurrent + 1 ) )
            {
                if ( ulTime < m_nodes [ uiCurrent ].m_ulTime )
                    return Eval ( m_nodes [ Index ( uiCurrent - 1 ) ], m_nodes [ uiCurrent ], ulTime, output );
            }
        }

        return true;
    }

    unsigned long       GetOldestEntry      ( T* output )
    {
        if ( Size () > 0 )
        {
            *output = m_nodes [ m_uiStartIdx ].data;
            return m_nodes [ m_uiStartIdx ].m_ulTime;
        }
        else
            return 0UL;
    }

    unsigned int        Size                ( ) const
    {
        return m_uiSize;
    }

    void                Clear               ( )
    {
        m_uiStartIdx = 0;
        m_uiEndIdx = 0;
        m_uiSize = 0;
    }

protected:
    virtual bool        Eval                ( const VecMap& Left,
                                              const VecMap& Right,
                                              unsigned long ulTimeEval,
                                              T* output )
    {
        // Check for being the same or maybe wrap around
        if ( Left.m_ulTime >= Right.m_ulTime )
        {
            *output = Right.data;
            return true;
        }

        // Find the relative position of ulTimeEval between R.m_ulTimeStamp and L.m_ulTimeStamp
        float fAlpha = Unlerp ( Left.m_ulTime, ulTimeEval, Right.m_ulTime );

        // Lerp between Right.pos and Left.pos
        *output = Lerp ( Left.data, fAlpha, Right.data );
        return true;
    }

private:
    unsigned int        Index               ( unsigned int uiIndex ) const
    {
        return ( uiIndex % uiMaxElements );
    }


private:
    VecMap          m_nodes [ uiMaxElements ];
    unsigned int    m_uiStartIdx;
    unsigned int    m_uiEndIdx;
    unsigned int    m_uiSize;
};
