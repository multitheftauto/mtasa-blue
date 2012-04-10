/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CMemStats.h
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/


class CMemStatsInterface
{
public:
    virtual             ~CMemStatsInterface   ( void ) {}

    virtual void        Draw                    ( void ) = 0;
    virtual void        SetEnabled              ( bool bEnabled ) = 0;
    virtual bool        IsEnabled               ( void ) = 0;
};

CMemStatsInterface* GetMemStats ( void );
