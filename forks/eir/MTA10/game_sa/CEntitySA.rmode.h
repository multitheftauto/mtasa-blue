/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CEntitySA.rmode.h
*  PURPOSE:     Entity render mode management
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _ENTITY_RENDER_MODES_
#define _ENTITY_RENDER_MODES_

struct entityRenderModes_t
{
    template <typename dataType>
    struct genericProperty
    {
        inline genericProperty( dataType defValue )
        {
            this->defaultValue = defValue;
            this->isApplied = false;
        }

        operator dataType( void )
        {
            if ( !isApplied )
                return defaultValue;

            return value;
        }

        operator const dataType( void ) const
        {
            if ( !isApplied )
                return defaultValue;

            return value;
        }

        inline void Set( const dataType& value )
        {
            this->value = value;

            isApplied = true;
        }

        inline bool IsApplied( void ) const
        {
            return isApplied;
        }

        inline void Reset( void )
        {
            isApplied = false;
        }

        dataType& operator =( const dataType& value )
        {
            Set( value );

            return this->value;
        }

    protected:
        dataType value;
        dataType defaultValue;
        bool isApplied;
    };

    typedef genericProperty <bool> boolProperty;
    typedef genericProperty <float> floatProperty;
    typedef genericProperty <int> intProperty;

    entityRenderModes_t( void ) :
        doLighting( true ),
        doAmbientLighting( true ),
        doDirectionalLighting( true ),
        doPointLighting( true ),
        doSpotLighting( true ),
        doMaterialLighting( true ),
        doReflection( true ),
        alphaClamp( 100.0f / 255.0f )
    {
        return;
    }

    void Reset( void )
    {
        doLighting.Reset();
        doAmbientLighting.Reset();
        doDirectionalLighting.Reset();
        doPointLighting.Reset();
        doSpotLighting.Reset();
        doMaterialLighting.Reset();
        doReflection.Reset();
        alphaClamp.Reset();
    }

    boolProperty doLighting;
    boolProperty doAmbientLighting;
    boolProperty doDirectionalLighting;
    boolProperty doPointLighting;
    boolProperty doSpotLighting;
    boolProperty doMaterialLighting;
    boolProperty doReflection;

    floatProperty alphaClamp;
};

class CEntitySA;

namespace EntityRender
{
    eRenderModeValueType    GetPreferedEntityRenderModeType ( eEntityRenderMode rMode );

    rModeResult         ValidateEntityRenderModeBool        ( eEntityRenderMode rMode, bool value );
    rModeResult         ValidateEntityRenderModeInt         ( eEntityRenderMode rMode, int value );
    rModeResult         ValidateEntityRenderModeFloat       ( eEntityRenderMode rMode, float value );

    rModeResult         SetEntityRenderModeBool             ( entityRenderModes_t& rModes, eEntityRenderMode rMode, bool value );
    rModeResult         SetEntityRenderModeInt              ( entityRenderModes_t& rModes, eEntityRenderMode rMode, int value );
    rModeResult         SetEntityRenderModeFloat            ( entityRenderModes_t& rModes, eEntityRenderMode rMode, float value );

    rModeResult         GetEntityRenderModeBool             ( const entityRenderModes_t& rModes, eEntityRenderMode rMode, bool& value );
    rModeResult         GetEntityRenderModeInt              ( const entityRenderModes_t& rModes, eEntityRenderMode rMode, int& value );
    rModeResult         GetEntityRenderModeFloat            ( const entityRenderModes_t& rModes, eEntityRenderMode rMode, float& value );

    rModeResult         ResetEntityRenderMode               ( entityRenderModes_t& rModes, eEntityRenderMode rMode );

    void                SetRenderingEntity      ( CEntitySA *entity );
    CEntitySA*          GetRenderingEntity      ( void );
};

#endif //_ENTITY_RENDER_MODES_