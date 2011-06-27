/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.h
*  PURPOSE:     Header file for RenderWare game engine class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CRENDERWARESA
#define __CRENDERWARESA

#define WIN32_LEAN_AND_MEAN

#include <game/CRenderWare.h>

#include "CModelInfoSA.h"
#include "CColModelSA.h"
#include "Common.h"
#include <windows.h>
#include <stdio.h>

class CRenderWareSA : public CRenderWare
{
    public:
                        CRenderWareSA               ( enum eGameVersion version );
                        ~CRenderWareSA              ( void ) {};

    // Adds textures into the TXD of a model, eventually making a copy of each texture first
    void                ModelInfoTXDAddTextures     ( std::list < RwTexture* >& textures, unsigned short usModelID, bool bMakeCopy = true, std::list < RwTexture* >* pReplacedTextures = NULL, std::list < RwTexture* >* pAddedTextures = NULL, bool bAddRef = true );

    // Removes texture from the TXD of a model, eventually destroying each texture
    void                ModelInfoTXDRemoveTextures  ( std::list < RwTexture* >& textures, unsigned short usModelID, bool bDestroy = true, bool bKeepRaster = false, bool bRemoveRef = true );

    // Reads and parses a TXD file specified by a path (szTXD)
    RwTexDictionary *   ReadTXD                     ( const char *szTXD );

    // Reads and parses a DFF file specified by a path (szDFF) into a CModelInfo identified by the object id (usModelID)
    // uiModelID == 0 means no collisions will be loaded (be careful! seems crashy!)
    RpClump *           ReadDFF                     ( const char * szDFF, unsigned short usModelID );

    // Destroys a DFF instance
    void                DestroyDFF                  ( RpClump * pClump );

    // Destroys a TXD instance
    void                DestroyTXD                  ( RwTexDictionary * pTXD );

    // Destroys a texture
    void                DestroyTexture              ( RwTexture* pTex );

    // Reads and parses a COL3 file with an optional collision key name
    CColModel *         ReadCOL                     ( const char * szCOLFile );

    // Replaces a CColModel for a specific object identified by the object id (usModelID)
    void                ReplaceCollisions           ( CColModel * pColModel, unsigned short usModelID );

    // Positions the front seat by reading out the vector from the 'ped_frontseat' atomic in the clump (RpClump*)
    // and changing the vector in the CModelInfo class identified by the model id (usModelID)
    bool                PositionFrontSeat           ( RpClump * pClump, unsigned short usModelID );

    // Loads all atomics from a clump into a container struct and returns the number of atomics it loaded
    unsigned int        LoadAtomics                 ( RpClump * pClump, RpAtomicContainer * pAtomics );

    // Replaces all atomics for a specific model
    void                ReplaceAllAtomicsInModel    ( RpClump * pSrc, unsigned short usModelID );

    // Replaces all atomics in a clump
    void                ReplaceAllAtomicsInClump    ( RpClump * pDst, RpAtomicContainer * pAtomics, unsigned int uiAtomics );

    // Replaces the wheels in a vehicle
    void                ReplaceWheels               ( RpClump * pClump, RpAtomicContainer * pAtomics, unsigned int uiAtomics, const char * szWheel = "wheel" );

    // Repositions an atomic
    void                RepositionAtomic            ( RpClump * pDst, RpClump * pSrc, const char * szName );

    // Adds the atomics from a source clump (pSrc) to a destination clump (pDst)
    void                AddAllAtomics               ( RpClump * pDst, RpClump * pSrc );

    // Replaces a CClumpModelInfo (or CVehicleModelInfo, since its just for vehicles) clump with a new clump
    void                ReplaceVehicleModel         ( RpClump * pNew, unsigned short usModelID );

    // Replaces dynamic parts of the vehicle (models that have two different versions: 'ok' and 'dam'), such as doors
    // szName should be without the part suffix (e.g. 'door_lf' or 'door_rf', and not 'door_lf_dummy')
    bool                ReplacePartModels           ( RpClump * pClump, RpAtomicContainer * pAtomics, unsigned int uiAtomics, const char * szName );

    ushort              GetTXDIDForModelID          ( ushort usModelID );
    void*               GetD3DDataForTxdTexture     ( ushort usModelID, const char* szTextureName );
    void                InitModelTextureWatch       ( PFN_WATCH_CALLBACK pfnWatchCallback );
    void                BeginModelTextureWatch      ( ushort usModelID, const char* szTextureName );
    void                EndModelTextureWatch        ( ushort usModelID, const char* szTextureName );
    void                PulseModelTextureWatch      ( void );
    void                GetModelTextureNames        ( std::vector < SString >& outNameList, ushort usModelID );

private:
    static void         RwTexDictionaryRemoveTexture( RwTexDictionary* pTXD, RwTexture* pTex );
    static short        CTxdStore_GetTxdRefcount    ( unsigned short usTxdID );
    static bool         ListContainsNamedTexture    ( std::list < RwTexture* >& list, const char* szTexName );
    static bool         StaticGetTextureCB          ( RwTexture* texture, std::vector < SString >* pNameList );

    // Watched model textures
    struct SWatchedModelInfo
    {
        std::map < SString, void* > nameDataMap;  
    };
    std::map < ushort, SWatchedModelInfo >  m_watchedModelInfoMap;
    PFN_WATCH_CALLBACK                      m_pfnWatchCallback;
};

#endif
