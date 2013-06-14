/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        D3DStuff.cpp
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "d3d9.h"

namespace
{
    SString GUIDToString ( const GUID& g )
    {
        return SString (
                        "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                        g.Data1, g.Data2, g.Data3, g.Data4[0], g.Data4[1], g.Data4[2],
                        g.Data4[3], g.Data4[4], g.Data4[5], g.Data4[6], g.Data4[7] );
    }

    SString ToString( const D3DADAPTER_IDENTIFIER9& a )
    {
        return SString(
                        "    Driver:%s\n"
                        "    Description:%s\n"
                        "    DeviceName:%s\n"
                        "    DriverVersion:0x%08x 0x%08x\n"
                        "    VendorId:0x%08x  DeviceId:0x%08x  SubSysId:0x%08x  Revision:0x%08x  WHQLLevel:0x%08x\n"
                        "    DeviceIdentifier:%s"
                        , a.Driver
                        , a.Description
                        , a.DeviceName
                        , a.DriverVersion.HighPart
                        , a.DriverVersion.LowPart
                        , a.VendorId
                        , a.DeviceId
                        , a.SubSysId
                        , a.Revision
                        , a.WHQLLevel
                        , *GUIDToString( a.DeviceIdentifier )
                    );
    }

    SString ToString( const D3DDISPLAYMODE& a )
    {
        return SString(
                        " Width:%d"
                        " Height:%d"
                        " RefreshRate:%d"
                        " Format:%d"
                        , a.Width
                        , a.Height
                        , a.RefreshRate
                        , a.Format
                    );
    }

    SString ToString( const D3DCAPS9& a )
    {
        return SString(
                        " VertexShaderVersion:0x%08x"
                        " PixelShaderVersion:0x%08x"
                        " DeclTypes:0x%03x"
                        , a.VertexShaderVersion
                        , a.PixelShaderVersion
                        , a.DeclTypes
                    );
    }

    IDirect3D9* pD3D9 = NULL;
}


//////////////////////////////////////////////////////////
//
// BeginD3DStuff
//
// Look all busy and important in case any graphic drivers are looking
//
//////////////////////////////////////////////////////////
void BeginD3DStuff( void )
{
    HRESULT hr;

    pD3D9 = Direct3DCreate9( D3D_SDK_VERSION );

    if ( !pD3D9 )
    {
        WriteDebugEvent( "D3DStuff - Direct3DCreate9 failed" );
        return;
    }

    WriteDebugEvent( "D3DStuff -------------------------" );
    WriteDebugEvent( SString( "D3DStuff - Direct3DCreate9: 0x%08x", pD3D9 ) );

    // Get info about each connected adapter
    uint uiNumAdapters = pD3D9->GetAdapterCount();
    WriteDebugEvent( SString( "D3DStuff - %d Adapters", uiNumAdapters ) );

    for ( uint i = 0 ; i < uiNumAdapters ; i++ )
    {
        D3DADAPTER_IDENTIFIER9 Identifier;
        hr = pD3D9->GetAdapterIdentifier( i, 0, &Identifier );
        if ( FAILED( hr ) )
            WriteDebugEvent( SString( "D3DStuff %d GetAdapterIdentifier failed %d", i, hr ) );
        else
            WriteDebugEvent( SString( "D3DStuff %d Identifier - %s", i, *ToString( Identifier ) ) );

        D3DDISPLAYMODE DisplayMode;
        hr = pD3D9->GetAdapterDisplayMode( i, &DisplayMode );
        if ( FAILED( hr ) )
            WriteDebugEvent( SString( "D3DStuff %d GetAdapterDisplayMode failed %d", i, hr ) );
        else
            WriteDebugEvent( SString( "D3DStuff %d DisplayMode - %s", i, *ToString( DisplayMode ) ) );

        HMONITOR hMonitor = pD3D9->GetAdapterMonitor( i );
        UINT ModeCount = pD3D9->GetAdapterModeCount( i, D3DFMT_X8R8G8B8 );
        WriteDebugEvent( SString( "D3DStuff %d  hMonitor:0x%08x  ModeCount:%d", i, hMonitor, ModeCount ) );

        D3DCAPS9 Caps9;
        hr = pD3D9->GetDeviceCaps( i, D3DDEVTYPE_HAL, &Caps9 );
        if ( FAILED( hr ) )
            WriteDebugEvent( SString( "D3DStuff %d GetDeviceCaps failed %d", i, hr ) );
        else
            WriteDebugEvent( SString( "D3DStuff %d Caps9 - %s ", i, *ToString( Caps9 ) ) );

        WriteDebugEvent( "D3DStuff -------------------------" );
    }
}


//////////////////////////////////////////////////////////
//
// EndD3DStuff
//
// Clean up
//
//////////////////////////////////////////////////////////
void EndD3DStuff( void )
{
    SAFE_RELEASE( pD3D9 );
}
