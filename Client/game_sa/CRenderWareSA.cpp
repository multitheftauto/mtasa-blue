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

extern CGameSA* pGame;

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

bool IsPluginUsed(RwPluginRegEntry *pPluginRegistryEntry, void* pObject)
{
    if (pPluginRegistryEntry->pluginID >= (DWORD)0x0253F2F0)
    {
        //std::printf("IsPluginUsed: pluginId = %#.8x\n", pPluginRegistryEntry->pluginID);
        switch (pPluginRegistryEntry->pluginID)
        {
            /*
            case (DWORD)0x0253F2F3: // pipeline_set
            {
             return false;
            }
            */

        case (DWORD)0x0253F2F4: // unused_5
        {
            //return false;

            DWORD * pPluginData = (DWORD*)(pPluginRegistryEntry->offset + ((unsigned char*)pObject));

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

        case (DWORD)0x0253F2F6: // specular_material
        {
           // return false;

            DWORD * pPluginData = (DWORD*)(pPluginRegistryEntry->offset + ((unsigned char*)pObject));

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
        case (DWORD)0x0253F2F8: // effect_2d
        {
         
            DWORD * pPluginData = (DWORD*)(pPluginRegistryEntry->offset + ((unsigned char*)pObject));

            if (!*pPluginData)
            {
                return false;
            }
            return true;
           
        }


        case (DWORD)0x0253F2F9: // extra_vert_colour
        {
            //return false;

            DWORD * pPluginData = (DWORD*)(pPluginRegistryEntry->offset + ((unsigned char*)pObject));

            if (!*pPluginData)
            {
                return false;
            }
            return true;

        }
       // /*
        case (DWORD)0x0253F2FA: // collision_model
        {
        return false;
        }
       // */
        /*
        case (DWORD)0x0253F2FB: // gta_hanim
        {
        return false;
        }
        */
        case (DWORD)0x0253F2FC: // reflection_material
        {
           // return false;

            DWORD * pPluginData = (DWORD*)(pPluginRegistryEntry->offset + ((unsigned char*)pObject));

            // CCustomCarEnvMapPipeline::fakeEnvMapPipeMatData = 0xc02d18
            if (*pPluginData == (DWORD)0xc02d18 || !*pPluginData)
            {
                return false;
            }
            return true;

        }

        case (DWORD)0x0253F2FD: // breakable
        {
            //return false;

            DWORD * pPluginData = (DWORD*)(pPluginRegistryEntry->offset + ((unsigned char*)pObject));

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
    return true;;
}

int  __cdecl _rwPluginRegistryGetSize(RwPluginRegistry *pPluginRegistry, void* pObject)
{
    int iTotalPluginsSize = 0;

    RwPluginRegEntry *pPluginRegistryEntry = pPluginRegistry->firstRegEntry;
    for (iTotalPluginsSize = 0; pPluginRegistryEntry; pPluginRegistryEntry = pPluginRegistryEntry->nextRegEntry)
    {
        bool bPluginUsed = IsPluginUsed(pPluginRegistryEntry, pObject);
        if (!bPluginUsed)
        {
            //std::printf("rwPluginRegistryGetSize: skipping for pluginID: '%#.8x' | pObject = %p\n", pPluginRegistryEntry->pluginID, pObject);
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

RwPluginRegistry *__cdecl _rwPluginRegistryWriteDataChunks(RwPluginRegistry *a1, RwStream *stream, const void* object)
{

    auto _rwStreamWriteVersionedChunkHeader = (RwStream *(__cdecl*)(RwStream *stream, int type, int size, int version, unsigned __int16 buildNum))0x7ED270;


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
              //std::printf("_rwPluginRegistryWriteDataChunks: skipping for pluginID: '%#.8x' | pObject = %p\n", pPluginRegistryEntry->pluginID, (void*)object);
            }

            if (pPluginRegistryEntry->writeCB && bPluginUsed)
            {
                if (pPluginRegistryEntry->pluginID == (DWORD)0x0253F2F3)
                {
                    int i = 0;
                }

                //std::printf("rwWriteDataChunks: pluginID: '%#.8x' | pObject = %p\n", pPluginRegistryEntry->pluginID, object);

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
    unsigned int m_uiPosRule;
    unsigned short m_usNumVertices;
    short _pad0;
    RwV3d *m_pVertexPos;
    RwTextureCoordinates *m_pTexCoors;
    RwRGBA *m_pVertexColors;
    unsigned short m_usNumTriangles;
    short _pad1;
    RpTriangle *m_pTriangles;
    unsigned short *m_pMaterialAssignments;
    unsigned short m_usNumMaterials;
    short _pad2;
    RwTexture **m_pTextures;
    char *m_pTextureNames;
    char *m_pMaskNames;
    RwSurfaceProperties *m_pMaterialProperties;
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


RwStream *__cdecl PipelinePluginWriteCB(RwStream *stream, int length, unsigned char * pObject, int offsetInObject)
{
    auto RwStreamWrite = (RwStream *(__cdecl*)(RwStream *stream, void *buffer, int length))0x7ECB30;

    RwStreamWrite(stream, (void*)(pObject + offsetInObject), length);
    return stream;
}

void*__cdecl PipelinePluginDestructCB(void *object, int offsetInObject, int sizeInObject)
{
    return object;
}

struct ExtraVertColourPlugin
{
    RwRGBA* nightColors; // array of RwRGBAs per vertice
    RwRGBA* dayColors; // array of RwRGBAs per vertice
    float dnParam;
};

RwStream *__cdecl PluginExtraVertColourStreamWriteCB(RwStream *stream, int length, unsigned char * pObject, int offsetInObject)
{
    auto RwStreamWrite = (RwStream *(__cdecl*)(RwStream *stream, void *buffer, int length))0x7ECB30;

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
    int size = 0;
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
    unsigned int uiCount;
    C2dEffectSAInterface arrEffects [];
};

RwStream *__cdecl Plugin2DEffectStreamWriteCB(RwStream *stream, int length, unsigned char * pObject, int offsetInObject)
{
    auto RwStreamWrite = (RwStream *(__cdecl*)(RwStream *stream, void *buffer, int length))0x7ECB30;

    t2dEffectPlugin* pEffectPlugin = *(t2dEffectPlugin**)(pObject + offsetInObject);
    if (!pEffectPlugin)
    {
        return stream;
    }

    RwStreamWrite(stream, &pEffectPlugin->uiCount, 4);

    for (unsigned int i = 0; i < pEffectPlugin->uiCount; i++)
    {
        C2dEffectSAInterface& the2dEffect = pEffectPlugin->arrEffects[i];

        //C2dEffectSAInterface* p2dEffect = (C2dEffectSAInterface*)( ((unsigned char*)pEffectPlugin) + 4  + (sizeof(C2dEffectSAInterface)*i));
        //std::printf("2DEffectWrite: type: %d | type in hex: %#.8x | type addr: %p\n",
        //    p2dEffect->m_nType, p2dEffect->m_nType, (void*)&p2dEffect->m_nType);

        // Write the position
        RwStreamWrite(stream, &the2dEffect, 12);

        unsigned char arrTypeAndPadding [4] = {the2dEffect.m_nType, 0, 0, 0};
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

            unsigned char arrLast4Bytes[4] = {the2dEffect.pedAttractor.field_36, 0, the2dEffect.pedAttractor.m_nFlags , 0};   
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
            case(DWORD)0x253F2FD:
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
            case(DWORD)0x253F2F9:
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

void __cdecl On_rwPluginRegistryReadDataChunks(void* pObject, RwPluginRegEntry *pPluginRegistryEntry)
{
    // collision plugin
    if (pPluginRegistryEntry->pluginID == (DWORD)0x0253F2FA)
    {
        std::printf("colPlugin: %s\n", g_CurrentDFFBeingGeneratedFileName.c_str());
    }
}

DWORD RETURN__rwPluginRegistryReadDataChunks = 0x808A6F;
void _declspec(naked) HOOK__rwPluginRegistryReadDataChunks()
{
    _asm
    {
        pushad

        push    eax // pPluginRegistryEntry
        push    edi // pObject
        call    On_rwPluginRegistryReadDataChunks
        add     esp, 8

        popad

        mov     edx, [eax + 4]
        mov     eax, [eax]
        jmp     RETURN__rwPluginRegistryReadDataChunks
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

    HookInstall(0x808A6A, (DWORD)HOOK__rwPluginRegistryReadDataChunks, 5);

    HookInstall(0x808B00, (DWORD)_rwPluginRegistryGetSize, 5);
    HookInstall(0x808B40, (DWORD)_rwPluginRegistryWriteDataChunks, 5);

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

// Reads and parses a TXD file specified by a path (szTXD)
RwTexDictionary* CRenderWareSA::ReadTXD(const SString& strFilename, const CBuffer& fileData, bool bScriptAddTexture)
{
    // open the stream
    RwStream* streamTexture;
    RwBuffer  buffer;
    if (!fileData.IsEmpty())
    {
        buffer.ptr = (void*)fileData.GetData();
        buffer.size = fileData.GetSize();
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

// Reads and parses a DFF file specified by a path (szDFF) into a CModelInfo identified by the object id (usModelID)
// bLoadEmbeddedCollisions should be true for vehicles
// Any custom TXD should be imported before this call
RpClump* CRenderWareSA::ReadDFF(const SString& strFilename, const CBuffer& fileData, unsigned short usModelID, bool bLoadEmbeddedCollisions, RwTexDictionary* pTexDict)
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
    RwBuffer  buffer;
    if (!fileData.IsEmpty())
    {
        buffer.ptr = (void*)fileData.GetData();
        buffer.size = fileData.GetSize();
        streamModel = RwStreamOpen(STREAM_TYPE_BUFFER, STREAM_MODE_READ, &buffer);
    }
    else
        streamModel = RwStreamOpen(STREAM_TYPE_FILENAME, STREAM_MODE_READ, *strFilename);

    // get the modelinfo array
    DWORD* pPool = (DWORD*)ARRAY_ModelInfo;

    // check for errors
    if (streamModel == NULL)
    {
        throw;
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
        RpPrtStdGlobalDataSetStreamEmbedded((void*)pPool[usModelID]);

    // read the clump with all its extensions
    RpClump* pClump = RpClumpStreamRead(streamModel);

    if (!pClump)
    {
        throw;
    }
    // reset collision hack
    if (bLoadEmbeddedCollisions)
        RpPrtStdGlobalDataSetStreamEmbedded(NULL);

    // close the stream
    RwStreamClose(streamModel, NULL);
    

    return pClump;
    
}

bool CRenderWareSA::WriteTXD(const SString& strFilename, RwTexDictionary* pTxdDictionary)
{
    RwStream * pStream = RwStreamOpen(STREAM_TYPE_FILENAME, STREAM_MODE_WRITE, *strFilename);
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
    auto RpClumpStreamWrite = (void (__cdecl*)(RpClump*, RwStream *))0x74AA10;

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
    auto RpClumpStreamWrite = (void(__cdecl*)(RpClump*, RwStream *))0x74AA10;

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
