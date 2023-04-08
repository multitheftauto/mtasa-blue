#include "StdInc.h"
#include "CVehicleColors.h"
#include "Utils.h"

CVehicleColor CVehicleColors::GetRandomColor()
{
    // Grab a random index
    unsigned int uiSize = CountColors();
    if (uiSize > 0)
    {
        // Create a random index
        unsigned int uiRandomIndex = GetRandom(0, uiSize - 1);

        // Grab the random color we got off the list
        unsigned int                       uiIndex = 0;
        std::list<CVehicleColor>::iterator iter = m_Colors.begin();
        for (; iter != m_Colors.end(); ++iter)
        {
            if (uiIndex == uiRandomIndex)
            {
                return *iter;
            }

            ++uiIndex;
        }
    }

    // No items, return default color (black)
    return CVehicleColor();
}
