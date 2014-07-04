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
    SString strText; int iLeft; int iTop; int iRight; int iBottom; ulong ulColor; float fScaleX; float fScaleY; SString strFontName; CClientDxFont* pDxFontElement;
    eDXHorizontalAlign alignX; eDXVerticalAlign alignY; bool bClip; bool bWordBreak; bool bPostGUI;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strText );
    argStream.ReadNumber ( iLeft );
    argStream.ReadNumber ( iTop );
    argStream.ReadNumber ( iRight, iLeft );
    argStream.ReadNumber ( iBottom, iTop );
    argStream.ReadNumber ( ulColor, 0xFFFFFFFF );
    argStream.ReadNumber ( fScaleX, 1 );
    if ( argStream.NextIsNumber () )
        argStream.ReadNumber ( fScaleY );
    else
        fScaleY = fScaleX;
    MixedReadDxFontString ( argStream, strFontName, "default", pDxFontElement );
    argStream.ReadEnumString ( alignX, DX_ALIGN_LEFT );
    argStream.ReadEnumString ( alignY, DX_ALIGN_TOP );
    argStream.ReadBool ( bClip, false );
    argStream.ReadBool ( bWordBreak, false );
    argStream.ReadBool ( bPostGUI, false );

    if ( !argStream.HasErrors () )
    {
        // Get DX font
        ID3DXFont* pD3DXFont = CStaticFunctionDefinitions::ResolveD3DXFont ( strFontName, pDxFontElement );

        // Make format flag
        ulong ulFormat = alignX | alignY;
        //if ( ulFormat & DT_BOTTOM ) ulFormat |= DT_SINGLELINE;        MS says we should do this. Nobody tells me what to do.
        if ( bWordBreak )           ulFormat |= DT_WORDBREAK;
        if ( !bClip )               ulFormat |= DT_NOCLIP;

        CStaticFunctionDefinitions::DrawText ( iLeft, iTop, iRight, iBottom, ulColor, strText, fScaleX, fScaleY, ulFormat, pD3DXFont, bPostGUI );

        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "dxDrawText", *argStream.GetErrorMessage () ) );

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
//  bool dxDrawImage ( float posX, float posY, float width, float height, string filepath [, float rotation = 0, float rotationCenterOffsetX = 0, 
//      float rotationCenterOffsetY = 0, int color = white, bool postGUI = false ] )
    float fPosX; float fPosY; float fWidth; float fHeight; CClientMaterial* pMaterialElement; float fRotation;
        float fRotCenOffX; float fRotCenOffY; uint ulColor; bool bPostGUI;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( fPosX );
    argStream.ReadNumber ( fPosY );
    argStream.ReadNumber ( fWidth );
    argStream.ReadNumber ( fHeight );
    MixedReadMaterialString ( argStream, pMaterialElement );
    argStream.ReadNumber ( fRotation, 0 );
    argStream.ReadNumber ( fRotCenOffX, 0 );
    argStream.ReadNumber ( fRotCenOffY, 0 );
    argStream.ReadNumber ( ulColor, 0xffffffff );
    argStream.ReadBool ( bPostGUI, false );

    if ( !argStream.HasErrors () )
    {
        if ( pMaterialElement )
        {
            g_pCore->GetGraphics ()->DrawTextureQueued ( fPosX, fPosY, fWidth, fHeight, 0, 0, 1, 1, true, pMaterialElement->GetMaterialItem (), fRotation, fRotCenOffX, fRotCenOffY, ulColor, bPostGUI );
            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogError ( luaVM, "dxDrawImage can't load file" );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "dxDrawImage", *argStream.GetErrorMessage () ) );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::dxDrawImageSection ( lua_State* luaVM )
{
//  bool dxDrawImageSection ( float posX, float posY, float width, float height, float u, float v, float usize, float vsize, string filepath, 
//      [ float rotation = 0, float rotationCenterOffsetX = 0, float rotationCenterOffsetY = 0, int color = white, bool postGUI = false ] )
    float fPosX; float fPosY; float fWidth; float fHeight; float fU; float fV; float fSizeU; float fSizeV; CClientMaterial* pMaterialElement;
         float fRotation; float fRotCenOffX; float fRotCenOffY; uint ulColor; bool bPostGUI;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( fPosX );
    argStream.ReadNumber ( fPosY );
    argStream.ReadNumber ( fWidth );
    argStream.ReadNumber ( fHeight );
    argStream.ReadNumber ( fU );
    argStream.ReadNumber ( fV );
    argStream.ReadNumber ( fSizeU );
    argStream.ReadNumber ( fSizeV );
    MixedReadMaterialString ( argStream, pMaterialElement );
    argStream.ReadNumber ( fRotation, 0 );
    argStream.ReadNumber ( fRotCenOffX, 0 );
    argStream.ReadNumber ( fRotCenOffY, 0 );
    argStream.ReadNumber ( ulColor, 0xffffffff );
    argStream.ReadBool ( bPostGUI, false );

    if ( !argStream.HasErrors () )
    {
        if ( pMaterialElement )
        {
            g_pCore->GetGraphics ()->DrawTextureQueued ( fPosX, fPosY, fWidth, fHeight, fU, fV, fSizeU, fSizeV, false, pMaterialElement->GetMaterialItem (), fRotation, fRotCenOffX, fRotCenOffY, ulColor, bPostGUI );
            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogError ( luaVM, "dxDrawImageSection can't load file" );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "dxDrawImageSection", *argStream.GetErrorMessage () ) );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::dxGetTextWidth ( lua_State* luaVM )
{
//  float dxGetTextWidth ( string text, [float scale=1, mixed font="default"] )
    SString strText; float fScale; SString strFontName; CClientDxFont* pDxFontElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strText );
    argStream.ReadNumber ( fScale, 1 );
    MixedReadDxFontString ( argStream, strFontName, "default", pDxFontElement );

    if ( !argStream.HasErrors () )
    {
        ID3DXFont* pD3DXFont = CStaticFunctionDefinitions::ResolveD3DXFont ( strFontName, pDxFontElement );

        // Retrieve the longest line's extent
        std::stringstream ssText ( strText );
        std::string sLineText;
        float fWidth = 0.0f, fLineExtent = 0.0f;

        while( std::getline ( ssText, sLineText ) )
        {
            fLineExtent = g_pCore->GetGraphics ()->GetDXTextExtent ( sLineText.c_str ( ), fScale, pD3DXFont );
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
    float fScale; SString strFontName; CClientDxFont* pDxFontElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( fScale, 1 );
    MixedReadDxFontString ( argStream, strFontName, "default", pDxFontElement );

    if ( !argStream.HasErrors () )
    {
        ID3DXFont* pD3DXFont = CStaticFunctionDefinitions::ResolveD3DXFont ( strFontName, pDxFontElement );

        float fHeight = g_pCore->GetGraphics ()->GetDXFontHeight ( fScale, pD3DXFont );
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


int CLuaFunctionDefs::dxCreateTexture ( lua_State* luaVM )
{
//  element dxCreateTexture( string filepath )
    SString strFilePath;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strFilePath );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pParentResource = pLuaMain->GetResource ();
            CResource* pFileResource = pParentResource;
            SString strPath, strMetaPath;
            if ( CResourceManager::ParseResourcePathInput( strFilePath, pFileResource, strPath, strMetaPath ) )
            {
                if ( FileExists ( strPath ) )
                {
                    CClientTexture* pTexture = g_pClientGame->GetManager ()->GetRenderElementManager ()->CreateTexture ( strPath );
                    if ( pTexture )
                    {
                        // Make it a child of the resource's file root ** CHECK  Should parent be pFileResource, and element added to pParentResource's ElementGroup? **
                        pTexture->SetParent ( pParentResource->GetResourceDynamicEntity () );
                    }
                    lua_pushelement ( luaVM, pTexture );
                    return 1;
                }
                else
                    m_pScriptDebugging->LogBadPointer ( luaVM, "dxCreateTexture", "file-path", 1 );
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "dxCreateTexture", "file-path", 1 );
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "dxCreateTexture", *argStream.GetErrorMessage () ) );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::dxCreateShader ( lua_State* luaVM )
{
//  element dxCreateShader( string filepath [, float priority = 0, float maxdistance = 0, bool debug = false ] )
    SString strFilePath; float fPriority; float fMaxDistance; bool bDebug;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strFilePath );
    argStream.ReadNumber ( fPriority, 0.0f );
    argStream.ReadNumber ( fMaxDistance, 0.0f );
    argStream.ReadBool ( bDebug, false );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pParentResource = pLuaMain->GetResource ();
            CResource* pFileResource = pParentResource;
            SString strPath, strMetaPath;
            if ( CResourceManager::ParseResourcePathInput( strFilePath, pFileResource, strPath, strMetaPath ) )
            {
                if ( FileExists ( strPath ) )
                {
                    SString strRootPath = strPath.Left ( strPath.length () - strMetaPath.length () );
                    SString strStatus;
                    CClientShader* pShader = g_pClientGame->GetManager ()->GetRenderElementManager ()->CreateShader ( strPath, strRootPath, strStatus, fPriority, fMaxDistance, bDebug );
                    if ( pShader )
                    {
                        // Make it a child of the resource's file root ** CHECK  Should parent be pFileResource, and element added to pParentResource's ElementGroup? **
                        pShader->SetParent ( pParentResource->GetResourceDynamicEntity () );
                        lua_pushelement ( luaVM, pShader );
                        lua_pushstring ( luaVM, strStatus );    // String containing name of technique being used.
                        return 2;
                    }
                    else
                    {
                        // Replace any path in the error message with our own one
                        SString strRootPathWithoutResource = strRootPath.Left ( strRootPath.TrimEnd ( "\\" ).length () - SStringX ( pFileResource->GetName () ).length () ) ;
                        strStatus = strStatus.ReplaceI ( strRootPathWithoutResource, "" );
                        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Problem @ '%s' [%s]", "dxCreateShader", *strStatus ) );
                    }
                }
                else
                    m_pScriptDebugging->LogCustom ( luaVM, SString ( "Missing file @ '%s' [%s]", "dxCreateShader", *ConformResourcePath ( strPath, true ) ) );
            }
            else
                m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad file-path @ '%s' [%s]", "dxCreateShader", *strFilePath ) );
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "dxCreateShader", *argStream.GetErrorMessage () ) );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::dxCreateRenderTarget ( lua_State* luaVM )
{
//  element dxCreateRenderTarget( int sizeX, int sizeY [, int withAlphaChannel = false ] )
    uint uiSizeX; uint uiSizeY; bool bWithAlphaChannel;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( uiSizeX );
    argStream.ReadNumber ( uiSizeY );
    argStream.ReadBool ( bWithAlphaChannel, false );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        CResource* pParentResource = pLuaMain ? pLuaMain->GetResource () : NULL;
        if ( pParentResource )
        {
            CClientRenderTarget* pRenderTarget = g_pClientGame->GetManager ()->GetRenderElementManager ()->CreateRenderTarget ( uiSizeX, uiSizeY, bWithAlphaChannel );
            if ( pRenderTarget )
            {
                // Make it a child of the resource's file root ** CHECK  Should parent be pFileResource, and element added to pParentResource's ElementGroup? **
                pRenderTarget->SetParent ( pParentResource->GetResourceDynamicEntity () );
            }
            lua_pushelement ( luaVM, pRenderTarget );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "dxCreateRenderTarget", *argStream.GetErrorMessage () ) );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::dxCreateScreenSource ( lua_State* luaVM )
{
//  element dxCreateScreenSource( int sizeX, int sizeY )
    uint uiSizeX; uint uiSizeY;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( uiSizeX );
    argStream.ReadNumber ( uiSizeY );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        CResource* pParentResource = pLuaMain ? pLuaMain->GetResource () : NULL;
        if ( pParentResource )
        {
            CClientScreenSource* pScreenSource = g_pClientGame->GetManager ()->GetRenderElementManager ()->CreateScreenSource ( uiSizeX, uiSizeY );
            if ( pScreenSource )
            {
                // Make it a child of the resource's file root ** CHECK  Should parent be pFileResource, and element added to pParentResource's ElementGroup? **
                pScreenSource->SetParent ( pParentResource->GetResourceDynamicEntity () );
            }
            lua_pushelement ( luaVM, pScreenSource );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "dxCreateScreenSource", *argStream.GetErrorMessage () ) );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::dxGetMaterialSize ( lua_State* luaVM )
{
//  int, int dxGetMaterialSize( element material )
    CClientMaterial* pMaterial; SString strName;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMaterial );

    if ( !argStream.HasErrors () )
    {
        lua_pushnumber ( luaVM, pMaterial->GetMaterialItem ()->m_uiSizeX );
        lua_pushnumber ( luaVM, pMaterial->GetMaterialItem ()->m_uiSizeY );
        return 2;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "dxGetMaterialSize", *argStream.GetErrorMessage () ) );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::dxSetShaderValue ( lua_State* luaVM )
{
//  bool dxSetShaderValue( element shader, string name, mixed value )
    CClientShader* pShader; SString strName;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pShader );
    argStream.ReadString ( strName );

    if ( !argStream.HasErrors () )
    {
        // Try each mixed type in turn
        int iArgument = lua_type ( argStream.m_luaVM, argStream.m_iIndex );

        if ( iArgument == LUA_TLIGHTUSERDATA )
        {
            // Texture
            CClientTexture* pTexture;
            if ( argStream.ReadUserData ( pTexture ) )
            {
                bool bResult = pShader->GetShaderItem ()->SetValue ( strName, pTexture->GetTextureItem () );
                lua_pushboolean ( luaVM, bResult );
                return 1;
            }
        }
        else
        if ( iArgument == LUA_TBOOLEAN )
        {
            // bool
            bool bValue;
            if ( argStream.ReadBool ( bValue ) )
            {
                bool bResult = pShader->GetShaderItem ()->SetValue ( strName, bValue );
                lua_pushboolean ( luaVM, bResult );
                return 1;
            }
        }
        else
        if ( iArgument == LUA_TNUMBER || iArgument == LUA_TSTRING )
        {
            // float(s)
            float fBuffer[16];
            uint i;
            for ( i = 0 ; i < NUMELMS(fBuffer); )
            {
                fBuffer[i++] = static_cast < float > ( lua_tonumber ( argStream.m_luaVM, argStream.m_iIndex++ ) );
                iArgument = lua_type ( argStream.m_luaVM, argStream.m_iIndex );
                if ( iArgument != LUA_TNUMBER && iArgument != LUA_TSTRING )
                    break;
            }
            bool bResult = pShader->GetShaderItem ()->SetValue ( strName, fBuffer, i );
            lua_pushboolean ( luaVM, bResult );
            return 1;
        }
        else
        if ( iArgument == LUA_TTABLE )
        {
            // table (of floats)
            float fBuffer[16];
            uint i = 0;

            lua_pushnil ( luaVM );      // Loop through our table, beginning at the first key
            while ( lua_next ( luaVM, argStream.m_iIndex ) != 0 && i < NUMELMS(fBuffer) )
            {
                fBuffer[i++] = static_cast < float > ( lua_tonumber ( luaVM, -1 ) );    // Ignore the index at -2, and just read the value
                lua_pop ( luaVM, 1 );                     // Remove the item and keep the key for the next iteration
            }
            bool bResult = pShader->GetShaderItem ()->SetValue ( strName, fBuffer, i );
            lua_pushboolean ( luaVM, bResult );
            return 1;
        }
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "dxSetShaderValue", "Expected number, bool, table or texture at argument 3" ) );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "dxSetShaderValue", *argStream.GetErrorMessage () ) );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::dxSetShaderTessellation ( lua_State* luaVM )
{
//  bool dxSetShaderTessellation( element shader, int tessellationX, int tessellationY )
    CClientShader* pShader; uint uiTessellationX; uint uiTessellationY;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pShader );
    argStream.ReadNumber ( uiTessellationX );
    argStream.ReadNumber ( uiTessellationY );

    if ( !argStream.HasErrors () )
    {
        pShader->GetShaderItem ()->SetTessellation ( uiTessellationX, uiTessellationY );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "dxSetShaderTessellation", *argStream.GetErrorMessage () ) );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::dxSetShaderTransform ( lua_State* luaVM )
{
//  bool dxSetShaderTransform( element shader, lots )
    CClientShader* pShader; SShaderTransform transform;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pShader );

    argStream.ReadNumber ( transform.vecRot.fX, 0 );
    argStream.ReadNumber ( transform.vecRot.fY, 0 );
    argStream.ReadNumber ( transform.vecRot.fZ, 0 );
    argStream.ReadNumber ( transform.vecRotCenOffset.fX, 0 );
    argStream.ReadNumber ( transform.vecRotCenOffset.fY, 0 );
    argStream.ReadNumber ( transform.vecRotCenOffset.fZ, 0 );
    argStream.ReadBool ( transform.bRotCenOffsetOriginIsScreen, false );
    argStream.ReadNumber ( transform.vecPersCenOffset.fX, 0 );
    argStream.ReadNumber ( transform.vecPersCenOffset.fY, 0 );
    argStream.ReadBool ( transform.bPersCenOffsetOriginIsScreen, false );

    if ( !argStream.HasErrors () )
    {
        pShader->GetShaderItem ()->SetTransform ( transform );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "dxSetShaderTransform", *argStream.GetErrorMessage () ) );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::dxSetRenderTarget ( lua_State* luaVM )
{
//  bool setRenderTaget( element renderTarget [, bool clear = false ] )
    CClientRenderTarget* pRenderTarget; bool bClear;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pRenderTarget, NULL );
    argStream.ReadBool ( bClear, false );

    if ( !argStream.HasErrors () )
    {
        bool bResult;
        if ( pRenderTarget)
            bResult = g_pCore->GetGraphics ()->GetRenderItemManager ()->SetRenderTarget ( pRenderTarget->GetRenderTargetItem (), bClear );
        else
            bResult = g_pCore->GetGraphics ()->GetRenderItemManager ()->RestoreDefaultRenderTarget ();

        if ( bResult )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad usage @ '%s' [%s]", "dxSetRenderTarget", "dxSetRenderTarget can only be used inside certain events" ) );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "dxSetRenderTarget", *argStream.GetErrorMessage () ) );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::dxUpdateScreenSource ( lua_State* luaVM )
{
//  bool dxUpdateScreenSource( element screenSource )
    CClientScreenSource* pScreenSource;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pScreenSource );

    if ( !argStream.HasErrors () )
    {
        g_pCore->GetGraphics ()->GetRenderItemManager ()->UpdateScreenSource ( pScreenSource->GetScreenSourceItem () );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "dxUpdateScreenSource", *argStream.GetErrorMessage () ) );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::dxCreateFont ( lua_State* luaVM )
{
//  element dxCreateFont( string filepath [, int size=9, bool bold=false ] )
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
            CResource* pParentResource = pLuaMain->GetResource ();
            CResource* pFileResource = pParentResource;
            SString strPath;
            if ( CResourceManager::ParseResourcePathInput( strFilePath, pFileResource, strPath ) )
            {
                if ( FileExists ( strPath ) )
                {
                    CClientDxFont* pDxFont = g_pClientGame->GetManager ()->GetRenderElementManager ()->CreateDxFont ( strPath, iSize, bBold );
                    if ( pDxFont )
                    {
                        // Make it a child of the resource's file root ** CHECK  Should parent be pFileResource, and element added to pParentResource's ElementGroup? **
                        pDxFont->SetParent ( pParentResource->GetResourceDynamicEntity () );
                    }
                    lua_pushelement ( luaVM, pDxFont );
                    return 1;
                }
                else
                    m_pScriptDebugging->LogBadPointer ( luaVM, "dxCreateFont", "file-path", 1 );
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "dxCreateFont", "file-path", 1 );
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "dxCreateFont", *argStream.GetErrorMessage () ) );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::dxSetTestMode ( lua_State* luaVM )
{
//  bool dxSetTestMode( string testMode )
    eDxTestMode testMode;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadEnumString ( testMode, DX_TEST_MODE_NONE );

    if ( !argStream.HasErrors () )
    {
        g_pCore->GetGraphics ()->GetRenderItemManager ()->SetTestMode ( testMode );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "dxTestMode", *argStream.GetErrorMessage () ) );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::dxGetStatus ( lua_State* luaVM )
{
//  table dxGetStatus()

    CScriptArgReader argStream ( luaVM );

    if ( !argStream.HasErrors () )
    {
        SDxStatus dxStatus;
        g_pCore->GetGraphics ()->GetRenderItemManager ()->GetDxStatus ( dxStatus );

        lua_createtable ( luaVM, 0, 13 );

        lua_pushstring ( luaVM, "TestMode" );
        lua_pushstring ( luaVM, EnumToString ( dxStatus.testMode ) );
        lua_settable   ( luaVM, -3 );

        lua_pushstring ( luaVM, "VideoCardName" );
        lua_pushstring ( luaVM, dxStatus.videoCard.strName );
        lua_settable   ( luaVM, -3 );

        lua_pushstring ( luaVM, "VideoCardRAM" );
        lua_pushnumber ( luaVM, dxStatus.videoCard.iInstalledMemoryKB / 1024 );
        lua_settable   ( luaVM, -3 );

        lua_pushstring ( luaVM, "VideoCardPSVersion" );
        lua_pushstring ( luaVM, dxStatus.videoCard.strPSVersion );
        lua_settable   ( luaVM, -3 );

        lua_pushstring ( luaVM, "VideoMemoryFreeForMTA" );
        lua_pushnumber ( luaVM, dxStatus.videoMemoryKB.iFreeForMTA / 1024 );
        lua_settable   ( luaVM, -3 );

        lua_pushstring ( luaVM, "VideoMemoryUsedByFonts" );
        lua_pushnumber ( luaVM, dxStatus.videoMemoryKB.iUsedByFonts / 1024 );
        lua_settable   ( luaVM, -3 );

        lua_pushstring ( luaVM, "VideoMemoryUsedByTextures" );
        lua_pushnumber ( luaVM, dxStatus.videoMemoryKB.iUsedByTextures / 1024 );
        lua_settable   ( luaVM, -3 );

        lua_pushstring ( luaVM, "VideoMemoryUsedByRenderTargets" );
        lua_pushnumber ( luaVM, dxStatus.videoMemoryKB.iUsedByRenderTargets / 1024 );
        lua_settable   ( luaVM, -3 );

        lua_pushstring ( luaVM, "SettingWindowed" );
        lua_pushboolean ( luaVM, dxStatus.settings.bWindowed );
        lua_settable   ( luaVM, -3 );

        lua_pushstring ( luaVM, "SettingFXQuality" );
        lua_pushnumber ( luaVM, dxStatus.settings.iFXQuality );
        lua_settable   ( luaVM, -3 );

        lua_pushstring ( luaVM, "SettingDrawDistance" );
        lua_pushnumber ( luaVM, dxStatus.settings.iDrawDistance );
        lua_settable   ( luaVM, -3 );

        lua_pushstring ( luaVM, "SettingVolumetricShadows" );
        lua_pushboolean ( luaVM, dxStatus.settings.bVolumetricShadows );
        lua_settable   ( luaVM, -3 );

        lua_pushstring ( luaVM, "SettingStreamingVideoMemoryForGTA" );
        lua_pushnumber ( luaVM, dxStatus.settings.iStreamingMemory );
        lua_settable   ( luaVM, -3 );

        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "dxGetStatus", *argStream.GetErrorMessage () ) );

    // error: bad arguments
    lua_pushboolean ( luaVM, false );
    return 1;
}
