/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/ClientDragAndDrop.cpp
 *  PURPOSE:     Client drag and drop files handler
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#include "ClientDragAndDrop.h"

void HandleDroppedFiles(std::vector<std::string>& files)
{
    auto pLocalPlayer = g_pClientGame->GetLocalPlayer();

    if (pLocalPlayer)
    {
        CLuaArguments Arguments;
        CLuaArguments LuaTable;

        for (const auto& fullPath : files)
        {
            // std::string base_filename = path.substr(path.find_last_of("/\\") + 1)

            auto binaryFile = g_pNet->AllocateBinaryFile();
            bool bOk = binaryFile->FOpen(fullPath.c_str(), "rb", true);
            if (!bOk)
            {
                SAFE_DELETE(binaryFile);
            }

            long lSize = binaryFile->FTell();

            long lCurrentPos = binaryFile->FTell();
            binaryFile->FSeek(0, SEEK_END);
            long lFileSize = binaryFile->FTell();
            binaryFile->FSeek(lCurrentPos, SEEK_SET);
            auto ulSize = lFileSize - lCurrentPos;

            std::string content;
            content.resize(ulSize);
            binaryFile->FRead(content.data(), ulSize);
            //  lua_pushlstring(luaVM, buffer.data(), lBytesRead);
            LuaTable.PushString(fullPath.substr(fullPath.find_last_of("/\\") + 1));
            LuaTable.PushString(content);

            SAFE_DELETE(binaryFile);
        }
        Arguments.PushTable(&LuaTable);

        pLocalPlayer->CallEvent("onFilesDrop", Arguments, true);
    }
}
