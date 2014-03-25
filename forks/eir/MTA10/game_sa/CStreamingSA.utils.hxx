/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.utils.hxx
*  PURPOSE:     Internal data streamer utilities (not in global headers!)
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// Should be inluded once per streaming .cpp file

// ModelCheckDispatch and its API is established so it is easy to extend the resource
// loading algorithms. Any new handlers can be added with their id regions.
// Once we handle all resource loading routines of the engine, this will the common handler
// (we are still missing ASM code which does this dispatching, right?)

// Note: .scm scripts are excluded from the dispatchers, so they are never handled.
// Question: will we ever need them? dependency is on SP-compatibility.

// Default skeleton for model id type dispatching.
// A dispatch method returns either true or false.
//  true - the event was successfully handled
//  false - it could not be handled; might trigger another (default) handler
// (at least this is what it could mean :P)
template <bool defaultReturn, bool otherReturn = false>
struct ModelCheckDispatch abstract  // creating it on its own will optimize it away, so "abstract" makes sense
{
    // Use whatever attributes you want for these methods.
    // __forceinline was chosen as it is best and matches the GTA:SA representation.
    __forceinline bool DoBaseModel( modelId_t id )
    {
        return defaultReturn;
    }

    __forceinline bool DoTexDictionary( modelId_t id )
    {
        return defaultReturn;
    }

    __forceinline bool DoCollision( modelId_t id )
    {
        return defaultReturn;
    }

    __forceinline bool DoIPL( modelId_t id )
    {
        return defaultReturn;
    }

    __forceinline bool DoPathFind( modelId_t id )
    {
        return defaultReturn;
    }

    __forceinline bool DoAnimation( modelId_t id )
    {
        return defaultReturn;
    }

    __forceinline bool DoRecording( modelId_t id )
    {
        return defaultReturn;
    }

    __forceinline bool DoScript( modelId_t id )
    {
        return defaultReturn;
    }

    __forceinline bool DoOther( modelId_t id )
    {
        return otherReturn;
    }
};

// Quick macro so we do not have to repeat logic!
#define idRangeCheckExec( id, off, range, func ) \
{ \
    modelId_t offId = idOffset( (id), (off) ); \
    if ( idRangeCheckEx( offId, (range) ) ) \
        return func( offId ); \
}

// For those who are not satisfied with ExecuteDispatch.
// At least use this so we do not have multiple id region checks floating around!
#define executeDispatch( id, dispatch ) \
{ \
    idRangeCheckExec( (id), 0, MAX_MODELS, (dispatch).DoBaseModel ); \
    idRangeCheckExec( (id), DATA_TEXTURE_BLOCK, 5000, (dispatch).DoTexDictionary ); \
    idRangeCheckExec( (id), DATA_COLL_BLOCK, 256, (dispatch).DoCollision ); \
    idRangeCheckExec( (id), DATA_IPL_BLOCK, 256, (dispatch).DoIPL ); \
    idRangeCheckExec( (id), DATA_PATHFIND_BLOCK, REAL_MAX_PATH_SECTORS, (dispatch).DoPathFind ); \
    idRangeCheckExec( (id), DATA_ANIM_BLOCK, 180, (dispatch).DoAnimation ); \
    idRangeCheckExec( (id), DATA_RECORD_BLOCK, 75, (dispatch).DoRecording ); \
}

template <class type>
bool __forceinline ExecuteDispatch( modelId_t id, type dispatch )
{
    executeDispatch( id, dispatch );
    return dispatch.DoOther( id );;
}

