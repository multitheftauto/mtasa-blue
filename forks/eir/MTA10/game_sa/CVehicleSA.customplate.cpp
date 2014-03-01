/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CVehicleSA.customplate.cpp
*  PURPOSE:     C++fied logic of CMultiplerSA_LicensePlate module
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "gamesa_renderware.h"

// MTA fixes namespace.
namespace
{
    struct SPlateInfo
    {
        std::vector < RpMaterial* > plateMaterialList;
    };

    std::map < CVehicleModelInfoSAInterface*, SPlateInfo >  ms_ModelPlateMaterialsMap;
    std::vector < RwTexture* >                              ms_SavedTextureList;
    CVehicleModelInfoSAInterface*                           ms_pProcessingVehicleModelInfo = NULL;
    RwTexture*                                              ms_pLastGeneratedPlateText = NULL;
}

static unsigned char GetPlateTextureIndexFromZone( void )
{
    int zoneIndex = *(short*)0x00C81314;

    if ( zoneIndex == 1 )
        return 2;

    if ( zoneIndex <= 2 || zoneIndex > 4 )
        return 0;

    return 1;
}

static bool RenderLicensePlate( const char *plateText, RwTexture *charSetTexture, unsigned int unk, RwRaster *target )
{
    return ((bool (__cdecl*)( const char *plateText, RwTexture *charSetTexture, unsigned int unk, RwRaster *target ))0x006FDD70)( plateText, charSetTexture, unk, target );
}

