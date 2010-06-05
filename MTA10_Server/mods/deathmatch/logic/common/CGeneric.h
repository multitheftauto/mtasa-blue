/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/common/CGeneric.h
*  PURPOSE:     Generic template class
*  DEVELOPERS:  Oliver Brown <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

/*
Note from Oli: Read the notes on CGeneric and CGenericManager in CGenericManager.h
*/

#ifndef __CGENERIC_H
#define __CGENERIC_H

#include "CGenericManager.h"


template < typename TManager, typename TManaged > class CGeneric
{
public:
    explicit CGeneric ( CGenericManager < TManager, TManaged >* pManager )
    {
        m_pParent = static_cast < TManaged* > ( this );
        m_pManager = pManager;
        m_pManager->GetParent ()->AddToList ( m_pParent );
    }

    virtual ~CGeneric ( void )
    {
        m_pManager->GetParent ()->RemoveFromList ( m_pParent );
    }

protected:
    TManaged*                                   m_pParent;
    CGenericManager < TManager, TManaged >*     m_pManager;

};

#endif
