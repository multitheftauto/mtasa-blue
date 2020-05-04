/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CClientColModel.h
 *  PURPOSE:     Model collision (.col file) entity class
 *
 *****************************************************************************/

#pragma once

#include <list>
#include "CClientEntity.h"

#define MAX_COLLISION_SIZE 128             // collision bounding box in each axis
#define MAX_COLLISION_SIZE2 256            // twice bigger than MAX_COLLISION_SIZE

class CClientColModel : public CClientEntity
{
    DECLARE_CLASS(CClientColModel, CClientEntity)
public:
    CClientColModel(class CClientManager* pManager, ElementID ID);
    ~CClientColModel();

    eClientEntityType GetType() const { return CCLIENTCOL; }

    bool           LoadCol(const SString& strFile, bool bIsRawData);
    bool           Load(bool isRaw, SString input);
    unsigned short GetVerticesCount() const { return m_usVerticesCount; };
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
            return (MAX_COLLISION_SIZE >= vec.fX + fRadius && vec.fX + fRadius >= -MAX_COLLISION_SIZE && MAX_COLLISION_SIZE >= vec.fY + fRadius &&
                    vec.fY + fRadius >= -MAX_COLLISION_SIZE && MAX_COLLISION_SIZE >= vec.fZ + fRadius && vec.fZ + fRadius >= -MAX_COLLISION_SIZE) &&
                   (MAX_COLLISION_SIZE >= vec.fX - fRadius && vec.fX - fRadius >= -MAX_COLLISION_SIZE && MAX_COLLISION_SIZE >= vec.fY - fRadius &&
                    vec.fY - fRadius >= -MAX_COLLISION_SIZE && MAX_COLLISION_SIZE >= vec.fZ - fRadius && vec.fZ - fRadius >= -MAX_COLLISION_SIZE);
        }
        else
        {
            return (MAX_COLLISION_SIZE >= vec.fX && vec.fX >= -MAX_COLLISION_SIZE && MAX_COLLISION_SIZE >= vec.fY && vec.fY >= -MAX_COLLISION_SIZE &&
                    MAX_COLLISION_SIZE >= vec.fZ && vec.fZ >= -MAX_COLLISION_SIZE);
        }
    }
    static bool CheckMoveVector(CVector& vec, float fRadius = 0)
    {
        if (fRadius > 0)
        {
            return ((MAX_COLLISION_SIZE2 >= vec.fX + fRadius >= -MAX_COLLISION_SIZE2 && MAX_COLLISION_SIZE2 >= vec.fY + fRadius >= -MAX_COLLISION_SIZE2 &&
                     MAX_COLLISION_SIZE2 >= vec.fZ + fRadius >= -MAX_COLLISION_SIZE2) &&
                    (MAX_COLLISION_SIZE2 >= vec.fX - fRadius >= -MAX_COLLISION_SIZE2 && MAX_COLLISION_SIZE2 >= vec.fY - fRadius >= -MAX_COLLISION_SIZE2 &&
                     MAX_COLLISION_SIZE2 >= vec.fZ - fRadius >= -MAX_COLLISION_SIZE2));
        }
        return (MAX_COLLISION_SIZE2 >= vec.fX && vec.fX >= -MAX_COLLISION_SIZE2 && MAX_COLLISION_SIZE2 >= vec.fY && vec.fY >= -MAX_COLLISION_SIZE2 &&
                    MAX_COLLISION_SIZE2 >= vec.fZ && vec.fZ >= -MAX_COLLISION_SIZE2);
    }
    static bool CompareVector(CVector& vecMin, CVector& vecMax) { return vecMax.fX >= vecMin.fX && vecMax.fY >= vecMin.fY && vecMax.fZ >= vecMin.fZ; }
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
    unsigned short                m_usVerticesCount;
    CColModel*                    m_pColModel;
    std::list<unsigned short>     m_Replaced;

    // m_bHasChanged should be set to true when the bounding box has changed.
    // This ensures that the bounding box is only recalculated when necessary.
    // This is true by default as an object loaded collision could have the wrong bounding box.
    bool b_hasChanged = true;
};
