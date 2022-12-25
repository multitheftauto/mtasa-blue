/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CAccountManager.h
 *  PURPOSE:     User account manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CAccountManager;

#pragma once

#include "CAccount.h"
typedef uint SDbConnectionId;

#define GUEST_ACCOUNT_NAME          "guest"
#define HTTP_GUEST_ACCOUNT_NAME     "http_guest"
#define CONSOLE_ACCOUNT_NAME        "Console"
#define CONSOLE_NICK_NAME           "Console"

//
// CFastList with additional name->account mapping
//
class CMappedAccountList : protected CFastList<CAccount*>
{
public:
    typedef CFastList<CAccount*>                 Super;
    typedef CFastList<CAccount*>::iterator       iterator;
    typedef CFastList<CAccount*>::const_iterator const_iterator;

    // CMappedList functions
    bool     contains(CAccount* item) const { return Super::contains(item); }
    iterator begin() { return Super::begin(); }
    iterator end() { return Super::end(); }

    void push_back(CAccount* item)
    {
        assert(!MapContainsPair(m_NameAccountMap, item->GetName(), item));
        MapInsert(m_NameAccountMap, item->GetName(), item);
        Super::push_back(item);
        assert(m_NameAccountMap.size() == size());
    }

    void remove(CAccount* item)
    {
        MapRemovePair(m_NameAccountMap, item->GetName(), item);
        Super::remove(item);
        assert(m_NameAccountMap.size() == size());
    }

    void clear()
    {
        assert(m_NameAccountMap.size() == size());
        m_NameAccountMap.clear();
        Super::clear();
    }

    size_t size() const { return Super::size(); }

    // Account functions
    void FindAccountMatches(std::vector<CAccount*>* pOutResults, const SString& strName, bool bCaseSensitive)
    {
        MultiFind(m_NameAccountMap, strName, pOutResults);

        // If case sensitive, then remove non-exact matches
        if (bCaseSensitive)
        {
            for (uint i = 0; i < pOutResults->size(); ++i)
            {
                CAccount* pAccount = pOutResults->at(i);
                if (pAccount->GetName() != strName)
                {
                    pOutResults->erase(pOutResults->begin() + i);
                    i--;
                }
            }
        }
    }

    void ChangingName(CAccount* pAccount, const SString& strOldName, const SString& strNewName)
    {
        if (MapContainsPair(m_NameAccountMap, strOldName, pAccount))
        {
            MapRemovePair(m_NameAccountMap, strOldName, pAccount);
            assert(!MapContainsPair(m_NameAccountMap, strNewName, pAccount));
            MapInsert(m_NameAccountMap, strNewName, pAccount);
        }
    }

protected:
    struct CaseInsensitiveCompare
    {
        bool operator()(const SString& strLhs, const SString& strRhs) const { return stricmp(strLhs, strRhs) < 0; }
    };

    std::multimap<SString, CAccount*, CaseInsensitiveCompare> m_NameAccountMap;
};

// Returns true if the item is in the itemList
template <class T>
bool ListContains(const CMappedAccountList& itemList, const T& item)
{
    return itemList.contains(item);
}

//
// CAccountManager
//
class CAccountManager
{
    friend class CAccount;

public:
    static inline constexpr size_t MIN_USERNAME_LENGTH = 1;
    static inline constexpr size_t MAX_USERNAME_LENGTH = 64;

    CAccountManager(const SString& strDbPathFilename);
    ~CAccountManager();

    void DoPulse();

    bool Load();
    void Save(bool bForce = false);
    void Save(CAccount* pParent, bool bCheckForErrors = true);

    bool SaveSettings();
    bool IntegrityCheck();

    CAccount* Get(const char* szName, const char* szPassword = nullptr, bool caseSensitive = true);
    CAccount* GetAccountFromScriptID(uint uiScriptID);
    SString   GetActiveCaseVariation(const SString& strName);

    bool LogIn(CClient* pClient, CClient* pEchoClient, const std::string& strAccountName, const char* szPassword);
    bool LogOut(CClient* pClient, CClient* pEchoClient);

    std::shared_ptr<CLuaArgument> GetAccountData(CAccount* pAccount, const char* szKey);
    bool                          SetAccountData(CAccount* pAccount, const char* szKey, const SString& strValue, int iType);
    bool                          CopyAccountData(CAccount* pFromAccount, CAccount* pToAccount);
    bool                          GetAllAccountData(CAccount* pAccount, lua_State* pLua);

    void      GetAccountsBySerial(const SString& strSerial, std::vector<CAccount*>& outAccounts);
    void      GetAccountsByIP(const SString& strIP, std::vector<CAccount*>& outAccounts);
    CAccount* GetAccountByID(int ID);
    void      GetAccountsByData(const SString& dataName, const SString& value, std::vector<CAccount*>& outAccounts);

    CAccount* AddGuestAccount(const SString& strName);
    CAccount* AddConsoleAccount(const SString& strName);
    CAccount* AddPlayerAccount(const SString& strName, const SString& strPassword, int iUserID, const SString& strIP, const SString& strSerial,
                               const SString& strHttpPassAppend);
    CAccount* AddNewPlayerAccount(const SString& strName, const SString& strPassword);
    bool      RemoveAccount(CAccount* pAccount);
    bool      IsAuthorizedSerialRequired(CAccount* pAccount);
    bool      IsHttpLoginAllowed(CAccount* pAccount, const SString& strIp);

protected:
    void AddToList(CAccount* pAccount) { m_List.push_back(pAccount); }
    void RemoveFromList(CAccount* pAccount);

    void MarkAsChanged(CAccount* pAccount);
    void ChangingName(CAccount* pAccount, const SString& strOldName, const SString& strNewName);
    void LoadAccountSerialUsage(CAccount* pAccount);
    void SaveAccountSerialUsage(CAccount* pAccount);
    void ReconnectToDatabase();

public:
    void        RemoveAll();
    static void StaticDbCallback(CDbJobData* pJobData, void* pContext);
    void        DbCallback(CDbJobData* pJobData);
    static bool IsValidAccountName(const SString& strName);
    static bool IsValidPassword(const SString& strPassword);
    static bool IsValidNewAccountName(const SString& strName);
    static bool IsValidNewPassword(const SString& strPassword);

    CMappedAccountList::const_iterator IterBegin() { return m_List.begin(); };
    CMappedAccountList::const_iterator IterEnd() { return m_List.end(); };

protected:
    CMappedAccountList m_List;
    bool               m_bChangedSinceSaved;
    long long          m_llLastTimeSaved;
    CConnectHistory    m_AccountProtect;
    SDbConnectionId    m_hDbConnection;
    CDatabaseManager*  m_pDatabaseManager;
    int                m_iAccounts;
    SString            m_strDbPathFilename;
};
