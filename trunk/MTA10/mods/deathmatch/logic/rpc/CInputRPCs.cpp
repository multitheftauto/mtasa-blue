/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CInputRPCs.cpp
*  PURPOSE:     Input remote procedure calls
*  DEVELOPERS:  Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#include <StdInc.h>
#include "CInputRPCs.h"

void CInputRPCs::LoadFunctions ( void )
{
    AddHandler ( BIND_KEY, BindKey, "BindKey" );
    AddHandler ( UNBIND_KEY, UnbindKey, "UnbindKey" );
    AddHandler ( TOGGLE_CONTROL_ABILITY, ToggleControl, "ToggleControl" );
    AddHandler ( TOGGLE_ALL_CONTROL_ABILITY, ToggleAllControls, "ToggleAllControls" );
    AddHandler ( SET_CONTROL_STATE, SetControlState, "SetControlState" );
    AddHandler ( SHOW_CURSOR, ShowCursor, "ShowCursor" );
	AddHandler ( SHOW_CHAT, ShowChat, "ShowChat" );
}


void CInputRPCs::BindKey ( NetBitStreamInterface& bitStream )
{
    CKeyBindsInterface* pKeyBinds = g_pCore->GetKeyBinds ();
    if ( pKeyBinds )
    {
        unsigned char ucKeyLength = 0;
        if ( bitStream.Read ( ucKeyLength ) )
        {
            char* szKey = new char [ ucKeyLength + 1 ];            
			szKey [ ucKeyLength ] = NULL;

            unsigned char ucKeyState;
            if ( bitStream.Read ( szKey, ucKeyLength ) && bitStream.Read ( ucKeyState ) )
            {               
                bool bState = ( ucKeyState == 1 );
                const SBindableKey* pKey = pKeyBinds->GetBindableFromKey ( szKey );
                if ( pKey )
                {
                    // Make sure we only have it once
                    if ( !pKeyBinds->FunctionExists ( szKey, CClientGame::StaticProcessServerKeyBind, true, bState ) )
                    {
                        pKeyBinds->AddFunction ( pKey, CClientGame::StaticProcessServerKeyBind, bState, false );
                    }
                }
                else
                {
                    SBindableGTAControl* pControl = pKeyBinds->GetBindableFromControl ( szKey );
                    if ( pControl )
                    {
                        // Make sure we only have it once
                        if ( !pKeyBinds->ControlFunctionExists ( szKey, CClientGame::StaticProcessServerControlBind, true, bState ) )
                        {
                            pKeyBinds->AddControlFunction ( pControl, CClientGame::StaticProcessServerControlBind, bState );
                        }
                    }
                }
            }
            delete [] szKey;
        }
    }
}


void CInputRPCs::UnbindKey ( NetBitStreamInterface& bitStream )
{
    CKeyBindsInterface* pKeyBinds = g_pCore->GetKeyBinds ();
    if ( pKeyBinds )
    {        
        unsigned char ucKeyLength = 0;
        if ( bitStream.Read ( ucKeyLength ) )
        {
            char* szKey = new char [ ucKeyLength + 1 ];
			szKey [ ucKeyLength ] = NULL;

            unsigned char ucKeyState;
            if ( bitStream.Read ( szKey, ucKeyLength ) && bitStream.Read ( ucKeyState ) )
			{
                bool bState = ( ucKeyState == 1 );
                const SBindableKey* pKey = pKeyBinds->GetBindableFromKey ( szKey );
                if ( pKey )
                {
                    pKeyBinds->RemoveFunction ( szKey, CClientGame::StaticProcessServerKeyBind, true, bState );
                }
                else
                {
                    SBindableGTAControl* pControl = pKeyBinds->GetBindableFromControl ( szKey );
                    if ( pControl )
                    {
                        pKeyBinds->RemoveControlFunction ( szKey, CClientGame::StaticProcessServerControlBind, true, bState );
                    }
                }
            }
            delete [] szKey;
        }
    }
}


void CInputRPCs::ToggleControl ( NetBitStreamInterface& bitStream )
{
    unsigned char ucControlLength = 0;
    if ( bitStream.Read ( ucControlLength ) )
    {
        char* szControl = new char [ ucControlLength + 1 ];
		szControl [ ucControlLength ] = NULL;

        unsigned char ucEnabled;
        if ( bitStream.Read ( szControl, ucControlLength ) && bitStream.Read ( ucEnabled ) )
        {
            if ( szControl [ 0 ] )
            {
                g_pCore->GetKeyBinds ()->SetControlEnabled ( szControl, ucEnabled == 1 );
            }
        }
        delete [] szControl;
    }
}


void CInputRPCs::ToggleAllControls ( NetBitStreamInterface& bitStream )
{
    unsigned char ucGTAControls, ucMTAControls, ucEnabled;
    if ( bitStream.Read ( ucGTAControls ) &&
         bitStream.Read ( ucMTAControls ) &&
         bitStream.Read ( ucEnabled ) )
    {
        g_pCore->GetKeyBinds ()->SetAllControlsEnabled ( ucGTAControls == 1, ucMTAControls == 1, ucEnabled == 1 );
    }
}


void CInputRPCs::SetControlState ( NetBitStreamInterface& bitStream )
{
    unsigned char ucControlLength = 0;
    if ( bitStream.Read ( ucControlLength ) )
    {
        char* szControl = new char [ ucControlLength + 1 ];
		szControl [ ucControlLength ] = NULL;

        unsigned char ucState;
        if ( bitStream.Read ( szControl, ucControlLength ) && bitStream.Read ( ucState ) )
        {
            if ( *szControl )
            {
                CKeyBindsInterface* pKeyBinds = g_pCore->GetKeyBinds ();
                if ( pKeyBinds )
                {
                    SBindableGTAControl* pControl = pKeyBinds->GetBindableFromControl ( szControl );
                    if ( pControl )
                    {
                        pControl->bState = ucState == 1;
                    }
                }
            }
        }
        delete [] szControl;
    }
}


void CInputRPCs::ShowCursor ( NetBitStreamInterface& bitStream )
{
    unsigned char ucShow;
    unsigned short usResource;
    if ( bitStream.Read ( ucShow ) &&
         bitStream.Read ( usResource ) )
    {
        CResource* pResource = g_pClientGame->GetResourceManager ()->GetResource ( usResource );
        if ( pResource )
        {
            pResource->ShowCursor ( ucShow == 1 );
        }
    }
}


void CInputRPCs::ShowChat ( NetBitStreamInterface& bitStream )
{
    unsigned char ucShow;
    if ( bitStream.Read ( ucShow ) )
    {
		g_pCore->SetChatVisible ( ucShow == 1 );
    }
}