/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CEntitySA.render.hxx
*  PURPOSE:     Internal entity rendering definitions
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// Make sure the rendering guards the entities it uses.
struct unorderedEntityRenderChainInfo
{
    void InitFirst( void )
    {
    }

    void InitLast( void )
    {
    }

    bool operator <( const unorderedEntityRenderChainInfo& right )
    {
        return false;
    }

    typedef void (__cdecl*callback_t)( CEntitySAInterface *intf, float dist );

    CEntitySAInterface  *entity;
    bool isReferenced;

    inline void Execute( void )
    { }
};

typedef CRenderChainInterface <unorderedEntityRenderChainInfo> mainEntityRenderChain_t;

// Render lights.
inline RpLight*& GetFirstLight( void )
{
    return *(RpLight**)0x00C886E8;
}

inline RpLight*& GetSecondLight( void )
{
    return *(RpLight**)0x00C886EC;
}

// Render helper for world entities.
// Should be used for all rendering lists.
template <typename renderCallback>
AINLINE void RenderInstances( renderCallback& cb )
{
    switch( Entity::GetWorldRenderMode() )
    {
    default:
    case WORLD_RENDER_ORIGINAL:
        // Render like usual.
        cb.OnRenderStage( false, true, false );
        break;
    case WORLD_RENDER_MESHLOCAL_ALPHAFIX:
        RenderCallbacks::SetAlphaSortingEnabled( true );
        RenderCallbacks::SetAlphaSortingParams( true, true, true );

        // Render opaque, translucent and depth on a per-mesh basis.
        cb.OnRenderStage( true, true, true );

        RenderCallbacks::SetAlphaSortingEnabled( false );
        break;
    case WORLD_RENDER_SCENE_ALPHAFIX:
        RenderCallbacks::SetAlphaSortingEnabled( true );

        // Render opaque pixels.
        RenderCallbacks::SetAlphaSortingParams( true, false, false );

        cb.OnRenderStage( true, false, false );

        // Render translucent pixels, in reverse order.
        RenderCallbacks::SetAlphaSortingParams( false, true, false );

        cb.OnRenderStage( true, false, true );

        // Render depth.
        RenderCallbacks::SetAlphaSortingParams( false, false, true );

        cb.OnRenderStage( true, true, false );

        RenderCallbacks::SetAlphaSortingEnabled( false );
        break;
    }
}

// Exports of all internal rendering chains.
extern mainEntityRenderChain_t groundAlphaEntities;
extern mainEntityRenderChain_t staticRenderEntities;
extern mainEntityRenderChain_t lowPriorityRenderEntities;