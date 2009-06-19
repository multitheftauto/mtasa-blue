/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CFxSystem.h
*  PURPOSE:		Game effects system interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CFxSystem_H
#define __CFxSystem_H

class CFxSystemSAInterface;

class CFxSystem
{
public:
    virtual CFxSystemSAInterface *      GetInterface    ( void ) = 0;

    virtual void                        Play                ( void ) = 0;
    virtual void                        Pause               ( void ) = 0;
    virtual void                        Stop                ( void ) = 0;
    virtual void                        PlayAndKill         ( void ) = 0;
    virtual void                        Kill                ( void ) = 0;
    virtual void                        AttachToBone        ( CEntity * pEntity, int iBone ) = 0;    
    virtual void                        SetMatrix           ( RwMatrix * pMatrix ) = 0;
    virtual void                        SetLocalParticles   ( unsigned char uc_1 ) = 0;
    virtual void                        SetConstTime        ( unsigned char uc_1, float fTime ) = 0;
    virtual int                         GetPlayStatus       ( void ) = 0;
};

#endif