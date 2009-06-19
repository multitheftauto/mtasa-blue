/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CFxManagerSA.h
*  PURPOSE:		Game effects manager
*  DEVELOPERS:	Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CFXMANAGERSA_H
#define __CFXMANAGERSA_H

#include <game/CFxManager.h>
#include <game/CFxSystem.h>

#define FUNC_CFxManager_CreateFxSystem      0x4a9bb0;
#define FUNC_CFxManager_LoadFxSystemBP      0x5c1f50;
#define FUNC_CFxManager_LoadFxProject       0x5c2420;

class CFxManagerSAInterface
{
public:
};

class CFxManagerSA : public CFxManager
{
public:
                                    CFxManagerSA        ( CFxManagerSAInterface * pInterface )  { m_pInterface = pInterface; }                

    CFxSystem *                     CreateFxSystem      ( char * szName, RwMatrix * p_1, RwMatrix * p_2, unsigned char uc_3 );
    void                            LoadFxSystemBP      ( char * szFilename, int iLine );
    unsigned char                   LoadFxProject       ( char * szFilename );

    CFxSystem *                     GetFxSystem         ( CFxSystemSAInterface * pInterface );
private:
    CFxManagerSAInterface *         m_pInterface;
    std::vector < CFxSystem * >     m_Systems;
};

#endif