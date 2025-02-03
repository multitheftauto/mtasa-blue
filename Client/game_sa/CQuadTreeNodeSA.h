/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CQuadTreeNodeSA.h
 *  PURPOSE:     Quad tree node class interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPtrNodeSingleListSA.h"
#include "CRect.h"

template<class T>
class CQuadTreeNodesSAInterface
{
public:
    void RemoveAllItems();
    char AddItem(T* item, CRect* boudingBox);

private:
    float              m_fX;
    float              m_fY;
    float              m_fW;
    float              m_fH;
    CPtrNodeSingleListSAInterface<T> m_pItemList;
    CQuadTreeNodesSAInterface<T>* m_childrens[4];
    uint32_t           m_level;
};
static_assert(sizeof(CQuadTreeNodesSAInterface<void>) == 0x28, "Wrong CQuadTreeNodesSAInterface size");

template <class T>
void CQuadTreeNodesSAInterface<T>::RemoveAllItems()
{
    if (m_level)
    {
        for (size_t i = 0; i < 4; i++)
        {
            m_childrens[i]->RemoveAllItems();
        }
    }
    else
    {
        m_pItemList.RemoveAllItems();
    }
};

template <class T>
char CQuadTreeNodesSAInterface<T>::AddItem(T* item, CRect* boudingBox)
{
    typedef char(__thiscall * CQuadTreeNode_AddItem_t)(CQuadTreeNodesSAInterface*, void*, CRect*);
    return ((CQuadTreeNode_AddItem_t)(0x552CD0))(this, item, boudingBox);
};
