/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CRadarSA.cpp
 *  PURPOSE:     Game radar
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <CRect.h>
#include "CRadarSA.h"

CMarkerSA* Markers[MAX_MARKERS];

CRadarSA::CRadarSA()
{
    for (int i = 0; i < MAX_MARKERS; i++)
        Markers[i] = new CMarkerSA((CMarkerSAInterface*)(ARRAY_CMarker + i * sizeof(CMarkerSAInterface)));
}

CRadarSA::~CRadarSA()
{
    for (int i = 0; i < MAX_MARKERS; i++)
    {
        if (Markers[i])
            delete Markers[i];
    }
}

CMarker* CRadarSA::CreateMarker(CVector* vecPosition)
{
    CMarkerSA* marker;
    marker = (CMarkerSA*)GetFreeMarker();
    if (marker)
    {
        marker->Init();
        marker->SetPosition(vecPosition);
    }

    return marker;
}

CMarker* CRadarSA::GetFreeMarker()
{
    int Index;
    Index = 0;
    while ((Index < MAX_MARKERS) && (Markers[Index]->GetInterface()->bTrackingBlip))
    {
        Index++;
    }
    if (Index >= MAX_MARKERS)
        return NULL;
    else
        return Markers[Index];
}

void CRadarSA::DrawAreaOnRadar(float fX1, float fY1, float fX2, float fY2, const SharedUtil::SColor color)
{
    // Convert color to required abgr at the last moment
    unsigned long abgr = color.A << 24 | color.B << 16 | color.G << 8 | color.R;
    CRect         myRect(fX1, fY2, fX2, fY1);
    DWORD         dwFunc = FUNC_DrawAreaOnRadar;
    // clang-format off
    __asm
    {
        push    eax

        push    1           //bool
        lea     eax, abgr
        push    eax
        lea     eax, myRect
        push    eax
        call    dwFunc
        add     esp, 12

        pop     eax
    }
    // clang-format on
}

#include <d3d9.h>
#include <d3dx9.h>
#include <game/RenderWare.h>
#include "gamesa_renderware.h"

using D3DXCreateTextureFromFileExA_t = HRESULT(WINAPI*)(IDirect3DDevice9* device, LPCSTR srcFile, UINT width, UINT height, UINT mipLevels, DWORD usage,
                                                        D3DFORMAT format, D3DPOOL pool, DWORD filter, DWORD mipFilter, D3DCOLOR colorKey,
                                                        D3DXIMAGE_INFO* srcInfo, PALETTEENTRY* palette, LPDIRECT3DTEXTURE9* texture);

static D3DXCreateTextureFromFileExA_t d3dxCreateTextureFromFileExFunc = nullptr;

static D3DXCreateTextureFromFileExA_t GetD3DXCreateTextureFunc()
{
    if (d3dxCreateTextureFromFileExFunc)
        return d3dxCreateTextureFromFileExFunc;

    HMODULE d3dxModule = GetModuleHandleA("d3dx9_42.dll");
    if (!d3dxModule)
        d3dxModule = GetModuleHandleA("d3dx9_43.dll");
    if (!d3dxModule)
        d3dxModule = LoadLibraryA("d3dx9_42.dll");
    if (!d3dxModule)
        d3dxModule = LoadLibraryA("d3dx9_43.dll");

    if (d3dxModule)
        d3dxCreateTextureFromFileExFunc = reinterpret_cast<D3DXCreateTextureFromFileExA_t>(GetProcAddress(d3dxModule, "D3DXCreateTextureFromFileExA"));

    return d3dxCreateTextureFromFileExFunc;
}

static IDirect3DTexture9* radarTileD3DTextures[144] = {nullptr};

