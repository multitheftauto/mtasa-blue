/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientColModel.h
 *  PURPOSE:     Model collision (.col file) entity class
 *
 *****************************************************************************/

#pragma once

#include <list>
#include "CClientEntity.h"

constexpr int MaxCollisionSize = 128;            // collision bounding box in each axis
constexpr int MaxCollisionSize2 = MaxCollisionSize * 2;

class CClientColModel : public CClientEntity
{
    DECLARE_CLASS(CClientColModel, CClientEntity)
public:
    CClientColModel(class CClientManager* pManager, ElementID ID);
    ~CClientColModel();

    eClientEntityType GetType() const { return CCLIENTCOL; }

    bool           LoadCol(const SString& strFile, bool bIsRawData);
    bool           Load(bool isRaw, SString input);
    int            GetVerticesCount() const { return m_iVerticesCount; };
    void           SetCollisionHasChanged(bool bChanged) { b_hasChanged = bChanged; };
    bool           HasChanged() const { return b_hasChanged; };
    bool           IsLoaded() { return m_pColModel != NULL; };

    bool Replace(unsigned short usModel);
    void Restore(unsigned short usModel);
    void RestoreAll();

    bool        HasReplaced(unsigned short usModel);
    static bool IsCOLData(const SString& strData);
    static bool CheckVector(CVector& vec, float fRadius = 0)
    {
        if (fRadius > 0)
        {
            return (MaxCollisionSize >= vec.fX + fRadius && vec.fX + fRadius >= -MaxCollisionSize && MaxCollisionSize >= vec.fY + fRadius &&
                    vec.fY + fRadius >= -MaxCollisionSize && MaxCollisionSize >= vec.fZ + fRadius && vec.fZ + fRadius >= -MaxCollisionSize) &&
                   (MaxCollisionSize >= vec.fX - fRadius && vec.fX - fRadius >= -MaxCollisionSize && MaxCollisionSize >= vec.fY - fRadius &&
                    vec.fY - fRadius >= -MaxCollisionSize && MaxCollisionSize >= vec.fZ - fRadius && vec.fZ - fRadius >= -MaxCollisionSize);
        }
        return (MaxCollisionSize >= vec.fX && vec.fX >= -MaxCollisionSize && MaxCollisionSize >= vec.fY && vec.fY >= -MaxCollisionSize &&
                    MaxCollisionSize >= vec.fZ && vec.fZ >= -MaxCollisionSize);
    }
    static bool CheckMoveVector(CVector& vec, float fRadius = 0)
    {
        if (fRadius > 0)
        {
            return ((MaxCollisionSize2 >= vec.fX + fRadius >= -MaxCollisionSize2 && MaxCollisionSize2 >= vec.fY + fRadius >= -MaxCollisionSize2 &&
                     MaxCollisionSize2 >= vec.fZ + fRadius >= -MaxCollisionSize2) &&
                    (MaxCollisionSize2 >= vec.fX - fRadius >= -MaxCollisionSize2 && MaxCollisionSize2 >= vec.fY - fRadius >= -MaxCollisionSize2 &&
                     MaxCollisionSize2 >= vec.fZ - fRadius >= -MaxCollisionSize2));
        }
        return (MaxCollisionSize2 >= vec.fX && vec.fX >= -MaxCollisionSize2 && MaxCollisionSize2 >= vec.fY && vec.fY >= -MaxCollisionSize2 &&
                    MaxCollisionSize2 >= vec.fZ && vec.fZ >= -MaxCollisionSize2);
    }

    // CompareVector returns true if all points in the second vector are greater than the corresponding points in the first vector
    static bool CompareVector(CVector& vecMin, CVector& vecMax) { return vecMax.fX >= vecMin.fX && vecMax.fY >= vecMin.fY && vecMax.fZ >= vecMin.fZ; }

    // AlignVector clamps a "source" vector between a min and max vector. Or something like that. I don't know.
    static void AlignVector(CVector& destMin, CVector& destMax, const CVector& src)
    {
        if (src.fX < destMax.fX)
            destMax.fX = src.fX;
        if (src.fY < destMax.fY)
            destMax.fY = src.fY;
        if (src.fZ < destMax.fZ)
            destMax.fZ = src.fZ;
        if (src.fX > destMin.fX)
            destMin.fX = src.fX;
        if (src.fY > destMin.fY)
            destMin.fY = src.fY;
        if (src.fZ > destMin.fZ)
            destMin.fZ = src.fZ;
    }
    void UpdateVerticesCount();

    CColModelSAInterface* GetColModelInterface();

    // Sorta a hack that these are required by CClientEntity...
    void Unlink(){};
    void GetPosition(CVector& vecPosition) const {};
    void SetPosition(const CVector& vecPosition){};

private:
    bool LoadFromFile(SString filePath);
    bool LoadFromBuffer(SString buffer);

    void InternalRestore(unsigned short usModel);

    class CClientColModelManager* m_pColModelManager;
    int                           m_iVerticesCount;
    CColModel*                    m_pColModel;
    std::list<unsigned short>     m_Replaced;

    // m_bHasChanged should be set to true when the bounding box has changed.
    // This ensures that the bounding box is only recalculated when necessary.
    // This is true by default as an object loaded collision could have the wrong bounding box.
    bool b_hasChanged = true;
};
