/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CRenderWareSA.cpp
 *  PURPOSE:     RenderWare mapping to Grand Theft Auto: San Andreas
 *               and miscellaneous rendering functions
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *  RenderWare is © Criterion Software
 *
 *****************************************************************************/

#include "StdInc.h"
#define RWFUNC_IMPLEMENT
#include "gamesa_renderware.h"
#include "gamesa_renderware.hpp"
#include "CRenderWareSA.ShaderMatching.h"
#include "C2dEffectSA.h"
#include <D3dx9tex.h>

extern CGameSA* pGame;

RwInt32& CRenderWareSA::_RwD3D9RasterExtOffset = *reinterpret_cast<int*>(0xB4E9E0);

// RwFrameForAllObjects struct and callback used to replace dynamic vehicle parts
struct SReplaceParts
{
    const char*        szName;                    // name of the part you want to replace (e.g. 'door_lf' or 'door_rf')
    unsigned char      ucIndex;                   // index counter for internal usage (0 is the 'ok' part model, 1 is the 'dam' part model)
    RpAtomicContainer* pReplacements;             // replacement atomics
    unsigned int       uiReplacements;            // number of replacements
};
static RwObject* ReplacePartsCB(RwObject* object, SReplaceParts* data)
{
    RpAtomic* Atomic = (RpAtomic*)object;
    char      szAtomicName[16] = {0};

    // iterate through our loaded atomics
    for (unsigned int i = 0; i < data->uiReplacements; i++)
    {
        // get the correct atomic name based on the object # in our parent frame
        if (data->ucIndex == 0)
            snprintf(&szAtomicName[0], 16, "%s_ok", data->szName);
        else
            snprintf(&szAtomicName[0], 16, "%s_dam", data->szName);

        // see if we have such an atomic in our replacement atomics array
        if (strncmp(&data->pReplacements[i].szName[0], &szAtomicName[0], 16) == 0)
        {
            // if so, override the geometry
            RpAtomicSetGeometry(Atomic, data->pReplacements[i].atomic->geometry, 0);

            // and copy the matrices
            RwFrameCopyMatrix(RpGetFrame(Atomic), RpGetFrame(data->pReplacements[i].atomic));

            object = (RwObject*)data->pReplacements[i].atomic;
            data->ucIndex++;
        }
    }

    return object;
}

// RpClumpForAllAtomics callback used to add atomics to a vehicle
static bool AddAllAtomicsCB(RpAtomic* atomic, void* pClump)
{
    RpClump* data = reinterpret_cast<RpClump*>(pClump);
    RwFrame* pFrame = RpGetFrame(data);

    // add the atomic to the frame
    RpAtomicSetFrame(atomic, pFrame);
    RpClumpAddAtomic(data, atomic);

    return true;
}

// RpClumpForAllAtomics struct and callback used to replace all wheels with a given wheel model
struct SReplaceWheels
{
    const char*        szName;                    // name of the new wheel model
    RpClump*           pClump;                    // the vehicle's clump
    RpAtomicContainer* pReplacements;             // replacement atomics
    unsigned int       uiReplacements;            // number of replacements
};
static bool ReplaceWheelsCB(RpAtomic* atomic, void* pData)
{
    SReplaceWheels* data = reinterpret_cast<SReplaceWheels*>(pData);
    RwFrame*        Frame = RpGetFrame(atomic);

    // find our wheel atomics
    if (strncmp(&Frame->szName[0], "wheel_lf_dummy", 16) == 0 || strncmp(&Frame->szName[0], "wheel_rf_dummy", 16) == 0 ||
        strncmp(&Frame->szName[0], "wheel_lm_dummy", 16) == 0 || strncmp(&Frame->szName[0], "wheel_rm_dummy", 16) == 0 ||
        strncmp(&Frame->szName[0], "wheel_lb_dummy", 16) == 0 || strncmp(&Frame->szName[0], "wheel_rb_dummy", 16) == 0)
    {
        // find a replacement atomic
        for (unsigned int i = 0; i < data->uiReplacements; i++)
        {
            // see if it's name is equal to the specified wheel
            if (strncmp(&data->pReplacements[i].szName[0], data->szName, 16) == 0)
            {
                // clone our wheel atomic
                RpAtomic* WheelAtomic = RpAtomicClone(data->pReplacements[i].atomic);
                RpAtomicSetFrame(WheelAtomic, Frame);

                // add it to the clump
                RpClumpAddAtomic(data->pClump, WheelAtomic);

                // delete the current atomic
                RpClumpRemoveAtomic(data->pClump, atomic);
            }
        }
    }

    return true;
}

// RpClumpForAllAtomics struct and callback used to replace all atomics for a vehicle
struct SReplaceAll
{
    RpClump*           pClump;                    // the vehicle's clump
    RpAtomicContainer* pReplacements;             // replacement atomics
    unsigned int       uiReplacements;            // number of replacements
};
static bool ReplaceAllCB(RpAtomic* atomic, void* pData)
{
    SReplaceAll* data = reinterpret_cast<SReplaceAll*>(pData);
    RwFrame*     Frame = RpGetFrame(atomic);
    if (Frame == NULL)
        return true;

    // find a replacement atomic
    for (unsigned int i = 0; i < data->uiReplacements; i++)
    {
        // see if we can find an atomic with the same name
        if (strncmp(&data->pReplacements[i].szName[0], &Frame->szName[0], 16) == 0)
        {
            // copy the matrices
            RwFrameCopyMatrix(RpGetFrame(atomic), RpGetFrame(data->pReplacements[i].atomic));

            // set the new atomic's frame to the current one
            RpAtomicSetFrame(data->pReplacements[i].atomic, Frame);

            // override all engine and material related callbacks and pointers
            data->pReplacements[i].atomic->renderCallback = atomic->renderCallback;
            data->pReplacements[i].atomic->frame = atomic->frame;
            data->pReplacements[i].atomic->render = atomic->render;
            data->pReplacements[i].atomic->interpolation = atomic->interpolation;
            data->pReplacements[i].atomic->info = atomic->info;

            // add the new atomic to the vehicle clump
            RpClumpAddAtomic(data->pClump, data->pReplacements[i].atomic);

            // remove the current atomic
            RpClumpRemoveAtomic(data->pClump, atomic);
        }
    }

    return true;
}

// RpClumpForAllAtomics struct and callback used to load the atomics from a specific clump into a container
struct SLoadAtomics
{
    RpAtomicContainer* pReplacements;             // replacement atomics
    unsigned int       uiReplacements;            // number of replacements
};
static bool LoadAtomicsCB(RpAtomic* atomic, void* pData)
{
    SLoadAtomics* data = reinterpret_cast<SLoadAtomics*>(pData);
    RwFrame*      Frame = RpGetFrame(atomic);

    // add the atomic to the container
    data->pReplacements[data->uiReplacements].atomic = atomic;
    strncpy(&data->pReplacements[data->uiReplacements].szName[0], &Frame->szName[0], 16);

    // and increment the counter
    data->uiReplacements++;

    return true;
}

static bool                  g_CurrentReadDFFWithoutReplacingCOL = false;
static int                   g_CurrentDFFWriteModelID = 0;
static CColModelSAInterface* g_CurrentReadDFFCOLModel = nullptr;

/*
0x0253f200: atomic_visibility_distance
0x0253f201: clump_visibility_distance
0x0253f202: frame_visibility_distance

0x0253f2f3: pipeline_set
0x0253f2f4: unused_5
0x0253f2f5: texdictionary_link
0x0253f2f6: specular_material
0x0253f2f7: unused_8
0x0253f2f8: effect_2d
0x0253f2f9: extra_vert_colour
0x0253f2fa: collision_model
0x0253f2fb: gta_hanim
0x0253f2fc: reflection_material
0x0253f2fd: breakable
0x0253f2fe: frame
0x0253f2ff: unused_16*/

bool IsPluginUsed(RwPluginRegEntry* pPluginRegistryEntry, void* pObject)
{
    if (pPluginRegistryEntry->pluginID >= (DWORD)0x0253F2F0)
    {
        // std::printf("IsPluginUsed: pluginId = %#.8x\n", pPluginRegistryEntry->pluginID);
        switch (pPluginRegistryEntry->pluginID)
        {
                /*
                case (DWORD)0x0253F2F3: // pipeline_set
                {
                 return false;
                }
                */

            case (DWORD)0x0253F2F4:            // unused_5
            {
                // return false;

                DWORD* pPluginData = (DWORD*)(pPluginRegistryEntry->offset + ((unsigned char*)pObject));

                if (!*pPluginData)
                {
                    return false;
                }
                return true;
            }
                /*
                case (DWORD)0x0253F2F5: // texdictionary_link
                {
                return false;
                }
                */

            case (DWORD)0x0253F2F6:            // specular_material
            {
                // return false;

                DWORD* pPluginData = (DWORD*)(pPluginRegistryEntry->offset + ((unsigned char*)pObject));

                if (!*pPluginData)
                {
                    return false;
                }
                return true;
            }

            /*
            case (DWORD)0x0253F2F7: // unused_8
            {
            return false;
            }
            */
            case (DWORD)0x0253F2F8:            // effect_2d
            {
                DWORD* pPluginData = (DWORD*)(pPluginRegistryEntry->offset + ((unsigned char*)pObject));

                if (!*pPluginData)
                {
                    return false;
                }
                return true;
            }

            case (DWORD)0x0253F2F9:            // extra_vert_colour
            {
                // return false;

                DWORD* pPluginData = (DWORD*)(pPluginRegistryEntry->offset + ((unsigned char*)pObject));

                if (!*pPluginData)
                {
                    return false;
                }
                return true;
            }

            case (DWORD)0x0253F2FA:            // collision_model
            {
                // Is vehicle model?
                if (g_CurrentDFFWriteModelID >= 400 && g_CurrentDFFWriteModelID <= 611)
                {
                    return true;
                }
                return false;
            }

            /*
            case (DWORD)0x0253F2FB: // gta_hanim
            {
            return false;
            }
            */
            case (DWORD)0x0253F2FC:            // reflection_material
            {
                // return false;

                DWORD* pPluginData = (DWORD*)(pPluginRegistryEntry->offset + ((unsigned char*)pObject));

                // CCustomCarEnvMapPipeline::fakeEnvMapPipeMatData = 0xc02d18
                if (*pPluginData == (DWORD)0xc02d18 || !*pPluginData)
                {
                    return false;
                }
                return true;
            }

            case (DWORD)0x0253F2FD:            // breakable
            {
                // return false;

                DWORD* pPluginData = (DWORD*)(pPluginRegistryEntry->offset + ((unsigned char*)pObject));

                if (!*pPluginData)
                {
                    return false;
                }
                return true;
            }

            /*
            case (DWORD)0x0253F2FE: // frame
            {
            return false;
            }

            case (DWORD)0x0253F2FF: // unused_16
            {
            return false;
            }
            */
            default:
            {
                return true;
            }
        }
    }
    return true;
    ;
}

