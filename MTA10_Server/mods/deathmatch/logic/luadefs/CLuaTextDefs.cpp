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
    CLuaCFunctions::AddFunction ( "textCreateDisplay", textCreateDisplay );
    CLuaCFunctions::AddFunction ( "textDestroyDisplay", textDestroyDisplay );
    CLuaCFunctions::AddFunction ( "textCreateTextItem", textCreateTextItem );
    CLuaCFunctions::AddFunction ( "textDestroyTextItem", textDestroyTextItem );

    CLuaCFunctions::AddFunction ( "textDisplayAddText", textDisplayAddText );
    CLuaCFunctions::AddFunction ( "textDisplayRemoveText", textDisplayRemoveText );
    CLuaCFunctions::AddFunction ( "textDisplayAddObserver", textDisplayAddObserver );
    CLuaCFunctions::AddFunction ( "textDisplayRemoveObserver", textDisplayRemoveObserver );
    CLuaCFunctions::AddFunction ( "textDisplayIsObserver", textDisplayIsObserver );
    CLuaCFunctions::AddFunction ( "textDisplayGetObservers", textDisplayGetObservers );

    CLuaCFunctions::AddFunction ( "textItemSetText", textItemSetText );
    CLuaCFunctions::AddFunction ( "textItemGetText", textItemGetText );
    CLuaCFunctions::AddFunction ( "textItemSetScale", textItemSetScale );
    CLuaCFunctions::AddFunction ( "textItemGetScale", textItemGetScale );
    CLuaCFunctions::AddFunction ( "textItemSetPosition", textItemSetPosition );
    CLuaCFunctions::AddFunction ( "textItemGetPosition", textItemGetPosition );
    CLuaCFunctions::AddFunction ( "textItemSetColor", textItemSetColor );
    CLuaCFunctions::AddFunction ( "textItemGetColor", textItemGetColor );
    CLuaCFunctions::AddFunction ( "textItemSetPriority", textItemSetPriority );
    CLuaCFunctions::AddFunction ( "textItemGetPriority", textItemGetPriority );
}

void CLuaTextDefs::AddClass ( lua_State* luaVM )
{
    //////////////////////////
    //  TextDisplay class
    //////////////////////////
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "create", "textCreateDisplay" );
    lua_classfunction ( luaVM, "destroy", "textDestroyDisplay" );
    lua_classfunction ( luaVM, "addObserver", "textDisplayAddObserver" );
    lua_classfunction ( luaVM, "removeObserver", "textDisplayRemoveObserver" );
    lua_classfunction ( luaVM, "addText", "textDisplayAddText" );
    lua_classfunction ( luaVM, "removeText", "textDisplayRemoveText" );

    lua_classfunction ( luaVM, "isObserver", "textDisplayIsObserver" );
    lua_classfunction ( luaVM, "getObservers", "textDisplayGetObservers" );

    lua_classvariable ( luaVM, "observers", NULL, "textDisplayGetObservers" );

    lua_registerclass ( luaVM, "TextDisplay" );
    //////////////////////////



    //////////////////////////
    //  TextItem class
    //////////////////////////
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "create", "textCreateTextItem" );
    lua_classfunction ( luaVM, "destroy", "textDestroyTextItem" );

    lua_classfunction ( luaVM, "getColor", "textItemGetColor" );
    lua_classfunction ( luaVM, "getPosition", "textItemGetPosition" );
    lua_classfunction ( luaVM, "getPriority", "textItemGetPriority" );
    lua_classfunction ( luaVM, "getScale", "textItemGetScale" );
    lua_classfunction ( luaVM, "getText", "textItemGetText" );

    lua_classfunction ( luaVM, "setColor", "textItemSetColor" );
    lua_classfunction ( luaVM, "setPosition", "textItemSetPosition" );
    lua_classfunction ( luaVM, "setPriority", "textItemSetPriority" );
    lua_classfunction ( luaVM, "setScale", "textItemSetScale" );
    lua_classfunction ( luaVM, "setText", "textItemSetText" );

    lua_classvariable ( luaVM, "priority", "textItemSetPriority", "textItemGetPriority" );
    lua_classvariable ( luaVM, "scale", "textItemSetScale", "textItemGetScale" );
    lua_classvariable ( luaVM, "text", "textItemSetText", "textItemGetText" );
    //lua_classvariable ( luaVM, "color", "textItemSetColor", "textItemGetColor", CLuaOOPDefs::TextItemSetColor, CLuaOOPDefs::TextItemGetColor ); // color
    lua_classvariable ( luaVM, "position", "textItemSetPosition", "textItemGetPosition" );

    lua_registerclass ( luaVM, "TextItem" );
    //////////////////////////
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
    unsigned char ucShadowAlpha;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strText, "" );
    argStream.ReadNumber ( fX, 0.5f );
    argStream.ReadNumber ( fY, 0.5f );

    if ( argStream.NextIsString( ) )
    {
        SString strPriority;
        argStream.ReadString ( strPriority );

        if ( strPriority == "high" )        iPriority = PRIORITY_HIGH;
        else if ( strPriority == "medium" ) iPriority = PRIORITY_MEDIUM;
        else                                iPriority = PRIORITY_LOW;

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
        unsigned char ucFormat = 0; 
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

        if ( strPriority == "high" )        iPriority = PRIORITY_HIGH;
        else if ( strPriority == "medium" ) iPriority = PRIORITY_MEDIUM;
        else                                iPriority = PRIORITY_LOW;

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
