/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CFxSystemSA.h
*  PURPOSE:		Game effects system
*  DEVELOPERS:	Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CFXSYSTEMSA_H
#define __CFXSYSTEMSA_H

#include <game/CFxSystem.h>

#define FUNC_CFxSystem_Play                 0x4aa2f0
#define FUNC_CFxSystem_Pause                0x4aa370
#define FUNC_CFxSystem_Stop                 0x4aa390
#define FUNC_CFxSystem_PlayAndKill          0x4aa3d0
#define FUNC_CFxSystem_Kill                 0x4aa3f0
#define FUNC_CFxSystem_AttachToBone         0x4aa400
#define FUNC_CFxSystem_AddParticle          0x4aa540
#define FUNC_CFxSystem_EnablePrim           0x4aa610
#define FUNC_CFxSystem_SetMatrix            0x4aa630
#define FUNC_CFxSystem_SetOffsetPos         0x4aa660
#define FUNC_CFxSystem_AddOffsetPos         0x4aa690
#define FUNC_CFxSystem_SetConstTime         0x4aa6c0
#define FUNC_CFxSystem_SetRateMult          0x4aa6f0
#define FUNC_CFxSystem_SetTimeMult          0x4aa6f0
#define FUNC_CFxSystem_SetVelAdd            0x4aa730
#define FUNC_CFxSystem_GetPlayStatus        0x4aa900
#define FUNC_CFxSystem_SetLocalParticles    0x4aa910

class CFxSystemSAInterface
{
public:
};

class CFxSystemSA : public CFxSystem
{
public:
                                CFxSystemSA         ( CFxSystemSAInterface * pInterface ) { m_pInterface = pInterface; }

    CFxSystemSAInterface *      GetInterface        ( void )        { return m_pInterface; }

    void                        Play                ( void );
    void                        Pause               ( void );
    void                        Stop                ( void );
    void                        PlayAndKill         ( void );
    void                        Kill                ( void );
    void                        AttachToBone        ( CEntity * pEntity, int iBone );    
    void                        SetMatrix           ( RwMatrix * pMatrix );
    void                        SetLocalParticles   ( unsigned char uc_1 );
    void                        SetConstTime        ( unsigned char uc_1, float fTime );
    int                         GetPlayStatus       ( void );

private:
    CFxSystemSAInterface *      m_pInterface;
};

#endif