int __cdecl _rwPluginRegistryGetSize(RwPluginRegistry* pPluginRegistry, void* pObject)
{
    int iTotalPluginsSize = 0;

    RwPluginRegEntry* pPluginRegistryEntry = pPluginRegistry->firstRegEntry;
    for (iTotalPluginsSize = 0; pPluginRegistryEntry; pPluginRegistryEntry = pPluginRegistryEntry->nextRegEntry)
    {
        bool bPluginUsed = IsPluginUsed(pPluginRegistryEntry, pObject);
        if (!bPluginUsed)
        {
            // std::printf("rwPluginRegistryGetSize: skipping for pluginID: '%#.8x' | pObject = %p\n", pPluginRegistryEntry->pluginID, pObject);
        }

        if (pPluginRegistryEntry->getSizeCB && bPluginUsed)
        {
            // std::printf("rwPluginRegistryGetSize: pluginID: '%#.8x' | pObject = %p\n", pPluginRegistryEntry->pluginID, pObject);

            int iRegistryEntrySize = pPluginRegistryEntry->getSizeCB(pObject, pPluginRegistryEntry->offset, pPluginRegistryEntry->size);
            if (iRegistryEntrySize > 0)
            {
                iTotalPluginsSize += iRegistryEntrySize + 12;
            }

            // iTotalPluginsSize += 12;
        }
    }
    return iTotalPluginsSize;
}

RwPluginRegistry* __cdecl _rwPluginRegistryWriteDataChunks(RwPluginRegistry* a1, RwStream* stream, const void* object)
{
    auto _rwStreamWriteVersionedChunkHeader = (RwStream * (__cdecl*)(RwStream * stream, int type, int size, int version, unsigned __int16 buildNum))0x7ED270;

    int iTotalPluginsSize = _rwPluginRegistryGetSize(a1, (void*)object);

    // This check fixes a crash for writing TXD
    /*if (iTotalPluginsSize <= 0)
    {
        return a1;
    }*/

    if (!_rwStreamWriteVersionedChunkHeader(stream, 3, iTotalPluginsSize, 0x36003, 0xFFFFu))
    {
        return nullptr;
    }

    if (!a1->firstRegEntry)
    {
        return a1;
    }

    for (auto pPluginRegistryEntry = a1->firstRegEntry; pPluginRegistryEntry; pPluginRegistryEntry = pPluginRegistryEntry->nextRegEntry)
    {
        RwPluginDataChunkGetSizeCallBack GetSize = pPluginRegistryEntry->getSizeCB;
        if (GetSize)
        {
            bool bPluginUsed = IsPluginUsed(pPluginRegistryEntry, (void*)object);
            if (!bPluginUsed)
            {
                // std::printf("_rwPluginRegistryWriteDataChunks: skipping for pluginID: '%#.8x' | pObject = %p\n", pPluginRegistryEntry->pluginID,
                // (void*)object);
            }

            if (pPluginRegistryEntry->writeCB && bPluginUsed)
            {
                if (pPluginRegistryEntry->pluginID == (DWORD)0x0253F2F3)
                {
                    int i = 0;
                }

                // std::printf("rwWriteDataChunks: pluginID: '%#.8x' | pObject = %p\n", pPluginRegistryEntry->pluginID, object);

                int iPluginDataSize = GetSize(object, pPluginRegistryEntry->offset, pPluginRegistryEntry->size);
                if (iPluginDataSize > 0)
                {
                    if (!_rwStreamWriteVersionedChunkHeader(stream, pPluginRegistryEntry->pluginID, iPluginDataSize, 0x36003, 0xFFFFu))
                    {
                        return nullptr;
                    }

                    if (!pPluginRegistryEntry->writeCB(stream, iPluginDataSize, object, pPluginRegistryEntry->offset, pPluginRegistryEntry->size))
                    {
                        return nullptr;
                    }
                }
            }
        }
    }

    return a1;
}

#pragma pack(push, 1)
struct GeometryMeshExt
{
    unsigned int          m_uiPosRule;
    unsigned short        m_usNumVertices;
    short                 _pad0;
    RwV3d*                m_pVertexPos;
    RwTextureCoordinates* m_pTexCoors;
    RwRGBA*               m_pVertexColors;
    unsigned short        m_usNumTriangles;
    short                 _pad1;
    RpTriangle*           m_pTriangles;
    unsigned short*       m_pMaterialAssignments;
    unsigned short        m_usNumMaterials;
    short                 _pad2;
    RwTexture**           m_pTextures;
    char*                 m_pTextureNames;
    char*                 m_pMaskNames;
    RwSurfaceProperties*  m_pMaterialProperties;
};
#pragma pack(pop)

int BreakableStreamGetSizeCB(unsigned char* pObject, int pluginOffset)
{
    int size = 0;

    GeometryMeshExt* pGeometryMeshExt = *(GeometryMeshExt**)(pObject + pluginOffset);
    if (pGeometryMeshExt)
    {
        size += 4;
        size += 52;

        size += (12 * pGeometryMeshExt->m_usNumVertices);
        size += (8 * pGeometryMeshExt->m_usNumVertices);
        size += (4 * pGeometryMeshExt->m_usNumVertices);
        size += (6 * pGeometryMeshExt->m_usNumTriangles);
        size += (2 * pGeometryMeshExt->m_usNumTriangles);
        size += (32 * pGeometryMeshExt->m_usNumMaterials);
        size += (32 * pGeometryMeshExt->m_usNumMaterials);
        size += (12 * pGeometryMeshExt->m_usNumMaterials);
    }
    return size;
}

RwStream* __cdecl PipelinePluginWriteCB(RwStream* stream, int length, unsigned char* pObject, int offsetInObject)
{
    auto RwStreamWrite = (RwStream * (__cdecl*)(RwStream * stream, void* buffer, int length))0x7ECB30;

    RwStreamWrite(stream, (void*)(pObject + offsetInObject), length);
    return stream;
}

void* __cdecl PipelinePluginDestructCB(void* object, int offsetInObject, int sizeInObject)
{
    return object;
}

struct ExtraVertColourPlugin
{
    RwRGBA* nightColors;            // array of RwRGBAs per vertice
    RwRGBA* dayColors;              // array of RwRGBAs per vertice
    float   dnParam;
};

RwStream* __cdecl PluginExtraVertColourStreamWriteCB(RwStream* stream, int length, unsigned char* pObject, int offsetInObject)
{
    auto RwStreamWrite = (RwStream * (__cdecl*)(RwStream * stream, void* buffer, int length))0x7ECB30;

    // GTA SA was dereferencing the pointer `pObject + offsetInObject` and causing the game to crash
    // in the write callback function. Fixed it.
    ExtraVertColourPlugin* pExtraVertColour = (ExtraVertColourPlugin*)(pObject + offsetInObject);
    RwStreamWrite(stream, pExtraVertColour, 4);
    if (pExtraVertColour->nightColors)
    {
        RpGeometry* pGeometry = (RpGeometry*)pObject;
        RwStreamWrite(stream, pExtraVertColour->nightColors, 4 * ((RpGeometry*)pObject)->vertices_size);
    }
    return stream;
}

int PluginExtraVertColourStreamGetSizeCB(unsigned char* pObject, int pluginOffset)
{
    int         size = 0;
    RpGeometry* pGeometry = (RpGeometry*)pObject;
    if (pGeometry)
    {
        size += 4;
        size += 4 * pGeometry->vertices_size;
    }
    return size;
}

struct t2dEffectPlugin
{
    unsigned int         uiCount;
    C2dEffectSAInterface arrEffects[];
};

RwStream* __cdecl Plugin2DEffectStreamWriteCB(RwStream* stream, int length, unsigned char* pObject, int offsetInObject)
{
    auto RwStreamWrite = (RwStream * (__cdecl*)(RwStream * stream, void* buffer, int length))0x7ECB30;

    t2dEffectPlugin* pEffectPlugin = *(t2dEffectPlugin**)(pObject + offsetInObject);
    if (!pEffectPlugin)
    {
        return stream;
    }

    RwStreamWrite(stream, &pEffectPlugin->uiCount, 4);

    for (unsigned int i = 0; i < pEffectPlugin->uiCount; i++)
    {
        C2dEffectSAInterface& the2dEffect = pEffectPlugin->arrEffects[i];

        // C2dEffectSAInterface* p2dEffect = (C2dEffectSAInterface*)( ((unsigned char*)pEffectPlugin) + 4  + (sizeof(C2dEffectSAInterface)*i));
        // std::printf("2DEffectWrite: type: %d | type in hex: %#.8x | type addr: %p\n",
        //    p2dEffect->m_nType, p2dEffect->m_nType, (void*)&p2dEffect->m_nType);

        // Write the position
        RwStreamWrite(stream, &the2dEffect, 12);

        unsigned char arrTypeAndPadding[4] = {the2dEffect.m_nType, 0, 0, 0};
        RwStreamWrite(stream, &arrTypeAndPadding, 4);

        switch (the2dEffect.m_nType)
        {
            case EFFECT_LIGHT:
            {
                // 80 bytes = write look x, y, and z values as well
                int iSizeOfStruct = 80;
                RwStreamWrite(stream, &iSizeOfStruct, 4);

                // write 20 bytes from m_color till m_fShadowSize
                RwStreamWrite(stream, &the2dEffect.light, 20);

                // write 4 bytes from m_nCoronaFlashType till m_nShadowColorMultiplier
                RwStreamWrite(stream, &the2dEffect.light.m_nCoronaFlashType, 4);

                unsigned char* pFlags = reinterpret_cast<unsigned char*>(&the2dEffect.light.m_nFlags);
                RwStreamWrite(stream, pFlags, 1);
                RwStreamWrite(stream, &the2dEffect.light.m_pCoronaTex->name, 24);
                RwStreamWrite(stream, &the2dEffect.light.m_pShadowTex->name, 24);
                RwStreamWrite(stream, &the2dEffect.light.m_nShadowZDistance, 1);
                pFlags++;
                RwStreamWrite(stream, pFlags, 1);

                // write look X, Y, and Z directions
                RwStreamWrite(stream, &the2dEffect.light.offsetX, 3);

                short sPadding = 0;
                RwStreamWrite(stream, &sPadding, 2);
                break;
            }
            case EFFECT_PARTICLE:
            {
                int iSizeOfStruct = 24;
                RwStreamWrite(stream, &iSizeOfStruct, 4);
                RwStreamWrite(stream, &the2dEffect.particle, iSizeOfStruct);
                break;
            }
            case EFFECT_ATTRACTOR:
            {
                int iSizeOfStruct = 56;
                RwStreamWrite(stream, &iSizeOfStruct, 4);
                RwStreamWrite(stream, &the2dEffect.pedAttractor.m_nAttractorType, 1);

                int iPadding = 0;
                RwStreamWrite(stream, &iPadding, 3);

                RwStreamWrite(stream, &the2dEffect.pedAttractor, 36);
                RwStreamWrite(stream, &the2dEffect.pedAttractor.m_szScriptName, 8);
                RwStreamWrite(stream, &the2dEffect.pedAttractor.m_nPedExistingProbability, 1);
                RwStreamWrite(stream, &iPadding, 3);

                unsigned char arrLast4Bytes[4] = {the2dEffect.pedAttractor.field_36, 0, the2dEffect.pedAttractor.m_nFlags, 0};
                RwStreamWrite(stream, &arrLast4Bytes, 4);
                break;
            }
            case EFFECT_ENEX:
            {
                int iSizeOfStruct = 44;
                RwStreamWrite(stream, &iSizeOfStruct, 4);
                RwStreamWrite(stream, &the2dEffect.enEx, iSizeOfStruct);
                break;
            }
            case EFFECT_ROADSIGN:
            {
                int iSizeOfStruct = 88;
                RwStreamWrite(stream, &iSizeOfStruct, 4);

                RwStreamWrite(stream, &the2dEffect.roadsign, 22);
                RwStreamWrite(stream, the2dEffect.roadsign.m_pText, 64);

                short sPadding = 0;
                RwStreamWrite(stream, &sPadding, 2);
                break;
            }
            case EFFECT_SLOTMACHINE_WHEEL:
            {
                int iSizeOfStruct = 4;
                RwStreamWrite(stream, &iSizeOfStruct, 4);
                RwStreamWrite(stream, &the2dEffect.iSlotMachineIndex, iSizeOfStruct);
                break;
            }
            case EFFECT_COVER_POINT:
            {
                int iSizeOfStruct = 12;
                RwStreamWrite(stream, &iSizeOfStruct, 4);
                RwStreamWrite(stream, &the2dEffect.coverPoint, iSizeOfStruct);
                break;
            }
            case EFFECT_ESCALATOR:
            {
                int iSizeOfStruct = 40;
                RwStreamWrite(stream, &iSizeOfStruct, 4);
                RwStreamWrite(stream, &the2dEffect.escalator, iSizeOfStruct);
                break;
            }
            default:
            {
                int iSizeOfStruct = 0;
                RwStreamWrite(stream, &iSizeOfStruct, 4);
            }
        }
    }

    return stream;
}

