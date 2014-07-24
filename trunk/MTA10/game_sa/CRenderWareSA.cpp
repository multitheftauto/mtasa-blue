/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.cpp
*  PURPOSE:     RenderWare mapping to Grand Theft Auto: San Andreas
*               and miscellaneous rendering functions
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               arc_
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

#include "StdInc.h"
#define RWFUNC_IMPLEMENT
#include "gamesa_renderware.h"
#include "gamesa_renderware.hpp"
#include "CRenderWareSA.ShaderMatching.h"

extern CGameSA * pGame;



// RwFrameForAllObjects struct and callback used to replace dynamic vehicle parts
struct SReplaceParts
{
    const char *szName;                         // name of the part you want to replace (e.g. 'door_lf' or 'door_rf')
    unsigned char ucIndex;                      // index counter for internal usage (0 is the 'ok' part model, 1 is the 'dam' part model)
    RpAtomicContainer *pReplacements;           // replacement atomics
    unsigned int uiReplacements;                // number of replacements
};
static RwObject* ReplacePartsCB ( RwObject * object, SReplaceParts * data )
{
    RpAtomic * Atomic = (RpAtomic*) object;
    char szAtomicName[16] = {0};

    // iterate through our loaded atomics
    for ( unsigned int i = 0; i < data->uiReplacements; i++ ) {
        // get the correct atomic name based on the object # in our parent frame
        if ( data->ucIndex == 0 )
            snprintf ( &szAtomicName[0], 16, "%s_ok", data->szName );
        else
            snprintf ( &szAtomicName[0], 16, "%s_dam", data->szName );

        // see if we have such an atomic in our replacement atomics array
        if ( strncmp ( &data->pReplacements[i].szName[0], &szAtomicName[0], 16 ) == 0 ) {
            // if so, override the geometry
            RpAtomicSetGeometry ( Atomic, data->pReplacements[i].atomic->geometry, 0 );

            // and copy the matrices
            RwFrameCopyMatrix ( RpGetFrame ( Atomic ), RpGetFrame ( data->pReplacements[i].atomic ) );

            object = (RwObject*) data->pReplacements[i].atomic;
            data->ucIndex++;
        }
    }

    return object;
}

// RpClumpForAllAtomics callback used to add atomics to a vehicle
static RpAtomic* AddAllAtomicsCB (RpAtomic * atomic, RpClump * data)
{
    RwFrame * pFrame = RpGetFrame ( data );

    // add the atomic to the frame
    RpAtomicSetFrame ( atomic, pFrame );
    RpClumpAddAtomic ( data, atomic );

    return atomic;
}

// RpClumpForAllAtomics struct and callback used to replace all wheels with a given wheel model
struct SReplaceWheels
{
    const char *szName;                         // name of the new wheel model
    RpClump *pClump;                            // the vehicle's clump
    RpAtomicContainer *pReplacements;           // replacement atomics
    unsigned int uiReplacements;                // number of replacements
};
static RpAtomic* ReplaceWheelsCB (RpAtomic * atomic, SReplaceWheels * data)
{
    RwFrame * Frame = RpGetFrame ( atomic );

    // find our wheel atomics
    if ( strncmp ( &Frame->szName[0], "wheel_lf_dummy", 16 ) == 0 || strncmp ( &Frame->szName[0], "wheel_rf_dummy", 16 ) == 0 || 
         strncmp ( &Frame->szName[0], "wheel_lm_dummy", 16 ) == 0 || strncmp ( &Frame->szName[0], "wheel_rm_dummy", 16 ) == 0 || 
         strncmp ( &Frame->szName[0], "wheel_lb_dummy", 16 ) == 0 || strncmp ( &Frame->szName[0], "wheel_rb_dummy", 16 ) == 0 )
    {
        // find a replacement atomic
        for ( unsigned int i = 0; i < data->uiReplacements; i++ ) {
            // see if it's name is equal to the specified wheel
            if ( strncmp ( &data->pReplacements[i].szName[0], data->szName, 16 ) == 0 ) {
                // clone our wheel atomic
                RpAtomic * WheelAtomic = RpAtomicClone ( data->pReplacements[i].atomic );
                RpAtomicSetFrame ( WheelAtomic, Frame );

                // add it to the clump
                RpClumpAddAtomic ( data->pClump, WheelAtomic );

                // delete the current atomic
                RpClumpRemoveAtomic ( data->pClump, atomic );
            }
        }
    }

    return atomic;
}

