#pragma once

#include "StdInc.h"
#include "CXMLConfig.h"

class CVehiclesConfig : public CXMLConfig
{
public:
    CVehiclesConfig();

    // Inherited via CXMLConfig
    virtual bool Load() override;
    virtual bool Save() override;

private:
    void RegisterVehicles(CXMLNode* pNode);
};