int Plugin2DEffectStreamGetSizeCB(unsigned char* pObject, int pluginOffset)
{
    int size = 0;

    t2dEffectPlugin* pEffectPlugin = *(t2dEffectPlugin**)(pObject + pluginOffset);
    if (pEffectPlugin)
    {
        // number of effects
        size += 4;

        for (unsigned int i = 0; i < pEffectPlugin->uiCount; i++)
        {
            C2dEffectSAInterface& the2dEffect = pEffectPlugin->arrEffects[i];

            // std::printf("EffectGetSizeCB: effect type: %d\n", the2dEffect.m_nType);

            // common header size
            size += 20;

            switch (the2dEffect.m_nType)
            {
                case EFFECT_LIGHT:
                {
                    // 80 bytes = write look x, y, and z values as well
                    size += 80;
                    break;
                }
                case EFFECT_PARTICLE:
                {
                    size += 24;
                    break;
                }
                case EFFECT_ATTRACTOR:
                {
                    size += 56;
                    break;
                }
                case EFFECT_ENEX:
                {
                    size += 44;
                    break;
                }
                case EFFECT_ROADSIGN:
                {
                    size += 88;
                    break;
                }
                case EFFECT_SLOTMACHINE_WHEEL:
                {
                    size += 4;
                    break;
                }
                case EFFECT_COVER_POINT:
                {
                    size += 12;
                    break;
                }
                case EFFECT_ESCALATOR:
                {
                    size += 40;
                    break;
                }
            }
        }
    }
    return size;
}

int PluginCollisionStreamGetSizeCB(unsigned char* pObject, int pluginOffset);

RwStream* __cdecl PluginCollisionStreamWriteCB(RwStream* stream, int length, unsigned char* pObject, int offsetInObject)
{
    auto RwStreamWrite = (RwStream * (__cdecl*)(RwStream * stream, void* buffer, int length))0x7ECB30;

    auto pColModelInterface = g_CurrentReadDFFCOLModel;
    auto pColData = pColModelInterface->pColData;

    ColModelFileHeader colFileHeader = {0};
    memcpy(colFileHeader.version, "COL3", 4);
    colFileHeader.size = PluginCollisionStreamGetSizeCB(pObject, offsetInObject) - 8;
    colFileHeader.modelId = g_CurrentDFFWriteModelID;

    RwStreamWrite(stream, &colFileHeader, sizeof(ColModelFileHeader));
    RwStreamWrite(stream, &pColModelInterface->boundingBox, sizeof(CBoundingBoxSA));
    RwStreamWrite(stream, &pColData->numColSpheres, 7);

    char padding = 0;
    RwStreamWrite(stream, &padding, 1);

    unsigned char arrFlags[4] = {pColData->m_nFlags, 0, 0, 0};
    RwStreamWrite(stream, &arrFlags, 4);

    unsigned int offsetCollisionSphere = sizeof(CColDataSA);            // 0x30
    offsetCollisionSphere += 68;                                        // this really isn't needed but it's there.
    unsigned int offsetCollisionBoxes = offsetCollisionSphere + (sizeof(CColSphereSA) * pColData->numColSpheres);
    unsigned int offsetCollisionLinesOrDisks = offsetCollisionBoxes + (sizeof(CColBoxSA) * pColData->numColBoxes);
    unsigned int uiCollisionLinesOrDisksSize = sizeof(CColLineSA) * pColData->numLinesOrDisks;
    if (pColData->Flags.bUsesDisks)
    {
        uiCollisionLinesOrDisksSize = sizeof(CColDiskSA) * pColData->numLinesOrDisks;
    }
    unsigned int offsetCollisionVertices = offsetCollisionLinesOrDisks + uiCollisionLinesOrDisksSize;

    unsigned short usHighestVertexIndex = 0;
    if (pColData->pColTriangles)
    {
        for (unsigned short i = 0; i < pColData->numColTriangles; i++)
        {
            CColTriangleSA& colTriangle = pColData->pColTriangles[i];
            if (colTriangle.vertexIndex1 > usHighestVertexIndex)
                usHighestVertexIndex = colTriangle.vertexIndex1;
            if (colTriangle.vertexIndex2 > usHighestVertexIndex)
                usHighestVertexIndex = colTriangle.vertexIndex2;
            if (colTriangle.vertexIndex3 > usHighestVertexIndex)
                usHighestVertexIndex = colTriangle.vertexIndex3;
        }
    }

    unsigned int numVertices = usHighestVertexIndex > 0 ? (usHighestVertexIndex + 1) : 0;
    unsigned int offsetCollisionTriangles = offsetCollisionVertices + (sizeof(CCompressedVectorSA) * numVertices);
    unsigned int offsetCollisionTrianglePlanes = offsetCollisionTriangles + sizeof(CColTriangleSA) * pColData->numColTriangles;
    unsigned int offsetCollisionShadowVertices = offsetCollisionTrianglePlanes + 0;            // don't write triangle planes
    unsigned int offsetCollisionShadowTriangles = offsetCollisionShadowVertices + sizeof(CCompressedVectorSA) * pColData->m_nNumShadowVertices;

    unsigned theCollisionOffset = 0;

    theCollisionOffset = pColData->pColSpheres ? offsetCollisionSphere : 0;
    RwStreamWrite(stream, &theCollisionOffset, 4);

    theCollisionOffset = pColData->pColBoxes ? offsetCollisionBoxes : 0;
    RwStreamWrite(stream, &theCollisionOffset, 4);

    theCollisionOffset = pColData->m_pSuspensionLines ? uiCollisionLinesOrDisksSize : 0;
    RwStreamWrite(stream, &theCollisionOffset, 4);

    theCollisionOffset = pColData->m_pVertices ? offsetCollisionVertices : 0;
    RwStreamWrite(stream, &theCollisionOffset, 4);

    theCollisionOffset = pColData->pColTriangles ? offsetCollisionTriangles : 0;
    RwStreamWrite(stream, &theCollisionOffset, 4);

    theCollisionOffset = 0;
    RwStreamWrite(stream, &theCollisionOffset, 4);

    // shadow data exists for version >= COL3
    RwStreamWrite(stream, &pColData->m_nNumShadowTriangles, 4);

    theCollisionOffset = pColData->m_pShadowVertices ? offsetCollisionShadowVertices : 0;
    RwStreamWrite(stream, &theCollisionOffset, 4);

    theCollisionOffset = pColData->m_pShadowTriangles ? offsetCollisionShadowTriangles : 0;
    RwStreamWrite(stream, &theCollisionOffset, 4);

    if (pColData->pColSpheres && pColData->numColSpheres > 0)
    {
        RwStreamWrite(stream, pColData->pColSpheres, sizeof(CColSphereSA) * pColData->numColSpheres);
    }

    if (pColData->pColBoxes && pColData->numColBoxes > 0)
    {
        RwStreamWrite(stream, pColData->pColBoxes, sizeof(CColBoxSA) * pColData->numColBoxes);
    }

    if (pColData->m_pSuspensionLines && uiCollisionLinesOrDisksSize > 0)
    {
        RwStreamWrite(stream, pColData->m_pSuspensionLines, uiCollisionLinesOrDisksSize);
    }

    if (pColData->m_pVertices && numVertices > 0)
    {
        RwStreamWrite(stream, pColData->m_pVertices, sizeof(CCompressedVectorSA) * numVertices);
    }

    if (pColData->pColTriangles && pColData->numColTriangles > 0)
    {
        RwStreamWrite(stream, pColData->pColTriangles, sizeof(CColTriangleSA) * pColData->numColTriangles);
    }

    // Don't write triangle planes

    if (pColData->m_pShadowVertices && pColData->m_nNumShadowVertices > 0)
    {
        RwStreamWrite(stream, pColData->m_pShadowVertices, sizeof(CCompressedVectorSA) * pColData->m_nNumShadowVertices);
    }

    if (pColData->m_pShadowTriangles && pColData->m_nNumShadowTriangles > 0)
    {
        RwStreamWrite(stream, pColData->m_pShadowTriangles, sizeof(CColTriangleSA) * pColData->m_nNumShadowTriangles);
    }

    return stream;
}

int PluginCollisionStreamGetSizeCB(unsigned char* pObject, int pluginOffset)
{
    auto pColModelInterface = g_CurrentReadDFFCOLModel;
    auto pColData = pColModelInterface->pColData;

    unsigned int uiCollisionLinesOrDisksSize = sizeof(CColLineSA) * pColData->numLinesOrDisks;
    if (pColData->Flags.bUsesDisks)
    {
        uiCollisionLinesOrDisksSize = sizeof(CColDiskSA) * pColData->numLinesOrDisks;
    }

    unsigned short usHighestVertexIndex = 0;
    if (pColData->pColTriangles)
    {
        for (unsigned short i = 0; i < pColData->numColTriangles; i++)
        {
            CColTriangleSA& colTriangle = pColData->pColTriangles[i];
            if (colTriangle.vertexIndex1 > usHighestVertexIndex)
                usHighestVertexIndex = colTriangle.vertexIndex1;
            if (colTriangle.vertexIndex2 > usHighestVertexIndex)
                usHighestVertexIndex = colTriangle.vertexIndex2;
            if (colTriangle.vertexIndex3 > usHighestVertexIndex)
                usHighestVertexIndex = colTriangle.vertexIndex3;
        }
    }

    unsigned int numVertices = usHighestVertexIndex > 0 ? (usHighestVertexIndex + 1) : 0;

    int size = 0;

    size += sizeof(ColModelFileHeader);
    size += 88;            // COL3 data
    size += sizeof(CColSphereSA) * pColData->numColSpheres;
    size += sizeof(CColBoxSA) * pColData->numColBoxes;

    size += uiCollisionLinesOrDisksSize;
    size += sizeof(CCompressedVectorSA) * numVertices;
    size += sizeof(CColTriangleSA) * pColData->numColTriangles;

    // Ignore triangle planes
    size += sizeof(CCompressedVectorSA) * pColData->m_nNumShadowVertices;
    size += sizeof(CColTriangleSA) * pColData->m_nNumShadowTriangles;

    return size;
}