// RpClumpForAllAtomics struct and callback used to replace all atomics for a vehicle
struct SReplaceAll
{
    RpClump *pClump;                            // the vehicle's clump
    RpAtomicContainer *pReplacements;           // replacement atomics
    unsigned int uiReplacements;                // number of replacements
};
static RpAtomic* ReplaceAllCB (RpAtomic * atomic, SReplaceAll * data)
{
    RwFrame * Frame = RpGetFrame ( atomic );
    if ( Frame == NULL ) return atomic;

    // find a replacement atomic
    for ( unsigned int i = 0; i < data->uiReplacements; i++ ) {
        // see if we can find an atomic with the same name
        if ( strncmp ( &data->pReplacements[i].szName[0], &Frame->szName[0], 16 ) == 0 ) {
            // copy the matrices
            RwFrameCopyMatrix ( RpGetFrame ( atomic ), RpGetFrame ( data->pReplacements[i].atomic ) );

            // set the new atomic's frame to the current one
            RpAtomicSetFrame ( data->pReplacements[i].atomic, Frame );

            // override all engine and material related callbacks and pointers
            data->pReplacements[i].atomic->renderCallback   = atomic->renderCallback;
            data->pReplacements[i].atomic->frame            = atomic->frame;
            data->pReplacements[i].atomic->render           = atomic->render;
            data->pReplacements[i].atomic->interpolation    = atomic->interpolation;
            data->pReplacements[i].atomic->info             = atomic->info;

            // add the new atomic to the vehicle clump
            RpClumpAddAtomic ( data->pClump, data->pReplacements[i].atomic );

            // remove the current atomic
            RpClumpRemoveAtomic ( data->pClump, atomic );
        }
    }

    return atomic;
}

// RpClumpForAllAtomics struct and callback used to load the atomics from a specific clump into a container
struct SLoadAtomics
{
    RpAtomicContainer *pReplacements;           // replacement atomics
    unsigned int uiReplacements;                // number of replacements
};
static RpAtomic* LoadAtomicsCB (RpAtomic * atomic, SLoadAtomics * data)
{
    RwFrame * Frame = RpGetFrame(atomic);

    // add the atomic to the container
    data->pReplacements [ data->uiReplacements ].atomic = atomic;
    strncpy ( &data->pReplacements [ data->uiReplacements ].szName[0], &Frame->szName[0], 16 );

    // and increment the counter
    data->uiReplacements++;

    return atomic;
}


/////////////////////////////////////////////////////////////////////////////
//
//  CRenderWareSA
//
//
/////////////////////////////////////////////////////////////////////////////
CRenderWareSA::CRenderWareSA ( eGameVersion version )
{
    InitRwFunctions( version );

    InitTextureWatchHooks ();
    m_pMatchChannelManager = new CMatchChannelManager ();
    m_iRenderingEntityType = TYPE_MASK_WORLD;
    m_GTAVertexShadersDisabledTimer.SetMaxIncrement( 1000, true );
    m_bGTAVertexShadersEnabled = true;
}


CRenderWareSA::~CRenderWareSA ( void )
{
    SAFE_DELETE ( m_pMatchChannelManager );
}


// Reads and parses a TXD file specified by a path (szTXD)
RwTexDictionary * CRenderWareSA::ReadTXD ( const CBuffer& fileData )
{
    // open the stream
    RwBuffer buffer;
    buffer.ptr = (void*)fileData.GetData();
    buffer.size = fileData.GetSize();
    RwStream * streamTexture = RwStreamOpen ( STREAM_TYPE_BUFFER, STREAM_MODE_READ, &buffer );

    // check for errors
    if ( streamTexture == NULL )
        return NULL;

    // TXD header id: 0x16
    // find our txd chunk (dff loads textures, so correct loading order is: txd, dff)
    if ( RwStreamFindChunk ( streamTexture, 0x16, NULL, NULL ) == false )
    {
        RwStreamClose ( streamTexture, NULL );
        return NULL;
    }

    // read the texture dictionary from our model (txd)
    RwTexDictionary *pTex = RwTexDictionaryGtaStreamRead ( streamTexture );

    // close the stream
    RwStreamClose ( streamTexture, NULL );

    ScriptAddedTxd ( pTex );

    return pTex;
}

