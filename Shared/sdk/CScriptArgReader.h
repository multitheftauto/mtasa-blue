/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        CScriptArgReader.h    
*  PURPOSE:
*  DEVELOPERS:  Nobody knows
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/


/////////////////////////////////////////////////////////////////////////
//
// CScriptArgReader
//
//
// Attempt to simplify the reading of arguments from a script call
//
//////////////////////////////////////////////////////////////////////
class CScriptArgReader
{
public:
    CScriptArgReader ( lua_State* luaVM )
    {
        m_luaVM = luaVM;
        m_iIndex = 1;
        m_iErrorIndex = 0;
        m_bError = false;
        m_bIgnoreMismatchMatch = false;
        m_pPendingFunctionOutValue = NULL;
        m_pPendingFunctionIndex = -1;
    }

    CScriptArgReader ( void )
    {
        assert ( !IsReadFunctionPending () );
    }

    //
    // Read next number
    //
    template < class T >
    bool ReadNumber ( T& outValue )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex );
        if ( iArgument == LUA_TNUMBER || iArgument == LUA_TSTRING )
        {
            outValue = static_cast < T > ( lua_tonumber ( m_luaVM, m_iIndex++ ) );
            return true;
        }

        outValue = 0;
        SetTypeError ( "number" );
        m_iIndex++;
        return false;
    }

    //
    // Read next number, using default if needed
    //
    template < class T, class U >
    bool ReadNumber ( T& outValue, const U& defaultValue )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex );
        if ( iArgument == LUA_TNUMBER || iArgument == LUA_TSTRING )
        {
            outValue = static_cast < T > ( lua_tonumber ( m_luaVM, m_iIndex++ ) );
            return true;
        }
        else
        if ( iArgument == LUA_TNONE || m_bIgnoreMismatchMatch )
        {
            outValue = static_cast < T > ( defaultValue );
            return false;
        }

        outValue = 0;
        SetTypeError ( "number" );
        m_iIndex++;
        return false;
    }



    //
    // Read next bool
    //
    bool ReadBool ( bool& bOutValue )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex );
        if ( iArgument == LUA_TBOOLEAN )
        {
            bOutValue = lua_toboolean ( m_luaVM, m_iIndex++ ) ? true : false;
            return true;
        }

        bOutValue = false;
        SetTypeError ( "bool" );
        m_iIndex++;
        return false;
    }

    //
    // Read next bool, using default if needed
    //
    bool ReadBool ( bool& bOutValue, const bool bDefaultValue )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex );
        if ( iArgument == LUA_TBOOLEAN )
        {
            bOutValue = lua_toboolean ( m_luaVM, m_iIndex++ ) ? true : false;
            return true;
        }
        else
        if ( iArgument == LUA_TNONE || m_bIgnoreMismatchMatch )
        {
            bOutValue = bDefaultValue;
            return false;
        }

        bOutValue = false;
        SetTypeError ( "bool" );
        m_iIndex++;
        return false;
    }


    //
    // Read next string, using default if needed
    //
    bool ReadString ( SString& outValue, const char* defaultValue = NULL )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex );
        if ( iArgument == LUA_TSTRING || iArgument == LUA_TNUMBER )
        {
            outValue = lua_tostring ( m_luaVM, m_iIndex++ );
            return true;
        }
        else
        if ( iArgument == LUA_TNONE || m_bIgnoreMismatchMatch )
        {
            if ( defaultValue )
            {
                outValue = defaultValue;              
                return false;
            }
        }

        outValue = "";
        SetTypeError ( "string" );
        m_iIndex++;
        return false;
    }


    //
    // Read next string as an enum
    //
    template < class T >
    bool ReadEnumString ( T& outValue )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex );
        if ( iArgument == LUA_TSTRING )
        {
            SString strValue = lua_tostring ( m_luaVM, m_iIndex );
            if ( StringToEnum ( strValue, outValue ) )
            {
                m_iIndex++;
                return true;
            }
        }

        outValue = (T)0;
        SetTypeError ( GetEnumTypeName ( outValue ) );
        m_iIndex++;
        return false;
    }


    //
    // Read next string as an enum, using default if needed
    //
    template < class T >
    bool ReadEnumString ( T& outValue, const T& defaultValue )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex );
        if ( iArgument == LUA_TSTRING )
        {
            SString strValue = lua_tostring ( m_luaVM, m_iIndex );
            if ( StringToEnum ( strValue, outValue ) )
            {
                m_iIndex++;
                return true;
            }
        }
        else
        if ( iArgument == LUA_TNONE || m_bIgnoreMismatchMatch )
        {
            outValue = defaultValue;
            return false;
        }

        outValue = (T)0;
        SetTypeError ( GetEnumTypeName ( outValue ) );
        m_iIndex++;
        return false;
    }


    //
    // Read next userdata, using default if needed
    //
    template < class T >
    bool ReadUserData ( T*& outValue, T* defaultValue, bool bArgCanBeNil = false, bool bDefaultCanBeNil = false )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex );

        if ( iArgument == LUA_TLIGHTUSERDATA )
        {
            outValue = (T*)UserDataCast < T > ( (T*)0, lua_touserdata ( m_luaVM, m_iIndex++ ), m_luaVM );
            if ( outValue || bArgCanBeNil )
                return true;

            outValue = NULL;
            SetTypeError ( GetClassTypeName ( (T*)0 ), m_iIndex - 1 );
            return false;
        }
        else
        if ( iArgument == LUA_TNONE || m_bIgnoreMismatchMatch || ( iArgument == LUA_TNIL && bArgCanBeNil ) )
        {
            if ( defaultValue != (T*)-1 )
            {
                outValue = defaultValue;
                if ( outValue || bDefaultCanBeNil )
                    return false;
            }
        }

        outValue = NULL;
        SetTypeError ( GetClassTypeName ( (T*)0 ) );
        m_iIndex++;
        return false;
    }


    //
    // Read next userdata, using NULL default or no default
    //
    template < class T >
    bool ReadUserData ( T*& outValue, int defaultValue = -1 )
    {
        return ReadUserData ( outValue, (T*)defaultValue, defaultValue == NULL, true );
    }


    //
    // Read next wrapped userdata
    //
    template < class T, class U >
    bool ReadUserData ( U*& outValue )
    {
        if ( ReadUserData ( outValue ) )
        {
            SString strErrorExpectedType;
            if ( CheckWrappedUserDataType < T > ( outValue, strErrorExpectedType ) )
                return true;
            SetTypeError ( strErrorExpectedType, m_iIndex - 1 );
        }
        return false;
    }


    //
    // Read CLuaArguments
    //
    bool ReadLuaArguments ( CLuaArguments& outValue )
    {
        outValue.ReadArguments ( m_luaVM, m_iIndex );
        for ( int i = outValue.Count () ; i > 0 ; i-- )
        {
            m_iIndex++;
        }
        return true;
    }


    //
    // Read one CLuaArgument
    //
    bool ReadLuaArgument ( CLuaArgument& outValue )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex );
        if ( iArgument != LUA_TNONE )
        {
            outValue.Read ( m_luaVM, m_iIndex++ );
            return true;
        }

        SetTypeError ( "argument" );
        m_iIndex++;
        return false;
    }


    //
    // Read a function, but don't do it yet due to Lua statck issues
    //
    bool ReadFunction ( CLuaFunctionRef& outValue, int defaultValue = -2 )
    {
        assert ( !m_pPendingFunctionOutValue );

        int iArgument = lua_type ( m_luaVM, m_iIndex );
        if ( iArgument == LUA_TFUNCTION )
        {
            m_pPendingFunctionOutValue = &outValue;
            m_pPendingFunctionIndex = m_iIndex++;
            return true;
        }
        else
        if ( defaultValue == LUA_REFNIL )
        {
            outValue = CLuaFunctionRef ();
            return true;
        }

        SetTypeError ( "function", m_iIndex );
        m_iIndex++;
        return false;
    }

    //
    // Call after other arguments have been read
    //
    bool ReadFunctionComplete ( void )
    {
        if ( !m_pPendingFunctionOutValue )
            return true;

        assert ( m_pPendingFunctionIndex != -1 );
        *m_pPendingFunctionOutValue = luaM_toref ( m_luaVM, m_pPendingFunctionIndex );
        if ( VERIFY_FUNCTION( *m_pPendingFunctionOutValue ) )
        {
            m_pPendingFunctionIndex = -1;
            return true;
        }

        SetTypeError ( "function", m_pPendingFunctionIndex );
        m_pPendingFunctionIndex = -1;
        return false;
    }

    // Debug check
    bool IsReadFunctionPending ( void ) const
    {
        return m_pPendingFunctionOutValue && m_pPendingFunctionIndex != -1;
    }


    //
    // Peek at next type
    //
    bool NextIs ( int iArgument, int iOffset = 0 ) const  { return iArgument == lua_type ( m_luaVM, m_iIndex + iOffset ); }
    bool NextIsNone         ( int iOffset = 0 ) const  { return NextIs ( LUA_TNONE, iOffset ); }
    bool NextIsNil          ( int iOffset = 0 ) const  { return NextIs ( LUA_TNIL, iOffset ); }
    bool NextIsBool         ( int iOffset = 0 ) const  { return NextIs ( LUA_TBOOLEAN, iOffset ); }
    bool NextIsUserData     ( int iOffset = 0 ) const  { return NextIs ( LUA_TLIGHTUSERDATA, iOffset ); }
    bool NextIsNumber       ( int iOffset = 0 ) const  { return NextIs ( LUA_TNUMBER, iOffset ); }
    bool NextIsString       ( int iOffset = 0 ) const  { return NextIs ( LUA_TSTRING, iOffset ); }
    bool NextIsTable        ( int iOffset = 0 ) const  { return NextIs ( LUA_TTABLE, iOffset ); }
    bool NextIsFunction     ( int iOffset = 0 ) const  { return NextIs ( LUA_TFUNCTION, iOffset ); }
    bool NextCouldBeNumber  ( int iOffset = 0 ) const  { return NextIsNumber ( iOffset ) || NextIsString ( iOffset ); }
    bool NextCouldBeString  ( int iOffset = 0 ) const  { return NextIsNumber ( iOffset ) || NextIsString ( iOffset ); }

    template < class T >
    bool NextIsEnumString ( T&, int iOffset = 0 )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex + iOffset );
        if ( iArgument == LUA_TSTRING )
        {
            T eDummyResult;
            SString strValue = lua_tostring ( m_luaVM, m_iIndex + iOffset );
            return StringToEnum ( strValue, eDummyResult );
        }
        return false;
    }

    //
    // SetTypeError
    //
    void SetTypeError ( const SString& strExpectedType, int iIndex = -1 )
    {
        if ( iIndex == - 1 )
            iIndex = m_iIndex;
        if ( !m_bError || iIndex <= m_iErrorIndex )
        {
            m_bError = true;
            m_iErrorIndex = iIndex;
            m_strErrorExpectedType = strExpectedType;
        }
    }

    //
    // HasErrors - Optional check if there are any unread arguments
    //
    bool HasErrors ( bool bCheckUnusedArgs = false ) const
    {
        assert ( !IsReadFunctionPending () );
        if ( bCheckUnusedArgs && lua_type ( m_luaVM, m_iIndex ) != LUA_TNONE )
            return true;
        return m_bError;
    }

    //
    // GetErrorMessage
    //
    SString GetErrorMessage ( void ) const
    {
        if ( !m_bError )
            return "No error";

        int             iGotArgumentType    = lua_type ( m_luaVM, m_iErrorIndex );
        const SString&  strGotArgumentType  = EnumToString ( (eLuaType)iGotArgumentType );
        const char *    szGotArgumentValue  = lua_tostring ( m_luaVM, m_iErrorIndex );

        // Compose error message
        SString strMessage ( "Expected %s at argument %d", *m_strErrorExpectedType, m_iErrorIndex );

        // Avoid things like 'Expected element, got userdata'
        if ( iGotArgumentType != LUA_TLIGHTUSERDATA )
        {
            strMessage += SString ( ", got %s", *strGotArgumentType );

            // Append value if available
            if ( szGotArgumentValue && szGotArgumentValue[0] )
                strMessage += SString ( " '%s'", szGotArgumentValue );
        }

        return strMessage;
    }

    //
    // Strict off means mismatches are ignored if they have a default value
    //
    void SetStrict ( bool bStrictMode )
    {
        m_bIgnoreMismatchMatch = !bStrictMode;
    }


    bool                    m_bIgnoreMismatchMatch;
    bool                    m_bError;
    int                     m_iErrorIndex;
    SString                 m_strErrorExpectedType;
    int                     m_iIndex;
    lua_State*              m_luaVM;
    CLuaFunctionRef*        m_pPendingFunctionOutValue;
    int                     m_pPendingFunctionIndex;
};
