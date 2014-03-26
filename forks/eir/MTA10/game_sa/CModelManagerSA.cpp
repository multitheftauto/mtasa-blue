/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CModelManagerSA.cpp
*  PURPOSE:     DFF model entity manager
*               RenderWare extension
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "gamesa_renderware.h"

#include "CStreamingSA.utils.hxx"

extern CBaseModelInfoSAInterface **ppModelInfo;

CRwObjectSA *g_replObjectNative[MAX_MODELS];

CModelManagerSA::CModelManagerSA( void )
{
    // Reset information structs
    memset( g_replObjectNative, 0, sizeof(g_replObjectNative) );
}

CModelManagerSA::~CModelManagerSA( void )
{
    // Clear our models
    while ( !m_models.empty() )
        delete *m_models.begin();
}

#ifndef _MTA_BLUE
CModelSA* CModelManagerSA::CreateModel( CFile *file, modelId_t id )
#else
CModelSA* CModelManagerSA::CreateModel( const char *filename, modelId_t id, bool loadEmbeddedCollision )
#endif //_MTA_BLUE
{
#ifdef _MTA_BLUE
    CColModel *col = NULL;
    RpClump *clump = pGame->GetRenderWare()->ReadDFF( filename, id, loadEmbeddedCollision, col );

    if ( !clump )
        return NULL;

    CColModelSA *colSA = dynamic_cast <CColModelSA*> ( col );

    return new CModelSA( clump, colSA );
#else
    CColModelSA *col = NULL;
    RpClump *clump = pGame->GetRenderWare()->ReadDFF( file, id, col );

    if ( !clump )
        return NULL;

    return new CModelSA( clump, col );
#endif //_MTA_BLUE
}

CModelSA* CModelManagerSA::CloneClump( modelId_t model )
{
    if ( model > DATA_TEXTURE_BLOCK-1 )
        return NULL;

    CClumpModelInfoSAInterface *info = (CClumpModelInfoSAInterface*)ppModelInfo[model];

    if ( !info || info->GetRwModelType() != RW_CLUMP )
        return NULL;

    if ( !info->GetRwObject() )
        return NULL;

    return new CModelSA( RpClumpClone( info->GetRwObject() ), NULL );
}

CRpAtomicSA* CModelManagerSA::CloneAtomic( modelId_t model )
{
    if ( model > DATA_TEXTURE_BLOCK-1 )
        return NULL;

    CAtomicModelInfoSA *info = (CAtomicModelInfoSA*)ppModelInfo[model];

    if ( !info || info->GetRwModelType() != RW_ATOMIC )
        return NULL;

    if ( !info->GetRwObject() )
        return NULL;

    return new CRpAtomicSA( RpAtomicClone( info->GetRwObject() ) );
}

bool CModelManagerSA::GetRwModelType( modelId_t model, eRwType& type ) const
{
    if ( model > MAX_MODELS-1 )
        return false;

    CBaseModelInfoSAInterface *info = (CBaseModelInfoSAInterface*)ppModelInfo[model];

    if ( !info )
        return false;

    type = info->GetRwModelType();
    return true;
}

bool CModelManagerSA::RestoreModel( modelId_t id )
{
    if ( id > MAX_MODELS-1 )
        return false;

    CRwObjectSA *obj = g_replObjectNative[id];

    if ( !obj )
        return false;

    switch( obj->GetType() )
    {
    case RW_CLUMP:
        ((CModelSA*)obj)->Restore( id );
        break;
    case RW_ATOMIC:
        ((CRpAtomicSA*)obj)->Restore( id );
        break;
    }

    return true;
}

bool CModelManagerSA::RestoreCollision( modelId_t id )
{
    if ( id > MAX_MODELS-1 )
        return false;

    CColModelSA *col = g_colReplacement[id];

    if ( !col )
        return false;

    return col->Restore( id );
}

void CModelManagerSA::RestreamByModel( modelId_t model )
{
    // todo
}

void CModelManagerSA::RestreamByTXD( modelId_t model )
{
    // todo
}

