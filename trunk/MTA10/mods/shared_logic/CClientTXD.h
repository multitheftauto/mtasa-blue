/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTXD.h
*  PURPOSE:     TXD manager class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*****************************************************************************/

#ifndef __CCLIENTTXD_H
#define __CCLIENTTXD_H

#include <list>
#include "CClientEntity.h"

class CClientTXD: public CClientEntity
{
public:
                                    CClientTXD              ( class CClientManager* pManager, ElementID ID );
                                    ~CClientTXD             ( void );

    eClientEntityType               GetType                 ( void ) const              { return CCLIENTTXD; }

    bool                            LoadTXD                 ( const char* szFile );
    void                            UnloadTXD               ( void );
    inline bool                     IsLoaded                ( void )                    { return m_pTexture != NULL; };

    bool                            Import                  ( unsigned short usModel );
    bool                            IsImported              ( unsigned short usModel );

    void                            Remove                  ( unsigned short usModel );
    void                            RemoveAll               ( void );

    static bool                     IsImportableModel       ( unsigned short usModel );

    // Sorta a hack that these are required by CClientEntity...
    void                            Unlink                  ( void ) {};
    void                            GetPosition             ( CVector& vecPosition ) const {};
    void                            SetPosition             ( const CVector& vecPosition ) {};

private:
    void                            InternalRemove          ( unsigned short usModel );

    RwTexDictionary*                m_pTexture;
    std::list < unsigned short >    m_Imported;
};


#endif