template <class dispatchType>
bool __forceinline ExecuteDispatchEasy( modelId_t id, dispatchType dispatch )
{
    if ( id < MAX_MODELS )                                                                  return dispatch.DoBaseModel( id );
    if ( id >= DATA_TEXTURE_BLOCK && id < DATA_TEXTURE_BLOCK + MAX_TXD )                    return dispatch.DoTexDictionary( id - DATA_TEXTURE_BLOCK );
    if ( id >= DATA_COLL_BLOCK && id < DATA_COLL_BLOCK + 256 )                              return dispatch.DoCollision( id - DATA_COLL_BLOCK );
    if ( id >= DATA_IPL_BLOCK && id < DATA_IPL_BLOCK + 256 )                                return dispatch.DoIPL( id - DATA_IPL_BLOCK );
    if ( id >= DATA_PATHFIND_BLOCK && id < DATA_PATHFIND_BLOCK + REAL_MAX_PATH_SECTORS )    return dispatch.DoPathFind( id - DATA_PATHFIND_BLOCK );
    if ( id >= DATA_ANIM_BLOCK && id < DATA_ANIM_BLOCK + 180 )                              return dispatch.DoAnimation( id - DATA_ANIM_BLOCK );
    if ( id >= DATA_RECORD_BLOCK && id < DATA_RECORD_BLOCK + 75 )                           return dispatch.DoRecording( id - DATA_RECORD_BLOCK );

    return dispatch.DoOther( id );
}

// Use only if model id type information is chained (0-19999 base model, 20000-24999 txds, ...)
// If the compiler is smart, then it would detect ExecuteDispatchEasy as this
// We cannot be certain for now, as we are not the GTA:SA engine ourselves.
// That makes it hard to establish a perfect ID chain.
template <class dispatchType>
bool __forceinline ExecuteDispatchChain( modelId_t id, dispatchType dispatch )
{
    if ( id < DATA_TEXTURE_BLOCK )              return dispatch.DoBaseModel( id );
    if ( id < DATA_COLL_BLOCK )                 return dispatch.DoTexDictionary( idOffset( id, DATA_TEXTURE_BLOCK ) );
    if ( id < DATA_IPL_BLOCK )                  return dispatch.DoCollision( idOffset( id, DATA_COLL_BLOCK ) );
    if ( id < DATA_PATHFIND_BLOCK )             return dispatch.DoIPL( idOffset( id, DATA_IPL_BLOCK ) );
    if ( id < DATA_ANIM_BLOCK )                 return dispatch.DoPathFind( idOffset( id, DATA_PATHFIND_BLOCK ) );
    if ( id < DATA_RECORD_BLOCK )               return dispatch.DoAnimation( idOffset( id, DATA_ANIM_BLOCK ) );
    if ( id < DATA_RECORD_BLOCK + 75 )          return dispatch.DoRecording( idOffset( id, DATA_RECORD_BLOCK ) );

    return dispatch.DoOther( id );
}

// Define this to use your favorite dispatcher method.
// Might improve performance if the compiler optimizes a certain type of branching better.
// You could try experimenting with this.
// This method should be used if you are uncertain which method grants you best performance.
// Otherwise choose any dispatching method provided above.
#define DefaultDispatchExecute( id, dispatch )  ( ExecuteDispatch( (id), (dispatch) ) )

// Utility that loads the correct texture lookup routines for model loading.
struct TextureLookupApplicator
{
    AINLINE TextureLookupApplicator( modelId_t modelIndex )
    {
        CBaseModelInfoSAInterface *modelInfo = ppModelInfo[modelIndex];

        // MTA extension: Apply our global imports
        RwImportedScan::Apply( modelInfo->usTextureDictionary );

        // Decide about model type.
        eRwType rwType = modelInfo->GetRwModelType();

        appliedRemapCheck = false;

        if ( rwType == RW_ATOMIC )
        {
            if ( modelInfo && ( modelInfo->collFlags & COLL_WETROADREFLECT ) )
            {
                RwRemapScan::Apply();
                appliedRemapCheck = true;
            }
        }
        else if ( rwType == RW_CLUMP )
        {
            eModelType modelType = modelInfo->GetModelType();

            if ( modelType == MODEL_VEHICLE )
            {
                RwRemapScan::Apply();
                appliedRemapCheck = true;
            }
        }
    }

    AINLINE ~TextureLookupApplicator( void )
    {
        if ( appliedRemapCheck )
            RwRemapScan::Unapply();

        // MTA extension: remove the global imports handler from the scan stack.
        RwImportedScan::Unapply();
    }

    bool appliedRemapCheck;
};