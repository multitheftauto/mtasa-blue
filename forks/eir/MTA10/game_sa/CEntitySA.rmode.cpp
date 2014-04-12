/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CEntitySA.rmode.cpp
*  PURPOSE:     Entity render mode management
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

// This is a Direct3D 9 style interface for setting render properties to entities.
// It is made to allow flexible property assignment.
// It currently supports floats, bools and ints.

template <typename rModeDescType>
struct preferedRenderModeType_t
{
    rModeDescType modeDesc;
    eRenderModeValueType valueType;
};

typedef preferedRenderModeType_t <eEntityRenderMode> entityRenderModePrefered_t;

static const entityRenderModePrefered_t _preferEntityValueTypes[] =
{
    { ENTITY_RMODE_LIGHTING, RMODE_BOOLEAN },
    { ENTITY_RMODE_LIGHTING_AMBIENT, RMODE_BOOLEAN },
    { ENTITY_RMODE_LIGHTING_DIRECTIONAL, RMODE_BOOLEAN },
    { ENTITY_RMODE_LIGHTING_POINT, RMODE_BOOLEAN },
    { ENTITY_RMODE_LIGHTING_SPOT, RMODE_BOOLEAN },
    { ENTITY_RMODE_REFLECTION, RMODE_BOOLEAN },
    { ENTITY_RMODE_ALPHACLAMP, RMODE_FLOAT }
};

eRenderModeValueType EntityRender::GetPreferedEntityRenderModeType( eEntityRenderMode rMode )
{
    for ( unsigned int n = 0; n < NUMELMS(_preferEntityValueTypes); n++ )
    {
        const entityRenderModePrefered_t& desc = _preferEntityValueTypes[n];

        if ( desc.modeDesc == rMode )
            return desc.valueType;
    }

    return RMODE_VOID;
}

static const rModeResult invalidRenderModeResult( false, "invalid render mode" );

rModeResult EntityRender::ValidateEntityRenderModeBool( eEntityRenderMode rMode, bool value )
{
    rModeResult result;

    if ( rMode == ENTITY_RMODE_LIGHTING )
    {
        result.successful = true;
    }
    else if ( rMode == ENTITY_RMODE_LIGHTING_AMBIENT )
    {
        result.successful = true;
    }
    else if ( rMode == ENTITY_RMODE_LIGHTING_DIRECTIONAL )
    {
        result.successful = true;
    }
    else if ( rMode == ENTITY_RMODE_LIGHTING_POINT )
    {
        result.successful = true;
    }
    else if ( rMode == ENTITY_RMODE_LIGHTING_SPOT )
    {
        result.successful = true;
    }
    else if ( rMode == ENTITY_RMODE_LIGHTING_MATERIAL )
    {
        result.successful = true;
    }
    else if ( rMode == ENTITY_RMODE_REFLECTION )
    {
        result.successful = true;
    }
    else
    {
        result = invalidRenderModeResult;
    }

    return result;
}

rModeResult EntityRender::ValidateEntityRenderModeFloat( eEntityRenderMode rMode, float value )
{
    rModeResult result;

    if ( rMode == ENTITY_RMODE_ALPHACLAMP )
    {
        result.successful = ( value >= 0 && value <= 1.0f );
    }
    else
    {
        result = invalidRenderModeResult;
    }

    return result;
}

rModeResult EntityRender::ValidateEntityRenderModeInt( eEntityRenderMode rMode, int value )
{
    rModeResult result;

    if ( rMode == ENTITY_RMODE_ALPHACLAMP )
    {
        result.successful = ( value >= 0 && value <= 255 );
    }
    else
    {
        result = invalidRenderModeResult;
    }

    return result;
}

rModeResult EntityRender::SetEntityRenderModeBool( entityRenderModes_t& rModes, eEntityRenderMode rMode, bool value )
{
    rModeResult result = EntityRender::ValidateEntityRenderModeBool( rMode, value );

    if ( result.successful )
    {
        if ( rMode == ENTITY_RMODE_LIGHTING )
        {
            rModes.doLighting = value;
        }
        else if ( rMode == ENTITY_RMODE_LIGHTING_AMBIENT )
        {
            rModes.doAmbientLighting = value;
        }
        else if ( rMode == ENTITY_RMODE_LIGHTING_DIRECTIONAL )
        {
            rModes.doDirectionalLighting = value;
        }
        else if ( rMode == ENTITY_RMODE_LIGHTING_POINT )
        {
            rModes.doPointLighting = value;
        }
        else if ( rMode == ENTITY_RMODE_LIGHTING_SPOT )
        {
            rModes.doSpotLighting = value;
        }
        else if ( rMode == ENTITY_RMODE_LIGHTING_MATERIAL )
        {
            rModes.doMaterialLighting = value;
        }
        else if ( rMode == ENTITY_RMODE_REFLECTION )
        {
            rModes.doReflection = value;
        }
        else
        {
            result = invalidRenderModeResult;
        }
    }

    return result;
}

rModeResult EntityRender::SetEntityRenderModeFloat( entityRenderModes_t& rModes, eEntityRenderMode rMode, float value )
{
    rModeResult result = EntityRender::ValidateEntityRenderModeFloat( rMode, value );

    if ( result.successful )
    {
        if ( rMode == ENTITY_RMODE_ALPHACLAMP )
        {
            rModes.alphaClamp = value;
        }
        else
        {
            result = invalidRenderModeResult;
        }
    }

    return result;
}

