/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CClientModelCacheManager
{
public:
    virtual                     ~CClientModelCacheManager           ( void ) {}

    // CClientModelCacheManager interface
    virtual void                DoPulse                             ( void ) = 0;
    virtual void                OnRestreamModel                     ( ushort usModelId ) = 0;
};

CClientModelCacheManager* NewClientModelCacheManager ( void );
