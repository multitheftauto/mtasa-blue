/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:
*  PURPOSE:
*  DEVELOPERS:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/


///////////////////////////////////////////////////////////////
//
// CSourceLine
//
// Tokenized line
//
///////////////////////////////////////////////////////////////
class CSourceLine
{
    std::vector < SString > m_TokenList;
public:
    CSourceLine ( const SString& strText )
    {
        SetFromString ( strText );
    }
    void SetFromString ( const SString& strText )
    {
        strText.Replace ( "\t", " " ).Replace ( "  ", " ", true ).TrimEnd ( " " ).TrimStart ( " " ).Split ( " ", m_TokenList );
    }
    const SString& GetToken ( uint uiIndex ) const
    {
        if ( uiIndex < m_TokenList.size () )
            return m_TokenList [ uiIndex ];
        static const SString strDummy;
        return strDummy;
    }
    SString GetTextAfterToken ( int iIndex ) const
    {
        SString strResult;
        for ( uint i = iIndex + 1 ; i < m_TokenList.size () ; i++ )
            strResult += m_TokenList[i] + " ";
        return strResult;
    }
};


///////////////////////////////////////////////////////////////
//
// CBadLang
//
// Simple interpreter based call sequencer.
// Can save and restore state using a string.
//
///////////////////////////////////////////////////////////////
template < class T >
class CBadLang
{
protected:
    typedef SString (T::*PFNVOIDVOID) ( void );

    std::map < SString, PFNVOIDVOID >   m_FunctionMap;
    std::map < SString, SString >       m_VariableMap;
    std::vector < SString >             m_LineList;
    uint                                m_uiCurrLine;
    T*                                  m_pOuter;

public:
    CBadLang ( void )
    {
        m_uiCurrLine = 0;
        m_pOuter = 0;
    }

    // Init
    void SetSource ( T* pOuter, const SString& strSource )
    {
        m_pOuter = pOuter;
        strSource.Split ( "\n", m_LineList );
        for ( int i = m_LineList.size () - 1 ; i >= 0 ; i-- )
        {
            m_LineList[i] = m_LineList[i].Replace ( "\t", " " ).Replace ( "  ", " ", true ).TrimEnd ( " " ).TrimStart ( " " );
            if ( m_LineList[i].empty () )
                ListRemoveIndex ( m_LineList, i );
        }
        m_uiCurrLine = 0;
    }

    // Debug
    void Log ( const SString& strMessage )
    {
        AddReportLog ( 1071, strMessage );
    }

    // Functions
    void AddFunction ( const SString& strName, PFNVOIDVOID pfnCmdFunc )
    {
        MapSet ( m_FunctionMap, strName, pfnCmdFunc );
    }

    void CallFunction ( const SString& strName )
    {
        AddReportLog ( 1070, SString ( "CallFunction: %s", *strName ) );
        if ( PFNVOIDVOID* pfnCmdFunc = MapFind ( m_FunctionMap, strName ) )
            SetVariable ( "LastResult", ( m_pOuter->**pfnCmdFunc ) () );
        else
            Log ( SString ( "Unknown function: '%s'", *strName ) );
    }

    // Variables
    void SetVariable ( const SString& strName, const SString& strValue )
    {
        if ( strValue.empty () )
            MapRemove ( m_VariableMap, strName );
        else
            MapSet ( m_VariableMap, strName, strValue );
    }

    void SetVariable ( const SString& strName, int iValue )
    {
        SetVariable ( strName, SString ( "%d", iValue ) );
    }

    const SString& GetVariable ( const SString& strName )
    {
        if ( SString* pstrValue = MapFind ( m_VariableMap, strName ) )
            return *pstrValue;
        Log ( SString ( "Unknown variable: '%s'", *strName ) );
        static const SString strDummy;
        return strDummy;
    }

    int GetVariableInt ( const SString& strName )
    {
        return atoi ( GetVariable ( strName ) );
    }