// Reads and parses a DFF file specified by a path (szDFF) into a CModelInfo identified by the object id (usModelID)
// bLoadEmbeddedCollisions should be true for vehicles
// Any custom TXD should be imported before this call
RpClump * CRenderWareSA::ReadDFF ( const CBuffer& fileData, unsigned short usModelID, bool bLoadEmbeddedCollisions )
{
    // Set correct TXD as materials are processed at the same time
    if ( usModelID != 0 )
    {
        unsigned short usTxdId = ((CBaseModelInfoSAInterface**)ARRAY_ModelInfo)[usModelID]->usTextureDictionary;
        SetTextureDict ( usTxdId );
    }

    // open the stream
    RwBuffer buffer;
    buffer.ptr = (void*)fileData.GetData();
    buffer.size = fileData.GetSize();
    RwStream * streamModel = RwStreamOpen ( STREAM_TYPE_BUFFER, STREAM_MODE_READ, &buffer );

    // get the modelinfo array
    DWORD *pPool = ( DWORD* ) ARRAY_ModelInfo;

    // check for errors
    if ( streamModel == NULL )
        return NULL;

    // DFF header id: 0x10
    // find our dff chunk
    if ( RwStreamFindChunk ( streamModel, 0x10, NULL, NULL ) == false )
    {
        RwStreamClose ( streamModel, NULL );
        return NULL;
    }

    // rockstar's collision hack: set the global particle emitter to the modelinfo pointer of this model
    if ( bLoadEmbeddedCollisions )
        RpPrtStdGlobalDataSetStreamEmbedded ( ( void* ) pPool[usModelID] );

    // read the clump with all its extensions
    RpClump * pClump = RpClumpStreamRead ( streamModel );

    // reset collision hack
    if ( bLoadEmbeddedCollisions )
        RpPrtStdGlobalDataSetStreamEmbedded ( NULL );

    // close the stream
    RwStreamClose ( streamModel, NULL );

    return pClump;
}


//
// Returns list of atomics inside a clump
//
void CRenderWareSA::GetClumpAtomicList ( RpClump* pClump, std::vector < RpAtomic* >& outAtomicList )
{
    LOCAL_FUNCTION_START
        static RpAtomic* GetClumpAtomicListCB ( RpAtomic* pAtomic, std::vector < RpAtomic* >* pData )
        {
            pData->push_back ( pAtomic );
            return pAtomic;
        }
    LOCAL_FUNCTION_END

    RpClumpForAllAtomics ( pClump, LOCAL_FUNCTION::GetClumpAtomicListCB, &outAtomicList );
}


//
// Returns true if the clump geometry sort of matches
//
// ClumpA vs ClumpB(or)AtomicB
//
bool CRenderWareSA::DoContainTheSameGeometry ( RpClump* pClumpA, RpClump* pClumpB, RpAtomic* pAtomicB )
{
    // Fast check if comparing one atomic
    if ( pAtomicB )
    {
        RpGeometry* pGeometryA = ( ( RpAtomic* ) ( ( pClumpA->atomics.root.next ) - 0x8 ) )->geometry;    
        RpGeometry* pGeometryB = pAtomicB->geometry;
        return pGeometryA == pGeometryB;
    }

    // Get atomic list from both sides
    std::vector < RpAtomic* > atomicListA;
    std::vector < RpAtomic* > atomicListB;
    GetClumpAtomicList ( pClumpA, atomicListA );
    if ( pClumpB )
        GetClumpAtomicList ( pClumpB, atomicListB );
    if ( pAtomicB )
        atomicListB.push_back ( pAtomicB );

    // Count geometries that exist in both sides
    std::set < RpGeometry* > geometryListA;
    for ( uint i = 0 ; i < atomicListA.size () ; i++ )
        MapInsert ( geometryListA, atomicListA[i]->geometry );

    uint uiInBoth = 0;
    for ( uint i = 0 ; i < atomicListB.size () ; i++ )
        if ( MapContains ( geometryListA, atomicListB[i]->geometry ) )
            uiInBoth++;

    // If less than 50% match then assume it is not the same
    if ( uiInBoth * 2 < atomicListB.size () || atomicListB.size () == 0 )
        return false;

    return true;
}


