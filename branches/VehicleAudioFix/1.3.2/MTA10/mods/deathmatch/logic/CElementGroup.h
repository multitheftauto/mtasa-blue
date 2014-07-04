/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CElementGroup.h
*  PURPOSE:     Header for element group class
*  DEVELOPERS:  Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CElementGroup;

#ifndef CELEMENTGROUP_H
#define CELEMENTGROUP_H

#include "../../shared_logic/CClientEntity.h"

class CElementGroup
{
private:
    CFastList < CClientEntity* >    m_elements;
public:
                                    ~CElementGroup();
    void                            Add ( CClientEntity * element );
    void                            Remove ( CClientEntity * element );
    unsigned int                    GetCount ( void );
};

#endif