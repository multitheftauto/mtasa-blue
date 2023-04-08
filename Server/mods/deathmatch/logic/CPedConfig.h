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
