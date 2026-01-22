/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CAccount.h
 *  PURPOSE:     User account class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <list>
#include <string>
#include "lua/CLuaArgument.h"

class CClient;

class CAccountPassword
{
public:
    bool    SetPassword(const SString& strPassword);
    bool    IsPassword(const SString& strPassword);
    bool    CanChangePasswordTo(const SString& strPassword);
    SString GetPasswordHash();

    enum EAccountPasswordType
    {
        NONE,            // "" (empty string) possibly means 'not a password'
        PLAINTEXT,
        MD5,              // pass stored as sha256(md5)+salt+type, stored as "1"
        SHA256            // pass stored as sha256(plaintext)+salt+type, stored as "0"
    };

protected:
    SString GenerateSalt();

    SString m_strSha256;
    SString m_strSalt;

    SString m_strType;            // can be "1", "0" or "" as shown in EAccountPasswordType
};

enum class EAccountType
{
    Guest,
    Console,
    Player,
};

class CAccountData;
class CAccount
{
public:
    struct SSerialUsage
    {
        SString strSerial;
        SString strAddedIp;
        time_t  tAddedDate;
        SString strAuthWho;
        time_t  tAuthDate;
        SString strLastLoginIp;
        time_t  tLastLoginDate;
        time_t  tLastLoginHttpDate;
        bool    IsAuthorized() const { return tAuthDate != 0; }
    };

    ZERO_ON_NEW
    CAccount(class CAccountManager* pManager, EAccountType accountType, const std::string& strName, const std::string& strPassword = "", int iUserID = 0,
             const std::string& strIP = "", const std::string& strSerial = "", const SString& strHttpPassAppend = "");
    ~CAccount();

    bool IsRegistered() const { return m_AccountType != EAccountType::Guest; }
    bool IsConsoleAccount() const { return m_AccountType == EAccountType::Console; }
    void OnLoginSuccess(const SString& strSerial, const SString& strIp);
    void OnLoginHttpSuccess(const SString& strIp);

    const SString& GetName() const { return m_strName; }
    void           SetName(const std::string& strName);

    const EAccountType GetType() const { return m_AccountType; }

    void           SetPassword(const SString& strPassword);
    bool           IsPassword(const SString& strPassword, bool* pbUsedHttpPassAppend = nullptr);
    SString        GetPasswordHash();
    const SString& GetHttpPassAppend() const { return m_strHttpPassAppend; }
    void           SetHttpPassAppend(const SString& strHttpPassAppend);

    const std::string& GetIP() const { return m_strIP; }
    const std::string& GetSerial() const { return m_strSerial; }
    const int          GetID() const { return m_iUserID; }

    bool                       HasLoadedSerialUsage() const;
    void                       EnsureLoadedSerialUsage();
    std::vector<SSerialUsage>& GetSerialUsageList();
    SSerialUsage*              GetSerialUsage(const SString& strSerial);
    bool                       IsIpAuthorized(const SString& strIp);
    bool                       IsSerialAuthorized(const SString& strSerial);
    bool                       IsValidSerial(const std::string& serial) const noexcept;
    bool                       AddSerialForAuthorization(const SString& strSerial, const SString& strIp);
    bool                       AuthorizeSerial(const SString& strSerial, const SString& strWho);
    bool                       RemoveSerial(const SString& strSerial);
    void                       RemoveUnauthorizedSerials();
    bool                       SetAccountSerial(const std::string& serial) noexcept;

    CClient* GetClient() const { return m_pClient; }
    void     SetClient(CClient* pClient);

    void SetChanged(bool bChanged) { m_bChanged = bChanged; }
    bool HasChanged() const { return m_bChanged; }
    uint GetScriptID() const { return m_uiScriptID; }

    std::shared_ptr<CLuaArgument>             GetData(const std::string& strKey);
    bool                                      SetData(const std::string& strKey, const std::string& strValue, int iType);
    bool                                      HasData(const std::string& strKey);
    void                                      RemoveData(const std::string& strKey);
    std::map<SString, CAccountData>::iterator begin() { return m_Data.begin(); }
    std::map<SString, CAccountData>::iterator end() { return m_Data.end(); }

protected:
    CAccountData* GetDataPointer(const std::string& strKey);

    CAccountManager* m_pManager;

    EAccountType              m_AccountType;
    SString                   m_strName;
    CAccountPassword          m_Password;
    SString                   m_strHttpPassAppend;
    std::string               m_strIP;
    std::string               m_strSerial;
    int                       m_iUserID;
    bool                      m_bLoadedSerialUsage;
    std::vector<SSerialUsage> m_SerialUsageList;

    bool m_bChanged;

    class CClient* m_pClient;
    uint           m_uiScriptID;

    std::map<SString, CAccountData> m_Data;
};

class CAccountData
{
public:
    CAccountData(const std::string& strKey = "", const std::string& strValue = "", int iType = 0)
    {
        m_strKey = strKey;
        m_strValue = strValue;
        m_iType = iType;
    }

    const std::string& GetKey() const { return m_strKey; }
    const std::string& GetStrValue() const { return m_strValue; }
    int                GetType() const { return m_iType; }
    void               SetStrValue(const std::string& strValue) { m_strValue = strValue; }
    void               SetType(int iType) { m_iType = iType; }

private:
    std::string m_strKey;
    std::string m_strValue;
    int         m_iType;
};
