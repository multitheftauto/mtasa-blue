/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Drawing.cpp
*  PURPOSE:     Lua function definitions class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Chris McArthur <>
*               Derek Abdine <>
*               Christian Myhre Lundheim <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*
*****************************************************************************/

#include "StdInc.h"

int CLuaFunctionDefs::dxDrawLine ( lua_State* luaVM )
{
    // dxDrawLine ( int x,int y,int x2,int y2,int color, [float width=1,bool postGUI=false] )
    // Grab all argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    int iArgument5 = lua_type ( luaVM, 5 );
    int iArgument6 = lua_type ( luaVM, 6 );
    int iArgument7 = lua_type ( luaVM, 7 );

    // Check required arguments. Should all be numbers.
    if ( ( iArgument1 != LUA_TNUMBER && iArgument1 != LUA_TSTRING ) ||
        ( iArgument2 != LUA_TNUMBER && iArgument2 != LUA_TSTRING ) || 
        ( iArgument3 != LUA_TNUMBER && iArgument3 != LUA_TSTRING ) ||
        ( iArgument4 != LUA_TNUMBER && iArgument4 != LUA_TSTRING ) ||
        ( iArgument5 != LUA_TNUMBER && iArgument5 != LUA_TSTRING ) )
    {
        m_pScriptDebugging->LogBadType ( luaVM, "dxDrawLine" );
        lua_pushboolean ( luaVM, false );
        return 1;
    }    

    // Got a line width too?
    float fWidth = 1.0f;
    if ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING )
    {
        fWidth = static_cast < float > ( lua_tonumber ( luaVM, 6 ) );
    }

    // Got a post gui specifier?
    bool bPostGUI = false;
    if ( iArgument7 == LUA_TBOOLEAN )
    {
        bPostGUI = ( lua_toboolean ( luaVM, 7 ) ) ? true:false;
    }

    // Grab the arguments
    float fX1 = static_cast < float > ( lua_tonumber ( luaVM, 1 ) );
    float fY1 = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
    float fX2 = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );
    float fY2 = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );
    unsigned long ulColor = static_cast < unsigned long > ( lua_tonumber ( luaVM, 5 ) );

    // Draw it
    g_pCore->GetGraphics ()->DrawLineQueued ( fX1, fY1, fX2, fY2,fWidth, ulColor, bPostGUI );

    // Success
    lua_pushboolean ( luaVM, true );
    return 1;
}


int CLuaFunctionDefs::dxDrawLine3D ( lua_State* luaVM )
{
    // dxDrawLine3D ( float x,float y,float z,float x2,float y2,float z2,int color, [float width,bool postGUI,float zBuffer] )

    // Grab all argument types
    int iArgument1 = lua_type ( luaVM, 1 ); // X1
    int iArgument2 = lua_type ( luaVM, 2 ); // Y1
    int iArgument3 = lua_type ( luaVM, 3 ); // Z1
    int iArgument4 = lua_type ( luaVM, 4 ); // X2
    int iArgument5 = lua_type ( luaVM, 5 ); // Y2
    int iArgument6 = lua_type ( luaVM, 6 ); // Z2
    int iArgument7 = lua_type ( luaVM, 7 ); // Color
    int iArgument8 = lua_type ( luaVM, 8 ); // Width
    int iArgument9 = lua_type ( luaVM, 9 ); // Reserved: Post GUI
    int iArgument10 = lua_type ( luaVM, 10 ); // Reserved: Z-buffer

    // Check required arguments. Should all be numbers.
    if ( ( iArgument1 != LUA_TNUMBER && iArgument1 != LUA_TSTRING ) ||
        ( iArgument2 != LUA_TNUMBER && iArgument2 != LUA_TSTRING ) || 
        ( iArgument3 != LUA_TNUMBER && iArgument3 != LUA_TSTRING ) ||
        ( iArgument4 != LUA_TNUMBER && iArgument4 != LUA_TSTRING ) ||
        ( iArgument5 != LUA_TNUMBER && iArgument5 != LUA_TSTRING ) ||
        ( iArgument6 != LUA_TNUMBER && iArgument6 != LUA_TSTRING ) ||
        ( iArgument7 != LUA_TNUMBER && iArgument7 != LUA_TSTRING ))
    {
        m_pScriptDebugging->LogBadType ( luaVM, "dxDrawLine3D" );
        lua_pushboolean ( luaVM, false );
        return 1;
    }

    // Got a line width too?
    float fWidth = 1.0f;
    if ( iArgument8 == LUA_TNUMBER || iArgument8 == LUA_TSTRING )
    {
        fWidth = static_cast < float > ( lua_tonumber ( luaVM, 8 ) );
    }

    // Got a post gui specifier?
    bool bPostGUI = false;
    if ( iArgument9 == LUA_TBOOLEAN )
    {
        bPostGUI = ( lua_toboolean ( luaVM, 9 ) ) ? true : false;
    }

    // Grab the arguments
    CVector vecBegin, vecEnd;
    vecBegin.fX = static_cast < float > ( lua_tonumber ( luaVM, 1 ) );
    vecBegin.fY = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
    vecBegin.fZ = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );
    vecEnd.fX = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );
    vecEnd.fY = static_cast < float > ( lua_tonumber ( luaVM, 5 ) );
    vecEnd.fZ = static_cast < float > ( lua_tonumber ( luaVM, 6 ) );
    unsigned long ulColor = static_cast < unsigned long > ( lua_tonumber ( luaVM, 7 ) );

    // Draw it
    g_pCore->GetGraphics ()->DrawLine3DQueued ( vecBegin, vecEnd, fWidth, ulColor, bPostGUI );

    // Success
    lua_pushboolean ( luaVM, true );
    return 1;
}


