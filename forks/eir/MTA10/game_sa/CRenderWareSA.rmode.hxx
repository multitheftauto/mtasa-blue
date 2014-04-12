/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.rmode.hxx
*  PURPOSE:     RenderWare virtual render mode management
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RENDERWARE_RENDER_MODE_SYSTEM_
#define _RENDERWARE_RENDER_MODE_SYSTEM_

namespace RenderMode
{
    inline bool IsLightingEnabled( const entityRenderModes_t *renderModes )
    {
        if ( renderModes->doLighting.IsApplied() )
        {
            return renderModes->doLighting;
        }

        return true;
    }

    inline bool IsLightingEnabled( void )
    {
        CEntitySA *renderingEntity = EntityRender::GetRenderingEntity();

        if ( renderingEntity )
        {
            return IsLightingEnabled( &renderingEntity->m_renderModes );
        }

        return true;
    }

    inline bool GetRenderModeRwLightEnable( const entityRenderModes_t *renderModes, RpLight *light )
    {
        if ( !IsLightingEnabled( renderModes ) )
            return false;

        if ( light->subtype == LIGHT_TYPE_AMBIENT )                                     return renderModes->doAmbientLighting;
        if ( light->subtype == LIGHT_TYPE_DIRECTIONAL )                                 return renderModes->doDirectionalLighting;
        if ( light->subtype == LIGHT_TYPE_POINT )                                       return renderModes->doPointLighting;
        if ( light->subtype == LIGHT_TYPE_SPOT_1 || light->type == LIGHT_TYPE_SPOT_2 )  return renderModes->doSpotLighting;

        return true;
    }

    inline bool IsRwLightEnabled( RpLight *light )
    {
        CEntitySA *renderingEntity = EntityRender::GetRenderingEntity();

        if ( renderingEntity && !GetRenderModeRwLightEnable( &renderingEntity->m_renderModes, light ) )
            return false;

        return true;
    }

    inline bool IsDirectionalLightingEnabled( void )
    {
        CEntitySA *renderingEntity = EntityRender::GetRenderingEntity();

        if ( renderingEntity && renderingEntity->m_renderModes.doDirectionalLighting.IsApplied() )
        {
            return renderingEntity->m_renderModes.doDirectionalLighting;
        }

        return true;
    }

    inline bool IsMaterialLightingEnabled( void )
    {
        CEntitySA *renderingEntity = EntityRender::GetRenderingEntity();

        if ( renderingEntity && renderingEntity->m_renderModes.doMaterialLighting.IsApplied() )
        {
            return renderingEntity->m_renderModes.doMaterialLighting;
        }

        return true;
    }

    inline bool AreReflectionsEnabled( void )
    {
        CEntitySA *renderingEntity = EntityRender::GetRenderingEntity();

        if ( renderingEntity && renderingEntity->m_renderModes.doReflection.IsApplied() )
        {
            return renderingEntity->m_renderModes.doReflection;
        }
        
        return true;
    }

    inline unsigned int FloatToD3DColorValue( float value )
    {
        return (unsigned int)( value * 255.0f );
    }

    inline bool GetAlphaClamp( unsigned int& value )
    {
        CEntitySA *renderingEntity = EntityRender::GetRenderingEntity();

        if ( renderingEntity && renderingEntity->m_renderModes.alphaClamp.IsApplied() )
        {
            value = FloatToD3DColorValue( renderingEntity->m_renderModes.alphaClamp );
            return true;
        }
        
        return false;
    }
};

inline unsigned int GetWorldDefaultAlphaClamp( void )
{
    return 100;
}

inline unsigned int GetWorldObjectAlphaClamp( void )
{
    unsigned int defaultAlphaClamp = GetWorldDefaultAlphaClamp();

    RenderMode::GetAlphaClamp( defaultAlphaClamp );

    return defaultAlphaClamp;
}

#endif //_RENDERWARE_RENDER_MODE_SYSTEM_