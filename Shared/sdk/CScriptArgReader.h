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
        m_pPendingFunctionOutValue = NULL;
        m_pPendingFunctionIndex = -1;
        m_bResolvedErrorGotArgumentTypeAndValue = false;
        m_iErrorGotArgumentType = 0;
        m_bHasCustomMessage = false;
    }

    ~CScriptArgReader ( void )
    {
        assert ( !IsReadFunctionPending () );
    }

    //
    // Read next number
    //
    template < class T >
    void ReadNumber ( T& outValue )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex );
        if ( iArgument == LUA_TNUMBER || iArgument == LUA_TSTRING )
        {
            outValue = static_cast < T > ( lua_tonumber ( m_luaVM, m_iIndex++ ) );
            return;
        }

        outValue = 0;
        SetTypeError ( "number" );
        m_iIndex++;
    }

    //
    // Read next number, using default if needed
    //
    template < class T, class U >
    void ReadNumber ( T& outValue, const U& defaultValue )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex );
        if ( iArgument == LUA_TNUMBER || iArgument == LUA_TSTRING )
        {
            outValue = static_cast < T > ( lua_tonumber ( m_luaVM, m_iIndex++ ) );
            return;
        }
        else
        if ( iArgument == LUA_TNONE || iArgument == LUA_TNIL )
        {
            outValue = static_cast < T > ( defaultValue );
            m_iIndex++;
            return;
        }

        outValue = 0;
        SetTypeError ( "number" );
        m_iIndex++;
    }


    //
    // Read next bool
    //
    void ReadBool ( bool& bOutValue )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex );
        if ( iArgument == LUA_TBOOLEAN )
        {
            bOutValue = lua_toboolean ( m_luaVM, m_iIndex++ ) ? true : false;
            return;
        }

        bOutValue = false;
        SetTypeError ( "bool" );
        m_iIndex++;
    }

    //
    // Read next bool, using default if needed
    //
    void ReadBool ( bool& bOutValue, const bool bDefaultValue )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex );
        if ( iArgument == LUA_TBOOLEAN )
        {
            bOutValue = lua_toboolean ( m_luaVM, m_iIndex++ ) ? true : false;
            return;
        }
        else
        if ( iArgument == LUA_TNONE || iArgument == LUA_TNIL )
        {
            bOutValue = bDefaultValue;
            m_iIndex++;
            return;
        }

        bOutValue = false;
        SetTypeError ( "bool" );
        m_iIndex++;
    }


    //
    // Read next string, using default if needed
    //
    void ReadString ( SString& outValue, const char* defaultValue = NULL )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex );
        if ( iArgument == LUA_TSTRING || iArgument == LUA_TNUMBER )
        {
            outValue = lua_tostring ( m_luaVM, m_iIndex++ );
            return;
        }
        else
        if ( iArgument == LUA_TNONE || iArgument == LUA_TNIL )
        {
            if ( defaultValue )
            {
                outValue = defaultValue;              
                m_iIndex++;
                return;
            }
        }

        outValue = "";
        SetTypeError ( "string" );
        m_iIndex++;
    }


    //
    // Read next string as a string reference
    //
    void ReadCharStringRef ( SCharStringRef& outValue )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex );
        if ( iArgument == LUA_TSTRING )
        {
            outValue.pData = (char*)lua_tolstring ( m_luaVM, m_iIndex++, &outValue.uiSize );
            return;
        }

        outValue.pData = NULL;
        outValue.uiSize = 0;
        SetTypeError ( "string" );
        m_iIndex++;
    }


    //
    // Read next string as an enum
    //
    template < class T >
    void ReadEnumString ( T& outValue )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex );
        if ( iArgument == LUA_TSTRING )
        {
            SString strValue = lua_tostring ( m_luaVM, m_iIndex );
            if ( StringToEnum ( strValue, outValue ) )
            {
                m_iIndex++;
                return;
            }
        }

        outValue = (T)0;
        SetTypeError ( GetEnumTypeName ( outValue ) );
        m_iIndex++;
    }


    //
    // Read next string as an enum, using default if needed
    //
    template < class T >
    void ReadEnumString ( T& outValue, const T& defaultValue )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex );
        if ( iArgument == LUA_TSTRING )
        {
            SString strValue = lua_tostring ( m_luaVM, m_iIndex );
            if ( StringToEnum ( strValue, outValue ) )
            {
                m_iIndex++;
                return;
            }
        }
        else
        if ( iArgument == LUA_TNONE || iArgument == LUA_TNIL )
        {
            outValue = defaultValue;
            m_iIndex++;
            return;
        }

        outValue = (T)0;
        SetTypeError ( GetEnumTypeName ( outValue ) );
        m_iIndex++;
    }


    //
    // Read next string as a comma separated list of enums, using default if needed
    //
    template < class T >
    void ReadEnumStringList ( std::vector < T >& outValueList, const SString& strDefaultValue )
    {
        outValueList.clear ();
        int iArgument = lua_type ( m_luaVM, m_iIndex );
        SString strValue;
        if ( iArgument == LUA_TSTRING )
        {
            strValue = lua_tostring ( m_luaVM, m_iIndex );
        }
        else
        if ( iArgument == LUA_TNONE || iArgument == LUA_TNIL )
        {
            strValue = strDefaultValue;
        }
        else
        {
            T outValue;
            SetTypeError ( GetEnumTypeName ( outValue ) );
            m_iIndex++;
            return;
        }

        // Parse each part of the string
        std::vector < SString > inValueList;
        strValue.Split ( ",", inValueList );
        for ( uint i = 0 ; i < inValueList.size () ; i++ )
        {
            T outValue;
            if ( StringToEnum ( inValueList[i], outValue ) )
            {
                outValueList.push_back ( outValue );
            }
            else
            {
                // Error
                SetTypeError ( GetEnumTypeName ( outValue ) );
                m_bResolvedErrorGotArgumentTypeAndValue = true;
                m_iErrorGotArgumentType = lua_type ( m_luaVM, m_iErrorIndex );
                m_strErrorGotArgumentValue = inValueList[i];

                if ( iArgument == LUA_TSTRING )
                    m_iIndex++;
                return;
            }
        }

        // Success
        if ( iArgument == LUA_TSTRING )
            m_iIndex++;
    }


    //
    // Read next string or number as an enum
    //
    template < class T >
    void ReadEnumStringOrNumber ( T& outValue )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex );
        if ( iArgument == LUA_TSTRING )
        {
            SString strValue = lua_tostring ( m_luaVM, m_iIndex );
            if ( StringToEnum ( strValue, outValue ) )
            {
                m_iIndex++;
                return;
            }

            // If will be coercing a string to an enum, make sure string contains only digits
            uint uiPos = strValue.find_first_not_of ( "0123456789" );
            if ( uiPos != SString::npos || strValue.empty () )
                iArgument = LUA_TNONE;  //  Force error
        }

        if ( iArgument == LUA_TSTRING || iArgument == LUA_TNUMBER )
        {
            outValue = static_cast < T > ( (int)lua_tonumber ( m_luaVM, m_iIndex ) );
            if ( EnumValueValid ( outValue ) )
            {
                m_iIndex++;
                return;
            }
        }

        outValue = (T)0;
        SetTypeError ( GetEnumTypeName ( outValue ) );
        m_iIndex++;
    }


