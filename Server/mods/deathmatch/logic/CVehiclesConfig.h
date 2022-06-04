
class CVehiclesConfig;

#pragma once

#include "StdInc.h"

class CVehiclesConfig : public CXMLConfig
{
public:
    CVehiclesConfig();

    // Inherited via CXMLConfig
    virtual bool Load() override;
    virtual bool Save() override;

private:
    void RegisterVehicles();

    CXMLNode* m_pRootNode;
};