// Replaces a vehicle/weapon/ped model
void CRenderWareSA::ReplaceModel ( RpClump* pNew, unsigned short usModelID, DWORD dwFunc )
{
    CModelInfo* pModelInfo = pGame->GetModelInfo ( usModelID );
    if ( pModelInfo )
    {
        RpClump* pOldClump = (RpClump *)pModelInfo->GetRwObject ();
        if ( !DoContainTheSameGeometry ( pNew, pOldClump, NULL ) )
        {
            if ( pModelInfo->IsVehicle ( ) )
            {
                // Reset our valid upgrade list
                pModelInfo->ResetSupportedUpgrades ( );
            }

            // Make new clump container for the model geometry
            // Clone twice as the geometry render order seems to be reversed each time it is cloned.
            RpClump* pTemp = RpClumpClone ( pNew );
            RpClump* pNewClone = RpClumpClone ( pTemp );
            RpClumpDestroy ( pTemp );

            // Calling CVehicleModelInfo::SetClump() allocates a new CVehicleStructure.
            // So let's delete the old one first to avoid CPool<CVehicleStructure> depletion.
            if ( dwFunc == FUNC_LoadVehicleModel )
            {
                CVehicleModelInfoSAInterface* pVehicleModelInfoInterface = (CVehicleModelInfoSAInterface*)pModelInfo->GetInterface ();
                if ( pVehicleModelInfoInterface->pSomeInfo )
                {
                    DWORD dwDeleteFunc = FUNC_CVehicleStructure_delete;
                    CVehicleStructure* pSomeInfo = pVehicleModelInfoInterface->pSomeInfo;
                    __asm
                    {
                        mov     eax, pSomeInfo
                        push    eax
                        call    dwDeleteFunc
                        add     esp, 4
                    }
                    pVehicleModelInfoInterface->pSomeInfo = NULL;
                }
            }

            // ModelInfo::SetClump
            CBaseModelInfoSAInterface* pModelInfoInterface = pModelInfo->GetInterface ();
            __asm
            {
                mov     ecx, pModelInfoInterface
                push    pNewClone
                call    dwFunc
            }

            // Destroy old clump container
            RpClumpDestroy ( pOldClump );
        }
    }
}

// Replaces a vehicle model
void CRenderWareSA::ReplaceVehicleModel ( RpClump* pNew, unsigned short usModelID )
{
    ReplaceModel ( pNew, usModelID, FUNC_LoadVehicleModel );
}

// Replaces a weapon model
void CRenderWareSA::ReplaceWeaponModel ( RpClump * pNew, unsigned short usModelID )
{
    ReplaceModel ( pNew, usModelID, FUNC_LoadWeaponModel );
}

// Replaces a ped model
void CRenderWareSA::ReplacePedModel ( RpClump * pNew, unsigned short usModelID )
{
    ReplaceModel ( pNew, usModelID, FUNC_LoadPedModel );
}

// Reads and parses a COL3 file
CColModel * CRenderWareSA::ReadCOL ( const CBuffer& fileData )
{
    if ( fileData.GetSize() < sizeof( ColModelFileHeader ) + 16 )
        return NULL;

    const ColModelFileHeader& header = *(ColModelFileHeader*)fileData.GetData();

    // Load the col model
    if ( header.version[0] == 'C' && header.version[1] == 'O' && header.version[2] == 'L' )
    {
        unsigned char* pModelData = (unsigned char*)fileData.GetData() + sizeof( ColModelFileHeader );

        // Create a new CColModel
        CColModelSA* pColModel = new CColModelSA ();

        if ( header.version[3] == 'L' )
        {
            LoadCollisionModel ( pModelData, pColModel->GetInterface (), NULL );
        }
        else if ( header.version[3] == '2' )
        {
            LoadCollisionModelVer2 ( pModelData, header.size - 0x18, pColModel->GetInterface (), NULL );
        }
        else if ( header.version[3] == '3' )
        {
            LoadCollisionModelVer3 ( pModelData, header.size - 0x18, pColModel->GetInterface (), NULL );
        }

        // Return the collision model
        return pColModel;
    }

    return NULL;
}