static inline bool UpdateTextureLink( RwTexture*& texLink )
{
    RwTexture *oldTex = texLink;

    // If there is no texture, we actually cannot update it.
    // For that we must bail.
    if ( !oldTex )
    {
        return false;
    }

    RwTexture *newTex = RwFindTexture( oldTex->name, NULL );

    // If there is a new texture, destroy the old one and apply the new one.
    if ( newTex )
    {
        bool updateSuccess = false;

        if ( newTex != oldTex )
        {
            RwTextureDestroy( oldTex );

            texLink = newTex;

            updateSuccess = true;
        }
        
        // If the update failed, delete the reference from the new texture.
        if ( !updateSuccess )
        {
            RwTextureDestroy( newTex );
        }
    }

    return true;
}

struct _updateTexInfo
{
    int txdId;
    modelId_t modelIndex;
    bool requiresReload;
};

static bool RpMaterialUpdateTexture( RpMaterial *material, _updateTexInfo *texUpdate )
{
    // Update the main texture.
    bool success = UpdateTextureLink( material->texture );

    // Update the textures of all plugins aswell.
    // * Reflection mapping
    if ( CSpecMapMaterialSA *specMap = material->specMapMat )
    {
        bool success = UpdateTextureLink( specMap->specMap );

        if ( !success )
            return false;
    }

    if ( CEnvMapMaterialSA *envMap = material->envMapMat )
    {
        bool success = UpdateTextureLink( envMap->envTexture );

        if ( !success )
            return false;
    }

    // * Special effect plugin
    if ( CSpecialFXMatSA *specialFX = material->specialFX )
    {
        int effectType = specialFX->effectType;

        if ( effectType == 1 || effectType == 2 || effectType == 3 )
        {
            bool success = UpdateTextureLink( specialFX->bumpMapEffect.bumpTexture );

            if ( !success )
                return false;
        }

        if ( effectType == 3 )
        {
            bool success = UpdateTextureLink( specialFX->bumpMapEffect.bumpTexture2 );

            if ( !success )
                return false;
        }

        if ( effectType == 4 )
        {
            bool success = UpdateTextureLink( specialFX->effect4.fxTexture );

            if ( !success )
                return false;
        }

        if ( effectType == 6 )
        {
            bool success = UpdateTextureLink( specialFX->effect6.fxBlend.fxTexture );

            if ( !success )
                return false;
        }
    }

    return true;
}

static bool RpAtomicUpdateTextures( RpAtomic *atomic, _updateTexInfo *info )
{
    RpGeometry *geom = atomic->geometry;

    if ( geom )
    {
        // Update all linked materials.
        bool success = geom->ForAllMateria( RpMaterialUpdateTexture, info );

        if ( !success )
        {
            info->requiresReload = true;
            return false;
        }
    }

    return true;
}

struct restreamByModel
{
    typedef std::list <unsigned short> modelList_t;
    typedef std::list <CEntitySAInterface*> entityList_t;

    modelList_t restreamModels;
    entityList_t restreamEntities;

    inline void OnSector( Streamer::streamSectorEntry& entry )
    {
        for ( Streamer::streamSectorEntry::ptrNode_t *ptrNode = entry.GetList(); ptrNode != NULL; ptrNode = ptrNode->m_next )
        {
            CEntitySAInterface *entity = ptrNode->data;

            if ( std::find( restreamModels.begin(), restreamModels.end(), entity->GetModelIndex() ) != restreamModels.end() )
            {
                // Only delete the RenderWare object if it exists.
                // Otherwise some entities will crash us.
                if ( entity->GetRwObject() )
                {
                    entity->DeleteRwObject();

                    // If we need to restream a ped or vehicle, make sure we recreate their RenderWare objects.
                    // For that, we add them to a special list and process them later on.
                    eEntityType type = (eEntityType)entity->nType;

                    if ( type == ENTITY_TYPE_PED || type == ENTITY_TYPE_VEHICLE )
                    {
                        restreamEntities.push_back( entity );
                    }
                }
            }
        }
    }

