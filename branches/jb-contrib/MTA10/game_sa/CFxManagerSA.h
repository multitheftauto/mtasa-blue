/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CParticleSystemManagerSA.h
*  PURPOSE:     Header file for particle system manager class
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

#include <game/CFxManager.h>

#define FUNC_CFxManager__CreateFxSystem                                 0x4A9BE0

class CFxSystem;

class CFxManagerSAInterface
{
public:
};

class CFxManagerSA : public CFxManager
{
public:
    CFxManagerSA(CFxManagerSAInterface * pInterface);
    CFxSystem* CreateFxSystem(const SString & strName, CVector & vecPoint, RwMatrix * pMatrix, bool bUnk);
private:
    CFxManagerSAInterface * m_pInterface;
};
