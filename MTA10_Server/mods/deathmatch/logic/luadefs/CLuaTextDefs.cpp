/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaTextDefs.cpp
*  PURPOSE:     Lua text display function definitions class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Peter <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CLuaTextDefs::LoadFunctions ( void )
{
    CLuaCFunctions::AddFunction ( "textCreateDisplay", CLuaTextDefs::textCreateDisplay );
    CLuaCFunctions::AddFunction ( "textDestroyDisplay", CLuaTextDefs::textDestroyDisplay );
    CLuaCFunctions::AddFunction ( "textCreateTextItem", CLuaTextDefs::textCreateTextItem );
    CLuaCFunctions::AddFunction ( "textDestroyTextItem", CLuaTextDefs::textDestroyTextItem );

    CLuaCFunctions::AddFunction ( "textDisplayAddText", CLuaTextDefs::textDisplayAddText );
    CLuaCFunctions::AddFunction ( "textDisplayRemoveText", CLuaTextDefs::textDisplayRemoveText );
    CLuaCFunctions::AddFunction ( "textDisplayAddObserver", CLuaTextDefs::textDisplayAddObserver );
    CLuaCFunctions::AddFunction ( "textDisplayRemoveObserver", CLuaTextDefs::textDisplayRemoveObserver );
    CLuaCFunctions::AddFunction ( "textDisplayIsObserver", CLuaTextDefs::textDisplayIsObserver );
    CLuaCFunctions::AddFunction ( "textDisplayGetObservers", CLuaTextDefs::textDisplayGetObservers );

    CLuaCFunctions::AddFunction ( "textItemSetText", CLuaTextDefs::textItemSetText );
    CLuaCFunctions::AddFunction ( "textItemGetText", CLuaTextDefs::textItemGetText );
    CLuaCFunctions::AddFunction ( "textItemSetScale", CLuaTextDefs::textItemSetScale );
    CLuaCFunctions::AddFunction ( "textItemGetScale", CLuaTextDefs::textItemGetScale );
    CLuaCFunctions::AddFunction ( "textItemSetPosition", CLuaTextDefs::textItemSetPosition );
    CLuaCFunctions::AddFunction ( "textItemGetPosition", CLuaTextDefs::textItemGetPosition );
    CLuaCFunctions::AddFunction ( "textItemSetColor", CLuaTextDefs::textItemSetColor );
    CLuaCFunctions::AddFunction ( "textItemGetColor", CLuaTextDefs::textItemGetColor );
    CLuaCFunctions::AddFunction ( "textItemSetPriority", CLuaTextDefs::textItemSetPriority );
    CLuaCFunctions::AddFunction ( "textItemGetPriority", CLuaTextDefs::textItemGetPriority );
}


