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
