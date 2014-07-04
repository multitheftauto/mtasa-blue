/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

struct SModelCacheStats;


class CModelCacheManager
{
public:
    virtual                     ~CModelCacheManager                 ( void ) {}

    // CModelCacheManager interface
    virtual void                DoPulse                             ( void ) = 0;
    virtual void                GetStats                            ( SModelCacheStats& outStats ) = 0;
    virtual void                OnRestreamModel                     ( ushort usModelId ) = 0;
    virtual void                OnClientClose                       ( void ) = 0;
    virtual void                UpdatePedModelCaching               ( const std::map < ushort, float >& newNeedCacheList ) = 0;
    virtual void                UpdateVehicleModelCaching           ( const std::map < ushort, float >& newNeedCacheList ) = 0;
    virtual void                AddModelToPersistentCache           ( ushort usModelId ) = 0;
};

CModelCacheManager* NewModelCacheManager ( void );