    inline void RecreateRenderWareInstances( void )
    {
        // Recreate the RenderWare data of entities that absolutely require it.
        for ( entityList_t::const_iterator iter = restreamEntities.begin(); iter != restreamEntities.end(); iter++ )
        {
            CEntitySAInterface *entity = *iter;

            entity->CreateRwObject();

            // Prepare the entity.
            RwObject *rwobj = entity->GetRwObject();

            assert( rwobj != NULL );

            if ( entity->nType == ENTITY_TYPE_PED )
            {
                // The ped requires a static geometry structure.
                // Make sure it has one.
                RpClump *clump = (RpClump*)rwobj;

                if ( !clump->pStatic )
                {
                    clump->CreateStaticGeometry();

                    clump->pStatic->AllocateLink( 32 );
                }
            }
        }
    }
};

void CModelManagerSA::UpdateWorldTextures( int txdId )
{
    // Make sure the TXD instance exists.
    CTxdInstanceSA *txdInst = TextureManager::GetTxdPool()->Get( txdId );

    if ( !txdInst )
        return;

    restreamByModel restream;

    // Function to update the textures of models directly.
    // This is made to prevent restreaming, which causes a blink.
    for ( modelId_t n = 1; n < MAX_MODELS; n++ )
    {
        CBaseModelInfoSAInterface *model = ppModelInfo[n];

        if ( !model || model->usTextureDictionary != txdId )
            continue;

        RwObject *rwobj = model->GetRwObject();

        if ( rwobj )
        {
            eRwType rwtype = rwobj->type;

            _updateTexInfo info;
            info.txdId = txdId;
            info.modelIndex = n;
            info.requiresReload = false;

            if ( rwtype == RW_ATOMIC )
            {
                // Set up the texture lookup routines necessary for this model.
                TextureLookupApplicator texLookup( n );

                // Set current TXD instance as main lookup.
                txdInst->SetCurrent();

                RpAtomicUpdateTextures( (RpAtomic*)rwobj, &info );
            }
            else if ( rwtype == RW_CLUMP )
            {
                // Set up the texture lookup routines necessary for this model.
                TextureLookupApplicator texLookup( n );

                // Set current TXD instance as main lookup.
                txdInst->SetCurrent();

                ((RpClump*)rwobj)->ForAllAtomics( RpAtomicUpdateTextures, &info );
            }

            if ( info.requiresReload )
            {
                // We cannot get around just updating the textures here.
                // The complete model has to be reloaded from the resources to ensure integrity.
                restream.restreamModels.push_back( (unsigned short)n );
            }
        }
    }

    // Ideally, this case is rare.
    if ( !restream.restreamModels.empty() )
    {
        // Delete the RenderWare objects of all entities that have the models that should be restreamed.
        Streamer::ForAllStreamerSectors( restream, true, true, true, true, true );

        bool requireModelLoading = false;

        // Now free the resources from the model info storages.
        // The game will reload the resources for entities that are in sight asynchronically,
        // so we do not have to do that, unless vehicle or ped.
        for ( restreamByModel::modelList_t::const_iterator iter = restream.restreamModels.begin(); iter != restream.restreamModels.end(); iter++ )
        {
            modelId_t modelIndex = *iter;

            Streaming::FreeModel( modelIndex );

            // If we are a ped or vehicle, we need the RenderWare data ASAP.
            // That is why we must load it here.
            CBaseModelInfoSAInterface *modelInfo = ppModelInfo[ modelIndex ];

            eModelType modelType = modelInfo->GetModelType();

            if ( modelType == MODEL_PED || modelType == MODEL_VEHICLE )
            {
                Streaming::RequestModel( modelIndex, 0x10 );

                requireModelLoading = true;
            }
        }

        // Load all requested RenderWare data.
        if ( requireModelLoading )
        {
            Streaming::LoadAllRequestedModels( true );
        }

        // Now recreate important RenderWare links.
        restream.RecreateRenderWareInstances();
    }
}

namespace ModelManager
{
    // Callbacks for the model info class.
    // They should be called if game_sa references/dereferences a model.
    modelRequestCallback_t modelRequestCallback = NULL;     // when refCount > 0
    modelFreeCallback_t modelFreeCallback = NULL;           // when refCount == 0
};

void CModelManagerSA::SetRequestCallback( modelRequestCallback_t callback )
{
    ModelManager::modelRequestCallback = callback;
}

void CModelManagerSA::SetFreeCallback( modelFreeCallback_t callback )
{
    ModelManager::modelFreeCallback = callback;
}