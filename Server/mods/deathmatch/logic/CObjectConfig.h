
#pragma once

#include "StdInc.h"

class CObjectConfig
{
public:
    CObjectConfig(std::string_view strPath) : m_strPacth(strPath){};

    bool Load();

private:
    std::string_view m_strPacth;
};
