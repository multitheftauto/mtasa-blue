/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CMapLoader.cpp
*  PURPOSE:     Loading maps
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*				Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CMapLoader.h"
#include "./xml/CXMLFileImpl.h"

CMapLoader::CMapLoader ( char * szFilename, CDummyEntity * root )
{
 /*   m_root = root;
    cxmlfileimpl file ( szfilename );
    if ( file.parse() )
    {
        cxmlnode * firstnode = file.getrootnode();
        if ( firstnode )
        {
            char szbuffer[32];
            firstnode->gettagname ( szbuffer, 32 );
            if ( stricmp ( szbuffer, "map" ) == 0 )
            {
                m_root->*/
}

CMapLoader::~CMapLoader ()
{

}