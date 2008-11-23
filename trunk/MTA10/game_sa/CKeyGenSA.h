/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CKeyGenSA.h
*  PURPOSE:		Header file for game key generator class
*  DEVELOPERS:	Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CKeyGenSA_H
#define __CKeyGenSA_H

#include <game/CKeyGen.h>

#define FUNC_CKeyGen_GetKey_len         0x53ced0
#define FUNC_CKeyGen_GetKey             0x53cf00
#define FUNC_CKeyGen_GetUppercaseKey    0x53cf30
#define FUNC_CKeyGen_AppendStringToKey  0x53cf70

class CKeyGenSA : public CKeyGen
{
public:
    unsigned int            GetKey              ( const char * szString, int iLength );
    unsigned int            GetKey              ( const char * szString );
    unsigned int            GetUppercaseKey     ( const char * szString );
    unsigned int            AppendStringToKey   ( unsigned int uiKey, const char * szString );
};

#endif