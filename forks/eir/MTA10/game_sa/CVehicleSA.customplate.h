/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CVehicleSA.customplate.h
*  PURPOSE:     C++fied logic of CMultiplerSA_LicensePlate module
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _VEHICLE_RENDER_CUSTOMPLATE_
#define _VEHICLE_RENDER_CUSTOMPLATE_

// Exports by MTA team.
void OnMY_CAutomobile_CustomCarPlate_BeforeRenderingStart( CVehicleSAInterface* pVehicle, CVehicleModelInfoSAInterface* pVehicleModelInfo );
void OnMY_CAutomobile_CustomCarPlate_AfterRenderingStop( CVehicleModelInfoSAInterface* pVehicleModelInfo );

void OnMY_CCustomCarPlateMgr_SetupMaterialPlateTexture( struct RpMaterial * a, char * b, unsigned char c );

void OnMY_CVehicleModelInfo_SetCarCustomPlate( CVehicleModelInfoSAInterface* pVehicleModelInfo );
void OnMY_CVehicleModelInfo_SetCarCustomPlate_Post( void );

#endif //_VEHICLE_RENDER_CUSTOMPLATE_