void RegisterClumpPluginsCallBacks()
{
    RwPluginRegistry& clumpTKList = *(RwPluginRegistry*)0x8D6264;
    for (auto pPluginRegistryEntry = clumpTKList.firstRegEntry; pPluginRegistryEntry; pPluginRegistryEntry = pPluginRegistryEntry->nextRegEntry)
    {
        // collision plugin
        if (pPluginRegistryEntry->pluginID == (DWORD)0x0253F2FA)
        {
            pPluginRegistryEntry->writeCB = (RwPluginDataChunkWriteCallBack)PluginCollisionStreamWriteCB;
            pPluginRegistryEntry->getSizeCB = (RwPluginDataChunkGetSizeCallBack)PluginCollisionStreamGetSizeCB;
            break;
        }
    }
}

void RegisterAtomicPluginsCallBacks()
{
    RwPluginRegistry& atomicTKList = *(RwPluginRegistry*)0x8D624C;
    for (auto pPluginRegistryEntry = atomicTKList.firstRegEntry; pPluginRegistryEntry; pPluginRegistryEntry = pPluginRegistryEntry->nextRegEntry)
    {
        // pipeline_set plugin
        if (pPluginRegistryEntry->pluginID == (DWORD)0x0253F2F3)
        {
            pPluginRegistryEntry->writeCB = (RwPluginDataChunkWriteCallBack)PipelinePluginWriteCB;
            pPluginRegistryEntry->destructCB = (RwPluginObjectDestructor)PipelinePluginDestructCB;
            break;
        }
    }
}

void RegisterGeometryPluginsCallBacks()
{
    RwPluginRegistry& geometryTKList = *(RwPluginRegistry*)0x8D628C;
    for (auto pPluginRegistryEntry = geometryTKList.firstRegEntry; pPluginRegistryEntry; pPluginRegistryEntry = pPluginRegistryEntry->nextRegEntry)
    {
        switch (pPluginRegistryEntry->pluginID)
        {
            // breakable plugin
            case (DWORD)0x253F2FD:
            {
                pPluginRegistryEntry->getSizeCB = (RwPluginDataChunkGetSizeCallBack)BreakableStreamGetSizeCB;
                break;
            }

            // 2D effect plugin
            case (DWORD)0x253F2F8:
            {
                pPluginRegistryEntry->writeCB = (RwPluginDataChunkWriteCallBack)Plugin2DEffectStreamWriteCB;
                pPluginRegistryEntry->getSizeCB = (RwPluginDataChunkGetSizeCallBack)Plugin2DEffectStreamGetSizeCB;
                break;
            }

            /// ExtraVertColours plugin (night colors)
            case (DWORD)0x253F2F9:
            {
                pPluginRegistryEntry->writeCB = (RwPluginDataChunkWriteCallBack)PluginExtraVertColourStreamWriteCB;
                pPluginRegistryEntry->getSizeCB = (RwPluginDataChunkGetSizeCallBack)PluginExtraVertColourStreamGetSizeCB;
                break;
            }
        }
    }
}

SString g_CurrentDFFBeingGeneratedFileName;

void CRenderWareSA::SetCurrentDFFBeingGeneratedFileName(SString& strDFFName)
{
    g_CurrentDFFBeingGeneratedFileName = strDFFName;
}

void CRenderWareSA::SetCurrentDFFWriteModelID(int modelID)
{
    g_CurrentDFFWriteModelID = modelID;
}

void CRenderWareSA::SetCurrentReadDFFWithoutReplacingCOL(bool bReadWithoutReplacingCOL)
{
    g_CurrentReadDFFWithoutReplacingCOL = bReadWithoutReplacingCOL;
}

void CRenderWareSA::DeleteReadDFFCollisionModel()
{
    auto CColModel_Destructor = (void(__thiscall*)(CColModelSAInterface*, bool))0x4C4C00;
    if (g_CurrentReadDFFCOLModel)
    {
        CColModel_Destructor(g_CurrentReadDFFCOLModel, true);
        g_CurrentReadDFFCOLModel = nullptr;
    }
}

void __cdecl OnCCollisionPlugin__read(CColModelSAInterface* pColModel)
{
    auto                        CBaseModelInfo_SetColModel = (void(__thiscall*)(CBaseModelInfoSAInterface*, CColModelSAInterface*, bool))0x4C4BC0;
    CBaseModelInfoSAInterface*& CCollisionPlugin__ms_currentModel = *(CBaseModelInfoSAInterface**)0x9689E0;

    if (g_CurrentReadDFFWithoutReplacingCOL)
    {
        g_CurrentReadDFFCOLModel = pColModel;
        return;
    }

    auto pBaseModelInfoInterface = CCollisionPlugin__ms_currentModel;
    CBaseModelInfo_SetColModel(pBaseModelInfoInterface, pColModel, true);
    pBaseModelInfoInterface->m_nFlagsBtye2 |= 8;
}

DWORD RETURN_CCollisionPlugin__read = 0x41B2D0;
void _declspec(naked) HOOK_CCollisionPlugin__read()
{
    _asm
    {
        pushad
        push    esi 
        call    OnCCollisionPlugin__read
        add     esp, 4
        popad

        jmp     RETURN_CCollisionPlugin__read
    }
}

/////////////////////////////////////////////////////////////////////////////
//
//  CRenderWareSA
//
//
/////////////////////////////////////////////////////////////////////////////
CRenderWareSA::CRenderWareSA(eGameVersion version)
{
    InitRwFunctions(version);
    InitTextureWatchHooks();

    HookInstall(0x41B2B4, (DWORD)HOOK_CCollisionPlugin__read, 6);

    HookInstall(0x808B00, (DWORD)_rwPluginRegistryGetSize, 5);
    HookInstall(0x808B40, (DWORD)_rwPluginRegistryWriteDataChunks, 5);

    RegisterClumpPluginsCallBacks();
    RegisterAtomicPluginsCallBacks();
    RegisterGeometryPluginsCallBacks();

    m_pMatchChannelManager = new CMatchChannelManager();
    m_iRenderingEntityType = TYPE_MASK_WORLD;
    m_GTAVertexShadersDisabledTimer.SetMaxIncrement(1000, true);
    m_bGTAVertexShadersEnabled = true;
}

CRenderWareSA::~CRenderWareSA()
{
    SAFE_DELETE(m_pMatchChannelManager);
}

RwTexDictionary* CRenderWareSA::ReadTXD(const SString& strFilename, RwBuffer& buffer, bool bScriptAddTexture)
{
    // open the stream
    RwStream* streamTexture;
    if (buffer.size > 0)
    {
        streamTexture = RwStreamOpen(STREAM_TYPE_BUFFER, STREAM_MODE_READ, &buffer);
    }
    else
        streamTexture = RwStreamOpen(STREAM_TYPE_FILENAME, STREAM_MODE_READ, *strFilename);

    // check for errors
    if (streamTexture == NULL)
    {
        return NULL;
    }

    // TXD header id: 0x16
    // find our txd chunk (dff loads textures, so correct loading order is: txd, dff)
    if (RwStreamFindChunk(streamTexture, 0x16, NULL, NULL) == false)
    {
        RwStreamClose(streamTexture, NULL);
        return NULL;
    }

    // read the texture dictionary from our model (txd)
    RwTexDictionary* pTex = RwTexDictionaryGtaStreamRead(streamTexture);

    // close the stream
    RwStreamClose(streamTexture, NULL);

    if (bScriptAddTexture)
    {
        ScriptAddedTxd(pTex);
    }

    return pTex;
}

// Reads and parses a TXD file specified by a path (szTXD)
RwTexDictionary* CRenderWareSA::ReadTXD(const SString& strFilename, const CBuffer& fileData, bool bScriptAddTexture)
{
    RwBuffer buffer;
    if (!fileData.IsEmpty())
    {
        buffer.ptr = (void*)fileData.GetData();
        buffer.size = fileData.GetSize();
        return ReadTXD(strFilename, buffer, bScriptAddTexture);
    }
    buffer = {nullptr, 0};
    return ReadTXD(strFilename, buffer, bScriptAddTexture);
}

// Reads and parses a DFF file specified by a path (szDFF) into a CModelInfo identified by the object id (usModelID)
// bLoadEmbeddedCollisions should be true for vehicles
// Any custom TXD should be imported before this call
RpClump* CRenderWareSA::ReadDFF(const SString& strFilename, RwBuffer& buffer, unsigned short usModelID, bool bLoadEmbeddedCollisions, RwTexDictionary* pTexDict)
{
    if (pTexDict)
    {
        RwTexDictionarySetCurrent(pTexDict);
    }
    else if (usModelID != 0)
    {
        // Set correct TXD as materials are processed at the same time
        unsigned short usTxdId = ((CBaseModelInfoSAInterface**)ARRAY_ModelInfo)[usModelID]->usTextureDictionary;
        SetTextureDict(usTxdId);
    }

    // open the stream
    RwStream* streamModel;
    if (buffer.size > 0)
    {
        streamModel = RwStreamOpen(STREAM_TYPE_BUFFER, STREAM_MODE_READ, &buffer);
    }
    else
        streamModel = RwStreamOpen(STREAM_TYPE_FILENAME, STREAM_MODE_READ, *strFilename);

    // get the modelinfo array
    DWORD* pPool = (DWORD*)ARRAY_ModelInfo;

    // check for errors
    if (streamModel == NULL)
    {
        return NULL;
    }

    // DFF header id: 0x10
    // find our dff chunk
    if (RwStreamFindChunk(streamModel, 0x10, NULL, NULL) == false)
    {
        std::printf("couldn't find DFF chunk for model Id: %u\n", usModelID);
        RwStreamClose(streamModel, NULL);
        return NULL;
    }

    // rockstar's collision hack: set the global particle emitter to the modelinfo pointer of this model
    if (bLoadEmbeddedCollisions)
    {
        RpPrtStdGlobalDataSetStreamEmbedded((void*)pPool[usModelID]);
    }

    // read the clump with all its extensions
    RpClump* pClump = RpClumpStreamRead(streamModel);

    // reset collision hack
    if (bLoadEmbeddedCollisions)
        RpPrtStdGlobalDataSetStreamEmbedded(NULL);

    // close the stream
    RwStreamClose(streamModel, NULL);

    return pClump;
}

