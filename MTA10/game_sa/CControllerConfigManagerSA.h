/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CControllerConfigManagerSA.h
*  PURPOSE:		Header file for controller configuration manager class
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_CONROLLER_CONFIG_MANAGER
#define __CGAMESA_CONROLLER_CONFIG_MANAGER

#include <game/CControllerConfigManager.h>

#define FUNC_SetControllerKeyAssociatedWithAction       0x530490
#define FUNC_GetControllerKeyAssociatedWithAction       0x52F4F0
#define FUNC_GetNumOfSettingsForAction                  0x52F4A0
#define FUNC_ClearSettingsAssociatedWithAction          0x52FD70

#define CLASS_CControllerConfigManager                  0xB70198

class CControllerConfigManagerSA : public CControllerConfigManager
{
public:
							CControllerConfigManagerSA();
        void                SetControllerKeyAssociatedWithAction ( eControllerAction action, int iKey, eControllerType controllerType );
        int                 GetControllerKeyAssociatedWithAction ( eControllerAction action, eControllerType controllerType );
        int                 GetNumOfSettingsForAction ( eControllerAction action );
        void                ClearSettingsAssociatedWithAction ( eControllerAction action, eControllerType controllerType );
        unsigned char       GetInputType ( void );
        void                SetInputType ( unsigned char ucInputType );
        bool                IsMouseInverted ( void );
        void                SetMouseInverted ( bool bInverted );
        bool                GetFlyWithMouse ( void );
        void                SetFlyWithMouse ( bool bFlyWithMouse );
        bool                GetSteerWithMouse ( void );
        void                SetSteerWithMouse ( bool bFlyWithMouse );
};

#endif