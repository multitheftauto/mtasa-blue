/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// For debug drawing
struct SModelCacheStatItem
{
    SString         strTag;
    bool            bCache;
    ePuresyncType   syncType;
    ushort          usModelId;
    CVector         vecStart;
    CVector         vecEnd;
};


class CClientModelCacheManager
{
public:
    virtual                     ~CClientModelCacheManager           ( void ) {}

    // CClientModelCacheManager interface
    virtual void                DoPulse                             ( void ) = 0;
    virtual void                GetStats                            ( std::vector < SModelCacheStatItem >& outList ) = 0;
    virtual void                DrawStats                           ( void ) = 0;
};

CClientModelCacheManager* NewClientModelCacheManager ( void );