RpClump* CRenderWareSA::ReadDFF(const SString& strFilename, const CBuffer& fileData, unsigned short usModelID, bool bLoadEmbeddedCollisions,
                                RwTexDictionary* pTexDict)
{
    RwBuffer buffer;
    if (!fileData.IsEmpty())
    {
        buffer.ptr = (void*)fileData.GetData();
        buffer.size = fileData.GetSize();
        return ReadDFF(strFilename, buffer, usModelID, bLoadEmbeddedCollisions, pTexDict);
    }
    buffer = {nullptr, 0};
    return ReadDFF(strFilename, buffer, usModelID, bLoadEmbeddedCollisions, pTexDict);
}

bool CRenderWareSA::WriteTXD(const SString& strFilename, RwTexDictionary* pTxdDictionary)
{
    RwStream* pStream = RwStreamOpen(STREAM_TYPE_FILENAME, STREAM_MODE_WRITE, *strFilename);
    if (pStream)
    {
        RwTexDictionaryStreamWrite(pTxdDictionary, pStream);
        RwStreamClose(pStream, 0);
        return true;
    }
    return false;
}

bool CRenderWareSA::WriteDFF(const SString& strFilename, RpClump* pClump)
{
    auto RpClumpStreamWrite = (void(__cdecl*)(RpClump*, RwStream*))0x74AA10;

    RwStream* pStream = RwStreamOpen(STREAM_TYPE_FILENAME, STREAM_MODE_WRITE, *strFilename);
    if (pStream)
    {
        RpClumpStreamWrite(pClump, pStream);
        RwStreamClose(pStream, 0);
        return true;
    }
    return false;
}

bool CRenderWareSA::WriteTXD(void* pData, size_t dataSize, RwTexDictionary* pTxdDictionary)
{
    RwStream theStream;
    memset(&theStream, 0, sizeof(RwStream));

    theStream.type = STREAM_TYPE_BUFFER;
    theStream.mode = STREAM_MODE_WRITE;
    theStream.data.size = dataSize;
    theStream.data.ptr_file = pData;

    RwTexDictionaryStreamWrite(pTxdDictionary, &theStream);
    RwStreamClose(&theStream, 0);
    return true;
}

bool CRenderWareSA::WriteDFF(void* pData, size_t dataSize, RpClump* pClump)
{
    auto RpClumpStreamWrite = (void(__cdecl*)(RpClump*, RwStream*))0x74AA10;

    RwStream theStream;
    memset(&theStream, 0, sizeof(RwStream));

    theStream.type = STREAM_TYPE_BUFFER;
    theStream.mode = STREAM_MODE_WRITE;
    theStream.data.size = dataSize;
    theStream.data.ptr_file = pData;

    RpClumpStreamWrite(pClump, &theStream);
    RwStreamClose(&theStream, 0);
    return true;
}

RwTexDictionary* CRenderWareSA::CreateTextureDictionary(std::vector<RwTexture*>& vecTextures)
{
    auto RwTexDictionaryCreate = (RwTexDictionary * (__cdecl*)())0x7F3600;

    RwTexDictionary* pTextureDictionary = RwTexDictionaryCreate();
    for (auto& pTexture : vecTextures)
    {
        RwTexDictionaryAddTexture(pTextureDictionary, pTexture);
    }
    return pTextureDictionary;
}

void* RwEngineInstance_memoryAlloc_Skin()
{
    typedef void*(__cdecl * hmemoryAlloc)(void*, unsigned int);

    struct tRwGlobals
    {
        signed char  padding[324];
        hmemoryAlloc memoryAlloc;
    };

    void*&       rpSkinGlobals = *reinterpret_cast<void**>(0xC978B8);
    tRwGlobals*& RwEngineInstance = *reinterpret_cast<tRwGlobals**>(0xC97B24);
    return RwEngineInstance->memoryAlloc(rpSkinGlobals, 0x30116);
}

void* RwEngineInstance_memoryFuncs_rwmalloc(unsigned int size)
{
    typedef void*(__cdecl * hmemoryFuncs_rwmalloc)(unsigned int, unsigned int);

    struct tRwGlobals
    {
        char                  pad308[308];
        hmemoryFuncs_rwmalloc memoryFuncs_rwmalloc;
    };

    tRwGlobals*& RwEngineInstance = *reinterpret_cast<tRwGlobals**>(0xC97B24);
    return RwEngineInstance->memoryFuncs_rwmalloc(size, 0x30116);
}

// RwMatrixWeights
struct RwMatrixWeights
{
    float w0;
    float w1;
    float w2;
    float w3;
};

#pragma pack(push, 1)
struct RpSkin
{
    unsigned int     numBones;
    unsigned int     numBoneIds;
    unsigned char*   boneIds;
    RwMatrix*        skinToBoneMatrices;
    unsigned int     maxNumWeightsForVertex;
    unsigned int*    vertexBoneIndices;
    RwMatrixWeights* vertexBoneWeights;
    char             field_1C[8];
    // Split data start
    unsigned int    boneLimit;
    unsigned int    numMeshes;
    unsigned int    numRLE;
    unsigned char*  meshBoneRemapIndices;
    unsigned short* meshBoneRLECount;
    unsigned short* meshBoneRLE;
    // split data end
    void* pAllocatedMemory;            // does not include split data. Another block of memory is
                                       // allocated for split data and stored in meshBoneRemapIndices
};
#pragma pack(pop)

