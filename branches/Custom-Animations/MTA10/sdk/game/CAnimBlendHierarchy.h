/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CAnimBlendHierarchy.h
*  PURPOSE:     Animation blend hierarchy interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CAnimBlendHierarchy_H
#define __CAnimBlendHierarchy_H

class CAnimBlendHierarchySAInterface;

class CAnimBlendHierarchy
{
public:
    virtual CAnimBlendHierarchySAInterface *        GetInterface            ( void ) = 0;
    virtual int                                     GetAnimBlockID          ( void ) = 0;
};

#endif