static IDirect3DTexture9* GetRadarTileD3DTexture(int tileIndex)
{
    if (tileIndex < 0 || tileIndex >= 144)
        return nullptr;

    if (radarTileD3DTextures[tileIndex])
        return radarTileD3DTextures[tileIndex];

    IDirect3DDevice9* device = *reinterpret_cast<IDirect3DDevice9**>(0xC97C28);
    if (!device)
        return nullptr;

    auto createTexture = GetD3DXCreateTextureFunc();
    if (!createTexture)
        return nullptr;

    SString tilePath1 = SharedUtil::CalcMTASAPath(SharedUtil::PathJoin("cgui", "images", "radar_tiles", SString("radar%02d.png", tileIndex)));
    SString tilePath2 = SharedUtil::CalcMTASAPath(SharedUtil::PathJoin("MTA", "cgui", "images", "radar_tiles", SString("radar%02d.png", tileIndex)));
    SString tilePath3 = SharedUtil::CalcMTASAPath(SharedUtil::PathJoin("mta", "cgui", "images", "radar_tiles", SString("radar%02d.png", tileIndex)));

    SString finalPath;
    if (SharedUtil::FileExists(tilePath1))
        finalPath = tilePath1;
    else if (SharedUtil::FileExists(tilePath2))
        finalPath = tilePath2;
    else if (SharedUtil::FileExists(tilePath3))
        finalPath = tilePath3;

    if (!finalPath.empty())
    {
        createTexture(device, *finalPath, 256, 256, 1, 0, D3DFMT_DXT5, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, nullptr, nullptr,
                      &radarTileD3DTextures[tileIndex]);
    }

    return radarTileD3DTextures[tileIndex];
}

static RwTexture* radarRwTextures[144] = {nullptr};

static RwTexture* __cdecl GetRadarTextureForChunk(int chunkX, int chunkY)
{
    if (chunkX < 0 || chunkX >= 12 || chunkY < 0 || chunkY >= 12)
        return nullptr;

    int tileIndex = chunkY * 12 + chunkX;
    if (radarRwTextures[tileIndex])
        return radarRwTextures[tileIndex];

    IDirect3DTexture9* d3dTexture = GetRadarTileD3DTexture(tileIndex);
    if (!d3dTexture)
        return nullptr;

    auto rwRasterCreate = reinterpret_cast<RwRaster*(__cdecl*)(int, int, int, int)>(0x7FB230);
    auto rwTextureCreate = reinterpret_cast<RwTexture*(__cdecl*)(RwRaster*)>(0x7F37C0);

    // Create a genuine RenderWare raster with format rwRASTERFORMAT8888 (0x0500) and type rwRASTERTYPETEXTURE (4)
    RwRaster* raster = rwRasterCreate(256, 256, 32, 4 | 0x0500);
    if (!raster)
        return nullptr;

    raster->renderResource = d3dTexture;

    RwTexture* texture = rwTextureCreate(raster);
    if (!texture)
        return nullptr;

    radarRwTextures[tileIndex] = texture;
    return texture;
}

static RwTexture* GetRadarChunkTextureSafe(int chunkX, int chunkY, RwTexDictionary* dict)
{
    RwTexture* hdTexture = GetRadarTextureForChunk(chunkX, chunkY);
    if (hdTexture)
        return hdTexture;

    auto GetFirstTextureFunc = reinterpret_cast<RwTexture*(__cdecl*)(RwTexDictionary*)>(0x7316A0);
    return GetFirstTextureFunc(dict);
}

static void __declspec(naked) HOOK_GetRadarChunkTexture_DrawRadarSection()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;
    __asm {
        pushad
        push    [esp + 36]  // dict (esp + 32 from pushad + 4 from call return address)
        push    ebp  // chunkY
        push    esi  // chunkX
        call    GetRadarChunkTextureSafe
        add     esp, 12
        mov     [esp + 28], eax  // Overwrite EAX in pushad struct with returned RwTexture*
        popad
        retn
    }
}

static void __declspec(naked) HOOK_GetRadarChunkTexture_DrawRadarSectionMap()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;
    __asm {
        pushad
        push    [esp + 36]  // dict
        push    edi  // chunkY
        push    ebx  // chunkX
        call    GetRadarChunkTextureSafe
        add     esp, 12
        mov     [esp + 28], eax  // Overwrite EAX in pushad struct with returned RwTexture*
        popad
        retn
    }
}

static uint  lastRangeUpdateTime = 0;
static float currentRadarRange = 290.0f;

