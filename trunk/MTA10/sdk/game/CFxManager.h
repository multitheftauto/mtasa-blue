/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CFxManager.h
*  PURPOSE:		Game effects manager interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CFxManager_H
#define __CFxManager_H

class CFxSystem;

class CFxManager
{
public:
    virtual CFxSystem *                     CreateFxSystem      ( char * szName, RwMatrix * p_1, RwMatrix * p_2, unsigned char uc_3 ) = 0;
    virtual void                            LoadFxSystemBP      ( char * szFilename, int iLine ) = 0;
    virtual unsigned char                   LoadFxProject       ( char * szFilename ) = 0;
};

#endif