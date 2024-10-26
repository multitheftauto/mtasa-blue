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

    void* GetVTBL() const { return *reinterpret_cast<void* const*>(this); }
    bool  HasVTBL() const { return GetVTBL() != nullptr; }
    bool  IsPlaceableVTBL() const { return GetVTBL() == (void*)0x863C40; }

    bool HasMatrix() const noexcept { return matrix != nullptr; }
    void RemoveMatrix() { ((void(__thiscall*)(void*))0x54F3B0)(this); }

public:
    CSimpleTransformSAInterface m_transform;
    CMatrix_Padded*             matrix;            // This is actually XYZ*, change later
};
static_assert(sizeof(CPlaceableSAInterface) == 0x18, "Invalid size for CPlaceableSAInterface");
