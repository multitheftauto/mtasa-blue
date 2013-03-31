/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CVehicleModelInfoSA.config.h
*  PURPOSE:     Vehicle model resource and configuration management
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CVehicleModelInfoSA_config_H
#define _CVehicleModelInfoSA_config_H

// TexDictionary containing common vehicle textures
extern RwTexDictionary *g_vehicleTxd;

void    VehicleModels_Init( void );
void    VehicleModels_Shutdown( void );

#endif //_CVehicleModelInfoSA_config_H