int CLuaFunctionDefs::dxDrawText ( lua_State* luaVM )
{
//  bool dxDrawText ( string text, int left, int top [, int right=left, int bottom=top, int color=white, float scale=1, mixed font="default", 
//      string alignX="left", string alignY="top", bool clip=false, bool wordBreak=false, bool postGUI] )
    SString strText; int iLeft; int iTop; int iRight; int iBottom; ulong ulColor; float fScale; SString strFontName; CClientFont* pFontElement;
    eDXHorizontalAlign alignX; eDXVerticalAlign alignY; bool bClip; bool bWordBreak; bool bPostGUI;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strText );
    argStream.ReadNumber ( iLeft );
    argStream.ReadNumber ( iTop );
    argStream.ReadNumber ( iRight, iLeft );
    argStream.ReadNumber ( iBottom, iTop );
    argStream.ReadNumber ( ulColor, 0xFFFFFFFF );
    argStream.ReadNumber ( fScale, 1 );
    ScriptArgReadFont ( argStream, strFontName, "default", pFontElement );
    argStream.ReadEnumString ( alignX, DX_ALIGN_LEFT );
    argStream.ReadEnumString ( alignY, DX_ALIGN_TOP );
    argStream.ReadBool ( bClip, false );
    argStream.ReadBool ( bWordBreak, false );
    argStream.ReadBool ( bPostGUI, false );

    if ( !argStream.HasErrors () )
    {
        // Get DX font
        ID3DXFont* pDXFont = CStaticFunctionDefinitions::ResolveDXFont ( strFontName, pFontElement, fScale, fScale );

        // Make format flag
        ulong ulFormat = alignX | alignY;
        if ( ulFormat & DT_BOTTOM ) ulFormat |= DT_SINGLELINE;
        if ( bWordBreak )           ulFormat |= DT_WORDBREAK;
        if ( !bClip )               ulFormat |= DT_NOCLIP;

        CStaticFunctionDefinitions::DrawText ( iLeft, iTop, iRight, iBottom, ulColor, strText, fScale, fScale, ulFormat, pDXFont, bPostGUI );

        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "createFont", *argStream.GetErrorMessage () ) );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::dxDrawRectangle ( lua_State* luaVM )
{
    // dxDrawRectangle ( int x,int y,float width,float height,[int color=0xffffffff] )

    // Grab all argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) && 
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) && 
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) && 
        ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        int iX = static_cast < int > ( lua_tonumber ( luaVM, 1 ) );
        int iY = static_cast < int > ( lua_tonumber ( luaVM, 2 ) );
        int iWidth = static_cast < int > ( lua_tonumber ( luaVM, 3 ) );
        int iHeight = static_cast < int > ( lua_tonumber ( luaVM, 4 ) );
        unsigned long ulColor = 0xFFFFFFFF;

        int iArgument5 = lua_type ( luaVM, 5 );
        if ( ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) )
        {
            ulColor = static_cast < unsigned long > ( lua_tonumber ( luaVM, 5 ) );
        }

        // Got a post gui specifier?
        bool bPostGUI = false;
        int iArgument6 = lua_type ( luaVM, 6 );
        if ( iArgument6 == LUA_TBOOLEAN )
        {
            bPostGUI = ( lua_toboolean ( luaVM, 6 ) ) ? true:false;
        }

        g_pCore->GetGraphics ()->DrawRectQueued ( static_cast < float > ( iX ),
                                                  static_cast < float > ( iY ),
                                                  static_cast < float > ( iWidth ),
                                                  static_cast < float > ( iHeight ),
                                                  ulColor, bPostGUI );

        // Success
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "dxDrawRectangle" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::dxDrawImage ( lua_State* luaVM )
{
    // dxDrawImage ( float x,float y,float width,float height,string filename,[float rotation,
    //            float rotCenOffX, float rotCenOffY, int color=0xffffffff, bool postgui] )

    // Grab all argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    int iArgument5 = lua_type ( luaVM, 5 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) && 
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) && 
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) && 
        ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
        (                              iArgument5 == LUA_TSTRING ) )
    {
        float fX = static_cast < float > ( lua_tonumber ( luaVM, 1 ) );
        float fY = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
        float fWidth = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );
        float fHeight = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );
        const char * szFile = lua_tostring ( luaVM, 5 );
        float fRotation = 0;
        float fRotCenOffX = 0;
        float fRotCenOffY = 0;
        unsigned long ulColor = 0xFFFFFFFF;

        int iArgument6 = lua_type ( luaVM, 6 );
        if ( ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING ) )
        {
            fRotation = static_cast < float > ( lua_tonumber ( luaVM, 6 ) );
        }

        int iArgument7 = lua_type ( luaVM, 7 );
        if ( ( iArgument7 == LUA_TNUMBER || iArgument7 == LUA_TSTRING ) )
        {
            fRotCenOffX = static_cast < float > ( lua_tonumber ( luaVM, 7 ) );
        }

        int iArgument8 = lua_type ( luaVM, 8 );
        if ( ( iArgument8 == LUA_TNUMBER || iArgument8 == LUA_TSTRING ) )
        {
            fRotCenOffY = static_cast < float > ( lua_tonumber ( luaVM, 8 ) );
        }

        int iArgument9 = lua_type ( luaVM, 9 );
        if ( ( iArgument9 == LUA_TNUMBER || iArgument9 == LUA_TSTRING ) )
        {
            ulColor = static_cast < unsigned long > ( lua_tonumber ( luaVM, 9 ) );
        }

        // Got a post gui specifier?
        bool bPostGUI = false;
        int iArgument10 = lua_type ( luaVM, 10 );
        if ( iArgument10 == LUA_TBOOLEAN )
        {
            bPostGUI = ( lua_toboolean ( luaVM, 10 ) ) ? true:false;
        }

        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        CResource* pResource = pLuaMain ? pLuaMain->GetResource() : NULL;

        // Check for a valid (and sane) file path
        if ( pResource && szFile )
        {
            // Get the correct directory
            SString strPath;
            if ( CResourceManager::ParseResourcePathInput( szFile, pResource, strPath ) &&
                 g_pCore->GetGraphics ()->DrawTextureQueued ( fX, fY, fWidth, fHeight, 0, 0, 1, 1, true, strPath, fRotation, fRotCenOffX, fRotCenOffY, ulColor, bPostGUI ) )
            {
                // Success
                lua_pushboolean ( luaVM, true );
                return 1;
            }

            m_pScriptDebugging->LogError ( luaVM, "dxDrawImage can't load %s", szFile );
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "dxDrawImage" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::dxDrawImageSection ( lua_State* luaVM )
{
    // dxDrawImageSection ( float x,float y,float width,float height,float u, float v, float usize, float vsize, filename,[float rotation=0,
    //            float rotCenOffX=0, float rotCenOffY=0, int color=0xffffffff, bool postgui=false] )


    // Grab all argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    int iArgument5 = lua_type ( luaVM, 5 );
    int iArgument6 = lua_type ( luaVM, 6 );
    int iArgument7 = lua_type ( luaVM, 7 );
    int iArgument8 = lua_type ( luaVM, 8 );
    int iArgument9 = lua_type ( luaVM, 9 );
    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) && 
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) && 
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) && 
        ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
        ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) &&
        ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING ) &&
        ( iArgument7 == LUA_TNUMBER || iArgument7 == LUA_TSTRING ) &&
        ( iArgument8 == LUA_TNUMBER || iArgument8 == LUA_TSTRING ) &&
        (                              iArgument9 == LUA_TSTRING ) )
    {
        float fX = static_cast < float > ( lua_tonumber ( luaVM, 1 ) );
        float fY = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
        float fWidth = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );
        float fHeight = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );
        float fU = static_cast < float > ( lua_tonumber ( luaVM, 5 ) );
        float fV = static_cast < float > ( lua_tonumber ( luaVM, 6 ) );
        float fSizeU = static_cast < float > ( lua_tonumber ( luaVM, 7 ) );
        float fSizeV = static_cast < float > ( lua_tonumber ( luaVM, 8 ) );
        const char * szFile = lua_tostring ( luaVM, 9 );
        float fRotation = 0;
        float fRotCenOffX = 0;
        float fRotCenOffY = 0;
        unsigned long ulColor = 0xFFFFFFFF;

        int iArgument10 = lua_type ( luaVM, 10 );
        if ( ( iArgument10 == LUA_TNUMBER || iArgument10 == LUA_TSTRING ) )
        {
            fRotation = static_cast < float > ( lua_tonumber ( luaVM, 10 ) );
        }

        int iArgument11 = lua_type ( luaVM, 11 );
        if ( ( iArgument11 == LUA_TNUMBER || iArgument11 == LUA_TSTRING ) )
        {
            fRotCenOffX = static_cast < float > ( lua_tonumber ( luaVM, 11 ) );
        }

        int iArgument12 = lua_type ( luaVM, 12 );
        if ( ( iArgument12 == LUA_TNUMBER || iArgument12 == LUA_TSTRING ) )
        {
            fRotCenOffY = static_cast < float > ( lua_tonumber ( luaVM, 12 ) );
        }

        int iArgument13 = lua_type ( luaVM, 13 );
        if ( ( iArgument13 == LUA_TNUMBER || iArgument13 == LUA_TSTRING ) )
        {
            ulColor = static_cast < unsigned long > ( lua_tonumber ( luaVM, 13 ) );
        }

        // Got a post gui specifier?
        bool bPostGUI = false;
        int iArgument14 = lua_type ( luaVM, 14 );
        if ( iArgument14 == LUA_TBOOLEAN )
        {
            bPostGUI = ( lua_toboolean ( luaVM, 14 ) ) ? true:false;
        }

        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        CResource* pResource = pLuaMain ? pLuaMain->GetResource() : NULL;

        // Check for a valid (and sane) file path
        if ( pResource && szFile )
        {
            // Get the correct directory
            SString strPath;
            if ( CResourceManager::ParseResourcePathInput( szFile, pResource, strPath ) &&
                 g_pCore->GetGraphics ()->DrawTextureQueued ( fX, fY, fWidth, fHeight, fU, fV, fSizeU, fSizeV, false, strPath, fRotation, fRotCenOffX, fRotCenOffY, ulColor, bPostGUI ) )
            {
                // Success
                lua_pushboolean ( luaVM, true );
                return 1;
            }

            m_pScriptDebugging->LogError ( luaVM, "dxDrawImageSection can't load %s", szFile );
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "dxDrawImageSection" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::dxGetTextWidth ( lua_State* luaVM )
{
//  float dxGetTextWidth ( string text, [float scale=1, mixed font="default"] )
    SString strText; float fScale; SString strFontName; CClientFont* pFontElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strText );
    argStream.ReadNumber ( fScale, 1 );
    ScriptArgReadFont ( argStream, strFontName, "default", pFontElement );

    if ( !argStream.HasErrors () )
    {
        ID3DXFont* pDXFont = CStaticFunctionDefinitions::ResolveDXFont ( strFontName, pFontElement, fScale, fScale );

        // Retrieve the longest line's extent
        std::stringstream ssText ( strText );
        std::string sLineText;
        float fWidth = 0.0f, fLineExtent = 0.0f;

        while( std::getline ( ssText, sLineText ) )
        {
            fLineExtent = g_pCore->GetGraphics ()->GetDXTextExtent ( sLineText.c_str ( ), fScale, pDXFont );
            if ( fLineExtent > fWidth )
                fWidth = fLineExtent;
        }

        // Success
        lua_pushnumber ( luaVM, fWidth );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "dxGetTextWidth", *argStream.GetErrorMessage () ) );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::dxGetFontHeight ( lua_State* luaVM )
{
//  int dxGetFontHeight ( [float scale=1, mixed font="default"] )
    float fScale; SString strFontName; CClientFont* pFontElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( fScale, 1 );
    ScriptArgReadFont ( argStream, strFontName, "default", pFontElement );

    if ( !argStream.HasErrors () )
    {
        ID3DXFont* pDXFont = CStaticFunctionDefinitions::ResolveDXFont ( strFontName, pFontElement, fScale, fScale );

        float fHeight = g_pCore->GetGraphics ()->GetDXFontHeight ( fScale, pDXFont );
        // Success
        lua_pushnumber ( luaVM, fHeight );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "dxGetFontHeight", *argStream.GetErrorMessage () ) );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::CreateFont ( lua_State* luaVM )
{
//  element = createFont( filepath, int size=9, bool bold=false )
    SString strFilePath; int iSize; bool bBold;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strFilePath );
    argStream.ReadNumber ( iSize, 9 );
    argStream.ReadBool ( bBold, false );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource =  pLuaMain->GetResource();
            SString strPath, strMetaPath;
            if ( CResourceManager::ParseResourcePathInput( strFilePath, pResource, strPath, strMetaPath ) )
            {
                if ( FileExists ( strPath ) )
                {
                    CClientFont* pFont = CStaticFunctionDefinitions::CreateFont ( strPath, iSize, bBold, strMetaPath, pResource );
                    lua_pushelement ( luaVM, pFont );
                    return 1;
                }
                else
                    m_pScriptDebugging->LogBadPointer ( luaVM, "createFont", "file-path", 1 );
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "createFont", "file-path", 1 );
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "createFont", *argStream.GetErrorMessage () ) );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}