int CLuaTextDefs::textCreateDisplay ( lua_State* luaVM )
{
    // Get our current VM
    CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( luaMain )
    {
        // Create a text display and return it
        CTextDisplay* textDisplay = luaMain->CreateDisplay ();
        lua_pushtextdisplay ( luaVM, textDisplay );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTextDefs::textDestroyDisplay ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        // Grab our VM
        CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( luaMain )
        {
            // Grab the text display and check its existence
            CTextDisplay* pTextDisplay = lua_totextdisplay ( luaVM, 1 );
            if ( pTextDisplay )
            {
                // Destroy it
                luaMain->DestroyDisplay ( pTextDisplay );

                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "textDestroyDisplay" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTextDefs::textCreateTextItem ( lua_State* luaVM )
{
    const char * szText = "";
    float fX = 0.5f;
    float fY = 0.5f;
    int priority = PRIORITY_LOW;
    SColorRGBA color ( 255, 255, 255, 255 );
    float scale = 1.0f;
    unsigned char format = 0;
    unsigned char ucShadowAlpha = 0;
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
        szText = lua_tostring ( luaVM, 1 );

    if ( lua_type ( luaVM, 2 ) == LUA_TNUMBER )
        fX = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );

    if ( lua_type ( luaVM, 3 ) == LUA_TNUMBER )
        fY = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );

    if ( lua_type ( luaVM, 4 ) == LUA_TNUMBER )
        priority = static_cast < int > ( lua_tonumber ( luaVM, 4 ) );
    else if ( lua_type ( luaVM, 4 ) == LUA_TSTRING )
    {
        const char * szTemp = lua_tostring ( luaVM, 4 );
        if ( strcmp ( szTemp, "low" ) == 0 )
            priority = PRIORITY_LOW;
        else if ( strcmp ( szTemp, "medium" ) == 0 )
            priority = PRIORITY_MEDIUM;
        else if ( strcmp ( szTemp, "high" ) == 0 )
            priority = PRIORITY_HIGH;
    }
        
    if ( lua_type ( luaVM, 5 ) == LUA_TNUMBER )
        color.R = static_cast < unsigned char > ( lua_tonumber ( luaVM, 5 ) );

    if ( lua_type ( luaVM, 6 ) == LUA_TNUMBER )
        color.G = static_cast < unsigned char > ( lua_tonumber ( luaVM, 6 ) );

    if ( lua_type ( luaVM, 7 ) == LUA_TNUMBER )
        color.B = static_cast < unsigned char > ( lua_tonumber ( luaVM, 7 ) );

    if ( lua_type ( luaVM, 8 ) == LUA_TNUMBER )
        color.A = static_cast < unsigned char > ( lua_tonumber ( luaVM, 8 ) );

    if ( lua_type ( luaVM, 9 ) == LUA_TNUMBER )
        scale = static_cast < float > ( lua_tonumber ( luaVM, 9 ) );

    if ( lua_type ( luaVM, 10 ) == LUA_TSTRING )
    {
        const char * szTemp = lua_tostring ( luaVM, 10 );
        if ( !stricmp ( szTemp, "center" ) )
            format |= 0x00000001; // DT_CENTER
        else if ( !stricmp ( szTemp, "right" ) )
            format |= 0x00000002; // DT_RIGHT
    }
    if ( lua_type ( luaVM, 11 ) == LUA_TSTRING )
    {
        const char * szTemp = lua_tostring ( luaVM, 11 );
        if ( !stricmp ( szTemp, "center" ) )
            format |= 0x00000004; // DT_VCENTER
        else if ( !stricmp ( szTemp, "bottom" ) )
            format |= 0x00000008; // DT_BOTTOM
    }
    if ( lua_type ( luaVM, 12 ) == LUA_TNUMBER )
        ucShadowAlpha = static_cast < unsigned char > ( lua_tonumber ( luaVM, 12 ) );

    // Grab our virtual machine
    CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( luaMain )
    {
        CTextItem* pTextItem = luaMain->CreateTextItem ( szText, fX, fY, (eTextPriority)priority, color, scale, format, ucShadowAlpha );
        lua_pushtextitem ( luaVM, pTextItem );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTextDefs::textDestroyTextItem ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( luaMain )
        {
            // Grab the text item and verify its existence
            CTextItem* pTextItem = lua_totextitem ( luaVM, 1 ) ;
            if ( pTextItem )
            {
                luaMain->DestroyTextItem ( pTextItem );

                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "textDestroyTextItem" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTextDefs::textDisplayAddText ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
    {
        CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( luaMain )
        {
            // Grab the display and the item and check their validty
            CTextDisplay* pTextDisplay = lua_totextdisplay ( luaVM, 1 );
            CTextItem* pTextItem = lua_totextitem ( luaVM, 2 );
            if ( pTextDisplay && pTextItem )
            {
                pTextDisplay->Add ( pTextItem );

                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "textDisplayAddText" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTextDefs::textDisplayRemoveText ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
    {
        CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( luaMain )
        {
            // Grab the display and the item and check their validty
            CTextDisplay* pTextDisplay = lua_totextdisplay ( luaVM, 1 );
            CTextItem* pTextItem = lua_totextitem ( luaVM, 2 );
            if ( pTextDisplay )
            {
                if ( pTextItem )
                {
                    pTextDisplay->Remove ( pTextItem );

                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
                else
                    m_pScriptDebugging->LogBadPointer ( luaVM, "textDisplayRemoveText", "textItem", 2 );
            }
            else
                    m_pScriptDebugging->LogBadPointer ( luaVM, "textDisplayRemoveText", "textDisplay", 1 );
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "textDisplayRemoveText" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTextDefs::textDisplayAddObserver ( lua_State* luaVM )
{
    // Check argument validty
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
    {
        CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( luaMain )
        {
            // Grab the pointers and verify them
            CTextDisplay* pTextDisplay = lua_totextdisplay ( luaVM, 1 );
            CPlayer* pPlayer = lua_toplayer ( luaVM, 2 );
            if ( pTextDisplay && pPlayer )
            {
                pTextDisplay->AddObserver ( pPlayer );

                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "textDisplayAddObserver" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTextDefs::textDisplayRemoveObserver ( lua_State* luaVM )
{
    // Check argument validty
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
    {
        CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( luaMain )
        {
            // Grab the pointers and verify them
            CTextDisplay* pTextDisplay = lua_totextdisplay ( luaVM, 1 );
            CPlayer* pPlayer = lua_toplayer ( luaVM, 2 );
            if ( pTextDisplay )
            {
                if ( pPlayer )
                {
                    pTextDisplay->RemoveObserver ( pPlayer );

                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
                else
                    m_pScriptDebugging->LogBadPointer ( luaVM, "textDisplayRemoveObserver", "observer", 2 );
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "textDisplayRemoveObserver", "textDisplay", 1 );
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "textDisplayRemoveObserver" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaTextDefs::textDisplayIsObserver ( lua_State* luaVM )
{
    // Check argument validty
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
    {
        CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( luaMain )
        {
            // Grab the pointers and verify them
            CTextDisplay* pTextDisplay = lua_totextdisplay ( luaVM, 1 );
            CPlayer* pPlayer = lua_toplayer ( luaVM, 2 );
            if ( pTextDisplay )
            {
                if ( pPlayer )
                {
                    if ( pTextDisplay -> IsObserver ( pPlayer ) )
                        lua_pushboolean ( luaVM, true );
                    else
                        lua_pushboolean ( luaVM, false );

                    return 1;
                }
                else
                    m_pScriptDebugging->LogBadPointer ( luaVM, "textDisplayIsObserver", "observer", 2 );
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "textDisplayIsObserver", "textDisplay", 1 );
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "textDisplayIsObserver" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaTextDefs::textDisplayGetObservers ( lua_State* luaVM )
{
    // Check argument validty
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA)
    {
        CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( luaMain )
        {
            // Grab the pointer and verify it
            CTextDisplay* pTextDisplay = lua_totextdisplay ( luaVM, 1 );
            if ( pTextDisplay )
            {
                // We want a table obviously
                lua_newtable ( luaVM );

                // And make the text-display list all the observers
                pTextDisplay -> GetObservers ( luaVM );
                return 1;
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "textDisplayGetObservers", "textDisplay", 1 );
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "textDisplayGetObservers" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaTextDefs::textItemSetText ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TSTRING )
    {
        CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( luaMain )
        {
            CTextItem* pTextItem = lua_totextitem ( luaVM, 1 );
            const char* szText = lua_tostring ( luaVM, 2 );
            if ( szText && strlen( szText ) < 1024 && pTextItem )
            {
                pTextItem->SetText ( szText );
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "textItemSetText" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTextDefs::textItemGetText ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( luaMain )
        {
            CTextItem* pTextItem = lua_totextitem ( luaVM, 1 );
            if ( pTextItem )
            {
                char szBuffer[1024];
                pTextItem->GetText ( szBuffer, 1024 );
                lua_pushstring ( luaVM, szBuffer );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "textItemGetText" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTextDefs::textItemSetScale ( lua_State* luaVM )
{
    CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( luaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
             lua_type ( luaVM, 2 ) == LUA_TNUMBER )
        {
            CTextItem* pTextItem = lua_totextitem ( luaVM, 1 );
            if ( pTextItem )
            {
                pTextItem->SetScale ( static_cast < float > ( lua_tonumber ( luaVM, 2 ) ) );

                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "textItemSetScale" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTextDefs::textItemGetScale ( lua_State* luaVM )
{
    CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( luaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
        {
            CTextItem* pTextItem = lua_totextitem ( luaVM, 1 );
            if ( pTextItem )
            {
                lua_pushnumber ( luaVM, pTextItem->GetScale () );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "textItemGetScale" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTextDefs::textItemSetPosition ( lua_State* luaVM )
{
    CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( luaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
             lua_type ( luaVM, 2 ) == LUA_TNUMBER && 
             lua_type ( luaVM, 3 ) == LUA_TNUMBER )
        {
            CTextItem* pTextItem = lua_totextitem ( luaVM, 1 );
            if ( pTextItem )
            {
                CVector2D vecPosition ( static_cast < float > ( lua_tonumber ( luaVM, 2 ) ), static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );
                pTextItem->SetPosition ( vecPosition );

                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "textItemSetPosition" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTextDefs::textItemGetPosition ( lua_State* luaVM )
{
    CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( luaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA  )
        {
            CTextItem* pTextItem = lua_totextitem ( luaVM, 1 );
            if ( pTextItem )
            {
                CVector2D vecPosition = pTextItem->GetPosition ();
                lua_pushnumber ( luaVM, vecPosition.fX );
                lua_pushnumber ( luaVM, vecPosition.fY );
                return 2;
            }
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "textItemGetPosition" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTextDefs::textItemSetColor ( lua_State* luaVM )
{
    CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( luaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
             lua_type ( luaVM, 2 ) == LUA_TNUMBER && 
             lua_type ( luaVM, 3 ) == LUA_TNUMBER &&
             lua_type ( luaVM, 4 ) == LUA_TNUMBER &&
             lua_type ( luaVM, 5 ) == LUA_TNUMBER )
        {
            CTextItem* pTextItem = lua_totextitem ( luaVM, 1 );
            if ( pTextItem )
            {
                SColor color;
                color.R = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
                color.G = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );
                color.B = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );
                color.A = static_cast < unsigned char > ( lua_tonumber ( luaVM, 5 ) );
                pTextItem->SetColor ( color );

                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "textItemSetColor" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTextDefs::textItemGetColor ( lua_State* luaVM )
{
    CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( luaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
        {
            CTextItem* pTextItem = lua_totextitem ( luaVM, 1 );
            if ( pTextItem )
            {
                SColor color = pTextItem->GetColor ();
                lua_pushnumber ( luaVM, color.R );
                lua_pushnumber ( luaVM, color.G );
                lua_pushnumber ( luaVM, color.B );
                lua_pushnumber ( luaVM, color.A );
                return 4;
            }
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "textItemGetColor" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTextDefs::textItemSetPriority ( lua_State* luaVM )
{
    CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( luaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
            ( lua_type ( luaVM, 2 ) == LUA_TSTRING || lua_type ( luaVM, 2 ) == LUA_TNUMBER ) )
        {
            CTextItem* pTextItem = lua_totextitem ( luaVM, 1 );
            if ( pTextItem )
            {
                int priority = PRIORITY_LOW;
                if ( lua_type ( luaVM, 2 ) == LUA_TSTRING )
                {
                    const char * szTemp = lua_tostring ( luaVM, 2 );
                    if ( strcmp ( szTemp, "low" ) == 0 )
                        priority = PRIORITY_LOW;
                    else if ( strcmp ( szTemp, "medium" ) == 0 )
                        priority = PRIORITY_MEDIUM;
                    else if ( strcmp ( szTemp, "high" ) == 0 )
                        priority = PRIORITY_HIGH;
                }
                else
                {
                    priority = static_cast < int > ( lua_tonumber ( luaVM, 2 ) );
                }

                pTextItem->SetPriority ( (eTextPriority)priority );

                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "textItemSetPriority" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTextDefs::textItemGetPriority ( lua_State* luaVM )
{
    CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( luaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
        {
            CTextItem* pTextItem = lua_totextitem ( luaVM, 1 );
            if ( pTextItem )
            {
                lua_pushnumber ( luaVM, pTextItem->GetPriority () );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "textItemGetPriority" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}
