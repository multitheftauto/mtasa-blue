#include "StdInc.h"
#include "CPedConfig.h"
#include <fstream>
#include "CGame.h"
#include "models/CModelManager.h"

bool CPedConfig::Load()
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
                    CModelPed* pModel = new CModelPed(atoi(line.c_str()));
                    g_pGame->GetModelManager()->RegisterModel(pModel);
                }
            }
        }
        pFile.close();
        return true;
    }
    return false;
}
