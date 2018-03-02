#pragma once

#ifndef __CCLIENTIFP_H
#define __CCLIENTIFP_H

#include "CClientEntity.h"

class CClientIFP: public CClientEntity
{
public:
                                    CClientIFP              ( class CClientManager* pManager, ElementID ID );
                                    ~CClientIFP             ( void );

    virtual eClientEntityType       GetType                 ( void ) const              { return CCLIENTIFP; }

    bool                            LoadIFP                 ( const char* szFilePath, const char* szBlockName );
    void                            UnloadIFP               ( void );

    // Sorta a hack that these are required by CClientEntity...
    void                            Unlink                  ( void ) {};
    void                            GetPosition             ( CVector& vecPosition ) const {};
    void                            SetPosition             ( const CVector& vecPosition ) {};

};

#endif
