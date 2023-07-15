/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CObjectConfig.h"
#include <fstream>
#include "CHandlingEntry.h"
#include "models/CModelManager.h"
#include "CGame.h"

bool CObjectConfig::Load()
{
    std::ifstream pFile(m_strPacth.data(), std::ifstream::in);
    if (pFile.is_open())
    {
        std::string line;
        while (std::getline(pFile, line))
        {
            // It is not comment
            if (line[0] != '#')
            {
                // It is valid numberic char
                if (line[0] >= '0' && line[0] <= '9')
                {
                    CModelAtomic* pModel = new CModelAtomic(atoi(line.c_str()));
                    g_pGame->GetModelManager()->RegisterModel(pModel);
                }
            }
        }
        pFile.close();
        return true;
    }
    return false;
}
