/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CFx.h
*  PURPOSE:     Game effects interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

class CFxManager
{
public:
    virtual CFxSystem* CreateFxSystem(const SString & strName, CVector & vecPoint, RwMatrix * pMatrix, bool bUnk) = 0;
};