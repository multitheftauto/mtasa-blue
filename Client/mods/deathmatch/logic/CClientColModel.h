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

class CClientColModel : public CClientEntity
{
    DECLARE_CLASS(CClientColModel, CClientEntity)
public:
    CClientColModel(class CClientManager* pManager, ElementID ID);
    ~CClientColModel(void);

    eClientEntityType GetType(void) const { return CCLIENTCOL; }

    bool           LoadCol(const SString& strFile, bool bIsRawData);
    bool           IsLoaded(void) { return m_pColModel != NULL; };
    unsigned short GetVerticesCount() { return m_usVerticesCount; };
    void           SetCollisionHasChanged(bool bChanged) { b_hasChanged = bChanged; };
    bool           HasChanged() { return b_hasChanged; };

    bool Replace(unsigned short usModel);
    void Restore(unsigned short usModel);
    void RestoreAll(void);

    bool        HasReplaced(unsigned short usModel);
    static bool IsCOLData(const SString& strData);
    static bool CheckVector(CVector& vec, float fRadius = 0);
    static bool CheckMoveVector(CVector& vec, float fRadius = 0);

    static bool CompareVector(CVector& vecMin, CVector& vecMax);
    static void AlignVector(CVector& destMin, CVector& destMax, CVector& src);
    void        UpdateVerticesCount();

    CColModelSAInterface* GetColModelInterface();

    // Sorta a hack that these are required by CClientEntity...
    void Unlink(void){};
    void GetPosition(CVector& vecPosition) const {};
    void SetPosition(const CVector& vecPosition){};

private:
    void InternalRestore(unsigned short usModel);

    class CClientColModelManager* m_pColModelManager;
    unsigned short                m_usVerticesCount;
    CColModel*                    m_pColModel;
    std::list<unsigned short>     m_Replaced;
    bool                          b_hasChanged; // For updating bounding box, update only if necessary. True default due object loaded collision could have wrong bounding box.
};
