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
    CTextDisplay * pTextDisplay;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTextDisplay );

    if ( !argStream.HasErrors ( ) )
    {        
        CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( luaMain )
        {
            luaMain->DestroyDisplay ( pTextDisplay );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTextDefs::textCreateTextItem ( lua_State* luaVM )
{
    
    SString strText, strHorzAlign, strVertAlign;
    float fX, fY, fScale;
    int iPriority;
    SColorRGBA color ( 255, 255, 255, 255 );
    unsigned char ucFormat, ucShadowAlpha;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strText, "" );
    argStream.ReadNumber ( fX, 0.5f );
    argStream.ReadNumber ( fY, 0.5f );

    if ( argStream.NextIsString( ) )
    {
        SString strPriority;
        argStream.ReadString ( strPriority );

        if ( strPriority == "low" )         iPriority = PRIORITY_LOW;
        else if ( strPriority == "medium" ) iPriority = PRIORITY_MEDIUM;
        else if ( strPriority == "high" )   iPriority = PRIORITY_HIGH;

    }
    else
    {
        argStream.ReadNumber(iPriority, PRIORITY_LOW);
    }
    
    argStream.ReadNumber( color.R, 255 );
    argStream.ReadNumber( color.G, 255 );
    argStream.ReadNumber( color.B, 255 );
    argStream.ReadNumber( color.A, 255 );
    argStream.ReadNumber( fScale, 1 );
    argStream.ReadString( strHorzAlign, "left" );
    argStream.ReadString( strVertAlign, "top" );
    argStream.ReadNumber( ucShadowAlpha, 0);

    if ( !argStream.HasErrors ( ) )
    {
        
        if ( strHorzAlign == "center" ) 
            ucFormat |= 0x00000001; // DT_CENTER
        else if ( strHorzAlign == "right" )
            ucFormat |= 0x00000002; // DT_RIGHT

        if ( strVertAlign == "center" )
            ucFormat |= 0x00000004; // DT_VCENTER
        else if ( strVertAlign == "bottom" )
            ucFormat |= 0x00000008; // DT_BOTTOM
        
        // Grab our virtual machine
        CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( luaMain )
        {
            CTextItem* pTextItem = luaMain->CreateTextItem ( strText, fX, fY, (eTextPriority) iPriority, color, fScale, ucFormat, ucShadowAlpha );
            lua_pushtextitem ( luaVM, pTextItem );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );


    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTextDefs::textDestroyTextItem ( lua_State* luaVM )
{
    CTextItem * pTextItem;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTextItem );

    if ( !argStream.HasErrors ( ) )
    {
        CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( luaMain )
        { 
            luaMain->DestroyTextItem ( pTextItem );

            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTextDefs::textDisplayAddText ( lua_State* luaVM )
{
    CTextDisplay * pTextDisplay;
    CTextItem * pTextItem;  

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTextDisplay );
    argStream.ReadUserData ( pTextItem );

    if ( !argStream.HasErrors ( ) )
    {
        pTextDisplay->Add ( pTextItem );

        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTextDefs::textDisplayRemoveText ( lua_State* luaVM )
{
    CTextDisplay * pTextDisplay;
    CTextItem * pTextItem;  

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTextDisplay );
    argStream.ReadUserData ( pTextItem );

    if ( !argStream.HasErrors ( ) )
    {
        pTextDisplay->Remove ( pTextItem );

        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaTextDefs::textDisplayAddObserver ( lua_State* luaVM )
{
    CTextDisplay * pTextDisplay;
    CPlayer * pPlayer;  

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTextDisplay );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors ( ) )
    {
        pTextDisplay->AddObserver ( pPlayer );

        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTextDefs::textDisplayRemoveObserver ( lua_State* luaVM )
{
    CTextDisplay * pTextDisplay;
    CPlayer * pPlayer;  

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTextDisplay );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors ( ) )
    {
        pTextDisplay->RemoveObserver ( pPlayer );

        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaTextDefs::textDisplayIsObserver ( lua_State* luaVM )
{
    CTextDisplay * pTextDisplay;
    CPlayer * pPlayer;  

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTextDisplay );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors ( ) )
    {
        lua_pushboolean ( luaVM, pTextDisplay->IsObserver ( pPlayer ) );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaTextDefs::textDisplayGetObservers ( lua_State* luaVM )
{
    CTextDisplay * pTextDisplay;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTextDisplay );

    if ( !argStream.HasErrors ( ) )
    {
        lua_newtable ( luaVM );
        pTextDisplay->GetObservers ( luaVM );

        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaTextDefs::textItemSetText ( lua_State* luaVM )
{
    CTextItem * pTextItem;
    SString strText;  

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTextItem );
    argStream.ReadString ( strText );

    if ( !argStream.HasErrors ( ) )
    {
        pTextItem->SetText( strText );

        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaTextDefs::textItemGetText ( lua_State* luaVM )
{
    CTextItem * pTextItem;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTextItem );

    if ( !argStream.HasErrors ( ) )
    {
        lua_pushstring ( luaVM, pTextItem->GetText() );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaTextDefs::textItemSetScale ( lua_State* luaVM )
{
    CTextItem * pTextItem;
    float fScale;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTextItem );
    argStream.ReadNumber ( fScale );

    if ( !argStream.HasErrors ( ) )
    {
        pTextItem->SetScale ( fScale ); 

        lua_pushboolean ( luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTextDefs::textItemGetScale ( lua_State* luaVM )
{
    CTextItem * pTextItem;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTextItem );

    if ( !argStream.HasErrors ( ) )
    {
        lua_pushnumber ( luaVM, pTextItem->GetScale ( ) );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaTextDefs::textItemSetPosition ( lua_State* luaVM )
{
    CTextItem * pTextItem;
    CVector2D vecPosition;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTextItem );
    argStream.ReadNumber ( vecPosition.fX );
    argStream.ReadNumber ( vecPosition.fY );

    if ( !argStream.HasErrors ( ) )
    {
        pTextItem->SetPosition ( vecPosition );

        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaTextDefs::textItemGetPosition ( lua_State* luaVM )
{
    CTextItem * pTextItem;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTextItem );

    if ( !argStream.HasErrors ( ) )
    {
        CVector2D vecPosition = pTextItem->GetPosition ();
        lua_pushnumber ( luaVM, vecPosition.fX );
        lua_pushnumber ( luaVM, vecPosition.fY );
        return 2;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTextDefs::textItemSetColor ( lua_State* luaVM )
{
    CTextItem * pTextItem;
    SColor color;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTextItem );
    argStream.ReadNumber ( color.R );
    argStream.ReadNumber ( color.G );
    argStream.ReadNumber ( color.B);
    argStream.ReadNumber ( color.A );

    if ( !argStream.HasErrors ( ) )
    {
        pTextItem->SetColor ( color );

        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaTextDefs::textItemGetColor ( lua_State* luaVM )
{
    CTextItem * pTextItem;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTextItem );

    if ( !argStream.HasErrors ( ) )
    { 
        SColor color = pTextItem->GetColor ();
        lua_pushnumber ( luaVM, color.R );
        lua_pushnumber ( luaVM, color.G );
        lua_pushnumber ( luaVM, color.B );
        lua_pushnumber ( luaVM, color.A );
        return 4;

    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTextDefs::textItemSetPriority ( lua_State* luaVM )
{
    CTextItem * pTextItem;
    int iPriority;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTextItem );
    if ( argStream.NextIsString( ) )
    {
        SString strPriority;
        argStream.ReadString ( strPriority );

        if ( strPriority == "low" )         iPriority = PRIORITY_LOW;
        else if ( strPriority == "medium" ) iPriority = PRIORITY_MEDIUM;
        else if ( strPriority == "high" )   iPriority = PRIORITY_HIGH;

    }
    else
    {
        argStream.ReadNumber(iPriority);
    }

    if ( !argStream.HasErrors ( ) )
    {
        pTextItem->SetPriority ( (eTextPriority)iPriority );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaTextDefs::textItemGetPriority ( lua_State* luaVM )
{
    CTextItem * pTextItem;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pTextItem );

    if ( !argStream.HasErrors ( ) )
    {
        lua_pushnumber( luaVM, pTextItem->GetPriority( ) );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}
