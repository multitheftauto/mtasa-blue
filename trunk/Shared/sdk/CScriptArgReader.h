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
            return true;
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
            return true;
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
                return true;
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
            return true;
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
            outValue = (T*)UserDataCast < T > ( (T*)0, lua_touserdata ( m_luaVM, m_iIndex++ ) );
            if ( outValue || bArgCanBeNil )
                return true;

            outValue = NULL;
            SetTypeError ( GetClassTypeName ( (T*)0 ), m_iIndex - 1 );
            return false;
        }
        else
        if ( iArgument == LUA_TNONE || m_bIgnoreMismatchMatch )
        {
            if ( defaultValue != (T*)-1 )
            {
                outValue = defaultValue;
                if ( outValue || bDefaultCanBeNil )
                    return true;
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
    // Read next wrapped userdata, using default if needed
    //
    template < class T, class U >
    bool ReadUserData ( U*& outValue, U* defaultValue, bool bArgCanBeNil = false, bool bDefaultCanBeNil = false )
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
    // Read next wrapped userdata, using NULL default or no default
    //
    template < class T, class U >
    bool ReadUserData ( U*& outValue, int defaultValue = -1 )
    {
        return ReadUserData < T > ( outValue, (U*)defaultValue, defaultValue == NULL, true );
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
    bool ReadLuaArgument ( CLuaArgument*& outValue )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex );
        if ( iArgument != LUA_TNONE )
        {
            outValue = new CLuaArgument();
            outValue->Read ( m_luaVM, m_iIndex++ );
            return true;
        }

        outValue = NULL;
        SetTypeError ( "argument" );
        m_iIndex++;
        return false;
    }


    //
    // Peek at next type
    //
    bool NextIs ( int iArgument ) const  { return iArgument == lua_type ( m_luaVM, m_iIndex ); }
    bool NextIsNone         ( void ) const  { return NextIs ( LUA_TNONE ); }
    bool NextIsNil          ( void ) const  { return NextIs ( LUA_TNIL ); }
    bool NextIsBool         ( void ) const  { return NextIs ( LUA_TBOOLEAN ); }
    bool NextIsUserData     ( void ) const  { return NextIs ( LUA_TLIGHTUSERDATA ); }
    bool NextIsNumber       ( void ) const  { return NextIs ( LUA_TNUMBER ); }
    bool NextIsString       ( void ) const  { return NextIs ( LUA_TSTRING ); }
    bool NextIsTable        ( void ) const  { return NextIs ( LUA_TTABLE ); }
    bool NextIsFunction     ( void ) const  { return NextIs ( LUA_TFUNCTION ); }
    bool NextCouldBeNumber  ( void ) const  { return NextIsNumber () || NextIsString (); }
    bool NextCouldBeString  ( void ) const  { return NextIsNumber () || NextIsString (); }


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
};
