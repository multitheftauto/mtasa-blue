/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/lua/CLuaShared.cpp
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaShared.h"

#ifndef MTA_CLIENT
    #include "CGame.h"
#endif

// If compiled script, make sure correct chunkname is embedded
void CLuaShared::EmbedChunkName(SString strChunkName, const char** pcpOutBuffer, uint* puiOutSize)
{
    const char*& cpBuffer = *pcpOutBuffer;
    uint&        uiSize = *puiOutSize;

    if (!IsLuaCompiledScript(cpBuffer, uiSize))
        return;

    if (uiSize < 12)
        return;

    // Get size of name in original
    uint uiStringSizeOrig = *(uint*)(cpBuffer + 12);
    if (uiSize < 12 + 4 + uiStringSizeOrig)
        return;

    static CBuffer store;
    store.Clear();
    CBufferWriteStream stream(store);

    // Ensure name starts with @ and ends with NULL
    if (!strChunkName.BeginsWith("@"))
        strChunkName = "@" + strChunkName;
    if (strChunkName[strChunkName.length() - 1] != '\0')
        strChunkName.push_back('\0');

    // Header
    stream.WriteBytes(cpBuffer, 12);
    // New name size
    stream.Write(strChunkName.length());
    // New name bytes incl zero termination
    stream.WriteBytes(strChunkName.c_str(), strChunkName.length());
    // And the rest
    stream.WriteBytes(cpBuffer + 12 + 4 + uiStringSizeOrig, uiSize - 12 - 4 - uiStringSizeOrig);

    cpBuffer = store.GetData();
    uiSize = store.GetSize();
}

bool CLuaShared::CheckUTF8BOMAndUpdate(const char** pcpOutBuffer, uint* puiOutSize)
{
    const char*& cpBuffer = *pcpOutBuffer;
    uint&        uiSize = *puiOutSize;

    // UTF-8 BOM?  Compare by checking the standard UTF-8 BOM
    if (IsUTF8BOM(cpBuffer, uiSize))
    {
        // If there's a BOM, load ignoring the first 3 bytes
        cpBuffer += 3;
        uiSize -= 3;
        return true;
    }

    // Maybe not UTF-8, if we have a >80% heuristic detection confidence, assume it is
    return GetUTF8Confidence((const unsigned char*)cpBuffer, uiSize) >= 80;
}

void CLuaShared::LoadFunctions()
{
    CLuaBitDefs::LoadFunctions();
    CLuaCryptDefs::LoadFunctions();
    CLuaFileDefs::LoadFunctions();
    CLuaXMLDefs::LoadFunctions();
    CLuaPathDefs::LoadFunctions();
    CLuaTrainTrackDefs::LoadFunctions();
    CLuaUTFDefs::LoadFunctions();
    CLuaUtilDefs::LoadFunctions();
}

void CLuaShared::AddClasses(lua_State* luaVM)
{
    CLuaFileDefs::AddClass(luaVM);
    CLuaPathDefs::AddClass(luaVM);
    CLuaXMLDefs::AddClass(luaVM);
}

SharedUtil::CAsyncTaskScheduler* CLuaShared::GetAsyncTaskScheduler()
{
#ifdef MTA_CLIENT
    return g_pClientGame->GetAsyncTaskScheduler();
#else
    return g_pGame->GetAsyncTaskScheduler();
#endif
}