// Positions the front seat by reading out the vector from the 'ped_frontseat' atomic in the clump (RpClump*)
// and changing the vector in the CModelInfo class identified by the model id (usModelID)
bool CRenderWareSA::PositionFrontSeat ( RpClump *pClump, unsigned short usModelID )
{
    // get the modelinfo array (+5Ch contains a pointer to vehicle specific dummy data)
    DWORD *pPool = ( DWORD* ) ARRAY_ModelInfo;
    DWORD *pVehicleDummies = ( DWORD* ) ( pPool[usModelID] + 0x5C );

    // read out the 'ped_frontseat' frame
    RwFrame * pPedFrontSeat = RwFrameFindFrame ( RpGetFrame ( pClump ), "ped_frontseat" );
    if ( pPedFrontSeat == NULL )
        return false;

    // in the vehicle specific dummy data, +30h contains the front seat vector
    CVector *vecFrontSeat = ( CVector* ) ( pVehicleDummies + 0x30 );
    vecFrontSeat->fX = pPedFrontSeat->modelling.pos.x;
    vecFrontSeat->fY = pPedFrontSeat->modelling.pos.y;
    vecFrontSeat->fZ = pPedFrontSeat->modelling.pos.z;

    return true;
}

// Loads all atomics from a clump into a container struct and returns the number of atomics it loaded
unsigned int CRenderWareSA::LoadAtomics ( RpClump * pClump, RpAtomicContainer * pAtomics )
{
    // iterate through all atomics in the clump
    SLoadAtomics data = {0};
    data.pReplacements = pAtomics;
    RpClumpForAllAtomics ( pClump, ( void * ) LoadAtomicsCB, &data );
    
    // return the number of atomics that were added to the container
    return data.uiReplacements;
}

// Replaces all atomics for a specific model
typedef struct
{
    unsigned short usTxdID;
    RpClump* pClump;
} SAtomicsReplacer;
RpAtomic* AtomicsReplacer ( RpAtomic* pAtomic, SAtomicsReplacer* pData )
{
    ( (void (*)(RpAtomic*, void*))FUNC_AtomicsReplacer ) ( pAtomic, pData->pClump );
    // The above function adds a reference to the model's TXD. Remove it again.
    CTxdStore_RemoveRef ( pData->usTxdID );
    return pAtomic;
}

void CRenderWareSA::ReplaceAllAtomicsInModel ( RpClump * pNew, unsigned short usModelID )
{
    CModelInfo* pModelInfo = pGame->GetModelInfo ( usModelID );
    if ( pModelInfo )
    {
        RpAtomic* pOldAtomic = (RpAtomic *)pModelInfo->GetRwObject ();
        if ( !DoContainTheSameGeometry ( pNew, NULL, pOldAtomic ) )
        {
            // Clone the clump that's to be replaced (FUNC_AtomicsReplacer removes the atomics from the source clump)
            RpClump * pCopy = RpClumpClone ( pNew );

            // Replace the atomics
            SAtomicsReplacer data;
            data.usTxdID = ((CBaseModelInfoSAInterface**)ARRAY_ModelInfo)[usModelID]->usTextureDictionary;
            data.pClump = pCopy;

            MemPutFast < DWORD > ( (DWORD*)DWORD_AtomicsReplacerModelID, usModelID );
            RpClumpForAllAtomics ( pCopy, AtomicsReplacer, &data );

            // Get rid of the now empty copied clump
            RpClumpDestroy ( pCopy );
        }
    }
}

// Replaces all atomics in a vehicle
void CRenderWareSA::ReplaceAllAtomicsInClump ( RpClump * pDst, RpAtomicContainer * pAtomics, unsigned int uiAtomics )
{
    SReplaceAll data;
    data.pClump = pDst;
    data.pReplacements = pAtomics;
    data.uiReplacements = uiAtomics;
    RpClumpForAllAtomics ( pDst, ReplaceAllCB, &data );
}

// Replaces the wheels in a vehicle
void CRenderWareSA::ReplaceWheels ( RpClump * pClump, RpAtomicContainer * pAtomics, unsigned int uiAtomics, const char * szWheel )
{
    // get the clump's frame
    RwFrame * pFrame = RpGetFrame ( pClump );

    SReplaceWheels data;
    data.pClump = pClump;
    data.pReplacements = pAtomics;
    data.uiReplacements = uiAtomics;
    data.szName = szWheel;
    RpClumpForAllAtomics ( pClump, ReplaceWheelsCB, &data );
}

