/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CMapLoader.h
*  PURPOSE:     Header for map loader class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*				Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef CMAPLOADER_H
#define CMAPLOADER_H

#include "CDummyEntity.h"

class CMapLoader
{
public:
    CMapLoader ( char * szFilename, CDummyEntity * root );
    ~CMapLoader ();

private:
    CDummyEntity *  m_root;
    bool            m_bLoaded;
};

#endif