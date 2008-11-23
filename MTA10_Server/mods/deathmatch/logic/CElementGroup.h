/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CElementGroup.h
*  PURPOSE:     Element group class
*  DEVELOPERS:  Ed Lyons <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CElementGroup;

#ifndef CELEMENTGROUP_H
#define CELEMENTGROUP_H

#include <list>
#include "CElement.h"

class CElementGroup
{
private:
    list<CElement *>                m_elements;
    class CResource *               m_pResource;
public:
                                    CElementGroup ( CResource * resource )
                                    {
                                        m_pResource = resource;
                                    }
                                    ~CElementGroup();

    void                            Add ( CElement * element );
    void                            Remove ( CElement * element );
    inline CResource *              GetResource ( void ) { return m_pResource; }
    unsigned int                    GetCount ( void );

    list < CElement* > ::const_iterator IterBegin   ( void )    { return m_elements.begin (); };
    list < CElement* > ::const_iterator IterEnd     ( void )    { return m_elements.end (); };
};

#endif

