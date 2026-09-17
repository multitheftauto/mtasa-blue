/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/core/D3DProxyDeviceGuids.h
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <initguid.h>

// GUID for identifying CProxyDirect3DDevice9 instances via QueryInterface
// {B4C5C640-B3F2-4888-9D30-D41A59CC72F0}
DEFINE_GUID(CProxyDirect3DDevice9_GUID, 0xb4c5c640, 0xb3f2, 0x4888, 0x9d, 0x30, 0xd4, 0x1a, 0x59, 0xcc, 0x72, 0xf0);

// Reset DEFINE_GUID to extern-declaration mode so subsequent GUIDs
// in other headers do not get storage allocated here.
#undef INITGUID
#include <guiddef.h>