void CRenderWareSA::CopyGeometryPlugins(RpGeometry* pDestGeometry, RpGeometry* pSourceGeometry)
{
    RwTexture* CloneRwTexture(RwTexture * pTextureToCopyFrom);
    auto       breakable_operator_new = (void*(__cdecl*)(int a1))0x82119A;
    auto       CMemoryMgr_Malloc = (void*(__cdecl*)(int size))0x72F420;
    auto       _rpMeshHeaderCreate = (void*(__cdecl*)(int size))0x758920;

    CRenderWare*      pRenderWare = g_pCore->GetGame()->GetRenderWare();
    RwPluginRegistry& geometryTKList = *(RwPluginRegistry*)0x8D628C;
    for (auto pPluginRegistryEntry = geometryTKList.firstRegEntry; pPluginRegistryEntry; pPluginRegistryEntry = pPluginRegistryEntry->nextRegEntry)
    {
        DWORD* pDest = (DWORD*)(((uint8_t*)pDestGeometry) + pPluginRegistryEntry->offset);
        DWORD* pSource = (DWORD*)(((uint8_t*)pSourceGeometry) + pPluginRegistryEntry->offset);

        if (pPluginRegistryEntry->pluginID < 0x0253F2F0)
        {
            switch (pPluginRegistryEntry->pluginID)
            {
                // SkinPLG
                case 0x116:
                {
                    if (!*pSource)
                    {
                        break;
                    }

                    *pDest = (DWORD)RwEngineInstance_memoryAlloc_Skin();

                    RpSkin* pDestSkin = (RpSkin*)*pDest;
                    RpSkin* pSourceSkin = (RpSkin*)*pSource;
                    *pDestSkin = *pSourceSkin;

                    unsigned int skinDataSize = (pSourceSkin->numBones << 6) + 20 * pSourceGeometry->vertices_size + pSourceSkin->numBoneIds + 15;
                    uint8_t*     pDestMemory = (uint8_t*)RwEngineInstance_memoryFuncs_rwmalloc(skinDataSize);
                    memset(pDestMemory, 0, skinDataSize);

                    unsigned int sizeOfBoneIds = pSourceSkin->numBoneIds + 15;
                    unsigned int sizeOfSkinToBoneMatrices = sizeof(RwMatrix) * pSourceSkin->numBones;
                    unsigned int sizeOfVertexBoneIndices = sizeof(unsigned int) * pSourceGeometry->vertices_size;
                    unsigned int sizeOfVertexBoneWeights = sizeof(RwMatrixWeights) * pSourceGeometry->vertices_size;

                    pDestSkin->pAllocatedMemory = pDestMemory;
                    pDestSkin->boneIds = pDestMemory;
                    memcpy(pDestSkin->boneIds, pSourceSkin->boneIds, sizeOfBoneIds);
                    pDestMemory += sizeOfBoneIds;

                    pDestSkin->skinToBoneMatrices = (RwMatrix*)pDestMemory;
                    memcpy(pDestSkin->skinToBoneMatrices, pSourceSkin->skinToBoneMatrices, sizeOfSkinToBoneMatrices);
                    pDestMemory += sizeOfSkinToBoneMatrices;

                    pDestSkin->vertexBoneIndices = (unsigned int*)pDestMemory;
                    memcpy(pDestSkin->vertexBoneIndices, pSourceSkin->vertexBoneIndices, sizeOfVertexBoneIndices);
                    pDestMemory += sizeOfVertexBoneIndices;

                    pDestSkin->vertexBoneWeights = (RwMatrixWeights*)pDestMemory;
                    memcpy(pDestSkin->vertexBoneWeights, pSourceSkin->vertexBoneWeights, sizeOfVertexBoneWeights);

                    if (pSourceSkin->maxNumWeightsForVertex && pSourceSkin->meshBoneRemapIndices)
                    {
                        skinDataSize = pSourceSkin->numBones + 2 * (pSourceSkin->numMeshes + pSourceSkin->numRLE);
                        pDestMemory = (uint8_t*)RwEngineInstance_memoryFuncs_rwmalloc(skinDataSize);

                        unsigned int sizeOfMeshBoneRLECount = sizeof(unsigned short) * pSourceSkin->numMeshes;
                        unsigned int sizeOfMeshBoneRLE = sizeof(unsigned short) * pSourceSkin->numRLE;

                        pDestSkin->meshBoneRemapIndices = pDestMemory;

                        memcpy(pDestSkin->meshBoneRemapIndices, pSourceSkin->meshBoneRemapIndices, pSourceSkin->numBones);
                        pDestMemory += pSourceSkin->numBones;

                        pDestSkin->meshBoneRLECount = (unsigned short*)pDestMemory;
                        memcpy(pDestSkin->meshBoneRLECount, pSourceSkin->meshBoneRLECount, sizeOfMeshBoneRLECount);
                        pDestMemory += sizeOfMeshBoneRLECount;

                        pDestSkin->meshBoneRLE = (unsigned short*)pDestMemory;
                        memcpy(pDestSkin->meshBoneRLE, pSourceSkin->meshBoneRLE, sizeOfMeshBoneRLE);
                    }

                    break;
                }

                // BinMeshPLG
                case 0x50E:
                {
                    unsigned int size = (sizeof(RpMeshHeader) + ((sizeof(RpMesh) + 4) * pSourceGeometry->mesh->numMeshes));
                    unsigned int sizeOfIndicesInBytes = sizeof(RxVertexIndex) * pSourceGeometry->mesh->totalIndicesInMesh;
                    size += sizeOfIndicesInBytes;

                    pDestGeometry->mesh = (RpMeshHeader*)_rpMeshHeaderCreate(size);
                    memcpy(pDestGeometry->mesh, pSourceGeometry->mesh, sizeof(RpMeshHeader));

                    RpMesh*        pSourceMesh = (RpMesh*)(pSourceGeometry->mesh + 1);
                    RpMesh*        pDestMesh = (RpMesh*)(pDestGeometry->mesh + 1);
                    RxVertexIndex* meshIndices = (RxVertexIndex*)(pDestMesh + pSourceGeometry->mesh->numMeshes);

                    unsigned int numMeshes = pSourceGeometry->mesh->numMeshes;
                    while (numMeshes--)
                    {
                        pDestMesh->numIndices = pSourceMesh->numIndices;
                        pDestMesh->indices = meshIndices;
                        memcpy(pDestMesh->indices, pSourceMesh->indices, sizeof(RxVertexIndex) * pSourceMesh->numIndices);

                        pDestMesh->material = nullptr;
                        if (pSourceMesh->material)
                        {
                            for (int i = 0; i < pSourceGeometry->materials.entries; i++)
                            {
                                if (pSourceGeometry->materials.materials[i] == pSourceMesh->material)
                                {
                                    pDestMesh->material = pDestGeometry->materials.materials[i];
                                    break;
                                }
                            }
                        }

                        meshIndices += pSourceMesh->numIndices;
                        pDestMesh++;
                        pSourceMesh++;
                    }
                    break;
                }
            }
            continue;
        }

        if (!IsPluginUsed(pPluginRegistryEntry, pSourceGeometry) || !pPluginRegistryEntry->getSizeCB)
        {
            continue;
        }

        switch (pPluginRegistryEntry->pluginID)
        {
            // 2D effect plugin
            case 0x0253F2F8:
            {
                t2dEffectPlugin* pSourceEffectPlugin = *(t2dEffectPlugin**)(pSource);
                *pDest = (DWORD)CMemoryMgr_Malloc((pSourceEffectPlugin->uiCount << 6) + 4);

                t2dEffectPlugin* pDestEffectPlugin = *(t2dEffectPlugin**)(pDest);

                pDestEffectPlugin->uiCount = pSourceEffectPlugin->uiCount;

                for (unsigned int i = 0; i < pSourceEffectPlugin->uiCount; i++)
                {
                    C2dEffectSAInterface& theSource2DEffect = pSourceEffectPlugin->arrEffects[i];
                    C2dEffectSAInterface& theDest2DEffect = pDestEffectPlugin->arrEffects[i];
                    // pDestEffectPlugin->arrEffects[i] = theSource2DEffect;

                    theDest2DEffect.m_vecPosn = theSource2DEffect.m_vecPosn;
                    theDest2DEffect.m_nType = theSource2DEffect.m_nType;
                    unsigned char effectType = theSource2DEffect.m_nType;

                    switch (effectType)
                    {
                        case EFFECT_LIGHT:
                        {
                            theDest2DEffect.light = theSource2DEffect.light;
                            if (theSource2DEffect.light.m_pCoronaTex)
                            {
                                theDest2DEffect.light.m_pCoronaTex = pRenderWare->CloneRwTexture(theSource2DEffect.light.m_pCoronaTex);
                            }

                            if (theSource2DEffect.light.m_pShadowTex)
                            {
                                theDest2DEffect.light.m_pShadowTex = pRenderWare->CloneRwTexture(theSource2DEffect.light.m_pShadowTex);
                            }
                            break;
                        }
                        case EFFECT_PARTICLE:
                        {
                            theDest2DEffect.particle = theSource2DEffect.particle;
                            break;
                        }
                        case EFFECT_ATTRACTOR:
                        {
                            theDest2DEffect.pedAttractor = theSource2DEffect.pedAttractor;
                            break;
                        }
                        case EFFECT_ENEX:
                        {
                            theDest2DEffect.enEx = theSource2DEffect.enEx;
                            break;
                        }
                        case EFFECT_ROADSIGN:
                        {
                            theDest2DEffect.roadsign = theSource2DEffect.roadsign;

                            char* pText = (char*)CMemoryMgr_Malloc(64);
                            char* pSourceText = theSource2DEffect.roadsign.m_pText;
                            memcpy(pText, pSourceText, 64);
                            theDest2DEffect.roadsign.m_pText = pText;

                            break;
                        }
                        case EFFECT_SLOTMACHINE_WHEEL:
                        {
                            theDest2DEffect.iSlotMachineIndex = theSource2DEffect.iSlotMachineIndex;
                            break;
                        }
                        case EFFECT_COVER_POINT:
                        {
                            theDest2DEffect.coverPoint = theSource2DEffect.coverPoint;
                            break;
                        }
                        case EFFECT_ESCALATOR:
                        {
                            theDest2DEffect.escalator = theSource2DEffect.escalator;
                            break;
                        }
                        default:
                        {
                            printf("unknown 2D effect\n");
                        }
                    }
                }
                break;
            }

            // ExtraVertColours plugin (night colors)
            case 0x253F2F9:
            {
                ExtraVertColourPlugin* pSourceExtraVertColour = (ExtraVertColourPlugin*)(pSource);
                ExtraVertColourPlugin* pDestExtraVertColour = (ExtraVertColourPlugin*)(pDest);

                pDestExtraVertColour->nightColors = (RwRGBA*)CMemoryMgr_Malloc(4 * pSourceGeometry->vertices_size);
                pDestExtraVertColour->dayColors = (RwRGBA*)CMemoryMgr_Malloc(4 * pSourceGeometry->vertices_size);
                memcpy(pDestExtraVertColour->nightColors, pSourceExtraVertColour->nightColors, 4 * pSourceGeometry->vertices_size);
                break;
            }

            // breakable plugin
            case 0x0253F2FD:
            {
                int iRegistryEntrySize = pPluginRegistryEntry->getSizeCB(pSourceGeometry, pPluginRegistryEntry->offset, pPluginRegistryEntry->size);
                *pDest = (DWORD)breakable_operator_new(iRegistryEntrySize - 4);
                GeometryMeshExt* pDestGeometryMeshExt = *(GeometryMeshExt**)pDest;
                GeometryMeshExt* pSourceGeometryMeshExt = *(GeometryMeshExt**)pSource;
                *pDestGeometryMeshExt = *pSourceGeometryMeshExt;

                unsigned int sizeOfVertices = sizeof(RwV3d) * pSourceGeometryMeshExt->m_usNumVertices;
                unsigned int sizeOfTexCoords = sizeof(RwTextureCoordinates) * pSourceGeometryMeshExt->m_usNumVertices;
                unsigned int sizeOfVertexColors = sizeof(RwRGBA) * pSourceGeometryMeshExt->m_usNumVertices;
                unsigned int sizeOfTriangles = sizeof(RpBreakableTriangle) * pSourceGeometryMeshExt->m_usNumTriangles;
                unsigned int sizeOfMaterialAssignments = sizeof(unsigned short) * pSourceGeometryMeshExt->m_usNumTriangles;
                unsigned int sizeOfTextureNames = 32 * pSourceGeometryMeshExt->m_usNumMaterials;
                unsigned int sizeOfMaskNames = 32 * pSourceGeometryMeshExt->m_usNumMaterials;
                unsigned int sizeOfMaterialProperties = sizeof(RwSurfaceProperties) * pSourceGeometryMeshExt->m_usNumMaterials;

                uint8_t* pDestMemory = (uint8_t*)*pDest;
                pDestMemory += sizeof(GeometryMeshExt);

                pDestGeometryMeshExt->m_pVertexPos = (RwV3d*)pDestMemory;
                memcpy(pDestGeometryMeshExt->m_pVertexPos, pSourceGeometryMeshExt->m_pVertexPos, sizeOfVertices);
                pDestMemory += sizeOfVertices;

                pDestGeometryMeshExt->m_pTexCoors = (RwTextureCoordinates*)pDestMemory;
                memcpy(pDestGeometryMeshExt->m_pTexCoors, pSourceGeometryMeshExt->m_pTexCoors, sizeOfTexCoords);
                pDestMemory += sizeOfTexCoords;

                pDestGeometryMeshExt->m_pVertexColors = (RwRGBA*)pDestMemory;
                memcpy(pDestGeometryMeshExt->m_pVertexColors, pSourceGeometryMeshExt->m_pVertexColors, sizeOfVertexColors);
                pDestMemory += sizeOfVertexColors;

                pDestGeometryMeshExt->m_pTriangles = (RpTriangle*)pDestMemory;
                memcpy(pDestGeometryMeshExt->m_pTriangles, pSourceGeometryMeshExt->m_pTriangles, sizeOfTriangles);
                pDestMemory += sizeOfTriangles;

                pDestGeometryMeshExt->m_pMaterialAssignments = (unsigned short*)pDestMemory;
                memcpy(pDestGeometryMeshExt->m_pMaterialAssignments, pSourceGeometryMeshExt->m_pMaterialAssignments, sizeOfMaterialAssignments);
                pDestMemory += sizeOfMaterialAssignments;

                pDestGeometryMeshExt->m_pTextureNames = (char*)pDestMemory;
                memcpy(pDestGeometryMeshExt->m_pTextureNames, pSourceGeometryMeshExt->m_pTextureNames, sizeOfTextureNames);
                pDestMemory += sizeOfTextureNames;

                pDestGeometryMeshExt->m_pMaskNames = (char*)pDestMemory;
                memcpy(pDestGeometryMeshExt->m_pMaskNames, pSourceGeometryMeshExt->m_pMaskNames, sizeOfMaskNames);
                pDestMemory += sizeOfMaskNames;

                pDestGeometryMeshExt->m_pMaterialProperties = (RwSurfaceProperties*)pDestMemory;
                memcpy(pDestGeometryMeshExt->m_pMaterialProperties, pSourceGeometryMeshExt->m_pMaterialProperties, sizeOfMaterialProperties);

                break;
            }
        }
    }
}

//
// Returns list of atomics inside a clump
//
void CRenderWareSA::GetClumpAtomicList(RpClump* pClump, std::vector<RpAtomic*>& outAtomicList)
{
    RpClumpForAllAtomics(pClump,
                         [](RpAtomic* pAtomic, void* pData) {
                             reinterpret_cast<std::vector<RpAtomic*>*>(pData)->push_back(pAtomic);
                             return true;
                         },
                         &outAtomicList);
}

//
// Returns true if the clump geometry sort of matches
//
// ClumpA vs ClumpB(or)AtomicB
//
bool CRenderWareSA::DoContainTheSameGeometry(RpClump* pClumpA, RpClump* pClumpB, RpAtomic* pAtomicB)
{
    // Fast check if comparing one atomic
    if (pAtomicB)
    {
        RpGeometry* pGeometryA = ((RpAtomic*)((pClumpA->atomics.root.next) - 0x8))->geometry;
        RpGeometry* pGeometryB = pAtomicB->geometry;
        return pGeometryA == pGeometryB;
    }

    // Get atomic list from both sides
    std::vector<RpAtomic*> atomicListA;
    std::vector<RpAtomic*> atomicListB;
    GetClumpAtomicList(pClumpA, atomicListA);
    if (pClumpB)
        GetClumpAtomicList(pClumpB, atomicListB);
    if (pAtomicB)
        atomicListB.push_back(pAtomicB);

    // Count geometries that exist in both sides
    std::set<RpGeometry*> geometryListA;
    for (uint i = 0; i < atomicListA.size(); i++)
        MapInsert(geometryListA, atomicListA[i]->geometry);

    uint uiInBoth = 0;
    for (uint i = 0; i < atomicListB.size(); i++)
        if (MapContains(geometryListA, atomicListB[i]->geometry))
            uiInBoth++;

    // If less than 50% match then assume it is not the same
    if (uiInBoth * 2 < atomicListB.size() || atomicListB.size() == 0)
        return false;

    return true;
}