// Repositions an atomic
void CRenderWareSA::RepositionAtomic ( RpClump * pDst, RpClump * pSrc, const char * szName )
{
    RwFrame * pDstFrame = RpGetFrame ( pDst );
    RwFrame * pSrcFrame = RpGetFrame ( pSrc );
    RwFrameCopyMatrix ( RwFrameFindFrame ( pDstFrame, szName ), RwFrameFindFrame ( pSrcFrame, szName ) );
}

// Adds the atomics from a source clump (pSrc) to a destination clump (pDst)
void CRenderWareSA::AddAllAtomics ( RpClump * pDst, RpClump * pSrc )
{
    RpClumpForAllAtomics ( pSrc, AddAllAtomicsCB, pDst );
}

// Replaces dynamic parts of the vehicle (models that have two different versions: 'ok' and 'dam'), such as doors
// szName should be without the part suffix (e.g. 'door_lf' or 'door_rf', and not 'door_lf_dummy')
bool CRenderWareSA::ReplacePartModels ( RpClump * pClump, RpAtomicContainer * pAtomics, unsigned int uiAtomics, const char * szName )
{
    // get the part's dummy name
    char szDummyName[16] = {0};
    snprintf ( &szDummyName[0], 16, "%s_dummy", szName );

    // get the clump's frame
    RwFrame * pFrame = RpGetFrame ( pClump );

    // get the part's dummy frame
    RwFrame * pPart = RwFrameFindFrame ( pFrame, &szDummyName[0] );
    if ( pPart == NULL )
        return false;

    // now replace all the objects in the frame
    SReplaceParts data = {0};
    data.pReplacements = pAtomics;
    data.uiReplacements = uiAtomics;
    data.szName = szName;
    RwFrameForAllObjects ( pPart, ReplacePartsCB, &data );

    return true;
}

// Replaces a CColModel for a specific object identified by the object id (usModelID)
void CRenderWareSA::ReplaceCollisions ( CColModel* pCol, unsigned short usModelID )
{
    DWORD *pPool = (DWORD *) ARRAY_ModelInfo;
    CColModelSA * pColModel = (CColModelSA*) pCol;
    CModelInfoSA * pModelInfoSA = (CModelInfoSA*)(pGame->GetModelInfo ( usModelID ));

    // Apply some low-level hacks (copies the old col area and sets a flag)
    DWORD pColModelInterface = (DWORD)pColModel->GetInterface ();
    DWORD pOldColModelInterface = *((DWORD *) pPool [ usModelID ] + 20);
    MemOrFast < BYTE > ( pPool [usModelID ] + 0x13, 8 );
    MemPutFast < BYTE > ( pColModelInterface + 40, *((BYTE *)( pOldColModelInterface + 40 )) );

    // TODO: It seems that on entering the game, when this function is executed, the modelinfo array for this
    // model is still zero, leading to a crash!
    pModelInfoSA->IsLoaded ();
}

// Destroys a DFF instance
void CRenderWareSA::DestroyDFF ( RpClump * pClump )
{
    if ( pClump )
        RpClumpDestroy ( pClump );
}

// Destroys a TXD instance
void CRenderWareSA::DestroyTXD ( RwTexDictionary * pTXD )
{
    if ( pTXD )
        RwTexDictionaryDestroy ( pTXD );
}

// Destroys a texture instance
void CRenderWareSA::DestroyTexture ( RwTexture* pTex )
{
    if ( pTex )
    {
        ScriptRemovedTexture ( pTex );
        RwTextureDestroy ( pTex );
    }
}

void CRenderWareSA::RwTexDictionaryRemoveTexture ( RwTexDictionary* pTXD, RwTexture* pTex )
{
    if ( pTex->txd != pTXD )
        return;

    pTex->TXDList.next->prev = pTex->TXDList.prev;
    pTex->TXDList.prev->next = pTex->TXDList.next;
    pTex->txd = NULL;
}

short CRenderWareSA::CTxdStore_GetTxdRefcount ( unsigned short usTxdID )
{
    return *(short *)( *(*(DWORD **)0xC8800C) + 0xC*usTxdID + 4 );
}

