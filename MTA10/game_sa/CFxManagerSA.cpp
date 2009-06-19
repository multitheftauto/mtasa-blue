/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CFxManagerSA.cpp
*  PURPOSE:		Game effects manager
*  DEVELOPERS:	Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include <vector>
using namespace std;

CFxSystem * CFxManagerSA::CreateFxSystem ( char *szName, RwMatrix *p_1, RwMatrix *p_2, unsigned char uc_3 )
{
    CFxSystemSAInterface * pInterface = NULL;
    DWORD dwFunc = FUNC_CFxManager_CreateFxSystem;
    DWORD dwThis = (DWORD)m_pInterface;
    _asm
    {
        mov     ecx, dwThis
        push    uc_3
        push    p_2
        push    p_1
        push    szName
        call    dwFunc
        mov     pInterface, eax
    }

    if ( pInterface ) return GetFxSystem ( pInterface );
    return NULL;
}


void CFxManagerSA::LoadFxSystemBP ( char * szFilename, int iLine )
{
    DWORD dwFunc = FUNC_CFxManager_LoadFxSystemBP;
    DWORD dwThis = (DWORD) m_pInterface;
    _asm
    {
        mov     ecx, dwThis
        push    iLine
        push    szFilename
        call    dwFunc
    }
}


unsigned char CFxManagerSA::LoadFxProject ( char * szFilename )
{
    unsigned int ucReturn;
    DWORD dwFunc = FUNC_CFxManager_LoadFxProject;
    DWORD dwThis = (DWORD) m_pInterface;
    _asm
    {
        mov     ecx, dwThis
        push    szFilename
        call    dwFunc
        mov     ucReturn, eax
    }
    return ucReturn;
}


CFxSystem * CFxManagerSA::GetFxSystem ( CFxSystemSAInterface * pInterface )
{
    CFxSystem * pSystem = NULL;
    vector < CFxSystem * > ::iterator iter = m_Systems.begin ();
    for ( ; iter != m_Systems.end () ; iter++ )
    {
        pSystem = *iter;
        if ( pSystem->GetInterface () == pInterface )
        {
            return pSystem;
        }
    }
    pSystem = new CFxSystemSA ( pInterface );
    m_Systems.push_back ( pSystem );
    return pSystem;
}