rModeResult EntityRender::SetEntityRenderModeInt( entityRenderModes_t& rModes, eEntityRenderMode rMode, int value )
{
    rModeResult result = EntityRender::ValidateEntityRenderModeInt( rMode, value );

    if ( result.successful )
    {
        if ( rMode == ENTITY_RMODE_ALPHACLAMP )
        {
            rModes.alphaClamp = (float)value / 255.0f;
        }
        else
        {
            result = invalidRenderModeResult;
        }
    }

    return result;
}

rModeResult EntityRender::GetEntityRenderModeBool( const entityRenderModes_t& rModes, eEntityRenderMode rMode, bool& value )
{
    rModeResult result;

    if ( rMode == ENTITY_RMODE_LIGHTING )
    {
        value = rModes.doLighting;

        result.successful = true;
    }
    else if ( rMode == ENTITY_RMODE_LIGHTING_AMBIENT )
    {
        value = rModes.doAmbientLighting;

        result.successful = true;
    }
    else if ( rMode == ENTITY_RMODE_LIGHTING_DIRECTIONAL )
    {
        value = rModes.doDirectionalLighting;

        result.successful = true;
    }
    else if ( rMode == ENTITY_RMODE_LIGHTING_POINT )
    {
        value = rModes.doPointLighting;

        result.successful = true;
    }
    else if ( rMode == ENTITY_RMODE_LIGHTING_SPOT )
    {
        value = rModes.doSpotLighting;

        result.successful = true;
    }
    else if ( rMode == ENTITY_RMODE_LIGHTING_MATERIAL )
    {
        value = rModes.doMaterialLighting;

        result.successful = true;
    }
    else if ( rMode == ENTITY_RMODE_REFLECTION )
    {
        value = rModes.doReflection;

        result.successful = true;
    }
    else
    {
        result = invalidRenderModeResult;
    }

    return result;
}

rModeResult EntityRender::GetEntityRenderModeFloat( const entityRenderModes_t& rModes, eEntityRenderMode rMode, float& value )
{
    rModeResult result;

    if ( rMode == ENTITY_RMODE_ALPHACLAMP )
    {
        value = rModes.alphaClamp;

        result.successful = true;
    }
    else
    {
        result = invalidRenderModeResult;
    }

    return result;
}

rModeResult EntityRender::GetEntityRenderModeInt( const entityRenderModes_t& rModes, eEntityRenderMode rMode, int& value )
{
    rModeResult result;

    if ( rMode == ENTITY_RMODE_ALPHACLAMP )
    {
        value = (int)( rModes.alphaClamp * 255.0f );

        result.successful = true;
    }
    else
    {
        result = invalidRenderModeResult;
    }

    return result;
}

rModeResult EntityRender::ResetEntityRenderMode( entityRenderModes_t& rModes, eEntityRenderMode rMode )
{
    rModeResult result;

    if ( rMode == ENTITY_RMODE_LIGHTING )
    {
        rModes.doLighting.Reset();

        result.successful = true;
    }
    else if ( rMode == ENTITY_RMODE_LIGHTING_AMBIENT )
    {
        rModes.doAmbientLighting.Reset();

        result.successful = true;
    }
    else if ( rMode == ENTITY_RMODE_LIGHTING_DIRECTIONAL )
    {
        rModes.doDirectionalLighting.Reset();

        result.successful = true;
    }
    else if ( rMode == ENTITY_RMODE_LIGHTING_POINT )
    {
        rModes.doPointLighting.Reset();

        result.successful = true;
    }
    else if ( rMode == ENTITY_RMODE_LIGHTING_SPOT )
    {
        rModes.doSpotLighting.Reset();

        result.successful = true;
    }
    else if ( rMode == ENTITY_RMODE_REFLECTION )
    {
        rModes.doReflection.Reset();

        result.successful = true;
    }
    else if ( rMode == ENTITY_RMODE_ALPHACLAMP )
    {
        rModes.alphaClamp.Reset();

        result.successful = true;
    }
    else
    {
        result = invalidRenderModeResult;
    }

    return result;
}

rModeResult CEntitySA::SetEntityRenderModeBool( eEntityRenderMode rMode, bool value )
{
    return EntityRender::SetEntityRenderModeBool( m_renderModes, rMode, value );
}

rModeResult CEntitySA::SetEntityRenderModeFloat( eEntityRenderMode rMode, float value )
{
    return EntityRender::SetEntityRenderModeFloat( m_renderModes, rMode, value );
}

rModeResult CEntitySA::SetEntityRenderModeInt( eEntityRenderMode rMode, int value )
{
    return EntityRender::SetEntityRenderModeInt( m_renderModes, rMode, value );
}

rModeResult CEntitySA::GetEntityRenderModeBool( eEntityRenderMode rMode, bool& value ) const
{
    return EntityRender::GetEntityRenderModeBool( m_renderModes, rMode, value );
}

rModeResult CEntitySA::GetEntityRenderModeFloat( eEntityRenderMode rMode, float& value ) const
{
    return EntityRender::GetEntityRenderModeFloat( m_renderModes, rMode, value );
}

rModeResult CEntitySA::GetEntityRenderModeInt( eEntityRenderMode rMode, int& value ) const
{
    return EntityRender::GetEntityRenderModeInt( m_renderModes, rMode, value );
}

rModeResult CEntitySA::ResetEntityRenderMode( eEntityRenderMode rMode )
{
    return EntityRender::ResetEntityRenderMode( m_renderModes, rMode );
}

static CEntitySA *renderingEntity = NULL;

void EntityRender::SetRenderingEntity( CEntitySA *entity )
{
    renderingEntity = entity;
}

CEntitySA* EntityRender::GetRenderingEntity( void )
{
    return renderingEntity;
}