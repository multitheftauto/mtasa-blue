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

class CPedConfig
{
public:
    CPedConfig(std::string_view strPath) : m_strPacth(strPath){};

    bool Load();

private:
    std::string_view m_strPacth;
};
