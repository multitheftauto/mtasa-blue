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
    // dxDrawText ( string text,int left,int top [,int right=left,int bottom=top,int color=0xffffffff,float scale=1,string font="default",string alignX="left",string alignY="top",bool clip=false,bool wordBreak=false] )

    // Grab all argument types
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TSTRING ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) && 
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        const char * szText = lua_tostring ( luaVM, 1 );
        int iLeft = static_cast < int > ( lua_tonumber ( luaVM, 2 ) );
        int iTop = static_cast < int > ( lua_tonumber ( luaVM, 3 ) );
        int iRight = iLeft;
        int iBottom = iTop;        
        unsigned long ulColor = 0xFFFFFFFF;
        float fScale = 1.0f;
        eFontType fontType = FONT_DEFAULT;
        unsigned long ulFormat = 0;
        bool bClip = false;
        bool bPostGUI = false;

        int iArgument4 = lua_type ( luaVM, 4 );
        if ( ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
        {
            iRight = static_cast < int > ( lua_tonumber ( luaVM, 4 ) );

            int iArgument5 = lua_type ( luaVM, 5 );
            if ( ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) )
            {
                iBottom = static_cast < int > ( lua_tonumber ( luaVM, 5 ) );

                int iArgument6 = lua_type ( luaVM, 6 );
                if ( ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING ) )
                {
                    ulColor = static_cast < unsigned long > ( lua_tonumber ( luaVM, 6 ) );

                    int iArgument7 = lua_type ( luaVM, 7 );
                    if ( ( iArgument7 == LUA_TNUMBER || iArgument7 == LUA_TSTRING ) )
                    {
                        fScale = static_cast < float > ( lua_tonumber ( luaVM, 7 ) );

                        int iArgument8 = lua_type ( luaVM, 8 );
                        if ( iArgument8 == LUA_TSTRING )
                        {
                            const char * szFontName = lua_tostring ( luaVM, 8 );
                            fontType = g_pCore->GetGraphics ()->GetFontType ( const_cast < char * > ( szFontName ) );

                            if ( lua_type ( luaVM, 9 ) == LUA_TSTRING )
                            {
                                const char * szTemp = lua_tostring ( luaVM, 9 );
                                if ( !stricmp ( szTemp, "center" ) )
                                    ulFormat |= DT_CENTER;
                                else if ( !stricmp ( szTemp, "right" ) )
                                    ulFormat |= DT_RIGHT;

                                if ( lua_type ( luaVM, 10 ) == LUA_TSTRING )
                                {
                                    const char * szTemp = lua_tostring ( luaVM, 10 );
                                    if ( !stricmp ( szTemp, "center" ) )
                                        ulFormat |= DT_VCENTER;
                                    else if ( !stricmp ( szTemp, "bottom" ) )
                                        ulFormat |= ( DT_BOTTOM | DT_SINGLELINE );

                                    if ( lua_type ( luaVM, 11 ) == LUA_TBOOLEAN )
                                    {
                                        bClip = ( lua_toboolean ( luaVM, 11 ) ) ? true:false;

                                        if ( lua_type ( luaVM, 12 ) == LUA_TBOOLEAN )
                                        {
                                            if ( lua_toboolean ( luaVM, 12 ) )
                                                ulFormat |= DT_WORDBREAK;
                                            if ( lua_type ( luaVM, 13 ) == LUA_TBOOLEAN )
                                            {
                                                bPostGUI = ( lua_toboolean ( luaVM, 13 ) ) ? true:false;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if ( !bClip ) ulFormat |= DT_NOCLIP;

        ID3DXFont * pFont = g_pCore->GetGraphics ()->GetFont ( fontType );
        g_pCore->GetGraphics ()->DrawTextQueued ( iLeft, iTop, iRight, iBottom, ulColor, szText, fScale, fScale, ulFormat, pFont, bPostGUI );

        // Success
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "dxDrawText" );

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
    //            float rotCenOffX, float rotCenOffY, float startX 9, float startY, float endX, float endY 12, int color=0xffffffff, bool postgui] )

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
        float fStartX = 0;
        float fStartY = 0;
        float fEndX = 0;
        float fEndY = 0;
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
        int iArgument9 = lua_type( luaVM, 9);
        if((iArgument9 == LUA_TNUMBER || iArgument9 == LUA_TSTRING))
        {
            fStartX = static_cast<float>(lua_tonumber(luaVM, 9));
        }
        int iArgument10 = lua_type( luaVM, 10);
        if((iArgument10 == LUA_TNUMBER || iArgument10 == LUA_TSTRING))
        {
           fStartY = static_cast<float>(lua_tonumber(luaVM, 10));
        }
        int iArgument11 = lua_type( luaVM, 11);
        if((iArgument11 == LUA_TNUMBER || iArgument11 == LUA_TSTRING))
        {
           fEndX = static_cast<float>(lua_tonumber(luaVM, 11));
        }
        int iArgument12 = lua_type( luaVM, 12);
        if((iArgument12 == LUA_TNUMBER || iArgument12 == LUA_TSTRING))
        {
           fEndY = static_cast<float>(lua_tonumber(luaVM, 12));
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
                 g_pCore->GetGraphics ()->DrawTextureQueued ( fX, fY, fWidth, fHeight, strPath, fRotation, fRotCenOffX, fRotCenOffY, ulColor, bPostGUI ) )
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



int CLuaFunctionDefs::dxGetTextWidth ( lua_State* luaVM )
{
    // dxGetTextWidth ( string text, [float scale=1,string font="default"] )

    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        const char * szText = lua_tostring ( luaVM, 1 );
        float fScale = 1.0f;
        eFontType fontType = FONT_DEFAULT;

        int iArgument2 = lua_type ( luaVM, 2 );
        if ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING )
        {
            fScale = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );

            if ( lua_type ( luaVM, 3 ) == LUA_TSTRING )
            {
                const char * szFontName = lua_tostring ( luaVM, 3 );
                fontType = g_pCore->GetGraphics ()->GetFontType ( const_cast < char * > ( szFontName ) );
            }
        }

        ID3DXFont * pFont = g_pCore->GetGraphics ()->GetFont ( fontType );

        // Retrieve the longest line's extent
        std::stringstream ssText ( szText );
        std::string sLineText;
        float fWidth = 0.0f, fLineExtent = 0.0f;

        while( std::getline ( ssText, sLineText ) )
        {
            fLineExtent = g_pCore->GetGraphics ()->GetDXTextExtent ( sLineText.c_str ( ), fScale, pFont );
            if ( fLineExtent > fWidth )
                fWidth = fLineExtent;
        }
        // Success
        lua_pushnumber ( luaVM, fWidth );
        return 1;
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "dxGetTextWidth" );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::dxGetFontHeight ( lua_State* luaVM )
{
    // dxGetFontHeight ( [float scale=1,string font="default"] )

    float fScale = 1.0f;
    eFontType fontType = FONT_DEFAULT;

    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        fScale = static_cast < float > ( lua_tonumber ( luaVM, 1 ) );

        if ( lua_type ( luaVM, 2 ) == LUA_TSTRING )
        {
            const char * szFontName = lua_tostring ( luaVM, 2 );
            fontType = g_pCore->GetGraphics ()->GetFontType ( const_cast < char * > ( szFontName ) );
        }
    }

    ID3DXFont * pFont = g_pCore->GetGraphics ()->GetFont ( fontType );
    float fHeight = g_pCore->GetGraphics ()->GetDXFontHeight ( fScale, pFont );

    // Success
    lua_pushnumber ( luaVM, fHeight );
    return 1;
}