protected:
    //
    // Read next userdata, using rules and stuff
    //
    template < class T >
    void InternalReadUserData ( bool bAllowNilResult, T*& outValue, bool bHasDefaultValue, T* defaultValue = (T*)-2 )
    {
        outValue = NULL;
        int iArgument = lua_type ( m_luaVM, m_iIndex );

        if ( iArgument == LUA_TLIGHTUSERDATA )
        {
            outValue = (T*)UserDataCast < T > ( (T*)0, lua_touserdata ( m_luaVM, m_iIndex ), m_luaVM );
            if ( outValue )
            {
                m_iIndex++;
                return;
            }
        }
        else if ( iArgument == LUA_TUSERDATA )
        {
            outValue = (T*)UserDataCast < T > ( (T*)0, * ( ( void** ) lua_touserdata ( m_luaVM, m_iIndex ) ), m_luaVM );
            if ( outValue )
            {
                m_iIndex++;
                return;
            }
        }
        else
        if ( iArgument == LUA_TNONE || iArgument == LUA_TNIL )
        {
            if ( bHasDefaultValue )
                outValue = defaultValue;
            else
                outValue = NULL;

            if ( outValue || bAllowNilResult )
            {
                m_iIndex++;
                return;
            }
        }

        outValue = NULL;
        SetTypeError ( GetClassTypeName ( (T*)0 ) );
        m_iIndex++;
    }
