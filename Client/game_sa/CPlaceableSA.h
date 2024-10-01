/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <CMatrix_Pad.h>
#include <CVector.h>

#define VTBL_CPlaceable 0x863C40

class CSimpleTransformSAInterface            // 16 bytes
{
public:
    CVector m_translate;
    float   m_heading;
};

class CPlaceableSAInterface
{
public:
    virtual void* Destructor(bool free) = 0;

    bool IsPlaceableVTBL() const { return *reinterpret_cast<const std::uint32_t*>(this) == VTBL_CPlaceable; };
    bool HasVTBL() const { return *reinterpret_cast<const std::uint32_t*>(this) != 0; };

public:
    CSimpleTransformSAInterface m_transform;
    CMatrix_Padded*             matrix;            // This is actually XYZ*, change later
};
static_assert(sizeof(CPlaceableSAInterface) == 0x18, "Invalid size for CPlaceableSAInterface");
