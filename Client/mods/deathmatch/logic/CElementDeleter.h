/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CElementDeleter.h
 *  PURPOSE:     Header for element deleter class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#ifndef __CELEMENTDELETER_H
#define __CELEMENTDELETER_H

#include <memory>
#include <list>

class CElementDeleter
{
public:
    CElementDeleter(void);
    ~CElementDeleter(void) { DoDeleteAll(); };

    void Delete(class CClientEntity* pElement);
    void DeleteRecursive(class CClientEntity* pElement);
    bool DeleteElementSpecial(CClientEntity* pElement);
    void DeleteIFP(CClientEntity* pElement);

    void DoDeleteAll(void);
    bool OnClientSpecialElementDestroy(CClientEntity* pElement);
    void OnClientIFPElementDestroy(CClientEntity* pElement);
    bool IsBeingDeleted(class CClientEntity* pElement);

    void Unreference(class CClientEntity* pElement);

    void CleanUpForVM(CLuaMain* pLuaMain);

private:
    CMappedList<class CClientEntity*> m_List;
    // We are using shared_ptr for CClientIFP, and we must keep a reference
    // somewhere, so this is it.
    std::vector<std::shared_ptr<class CClientIFP> > m_vecIFPElements;
    bool                                            m_bAllowUnreference;
};

#endif
