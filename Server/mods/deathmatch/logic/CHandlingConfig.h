/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "StdInc.h"

class CHandlingConfig
{
public:
    CHandlingConfig(std::string_view strPath);

    bool Load();

private:
    void LoadHandlingParams(std::vector<char*> lineParams);

    std::string_view m_strPacth;
};
