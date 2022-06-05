#pragma once
#include "StdInc.h"

class CVehicleColors
{
public:
    void         AddColor(const CVehicleColor& colVehicle) { m_Colors.push_back(colVehicle); };
    unsigned int CountColors() { return static_cast<unsigned int>(m_Colors.size()); };
    void         RemoveAllColors() { m_Colors.clear(); };

    CVehicleColor GetRandomColor();

private:
    std::list<CVehicleColor> m_Colors;
};
