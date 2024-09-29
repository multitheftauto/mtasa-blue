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
    virtual void* Destructor(bool deletedFromPool) = 0;            //
    virtual void  Add_CRect() = 0;
    virtual void  Add() = 0;            //
    virtual void  Remove() = 0;
    virtual void  SetIsStatic() = 0;
    virtual void  SetModelIndex(std::uint32_t model) = 0;            //
    virtual void  SetModelIndexNoCreate(std::uint32_t model) = 0;
    virtual void  CreateRwObject() = 0;            //
    virtual void  DeleteRwObject() = 0;            //
    virtual void  GetBoundRect() = 0;
    virtual void  ProcessControl() = 0;            //
    virtual void  ProcessCollision() = 0;
    virtual void  ProcessShift() = 0;
    virtual void  TestCollision() = 0;
    virtual void  Teleport(float x, float y, float z, bool resetRotation) = 0;            //
    virtual void  SpecialEntityPreCollisionStuff() = 0;
    virtual void  SpecialEntityCalcCollisionSteps() = 0;
    virtual void  PreRender() = 0;                     //
    virtual void  Render() = 0;                        //
    virtual void  SetupLighting() = 0;                 //
    virtual void* RemoveLighting(bool) = 0;            //
    virtual void  FlagToDestroyWhenNextProcessed() = 0; //

    bool IsPlaceableVTBL() const { return *reinterpret_cast<const std::uint32_t*>(this) == VTBL_CPlaceable; };
    bool HasVTBL() const { return *reinterpret_cast<const std::uint32_t*>(this) != 0; };

public:
    CSimpleTransformSAInterface m_transform;
    CMatrix_Padded*             matrix;            // This is actually XYZ*, change later
};
static_assert(sizeof(CPlaceableSAInterface) == 0x18, "Invalid size for CPlaceableSAInterface");