static void UpdateDynamicRadarRange()
{
    uint  currentTime = GetTickCount32();
    float deltaTime = (lastRangeUpdateTime == 0) ? 0.016f : std::clamp(static_cast<float>(currentTime - lastRangeUpdateTime) / 1000.0f, 0.001f, 0.1f);
    lastRangeUpdateTime = currentTime;

    // Base range on foot
    float targetRange = 180.0f;

    using FindPlayerVehicle_t = void*(__cdecl*)(int playerId, bool includeRemote);
    auto FindPlayerVehicle = reinterpret_cast<FindPlayerVehicle_t>(0x56E0D0);

    void* vehicle = FindPlayerVehicle(-1, 0);
    if (vehicle)
    {
        // Velocity vector in GTA:SA physical entity (offset 0x44)
        auto* moveSpeed = reinterpret_cast<CVector*>(reinterpret_cast<uintptr_t>(vehicle) + 0x44);
        float speed = std::sqrt(moveSpeed->fX * moveSpeed->fX + moveSpeed->fY * moveSpeed->fY + moveSpeed->fZ * moveSpeed->fZ);

        // Vehicle subtype (offset 0x590): 3 = Heli, 4 = Plane, 5 = Boat
        uint vehicleType = *reinterpret_cast<uint*>(reinterpret_cast<uintptr_t>(vehicle) + 0x590);

        if (vehicleType == 4 || vehicleType == 3)  // Aircraft (Planes & Helicopters)
        {
            float speedFactor = std::clamp((speed - 0.25f) / 0.65f, 0.0f, 1.0f);
            targetRange = 280.0f + speedFactor * 220.0f;  // 280m to 500m
        }
        else if (vehicleType == 5)  // Boats
        {
            float speedFactor = std::clamp((speed - 0.3f) / 0.6f, 0.0f, 1.0f);
            targetRange = 180.0f + speedFactor * 140.0f;  // 180m to 320m
        }
        else  // Ground vehicles (Cars, Bikes, Trucks)
        {
            // Speed threshold before zooming out (approx 55 km/h)
            if (speed > 0.3f)
            {
                float speedFactor = std::clamp((speed - 0.3f) / (0.9f - 0.3f), 0.0f, 1.0f);
                targetRange = 180.0f + speedFactor * 170.0f;  // 180m (idle/slow) up to 350m (high speed freeway)
            }
            else
            {
                targetRange = 180.0f;
            }
        }
    }

    // Frame-rate independent exponential smoothing for 100% butter-smooth zoom transition
    float smoothSpeed = 3.5f;
    currentRadarRange += (targetRange - currentRadarRange) * (1.0f - std::exp(-smoothSpeed * deltaTime));

    // Update GTA:SA native radar range variable
    *reinterpret_cast<float*>(0xBA8314) = currentRadarRange;
}

static void HOOK_CRadar_DrawRadarMask()
{
    UpdateDynamicRadarRange();

    // Force linear texture filtering for radar mask and tiles (rwRENDERSTATETEXTUREFILTER = rwFILTERLINEAR)
    reinterpret_cast<int (*)(int, void*)>(0x7F9D50)(9, reinterpret_cast<void*>(2));

    // Call original CRadar::DrawRadarMask() at 0x585700
    reinterpret_cast<void (*)()>(0x585700)();
}

void CRadarSA::StaticSetHooks()
{
    // Hook texture lookup at 0x5861AF (CRadar::DrawRadarSection) and 0x586596 (CRadar::DrawRadarSectionMap) for HD PNG radar tiles
    HookInstallCall(0x5861AF, reinterpret_cast<DWORD>(HOOK_GetRadarChunkTexture_DrawRadarSection));
    HookInstallCall(0x586596, reinterpret_cast<DWORD>(HOOK_GetRadarChunkTexture_DrawRadarSectionMap));

    // Hook call to DrawRadarMask at 0x586887 inside CRadar::DrawRadarMap to update dynamic speed-based zoom every frame
    HookInstallCall(0x586887, reinterpret_cast<DWORD>(HOOK_CRadar_DrawRadarMask));
}