// Replaces a vehicle/weapon/ped model
void CRenderWareSA::ReplaceModel(RpClump* pNew, unsigned short usModelID, DWORD dwFunc)
{
    CModelInfo* pModelInfo = pGame->GetModelInfo(usModelID);
    if (pModelInfo)
    {
        RpClump* pOldClump = (RpClump*)pModelInfo->GetRwObject();
        if (!DoContainTheSameGeometry(pNew, pOldClump, NULL))
        {
            if (pModelInfo->IsVehicle())
            {
                // Reset our valid upgrade list
                pModelInfo->ResetSupportedUpgrades();
            }

            // Make new clump container for the model geometry
            // Clone twice as the geometry render order seems to be reversed each time it is cloned.
            RpClump* pTemp = RpClumpClone(pNew);
            RpClump* pNewClone = RpClumpClone(pTemp);
            RpClumpDestroy(pTemp);

            // Calling CVehicleModelInfo::SetClump() allocates a new CVehicleStructure.
            // So let's delete the old one first to avoid CPool<CVehicleStructure> depletion.
            if (dwFunc == FUNC_LoadVehicleModel)
            {
                CVehicleModelInfoSAInterface* pVehicleModelInfoInterface = (CVehicleModelInfoSAInterface*)pModelInfo->GetInterface();
                if (pVehicleModelInfoInterface->pVisualInfo)
                {
                    DWORD                               dwDeleteFunc = FUNC_CVehicleStructure_delete;
                    CVehicleModelVisualInfoSAInterface* info = pVehicleModelInfoInterface->pVisualInfo;
                    _asm
                        {
                        mov     eax, info
                        push    eax
                        call    dwDeleteFunc
                        add     esp, 4
                        }
                    pVehicleModelInfoInterface->pVisualInfo = nullptr;
                }
            }

            // ModelInfo::SetClump
            CBaseModelInfoSAInterface* pModelInfoInterface = pModelInfo->GetInterface();
            _asm
                {
                mov     ecx, pModelInfoInterface
                push    pNewClone
                call    dwFunc
                }

            // Destroy old clump container
            RpClumpDestroy(pOldClump);
        }
    }
}

// Replaces a vehicle model
void CRenderWareSA::ReplaceVehicleModel(RpClump* pNew, unsigned short usModelID)
{
    ReplaceModel(pNew, usModelID, FUNC_LoadVehicleModel);
}

// Replaces a weapon model
void CRenderWareSA::ReplaceWeaponModel(RpClump* pNew, unsigned short usModelID)
{
    ReplaceModel(pNew, usModelID, FUNC_LoadWeaponModel);
}

// Replaces a ped model
void CRenderWareSA::ReplacePedModel(RpClump* pNew, unsigned short usModelID)
{
    ReplaceModel(pNew, usModelID, FUNC_LoadPedModel);
}

// Reads and parses a COL3 file
CColModel* CRenderWareSA::ReadCOL(const CBuffer& fileData)
{
    if (fileData.GetSize() < sizeof(ColModelFileHeader) + 16)
        return NULL;

    const ColModelFileHeader& header = *(ColModelFileHeader*)fileData.GetData();

    // Load the col model
    if (header.version[0] == 'C' && header.version[1] == 'O' && header.version[2] == 'L')
    {
        unsigned char* pModelData = (unsigned char*)fileData.GetData() + sizeof(ColModelFileHeader);

        // Create a new CColModel
        CColModelSA* pColModel = new CColModelSA();

        if (header.version[3] == 'L')
        {
            LoadCollisionModel(pModelData, pColModel->GetInterface(), NULL);
        }
        else if (header.version[3] == '2')
        {
            LoadCollisionModelVer2(pModelData, header.size - 0x18, pColModel->GetInterface(), NULL);
        }
        else if (header.version[3] == '3')
        {
            LoadCollisionModelVer3(pModelData, header.size - 0x18, pColModel->GetInterface(), NULL);
        }

        // Return the collision model
        return pColModel;
    }

    return NULL;
}

// Loads all atomics from a clump into a container struct and returns the number of atomics it loaded
unsigned int CRenderWareSA::LoadAtomics(RpClump* pClump, RpAtomicContainer* pAtomics)
{
    // iterate through all atomics in the clump
    SLoadAtomics data = {0};
    data.pReplacements = pAtomics;
    RpClumpForAllAtomics(pClump, LoadAtomicsCB, &data);

    // return the number of atomics that were added to the container
    return data.uiReplacements;
}

// Replaces all atomics for a specific model
typedef struct
{
    unsigned short usTxdID;
    RpClump*       pClump;
} SAtomicsReplacer;
bool AtomicsReplacer(RpAtomic* pAtomic, void* data)
{
    SAtomicsReplacer* pData = reinterpret_cast<SAtomicsReplacer*>(data);
    ((void (*)(RpAtomic*, void*))FUNC_AtomicsReplacer)(pAtomic, pData->pClump);
    // The above function adds a reference to the model's TXD. Remove it again.
    CTxdStore_RemoveRef(pData->usTxdID);
    return true;
}

void CRenderWareSA::ReplaceAllAtomicsInModel(RpClump* pNew, unsigned short usModelID)
{
    CModelInfo* pModelInfo = pGame->GetModelInfo(usModelID);
    if (pModelInfo)
    {
        RpAtomic* pOldAtomic = (RpAtomic*)pModelInfo->GetRwObject();
        if (!DoContainTheSameGeometry(pNew, NULL, pOldAtomic))
        {
            // Clone the clump that's to be replaced (FUNC_AtomicsReplacer removes the atomics from the source clump)
            RpClump* pCopy = RpClumpClone(pNew);

            // Replace the atomics
            SAtomicsReplacer data;
            data.usTxdID = ((CBaseModelInfoSAInterface**)ARRAY_ModelInfo)[usModelID]->usTextureDictionary;
            data.pClump = pCopy;

            MemPutFast<DWORD>((DWORD*)DWORD_AtomicsReplacerModelID, usModelID);
            RpClumpForAllAtomics(pCopy, AtomicsReplacer, &data);

            // Get rid of the now empty copied clump
            RpClumpDestroy(pCopy);
        }
    }
}

// Replaces all atomics in a vehicle
void CRenderWareSA::ReplaceAllAtomicsInClump(RpClump* pDst, RpAtomicContainer* pAtomics, unsigned int uiAtomics)
{
    SReplaceAll data;
    data.pClump = pDst;
    data.pReplacements = pAtomics;
    data.uiReplacements = uiAtomics;
    RpClumpForAllAtomics(pDst, ReplaceAllCB, &data);
}

// Replaces the wheels in a vehicle
void CRenderWareSA::ReplaceWheels(RpClump* pClump, RpAtomicContainer* pAtomics, unsigned int uiAtomics, const char* szWheel)
{
    // get the clump's frame
    RwFrame* pFrame = RpGetFrame(pClump);

    SReplaceWheels data;
    data.pClump = pClump;
    data.pReplacements = pAtomics;
    data.uiReplacements = uiAtomics;
    data.szName = szWheel;
    RpClumpForAllAtomics(pClump, ReplaceWheelsCB, &data);
}

// Repositions an atomic
void CRenderWareSA::RepositionAtomic(RpClump* pDst, RpClump* pSrc, const char* szName)
{
    RwFrame* pDstFrame = RpGetFrame(pDst);
    RwFrame* pSrcFrame = RpGetFrame(pSrc);
    RwFrameCopyMatrix(RwFrameFindFrame(pDstFrame, szName), RwFrameFindFrame(pSrcFrame, szName));
}

// Adds the atomics from a source clump (pSrc) to a destination clump (pDst)
void CRenderWareSA::AddAllAtomics(RpClump* pDst, RpClump* pSrc)
{
    RpClumpForAllAtomics(pSrc, AddAllAtomicsCB, pDst);
}

// Replaces dynamic parts of the vehicle (models that have two different versions: 'ok' and 'dam'), such as doors
// szName should be without the part suffix (e.g. 'door_lf' or 'door_rf', and not 'door_lf_dummy')
bool CRenderWareSA::ReplacePartModels(RpClump* pClump, RpAtomicContainer* pAtomics, unsigned int uiAtomics, const char* szName)
{
    // get the part's dummy name
    char szDummyName[16] = {0};
    snprintf(&szDummyName[0], 16, "%s_dummy", szName);

    // get the clump's frame
    RwFrame* pFrame = RpGetFrame(pClump);

    // get the part's dummy frame
    RwFrame* pPart = RwFrameFindFrame(pFrame, &szDummyName[0]);
    if (pPart == NULL)
        return false;

    // now replace all the objects in the frame
    SReplaceParts data = {0};
    data.pReplacements = pAtomics;
    data.uiReplacements = uiAtomics;
    data.szName = szName;
    RwFrameForAllObjects(pPart, ReplacePartsCB, &data);

    return true;
}

// Replaces a CColModel for a specific object identified by the object id (usModelID)
void CRenderWareSA::ReplaceCollisions(CColModel* pCol, unsigned short usModelID)
{
    DWORD*        pPool = (DWORD*)ARRAY_ModelInfo;
    CColModelSA*  pColModel = (CColModelSA*)pCol;
    CModelInfoSA* pModelInfoSA = (CModelInfoSA*)(pGame->GetModelInfo(usModelID));

    // Apply some low-level hacks (copies the old col area and sets a flag)
    DWORD pColModelInterface = (DWORD)pColModel->GetInterface();
    DWORD pOldColModelInterface = *((DWORD*)pPool[usModelID] + 20);
    MemOrFast<BYTE>(pPool[usModelID] + 0x13, 8);
    MemPutFast<BYTE>(pColModelInterface + 40, *((BYTE*)(pOldColModelInterface + 40)));

    // TODO: It seems that on entering the game, when this function is executed, the modelinfo array for this
    // model is still zero, leading to a crash!
    pModelInfoSA->IsLoaded();
}

// Destroys a DFF instance
void CRenderWareSA::DestroyDFF(RpClump* pClump)
{
    if (pClump)
        RpClumpDestroy(pClump);
}

// Destroys a TXD instance
void CRenderWareSA::DestroyTXD(RwTexDictionary* pTXD)
{
    if (pTXD)
        RwTexDictionaryDestroy(pTXD);
}

// Destroys a texture instance
void CRenderWareSA::DestroyTexture(RwTexture* pTex)
{
    if (pTex)
    {
        ScriptRemovedTexture(pTex);
        RwTextureDestroy(pTex);
    }
}

RwTexture* CRenderWareSA::CloneRwTexture(RwTexture* pTextureToCopyFrom)
{
    auto       CClothesBuilder_CopyTexture = (RwTexture * (__cdecl*)(RwTexture * texture))0x5A5730;
    RwTexture* pCopiedTexture = CClothesBuilder_CopyTexture(pTextureToCopyFrom);
    if (pCopiedTexture)
    {
        memcpy(pCopiedTexture->name, pTextureToCopyFrom->name, RW_TEXTURE_NAME_LENGTH);
        memcpy(pCopiedTexture->mask, pTextureToCopyFrom->mask, RW_TEXTURE_NAME_LENGTH);
    }
    return pCopiedTexture;
}

