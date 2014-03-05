/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CFxSA.h
*  PURPOSE:     Game effects handling
*  DEVELOPERS:  Jax <>
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CFXSA_H
#define __CFXSA_H

#include <game/CFx.h>

#define FUNC_CFx_AddBlood                  0x49eb00
#define FUNC_CFx_AddWood                   0x49ee10
#define FUNC_CFx_AddSparks                 0x49f040
#define FUNC_CFx_AddTyreBurst              0x49f300
#define FUNC_CFx_AddBulletImpact           0x49f3d0
#define FUNC_CFx_AddPunchImpact            0x49f670
#define FUNC_CFx_AddDebris                 0x49f750
#define FUNC_CFx_AddGlass                  0x49f970
#define FUNC_CFx_AddWheelSpray             0x49fb30
#define FUNC_CFx_AddWheelGrass             0x49ff20
#define FUNC_CFx_AddWheelGravel            0x4a0170
#define FUNC_CFx_AddWheelMud               0x4a03c0
#define FUNC_CFx_AddWheelSand              0x4a0610
#define FUNC_CFx_AddWheelDust              0x4a09c0
#define FUNC_CFx_TriggerWaterHydrant       0x4a0d70
#define FUNC_CFx_TriggerGunshot            0x4a0de0
#define FUNC_CFx_TriggerTankFire           0x4a0fa0
#define FUNC_CFx_TriggerWaterSplash        0x4a1070
#define FUNC_CFx_TriggerBulletSplash       0x4a10e0
#define FUNC_CFx_TriggerFootSplash         0x4a1150

class CFxSAInterface
{
public:
                        CFxSAInterface              ( void );
                        ~CFxSAInterface             ( void );

    void                AssociateGameTranslators    ( void );

    unsigned int        GetEffectQuality            ( void )            { return m_fxQuality; }

    BYTE                                    m_pad[80];      // 0
    unsigned int                            m_count;        // 80
    unsigned int                            m_fxQuality;    // 84
};

class CFxSA : public CFx
{
public:
                        CFxSA                   ( CFxSAInterface *intf );
                        ~CFxSA                  ( void );

    void                AddBlood                ( CVector& pos, CVector& dir, int iCount, float fBrightness );
    void                AddWood                 ( CVector& pos, CVector& dir, int iCount, float fBrightness );
    void                AddSparks               ( CVector& pos, CVector& dir, float fForce, int iCount, CVector acrossLine, unsigned char ucBlurIf0, float fSpread, float fLife );
    void                AddTyreBurst            ( CVector& pos, CVector& dir );
    void                AddBulletImpact         ( CVector& pos, CVector& dir, int iSmokeSize, int iSparkCount, float fSmokeIntensity );
    void                AddPunchImpact          ( CVector& pos, CVector& dir, int );
    void                AddDebris               ( CVector& pos, RwColor& rwColor, float fScale, int iCount );
    void                AddGlass                ( CVector& pos, RwColor& rwColor, float fScale, int iCount );
    void                TriggerWaterHydrant     ( CVector& pos );
    void                TriggerGunshot          ( CEntity* pEntity, CVector& pos, CVector& dir, bool bIncludeSparks );
    void                TriggerTankFire         ( CVector& pos, CVector& dir );
    void                TriggerWaterSplash      ( CVector& pos );
    void                TriggerBulletSplash     ( CVector& pos );
    void                TriggerFootSplash       ( CVector& pos );

private:
    CFxSAInterface*     m_interface;
};

extern CFxSAInterface*  g_effectManager;

#endif
