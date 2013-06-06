/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CVehicleModelInfoSA.render.h
*  PURPOSE:     Vehicle model info rendering module
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CVehicleModelInfoSA_render_H
#define _CVehicleModelInfoSA_render_H

class CVehicleSA;

void __cdecl CacheVehicleRenderCameraSettings( unsigned char alpha, RwObject *obj );
void __cdecl ClearVehicleRenderChains( void );
void __cdecl ExecuteVehicleRenderChains( unsigned char renderAlpha );

void __cdecl SetVehicleLightsFlags( class CVehicleSAInterface *vehicle );

void __cdecl RpClumpSetupVehicleMaterials( RpClump *clump, class CVehicleSA *gameVehicle );
void __cdecl RpClumpRestoreVehicleMaterials( RpClump *clump );

template <class type>
struct UniqueContainer : std::vector <type>
{
    bool __forceinline Add( type item )
    {
        unsigned int count = size();

        for ( unsigned int n = 0; n < count; n++ )
        {
            if ( at( n ) == item )
                return false;
        }

        push_back( item );
        return true;
    }
};

typedef UniqueContainer <RpMaterial*> MaterialContainer;

void VehicleModelInfoRender_Init( void );
void VehicleModelInfoRender_SetupDevice( void );
void VehicleModelInfoRender_Reset( void );
void VehicleModelInfoRender_OnInvalidate( void );
void VehicleModelInfoRender_Shutdown( void );

#endif //_CVehicleModelInfoSA_render_H