    bool CheckCondition ( const SString& strVariableName, const SString& strOperand, const SString& strValue )
    {
        int iResult = stricmp ( GetVariable ( strVariableName ), strValue );
        if ( strOperand == "==" )   return iResult == 0;
        if ( strOperand == "!=" )   return iResult != 0;
        Log ( SString ( "Unknown operator: '%s'", *strOperand ) );
        return false;
    }

    // Serializationish
    SString SaveStateToString ( void )
    {
        // Save PC - Count valid lines between current line and previous label
        int iBackCount = 0;
        int i = 0;
        if ( !AtEnd () )
            for ( i = m_uiCurrLine ; i >= 0 ; i-- )
                if ( IsLineValid ( m_LineList[i] ) && ++iBackCount )
                    if ( IsLineLabel ( m_LineList[i] ) )
                        break;
        SetVariable ( "_pc_label", m_LineList[i] );
        SetVariable ( "_pc_offset", iBackCount - 1 );

        // Save variables
        CArgMap argMap ( "=", "&", " " );
        for ( std::map < SString, SString >::iterator iter = m_VariableMap.begin () ; iter != m_VariableMap.end () ; ++iter )
            argMap.Set ( iter->first, iter->second );

        return argMap.ToString ();
    }

    void RestoreStateFromString ( const SString& strText )
    {
        CArgMap argMap ( "=", "&", " " );
        argMap.SetFromString ( strText );

        // Restore variables
        std::vector < SString > keys;
        argMap.GetKeys ( keys );
        for ( uint i = 0 ; i < keys.size () ; i++ )
            SetVariable ( keys[i], argMap.Get ( keys[i] ) );

        // Restore PC
        SString strLabel = GetVariable ( "_pc_label" );
        int iOffset = GetVariableInt ( "_pc_offset" );
        m_uiCurrLine = m_LineList.size ();
        for ( uint i = 0 ; i < m_LineList.size (); i++ )            // Find label
            if ( m_LineList[i].BeginsWith ( strLabel ) )
                for ( ; i < m_LineList.size (); i++ )               // Find offset
                    if ( IsLineValid ( m_LineList[i] ) && !iOffset-- )
                    {
                        m_uiCurrLine = i;
                        return;
                    }
    }

    // Status
    bool IsLineValid ( const SString& strLine )
    {
        return !CSourceLine ( strLine ).GetToken ( 0 ).empty ();
    }

    bool IsLineLabel ( const SString& strLine )
    {
        return CSourceLine ( strLine ).GetToken ( 0 ).EndsWith ( ":" );
    }

    bool AtEnd ( void )
    {
        return m_uiCurrLine >= m_LineList.size ();
    }

    // Process
    void ProcessNextLine ( void )
    {
        if ( !AtEnd () )
            ProcessCommand ( m_LineList [ m_uiCurrLine++ ] );
    }

    void ProcessCommand ( const SString& strLine )
    {
        CSourceLine line = strLine;
        if ( line.GetToken ( 0 ).CompareI ( "CALL" ) )
        {
            CallFunction ( line.GetToken ( 1 ) );
        }
        else
        if ( line.GetToken ( 0 ).CompareI ( "IF" ) )
        {
            if ( CheckCondition ( line.GetToken ( 1 ), line.GetToken ( 2 ), line.GetToken ( 3 ) ) )
                ProcessCommand ( line.GetTextAfterToken ( 3 ) );
        }
        else
        if ( line.GetToken ( 0 ).CompareI ( "GOTO" ) )
        {
            for ( uint i = 0 ; i < m_LineList.size (); i++ )
                if ( m_LineList[i].BeginsWith ( line.GetToken ( 1 ) ) )
                {
                    m_uiCurrLine = i;
                    break;
                }
        }
        else
        if ( !IsLineLabel (  strLine ) )
        {
            Log ( SString ( "Unknown command in: '%s'", *strLine ) );
        }
    }
};
