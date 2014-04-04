/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CIPLSectorSA.h
*  PURPOSE:     Header file for IPL sector management
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _IPL_SECTOR_
#define _IPL_SECTOR_

#include "CQuadTreeSA.h"

// Module initialization.
void IPLSector_Init( void );
void IPLSector_Shutdown( void );

// IPL sector (buildings) instance
class CIPLFileSA    //size: 52 bytes
{
public:
    CIPLFileSA( const char *name ) : m_bounds( MAX_REGION, -MAX_REGION, -MAX_REGION, MAX_REGION )
    {
        m_iplId = -1;

        m_isInterior = false;
        m_isLoaded = false;
        m_streamingSectorLoad = false;
        m_doNotStream = true;

        m_terminateAtUnload = false;
        m_containsFarChunks = false;

        // Otherwise we encounter loading trouble in cached mode.
        assert( strlen( name ) < sizeof( m_name ) );

        // Store the .IPL filename in our field.
        strncpy( m_name, name, 17 );
        m_name[17] = '\0';
    }

    // Methods required by the sectorizer.
    inline bool     IsSectorFlagged( void ) const           { return m_streamingSectorLoad; }
    inline void     FlagSector( bool flagged )              { m_streamingSectorLoad = flagged; }
    inline bool     IsLoaded( void ) const                  { return m_isLoaded; }
    inline bool     IsStreamingDisabled( void ) const       { return m_doNotStream; }

    inline void     OnStreamOut( void )
    {
        if ( m_terminateAtUnload )
            m_doNotStream = true;
    }

    void* operator new ( size_t );
    void operator delete ( void *ptr );

    CBounds2D       m_bounds;               // 0
    char            m_name[18];             // 16

    typedef ValueRange <short> indexRange_t;

    // Unless those ranges are updated, native GTA:SA will only support up to MAX_SHORT buildings or dummies.
    // This poses a risk to total conversions such as GTA:SoL.
    // Solution: update struct and use "int" as rangeNumberType.
    indexRange_t    m_buildingRange;        // 34, pool index range of building pool
    indexRange_t    m_dummyRange;           // 38, pool index range of dummy pool

    short           m_iplId;                // 42

    bool            m_isInterior;           // 44
    bool            m_isLoaded;             // 45
    bool            m_streamingSectorLoad;  // 46, set by streaming to indicate that we want to load it
    bool            m_doNotStream;          // 47, 1
    bool            m_terminateAtUnload;    // 48
    bool            m_containsFarChunks;    // 49, is true if LOD distance of any instance is greater than 150.0f

    WORD            m_pad;                  // 50
};

// Quad tree of all IPL sectors.
typedef CQuadTreeNodeSAInterface <CIPLFileSA> iplFileQuadTreeNode_t;

#endif //_IPL_SECTOR_