void CRenderWareSA::AddTextureToDictionary(RwTexDictionary* pTextureDictionary, RwTexture* pTexture)
{
    if (pTexture)
    {
        RwTexDictionaryAddTexture(pTextureDictionary, pTexture);
    }
}

RwTexDictionary* CRenderWareSA::CopyTexturesFromDictionary(RwTexDictionary* pResultTextureDictionary, RwTexDictionary* pTextureDictionaryToCopyFrom)
{
    auto CClothesBuilder_CopyTexture = (RwTexture * (__cdecl*)(RwTexture * texture))0x5A5730;

    std::vector<RwTexture*> outTextureList;
    GetTxdTextures(outTextureList, pTextureDictionaryToCopyFrom);

    for (auto& pTexture : outTextureList)
    {
        RwTexture* pCopiedTexture = CloneRwTexture(pTexture);
        if (pCopiedTexture)
        {
            RwTexDictionaryAddTexture(pResultTextureDictionary, pCopiedTexture);
        }
    }
    return pResultTextureDictionary;
}

_rwD3D9RasterExt* CRenderWareSA::GetRasterExt(RwRaster* raster)
{
    return RASTEREXTFROMRASTER(raster->parent);
}

D3DFORMAT CRenderWareSA::GetRasterD3DFormat(RwRaster* raster)
{
    _rwD3D9RasterExt* rasterExt = RASTEREXTFROMRASTER(raster->parent);
    return rasterExt->d3dFormat;
}

bool CRenderWareSA::IsRasterCompressed(RwRaster* raster)
{
    _rwD3D9RasterExt* rasterExt = RASTEREXTFROMRASTER(raster->parent);
    return rasterExt->compressed;
}

void GetRasterRect(RwRaster* raster, RECT& rect)
{
    rect.left = raster->nOffsetX;
    rect.top = raster->nOffsetY;
    rect.right = raster->nOffsetX + raster->width;
    rect.bottom = raster->nOffsetY + raster->height;
}

RwTexture* CRenderWareSA::RwTextureCreateWithFormat(RwTexture* pTexture, D3DFORMAT textureFormat)
{
    auto RwTextureCreate = (RwTexture * (__cdecl*)(RwRaster * raster))0x007F37C0;
    auto RwRasterLock = (RwUInt8 * (__cdecl*)(RwRaster * raster, RwUInt8 level, RwInt32 lockMode))0x07FB2D0;
    auto RwRasterUnlock = (RwRaster * (__cdecl*)(RwRaster * raster))0x7FAEC0;
    auto RwD3D9RasterCreate = (RwRaster * (__cdecl*)(RwUInt32 width, RwUInt32 height, RwUInt32 d3dFormat, RwUInt32 flags))0x4CD050;

    RwRaster* raster = pTexture->raster;

    HRESULT hr = NULL;

    RwRaster* convertedRaster = RwD3D9RasterCreate(raster->width, raster->height, textureFormat, rwRASTERTYPETEXTURE | (textureFormat & 0x9000));
    if (!convertedRaster)
    {
        std::printf("RwD3D9RasterCreate: Failed\n");
        return nullptr;
    }

    _rwD3D9RasterExt* rasterExt = GetRasterExt(raster);
    _rwD3D9RasterExt* convertedRasterExt = GetRasterExt(convertedRaster);

    RECT sourceRect, destinationRect;
    GetRasterRect(raster, sourceRect);
    GetRasterRect(convertedRaster, destinationRect);

    IDirect3DSurface9* sourceSurface;
    hr = rasterExt->texture->GetSurfaceLevel(0, &sourceSurface);
    if (hr != D3D_OK)
    {
        std::printf("Get surface level for sourceSurface failed with error: %#.8x\n", hr);
        return nullptr;
    }

    IDirect3DSurface9* destinationSurface;
    hr = convertedRasterExt->texture->GetSurfaceLevel(0, &destinationSurface);
    if (hr != D3D_OK)
    {
        std::printf("Get surface level for destinationSurface failed with error: %#.8x\n", hr);
        return nullptr;
    }

    hr = g_pCore->GetGraphics()->LoadSurfaceFromSurface(destinationSurface, NULL, &destinationRect, sourceSurface, NULL, &sourceRect, D3DX_DEFAULT, 0);
    if (FAILED(hr))
    {
        std::printf("D3DXLoadSurfaceFromSurface failed with error: %#.8x\n", hr);
        return nullptr;
    }

    sourceSurface->Release();
    destinationSurface->Release();

    RwTexture* pConvertedTexture = RwTextureCreate(convertedRaster);
    memcpy(pConvertedTexture->name, pTexture->name, RW_TEXTURE_NAME_LENGTH);
    memcpy(pConvertedTexture->mask, pTexture->mask, RW_TEXTURE_NAME_LENGTH);

    std::printf("texture successfully converted to D3DFormat = %u FROM d3dFOrmat: %u\n", textureFormat, rasterExt->d3dFormat);

    return pConvertedTexture;
}

void CRenderWareSA::RwTexDictionaryRemoveTexture(RwTexDictionary* pTXD, RwTexture* pTex)
{
    if (pTex->txd != pTXD)
        return;

    pTex->TXDList.next->prev = pTex->TXDList.prev;
    pTex->TXDList.prev->next = pTex->TXDList.next;
    pTex->txd = NULL;
}

short CRenderWareSA::CTxdStore_GetTxdRefcount(unsigned short usTxdID)
{
    return *(short*)(*(*(DWORD**)0xC8800C) + 0xC * usTxdID + 4);
}

bool CRenderWareSA::RwTexDictionaryContainsTexture(RwTexDictionary* pTXD, RwTexture* pTex)
{
    return pTex->txd == pTXD;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::TxdForceUnload
//
// 1. Unload txd from memory
// 2. Cross fingers
//
// Why do we do this?
// Player model adds (seemingly) unnecessary refs
// (Will crash if anything is actually using the txd)
//
// No idea what will happen if there is a custom txd replacement
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::TxdForceUnload(ushort usTxdId, bool bDestroyTextures)
{
    RwTexDictionary* pTxd = CTxdStore_GetTxd(usTxdId);
    if (!pTxd)
        return;

    // We can abandon the textures instead of destroy. It might be safer, but will cause a memory leak
    if (bDestroyTextures)
    {
        // Unref the textures
        std::vector<RwTexture*> textureList;
        pGame->GetRenderWareSA()->GetTxdTextures(textureList, pTxd);
        for (std::vector<RwTexture*>::iterator iter = textureList.begin(); iter != textureList.end(); iter++)
        {
            RwTexture* pTexture = *iter;
            while (pTexture->refs > 1)
                RwTextureDestroy(pTexture);
            RwTextureDestroy(pTexture);
        }
    }

    // Need to have at least one ref for RemoveRef to work correctly
    if (CTxdStore_GetNumRefs(usTxdId) == 0)
    {
        CTxdStore_AddRef(usTxdId);
    }

    while (CTxdStore_GetNumRefs(usTxdId) > 0)
    {
        CTxdStore_RemoveRef(usTxdId);
    }
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetTXDIDForModelID
//
// Get a TXD ID associated with the model ID
//
////////////////////////////////////////////////////////////////
ushort CRenderWareSA::GetTXDIDForModelID(ushort usModelID)
{
    if (usModelID >= 20000 && usModelID < 25000)
    {
        // Get global TXD ID instead
        return usModelID - 20000;
    }
    else
    {
        // Get the CModelInfo's TXD ID

        // Ensure valid
        if (usModelID >= 20000 || !((CBaseModelInfoSAInterface**)ARRAY_ModelInfo)[usModelID])
            return 0;

        return ((CBaseModelInfoSAInterface**)ARRAY_ModelInfo)[usModelID]->usTextureDictionary;
    }
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetModelTextureNames
//
// Get list of texture names associated with the model
// Will try to load the model if needed
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::GetModelTextureNames(std::vector<SString>& outNameList, ushort usModelId)
{
    outNameList.clear();

    // Special case for CJ
    if (usModelId == 0)
    {
        outNameList.push_back("CJ");
        return;
    }

    ushort usTxdId = GetTXDIDForModelID(usModelId);

    if (usTxdId == 0)
        return;

    // Get the TXD corresponding to this ID
    RwTexDictionary* pTXD = CTxdStore_GetTxd(usTxdId);

    bool bLoadedModel = false;
    if (!pTXD)
    {
        // Try model load
        bLoadedModel = true;
        pGame->GetModelInfo(usModelId)->Request(BLOCKING, "CRenderWareSA::GetModelTextureNames");
        pTXD = CTxdStore_GetTxd(usTxdId);
    }

    std::vector<RwTexture*> textureList;
    GetTxdTextures(textureList, pTXD);

    for (std::vector<RwTexture*>::iterator iter = textureList.begin(); iter != textureList.end(); iter++)
    {
        outNameList.push_back((*iter)->name);
    }

    if (bLoadedModel)
        ((void(__cdecl*)(unsigned short))FUNC_RemoveModel)(usModelId);
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetTxdTextures
//
// If TXD must already be loaded
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::GetTxdTextures(std::vector<RwTexture*>& outTextureList, ushort usTxdId)
{
    if (usTxdId == 0)
        return;

    // Get the TXD corresponding to this ID
    RwTexDictionary* pTXD = CTxdStore_GetTxd(usTxdId);

    if (!pTXD)
        return;

    GetTxdTextures(outTextureList, pTXD);
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetTxdTextures
//
// Get textures from a TXD
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::GetTxdTextures(std::vector<RwTexture*>& outTextureList, RwTexDictionary* pTXD)
{
    if (pTXD)
    {
        RwTexDictionaryForAllTextures(pTXD, StaticGetTextureCB, &outTextureList);
    }
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::StaticGetTextureCB
//
// Callback used in GetTxdTextures
//
////////////////////////////////////////////////////////////////
bool CRenderWareSA::StaticGetTextureCB(RwTexture* texture, std::vector<RwTexture*>* pTextureList)
{
    pTextureList->push_back(texture);
    return true;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetTextureName
//
// Only called by CRenderItemManager::GetVisibleTextureNames ?
//
////////////////////////////////////////////////////////////////
const char* CRenderWareSA::GetTextureName(CD3DDUMMY* pD3DData)
{
    STexInfo** ppTexInfo = MapFind(m_D3DDataTexInfoMap, pD3DData);
    if (ppTexInfo)
        return (*ppTexInfo)->strTextureName;
    if (!pD3DData)
        return FAKE_NAME_NO_TEXTURE;
    return "";
}

//
// CFastHashMap functions
//
CD3DDUMMY* GetEmptyMapKey(CD3DDUMMY**)
{
    return FAKE_D3DTEXTURE_EMPTY_KEY;
}

CD3DDUMMY* GetDeletedMapKey(CD3DDUMMY**)
{
    return FAKE_D3DTEXTURE_DELETED_KEY;
}

RwFrame* CRenderWareSA::GetFrameFromName(RpClump* pRoot, SString strName)
{
    return RwFrameFindFrame(RpGetFrame(pRoot), strName);
}