bool CRenderWareSA::RwTexDictionaryContainsTexture ( RwTexDictionary* pTXD, RwTexture* pTex )
{
    return pTex->txd == pTXD;
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetTXDIDForModelID
//
// Get a TXD ID associated with the model ID
//
////////////////////////////////////////////////////////////////
ushort CRenderWareSA::GetTXDIDForModelID ( ushort usModelID )
{
    if ( usModelID >= 20000 && usModelID < 25000 )
    {
        // Get global TXD ID instead
        return usModelID - 20000;
    }
    else
    {
        // Get the CModelInfo's TXD ID

        // Ensure valid
        if ( usModelID >= 20000 || !((CBaseModelInfoSAInterface**)ARRAY_ModelInfo)[usModelID] )
            return 0;

        return ((CBaseModelInfoSAInterface**)ARRAY_ModelInfo)[usModelID]->usTextureDictionary;
    }
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetModelTextureNames
//
// Get list of texture names associated with the model
// Will try to load the model if needed
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::GetModelTextureNames ( std::vector < SString >& outNameList, ushort usModelId )
{
    outNameList.clear ();

    // Special case for CJ
    if ( usModelId == 0 )
    {
        outNameList.push_back( "CJ" );
        return;
    }

    ushort usTxdId = GetTXDIDForModelID ( usModelId );

    if ( usTxdId == 0 )
        return;

    // Get the TXD corresponding to this ID
    RwTexDictionary* pTXD = CTxdStore_GetTxd ( usTxdId );

    bool bLoadedModel = false;
    if ( !pTXD )
    {
        // Try model load
        bLoadedModel = true;
        pGame->GetModelInfo ( usModelId )->Request ( BLOCKING, "CRenderWareSA::GetModelTextureNames" );
        pTXD = CTxdStore_GetTxd ( usTxdId );
    }

    std::vector < RwTexture* > textureList;
    GetTxdTextures ( textureList, pTXD );

    for ( std::vector < RwTexture* > ::iterator iter = textureList.begin () ; iter != textureList.end () ; iter++ )
    {
        outNameList.push_back ( (*iter)->name );
    }

    if ( bLoadedModel )
        ( (void (__cdecl *)(unsigned short))FUNC_RemoveModel )( usModelId );
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetTxdTextures
//
// If TXD must already be loaded
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::GetTxdTextures ( std::vector < RwTexture* >& outTextureList, ushort usTxdId )
{
    if ( usTxdId == 0 )
        return;

    // Get the TXD corresponding to this ID
    RwTexDictionary* pTXD = CTxdStore_GetTxd ( usTxdId );

    if ( !pTXD )
        return;

    GetTxdTextures ( outTextureList, pTXD );
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetTxdTextures
//
// Get textures from a TXD
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::GetTxdTextures ( std::vector < RwTexture* >& outTextureList, RwTexDictionary* pTXD )
{
    if ( pTXD )
    {
        RwTexDictionaryForAllTextures ( pTXD, StaticGetTextureCB, &outTextureList );
    }
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::StaticGetTextureCB
//
// Callback used in GetTxdTextures
//
////////////////////////////////////////////////////////////////
bool CRenderWareSA::StaticGetTextureCB ( RwTexture* texture, std::vector < RwTexture* >* pTextureList )
{
    pTextureList->push_back ( texture );
    return true;
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetTextureName
//
// Only called by CRenderItemManager::GetVisibleTextureNames ?
//
////////////////////////////////////////////////////////////////
const char* CRenderWareSA::GetTextureName ( CD3DDUMMY* pD3DData )
{
    STexInfo** ppTexInfo = MapFind ( m_D3DDataTexInfoMap, pD3DData );
    if ( ppTexInfo )
        return (*ppTexInfo)->strTextureName;
    if ( !pD3DData )
        return FAKE_NAME_NO_TEXTURE;
    return "";
}


//
// CFastHashMap functions
//
CD3DDUMMY* GetEmptyMapKey ( CD3DDUMMY** )
{
    return FAKE_D3DTEXTURE_EMPTY_KEY;
}

CD3DDUMMY* GetDeletedMapKey ( CD3DDUMMY** )
{
    return FAKE_D3DTEXTURE_DELETED_KEY;
}

RwFrame * CRenderWareSA::GetFrameFromName ( RpClump * pRoot, SString strName )
{
    return RwFrameFindFrame ( RpGetFrame ( pRoot ), strName );
}