/*=========================================================
    CreateVehicleNameplateTexture

    Arguments:
        plateText - the text to render on the requested plate
        plateDesign - the design to give the plate
    Purpose:
        Creates a new vehicle nameplate texture and returns
        it if successful.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x006FDEA0
=========================================================*/
RwTexture* _CreateVehicleNameplateTexture( const char *plateText, unsigned char plateDesign )
{
    // Decide about the plate design.
    if ( plateDesign == -1 )
        plateDesign = GetPlateTextureIndexFromZone();

    RwRaster *plateRaster = RwRasterCreate(
        64,     // 64 pixels wide
        16,     // 16 pixels high
        32,     // 32bit color depth
        0x604
    );

    // Creation of our plate texture could fail.
    if ( !plateRaster )
        return NULL;

    // Make sure we have a plate character set.
    RpMaterial *matCharSet = *(RpMaterial**)0x00C3EF5C;
    RwTexture *texCharSet = matCharSet->texture;

    if ( !texCharSet )
    {
        RwRasterDestroy( plateRaster );
        return NULL;
    }

    // Render our license plate.
    unsigned int unk = ( (unsigned int*)0x00C3EF6C )[ plateDesign ];

    bool success = RenderLicensePlate( plateText, texCharSet, unk, plateRaster );

    if ( !success )
    {
        RwRasterDestroy( plateRaster );
        return NULL;
    }

    // Attempt to create our final texture.
    RwTexture *plateTexture = RwTextureCreate( plateRaster );

    if ( !plateTexture )
    {
        RwRasterDestroy( plateRaster );
        return NULL;
    }

    // Set the texture name to the raster text.
    plateTexture->SetName( plateText );

    plateTexture->flags_a = true;

    return plateTexture;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CCustomCarPlateMgr::CreatePlateTexture
//
// Optional optimization.
// Prevent GTA from generating a new texture for each occurrence of 'carplate' in a model
//
//////////////////////////////////////////////////////////////////////////////////////////
RwTexture* CreateVehicleNameplateTexture( const char *plateText, unsigned char plateDesign )
{
    // Skip our code if not processing a VehicleModelInfo
    if ( ms_pProcessingVehicleModelInfo == NULL )
    {
        // Use LastGeneratedPlateTexture if we have one
        RwTexture *lastTex = ms_pLastGeneratedPlateText;

        if ( lastTex )
            return lastTex;
    }

    // Call original
    RwTexture *genTex = _CreateVehicleNameplateTexture( plateText, plateDesign );

    // Store the last generated texture.
    ms_pLastGeneratedPlateText = genTex;
    return genTex;
}

/*=========================================================
    RwMaterialSetLicensePlate

    Arguments:
        mat - vehicle atomic geometry material
        plate - contains name and style information
    Purpose:
        Scans through all materials of the vehicle model
        and updates the ones named "carplate" and "carpback".
    Binary offsets:
        (1.0 US and 1.0 EU): 0x006FE060
=========================================================*/
struct _licensePlate
{
    char text[8];
    unsigned char style;
    RpMaterial *plate;  // ext
};

static RpMaterial* HandleVehicleFrontNameplate( RpMaterial *mat, _licensePlate *info, unsigned char design )
{
    // Wire in the MTA team fix.
    OnMY_CCustomCarPlateMgr_SetupMaterialPlateTexture( mat, info->text, design );

    if ( !mat )
        return NULL;

    RwTexture *plateTexture = CreateVehicleNameplateTexture( info->text, design );

    if ( !plateTexture )
        return NULL;

    mat->SetTexture( plateTexture );

    RwTextureDestroy( plateTexture );
    return mat;
}

static RwTexture* GetBackPlateByDesign( unsigned char design )
{
    return ( (RwTexture**)0x00C3EF60 )[ design ];
}

static RpMaterial* HandleVehicleBackNameplate( RpMaterial *mat, unsigned char design )
{
    if ( design == -1 )
        design = GetPlateTextureIndexFromZone();

    RwTexture *backPlateTexture = GetBackPlateByDesign( design );

    mat->SetTexture( backPlateTexture );
    return mat;
}

static bool RwMaterialSetLicensePlate( RpMaterial *mat, _licensePlate *plate )
{
    if ( !mat->texture )
        return true;

    if ( strcmp( mat->texture->name, "carplate" ) == 0 )
    {
        plate->plate = mat;

        HandleVehicleFrontNameplate( mat, plate, *(unsigned char*)0x00C3EF80 );
        return true;
    }

    if ( strcmp( mat->texture->name, "carpback" ) == 0 )
        HandleVehicleBackNameplate( mat, *(unsigned char*)0x00C3EF80 );

    return true;
}

/*=========================================================
    RwAtomicSetLicensePlate

    Arguments:
        child - atomic of the vehicle model
        plate - license plate generation details
    Purpose:
        Generates the license plate for specific materials of
        the atomic's geometry.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x006FE0D0
=========================================================*/
static bool RwAtomicSetLicensePlate( RpAtomic *child, _licensePlate *plate )
{
    child->geometry->ForAllMateria( RwMaterialSetLicensePlate, plate );
    return true;
}

/*=========================================================
    GetRandomNameplateText

    Arguments:
        buffer - output memory allocation for nameplate text
                 generation
        max - size of the buffer, preferably (i * 3 + 4)
    Purpose:
        Generates a nameplate text message for the nameplate
        material routine.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x006FD5B0
=========================================================*/
#define RAND        (double)(rand() & 0xFFFF) / 0x7FFF
#define RANDCHAR    (char)( RAND * 0.23 )
#define RANDNUM     (char)( RAND * -9 )

static bool GetRandomNameplateText( char *buffer, size_t max )
{
    if ( max < 4 )
        return false;

    buffer[0] = 'A' - RANDCHAR;
    buffer[1] = 'A' - RANDCHAR;
    buffer[2] = '0' - RANDNUM;
    buffer[3] = '0' - RANDNUM;

    for ( unsigned int n = 4; n < max; )
    {
        buffer[n++] = '0' - RANDNUM;
        buffer[n++] = 'A' - RANDCHAR;
        buffer[n++] = 'A' - RANDCHAR;
    }

    return true;
}

/*=========================================================
    CVehicleModelInfoSAInterface::InitNameplate

    Purpose:
        Initializes the custom license plate which is shared
        for all vehicles of this model.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C9450
=========================================================*/
void CVehicleModelInfoSAInterface::InitNameplate( void )
{
    // Wire in the MTA team fix.
    OnMY_CVehicleModelInfo_SetCarCustomPlate( this );

    _licensePlate plate;

    // Get some random stuff into nameplate
    GetRandomNameplateText( plate.text, 8 );

    plate.style = plateDesign;
    plate.plate = NULL;

    GetRwObject()->ForAllAtomics( RwAtomicSetLicensePlate, &plate );

    if ( plate.plate )
        pPlateMaterial = plate.plate;

    // Wire in another fix by MTA team.
    OnMY_CVehicleModelInfo_SetCarCustomPlate_Post();
}

/*=========================================================
    CVehicleSAInterface::CreateLicensePlate

    Arguments:
        info - pointer to the vehicle model info associated
               with this vehicle
    Purpose:
        Creates the nameplate texture that should be rendered
        with this vehicle.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x006FDEA0
=========================================================*/
void CVehicleSAInterface::CreateLicensePlate( CVehicleModelInfoSAInterface *info )
{
    // Reset our plate texture.
    m_pCustomPlateTexture = NULL;

    // Does the vehicle model info have a ready-made plate for us?
    RpMaterial *plateMat = info->pPlateMaterial;

    if ( !plateMat )
        return;

    // Get the actual plate text.
    const char *plateText = info->GetPlateText();

    // If we do not have a valid plate text, simply use the texture
    // and forget about it.
    if ( !plateText )
    {
        RwTexture *plateTexture = plateMat->texture;

        // Bugfix: do not care about the texture, it does not have to be set.
        if ( plateTexture )
            plateTexture->refs++;

        // Set it to our vehicle.
        m_pCustomPlateTexture = plateTexture;
        return;
    }

    // Request a nameplate texture for our vehicle.
    RwTexture *plateTexture = CreateVehicleNameplateTexture( plateText, info->plateDesign );

    m_pCustomPlateTexture = plateTexture;

    // Update this model info's plate text. (why? I have no idea.)
    info->SetPlateText( plateText );
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CAutomobile::CustomCarPlate_BeforeRenderingStart
//
// Swap custom plate textures (GTA only does one, we do the rest here)
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_CAutomobile_CustomCarPlate_BeforeRenderingStart( CVehicleSAInterface* pVehicle, CVehicleModelInfoSAInterface* pVehicleModelInfo )
{
    if ( !pVehicle->m_pCustomPlateTexture )
        return;

    SPlateInfo* pPlateInfo = MapFind( ms_ModelPlateMaterialsMap, pVehicleModelInfo );
    if ( pPlateInfo && !pPlateInfo->plateMaterialList.empty() )
    {
        // Copy custom plate texture (for this vehicle) into the model 'carplate' materials
        for ( uint i = 0 ; i < pPlateInfo->plateMaterialList.size() ; i++ )
        {
            RpMaterial* pMaterial = pPlateInfo->plateMaterialList[i];
            // Only do replace if it's not the material that GTA is handling
            if ( pMaterial != pVehicleModelInfo->pPlateMaterial )
            {
                RwTexture* pOldTexture = pMaterial->texture;
                pOldTexture->refs++;
                ms_SavedTextureList.push_back( pOldTexture );
                pMaterial->SetTexture( pVehicle->m_pCustomPlateTexture );
            }
            else
                ms_SavedTextureList.push_back( NULL );
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CAutomobile::CustomCarPlate_AfterRenderingStop
//
// Restore custom plate textures
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_CAutomobile_CustomCarPlate_AfterRenderingStop( CVehicleModelInfoSAInterface* pVehicleModelInfo )
{
    if ( ms_SavedTextureList.empty() )
        return;

    SPlateInfo* pPlateInfo = MapFind( ms_ModelPlateMaterialsMap, pVehicleModelInfo );
    if ( pPlateInfo )
    {
        // Restore custom plate texture (for this vehicle) from the model 'carplate' materials
        for ( uint i = 0 ; i < pPlateInfo->plateMaterialList.size() ; i++ )
        {
            RpMaterial* pMaterial = pPlateInfo->plateMaterialList[i];
            RwTexture* pOldTexture = ms_SavedTextureList[i];
            // Only do restore if it's not the one that GTA handled
            if ( pMaterial != pVehicleModelInfo->pPlateMaterial )
            {
                assert( pOldTexture );
                pMaterial->SetTexture( pOldTexture );
                RwTextureDestroy( pOldTexture );
            }
            else
                assert( pOldTexture == NULL );
        }
        ms_SavedTextureList.clear();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CCustomCarPlateMgr::SetupMaterialPlateTexture
//
// Save plate material for using later (in BeforeRenderingStart())
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_CCustomCarPlateMgr_SetupMaterialPlateTexture( struct RpMaterial * a, char * b, unsigned char c )
{
    if ( ms_pProcessingVehicleModelInfo )
    {
        SPlateInfo& info = MapGet( ms_ModelPlateMaterialsMap, ms_pProcessingVehicleModelInfo );
        info.plateMaterialList.push_back( a );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// CVehicleModelInfo::SetCarCustomPlate
//
// Prepare to receive list of plate materials for this vehicle type
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_CVehicleModelInfo_SetCarCustomPlate( CVehicleModelInfoSAInterface* pVehicleModelInfo )
{
    assert( !ms_pLastGeneratedPlateText );
    ms_pProcessingVehicleModelInfo = pVehicleModelInfo;
    SPlateInfo& info = MapGet( ms_ModelPlateMaterialsMap, ms_pProcessingVehicleModelInfo );
    info.plateMaterialList.clear();
}

void OnMY_CVehicleModelInfo_SetCarCustomPlate_Post( void )
{
    // Done
    ms_pProcessingVehicleModelInfo = NULL;
    ms_pLastGeneratedPlateText = NULL;
}