public:

    //
    // Read next userdata
    // Userdata always valid if no error
    //  * value not userdata - error
    //  * nil value          - error
    //  * no arguments left  - error
    //  * result is NULL     - error
    //
    template < class T >
    void ReadUserData ( T*& outValue )
    {
        InternalReadUserData ( false, outValue, false );
    }

    //
    // Read next userdata, using default if needed
    // Userdata always valid if no error
    //  * value not userdata - error
    //  * nil value          - use defaultValue
    //  * no arguments left  - use defaultValue
    //  * result is NULL     - error
    //
    template < class T >
    void ReadUserData ( T*& outValue, T* defaultValue )
    {
        InternalReadUserData ( false, outValue, true, defaultValue );
    }

    //
    // Read next userdata, using NULL default if needed, allowing NULL result
    // Userdata might be NULL even when no error
    //  * value not userdata - error
    //  * nil value          - use NULL
    //  * no arguments left  - use NULL
    //
    template < class T >
    void ReadUserData ( T*& outValue, int*** defaultValue )
    {
        assert( defaultValue == NULL );
        InternalReadUserData ( true, outValue, true, (T*)NULL );
    }


    //
    // Read next wrapped userdata
    //
    template < class T, class U >
    void ReadUserData ( U*& outValue )
    {
        ReadUserData ( outValue );
        if ( outValue )
        {
            SString strErrorExpectedType;
            if ( CheckWrappedUserDataType < T > ( outValue, strErrorExpectedType ) )
                return;
            SetTypeError ( strErrorExpectedType, m_iIndex - 1 );
        }
    }


    //
    // Read CLuaArguments
    //
    void ReadLuaArguments ( CLuaArguments& outValue )
    {
        outValue.ReadArguments ( m_luaVM, m_iIndex );
        for ( int i = outValue.Count () ; i > 0 ; i-- )
        {
            m_iIndex++;
        }
    }


    //
    // Read one CLuaArgument
    //
    void ReadLuaArgument ( CLuaArgument& outValue )
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex );
        if ( iArgument != LUA_TNONE )
        {
            outValue.Read ( m_luaVM, m_iIndex++ );
            return;
        }

        SetTypeError ( "argument" );
        m_iIndex++;
    }


    //
    // Read a table of userdatas
    //
    template < class T >
    void ReadUserDataTable ( std::vector < T* >& outList )
    {
        if ( lua_type ( m_luaVM, m_iIndex ) != LUA_TTABLE )
        {
            SetTypeError ( "table" );
            m_iIndex++;
            return;
        }

        for ( lua_pushnil ( m_luaVM ) ; lua_next ( m_luaVM, m_iIndex ) != 0 ; lua_pop ( m_luaVM, 1 ) )
        {
            //int idx = lua_tonumber ( m_luaVM, -2 );
            int iArgumentType = lua_type ( m_luaVM, -1 );

            T* value = NULL;
            if ( iArgumentType == LUA_TLIGHTUSERDATA )
            {
                value = (T*)UserDataCast < T > ( (T*)0, lua_touserdata ( m_luaVM, -1 ), m_luaVM );
            }
            else if ( iArgumentType == LUA_TUSERDATA )
            {
                value = (T*)UserDataCast < T > ( (T*)0, * ( ( void** ) lua_touserdata ( m_luaVM, -1 ) ), m_luaVM );
            }

            if ( value != NULL )
                outList.push_back ( value );
        }
        m_iIndex++;
    }


    //
    // Read a function, but don't do it yet due to Lua stack issues
    //
    void ReadFunction ( CLuaFunctionRef& outValue, int defaultValue = -2 )
    {
        assert ( !m_pPendingFunctionOutValue );

        int iArgument = lua_type ( m_luaVM, m_iIndex );
        if ( iArgument == LUA_TFUNCTION )
        {
            m_pPendingFunctionOutValue = &outValue;
            m_pPendingFunctionIndex = m_iIndex++;
            return;
        }
        else if ( iArgument == LUA_TNONE || iArgument == LUA_TNIL )
        {
            // Only valid default value for function is nil
            if ( defaultValue == LUA_REFNIL )
            {
                outValue = CLuaFunctionRef ();
                m_iIndex++;
                return;
            }
        }

        SetTypeError ( "function", m_iIndex );
        m_iIndex++;
    }

    //
    // Call after other arguments have been read
    //
    void ReadFunctionComplete ( void )
    {
        if ( !m_pPendingFunctionOutValue )
            return;

        // As we are going to change the stack, save any error info already gotten
        ResolveErrorGotArgumentTypeAndValue ();

        assert ( m_pPendingFunctionIndex != -1 );
        *m_pPendingFunctionOutValue = luaM_toref ( m_luaVM, m_pPendingFunctionIndex );
        if ( VERIFY_FUNCTION( *m_pPendingFunctionOutValue ) )
        {
            m_pPendingFunctionIndex = -1;
            return;
        }

        SetTypeError ( "function", m_pPendingFunctionIndex );
        m_pPendingFunctionIndex = -1;
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

    template < class T >
    bool NextIsUserDataOfType ( int iOffset = 0 ) const
    {
        int iArgument = lua_type ( m_luaVM, m_iIndex + iOffset );
        if ( iArgument == LUA_TLIGHTUSERDATA )
        {
            if ( UserDataCast < T > ( (T*)0, lua_touserdata ( m_luaVM, m_iIndex + iOffset ), m_luaVM ) )
                return true;
        }
        return false;
    }

    //
    // Conditional reads. Default required in case condition is not met.
    //
    void ReadIfNextIsBool ( bool& bOutValue, const bool bDefaultValue )
    {
        if ( NextIsBool () )
            ReadBool ( bOutValue, bDefaultValue );
        else
            bOutValue = bDefaultValue;
    }

    template < class T >
    void ReadIfNextIsUserData ( T*& outValue, T* defaultValue )
    {
        if ( NextIsUserData () )
            ReadUserData ( outValue, defaultValue );
        else
            outValue = defaultValue;
    }

    template < class T, class U >
    void ReadIfNextIsNumber ( T& outValue, const U& defaultValue )
    {
        if ( NextIsNumber () )
            ReadNumber ( outValue, defaultValue );
        else
            outValue = defaultValue;
    }

    void ReadIfNextIsString ( SString& outValue, const char* defaultValue )
    {
        if ( NextIsString () )
            ReadString ( outValue, defaultValue );
        else
            outValue = defaultValue;
    }

    template < class T, class U >
    void ReadIfNextCouldBeNumber ( T& outValue, const U& defaultValue )
    {
        if ( NextCouldBeNumber () )
            ReadNumber ( outValue, defaultValue );
        else
            outValue = defaultValue;
    }

    void ReadIfNextCouldBeString ( SString& outValue, const char* defaultValue )
    {
        if ( NextCouldBeString () )
            ReadString ( outValue, defaultValue );
        else
            outValue = defaultValue;
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
            m_bResolvedErrorGotArgumentTypeAndValue = false;
            m_strErrorCategory = "Bad argument";
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
    SString GetErrorMessage ( void )
    {
        if ( !m_bError )
            return "No error";

        if ( m_bHasCustomMessage )
            return m_strCustomMessage;

        ResolveErrorGotArgumentTypeAndValue ();
        SString strGotArgumentType  = EnumToString ( (eLuaType)m_iErrorGotArgumentType );
        SString strGotArgumentValue = m_strErrorGotArgumentValue;

        // Compose error message
        SString strMessage ( "Expected %s at argument %d", *m_strErrorExpectedType, m_iErrorIndex );

        if ( m_iErrorGotArgumentType == LUA_TLIGHTUSERDATA )
        {
	        // Get name of userdata type
            strGotArgumentType = GetUserDataClassName ( lua_touserdata ( m_luaVM, m_iErrorIndex ), m_luaVM );
            strGotArgumentValue = "";
        }

        if ( !strGotArgumentType.empty () )
        {
            strMessage += SString ( ", got %s", *strGotArgumentType );

            // Append value if available
            if ( !strGotArgumentValue.empty () )
                strMessage += SString ( " '%s'", *strGotArgumentValue );
        }

        return strMessage;
    }

    //
    // Put off getting error type and value until just before we need it
    //
    void ResolveErrorGotArgumentTypeAndValue ( void )
    {
        if ( !m_bError || m_bResolvedErrorGotArgumentTypeAndValue )
            return;

        m_bResolvedErrorGotArgumentTypeAndValue = true;

        if ( !m_bHasCustomMessage )
        {
            m_iErrorGotArgumentType = lua_type ( m_luaVM, m_iErrorIndex );
            m_strErrorGotArgumentValue = lua_tostring ( m_luaVM, m_iErrorIndex );
        }
    }

    //
    // Set version error message
    //
    void SetVersionError ( const char* szMinReq, const char* szSide, const char* szReason )
    {
        SetCustomError ( SString ( "<min_mta_version> section in the meta.xml is incorrect or missing (expected at least %s %s because %s)", szSide, szMinReq, szReason ) );
    }

    //
    // Set custom error message
    //
    void SetCustomError ( const char* szReason, const char* szCategory = "Bad usage" )
    {
        if ( !m_bError )
        {
            m_bError = true;
            m_strErrorCategory = szCategory;
            m_bHasCustomMessage = true;
            m_strCustomMessage = szReason;
        }
    }

    //
    // Make full error message
    //
    SString GetFullErrorMessage ( void )
    {
        return SString ( "%s @ '%s' [%s]", *m_strErrorCategory, lua_tostring ( m_luaVM, lua_upvalueindex ( 1 ) ), *GetErrorMessage () );
    }

    bool                    m_bError;
    int                     m_iErrorIndex;
    SString                 m_strErrorExpectedType;
    int                     m_iIndex;
    lua_State*              m_luaVM;
    CLuaFunctionRef*        m_pPendingFunctionOutValue;
    int                     m_pPendingFunctionIndex;
    bool                    m_bResolvedErrorGotArgumentTypeAndValue;
    int                     m_iErrorGotArgumentType;
    SString                 m_strErrorGotArgumentValue;
    SString                 m_strErrorCategory;
    bool                    m_bHasCustomMessage;
    SString                 m_strCustomMessage;

};
