/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRenderModes.h
*  PURPOSE:     Clientside virtual entity render modes management
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CLIENT_RENDER_MODES_SYSTEM_
#define _CLIENT_RENDER_MODES_SYSTEM_

template <typename rModeDescType>
struct clientRenderModeStorage
{
    template <typename dataType>
    struct rModeProperty
    {
        inline rModeProperty( void )
        {
            isApplied = false;
        }
        
        inline void Set( const dataType& value )
        {
            this->value = value;

            isApplied = true;
        }

        inline void Reset( void )
        {
            isApplied = false;
        }

        operator dataType( void ) const
        {
            if ( !isApplied )
                return (dataType)0;

            return value;
        }

        const dataType& operator =( const dataType& value )
        {
            this->value = value;

            return value;
        }

        bool isApplied;

    protected:
        dataType value;
    };

    typedef rModeProperty <bool>    rModeBoolProperty;
    typedef rModeProperty <float>   rModeFloatProperty;
    typedef rModeProperty <int>     rModeIntProperty;

    struct rModeStorage
    {
        inline rModeStorage( rModeDescType modeType )
        {
            this->modeType = modeType;
        }

        rModeDescType modeType;

        rModeBoolProperty boolValue;
        rModeFloatProperty floatValue;
        rModeIntProperty intValue;
    };
    typedef std::list <rModeStorage> rModeProperties_t;

    // Storage functions, for saving and applying render modes.
    rModeProperties_t properties;

    rModeStorage*   GetPropertyStorage( rModeDescType rMode )
    {
        for ( rModeProperties_t::iterator iter = properties.begin(); iter != properties.end(); iter++ )
        {
            rModeStorage& store = *iter;

            if ( store.modeType == rMode )
                return &store;
        }

        return NULL;
    }

    rModeBoolProperty&  GetStoredBool( rModeDescType rMode )
    {
        rModeStorage *store = GetPropertyStorage( rMode );

        if ( !store )
        {
            properties.push_back( rModeStorage( rMode ) );

            store = &properties.back();
        }

        return store->boolValue;
    }

    rModeFloatProperty& GetStoredFloat( rModeDescType rMode )
    {
        rModeStorage *store = GetPropertyStorage( rMode );

        if ( !store )
        {
            properties.push_back( rModeStorage( rMode ) );

            store = &properties.back();
        }

        return store->floatValue;
    }

    rModeIntProperty&   GetStoredInt( rModeDescType rMode )
    {
        rModeStorage *store = GetPropertyStorage( rMode );

        if ( !store )
        {
            properties.push_back( rModeStorage( rMode ) );

            store = &properties.back();
        }

        return store->intValue;
    }

    void            StoreBool( rModeDescType rMode, bool value )
    {
        GetStoredBool( rMode ).Set( value );
    }

    void            StoreFloat( rModeDescType rMode, float value )
    {
        GetStoredFloat( rMode ).Set( value );
    }

    void            StoreInt( rModeDescType rMode, int value )
    {
        GetStoredInt( rMode ).Set( value );
    }

    void            ResetProperty( rModeDescType rMode )
    {
        rModeStorage *store = GetPropertyStorage( rMode );

        if ( store )
        {
            store->boolValue.Reset();
            store->floatValue.Reset();
            store->intValue.Reset();
        }
    }

    template <typename renderModeManager>
    void            ApplyStates( renderModeManager& manager )
    {
        for ( rModeProperties_t::iterator iter = properties.begin(); iter != properties.end(); iter++ )
        {
            rModeStorage& store = *iter;

            bool hasApplied = false;

            {
                eRenderModeValueType valueType = g_pGame->GetPreferedEntityRenderModeType( store.modeType );

                if ( valueType == RMODE_BOOLEAN )
                {
                    if ( store.boolValue.isApplied )
                    {
                        hasApplied = manager.Set( store.modeType, store.boolValue ).successful;
                    }
                }
                else if ( valueType == RMODE_FLOAT )
                {
                    if ( store.floatValue.isApplied )
                    {
                        hasApplied = manager.Set( store.modeType, store.floatValue ).successful;
                    }
                }
                else if ( valueType == RMODE_FLOAT )
                {
                    if ( store.intValue.isApplied )
                    {
                        hasApplied = manager.Set( store.modeType, store.intValue ).successful;
                    }
                }
            }

            if ( !hasApplied )
            {
                if ( store.boolValue.isApplied )    manager.Set( store.modeType, store.boolValue );
                if ( store.floatValue.isApplied )   manager.Set( store.modeType, store.floatValue );
                if ( store.intValue.isApplied )     manager.Set( store.modeType, store.intValue );
            }
        }
    }

    template <typename renderModeManager>
    void            UpdateStates( renderModeManager& manager, unsigned int numModes )
    {
        for ( unsigned int n = 0; n < numModes; n++ )
        {
            rModeDescType modeType = (rModeDescType)n;

            eRenderModeValueType valueType = g_pGame->GetPreferedEntityRenderModeType( modeType );

            // Reset previous property values.
            ResetProperty( modeType );

            if ( valueType == RMODE_VOID || valueType == RMODE_BOOLEAN )
            {
                bool boolValue;
                rModeResult result = manager.Get( modeType, boolValue );

                if ( result.successful )
                    StoreBool( modeType, boolValue );
            }
            if ( valueType == RMODE_VOID || valueType == RMODE_FLOAT )
            {
                float floatValue;
                rModeResult result = manager.Get( modeType, floatValue );

                if ( result.successful )
                    StoreFloat( modeType, floatValue );
            }
            if ( valueType == RMODE_VOID || valueType == RMODE_INT )
            {
                int intValue;
                rModeResult result = manager.Get( modeType, intValue );

                if ( result.successful )
                    StoreInt( modeType, intValue );
            }
        }
    }
};

template <typename rModeManager>
struct clientRenderModes_t
{
    // Well, this is a weird coding line, but it works!
    typedef typename rModeManager::rModeDescType rModeDescType;

    clientRenderModes_t( rModeManager& manager ) : m_manager( manager )
    {
        
    }

    ~clientRenderModes_t( void )
    {

    }

    // Set functions.
    rModeResult     SetBool( rModeDescType rMode, bool value )
    {
        rModeResult result = m_manager.Validate( rMode, value );

        if ( result.successful )
        {
            result = m_manager.Set( rMode, value );
        }

        return result;
    }

    rModeResult     SetFloat( rModeDescType rMode, float value )
    {
        rModeResult result = m_manager.Validate( rMode, value );

        if ( result.successful )
        {
            result = m_manager.Set( rMode, value );
        }

        return result;
    }

    rModeResult     SetInt( rModeDescType rMode, int value )
    {
        rModeResult result = m_manager.Validate( rMode, value );

        if ( result.successful )
        {
            result = m_manager.Set( rMode, value );
        }

        return result;
    }

    // Get functions.
    rModeResult     GetBool( rModeDescType rMode, bool& value ) const
    {
        return m_manager.Get( rMode, value );
    }

    rModeResult     GetFloat( rModeDescType rMode, float& value ) const
    {
        return m_manager.Get( rMode, value );
    }

    rModeResult     GetInt( rModeDescType rMode, int& value ) const
    {
        return m_manager.Get( rMode, value );
    }

    // Function to reset a property.
    rModeResult     Reset( rModeDescType rMode )
    {
        return m_manager.Reset( rMode );
    }

    rModeManager& m_manager;
};

#endif //_CLIENT_RENDER_MODES_SYSTEM_