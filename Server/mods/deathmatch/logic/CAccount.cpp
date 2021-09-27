/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CAccount.cpp
 *  PURPOSE:     User account class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CAccount::CAccount(CAccountManager* pManager, EAccountType accountType, const std::string& strName, const std::string& strPassword, int iUserID,
                   const std::string& strIP, const std::string& strSerial, const SString& strHttpPassAppend)
{
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::ACCOUNT);
    m_pClient = NULL;

    m_bChanged = false;
    m_pManager = pManager;
    m_AccountType = accountType;
    m_strName = strName.substr(0, CAccountManager::MAX_USERNAME_LENGTH);
    m_iUserID = iUserID;
    m_strIP = strIP;
    m_strSerial = strSerial;
    m_strHttpPassAppend = strHttpPassAppend;

    m_pManager->AddToList(this);

    if (m_Password.SetPassword(strPassword))
        m_pManager->MarkAsChanged(this);            // Save if password upgraded
}

CAccount::~CAccount()
{
    CIdArray::PushUniqueId(this, EIdClass::ACCOUNT, m_uiScriptID);
    if (m_pClient)
        m_pClient->SetAccount(NULL);

    m_pManager->RemoveFromList(this);
    m_pManager->MarkAsChanged(this);
}

void CAccount::SetName(const std::string& strName)
{
    std::string strNewName = strName.substr(0, CAccountManager::MAX_USERNAME_LENGTH);
    if (m_strName != strNewName)
    {
        m_pManager->ChangingName(this, m_strName, strNewName);
        m_strName = std::move(strNewName);
        m_pManager->MarkAsChanged(this);
    }
}

void CAccount::SetClient(CClient* pClient)
{
    m_pClient = pClient;
    // Clear data cache if not linked to a client
    if (!m_pClient)
        m_Data.clear();
}

bool CAccount::IsPassword(const SString& strPassword, bool* pbUsedHttpPassAppend)
{
    if (pbUsedHttpPassAppend == nullptr)
    {
        return m_Password.IsPassword(strPassword);
    }
    else
    {
        if (m_Password.IsPassword(strPassword))
        {
            *pbUsedHttpPassAppend = false;
            return true;
        }
        else
        {
            SString strPasswordHead = strPassword.Left(strPassword.length() - m_strHttpPassAppend.length());
            SString strPasswordTail = strPassword.Right(m_strHttpPassAppend.length());
            if (m_Password.IsPassword(strPasswordHead) && strPasswordTail == m_strHttpPassAppend)
            {
                *pbUsedHttpPassAppend = true;
                return true;
            }
            return false;
        }
    }
}

void CAccount::SetPassword(const SString& strPassword)
{
    if (m_Password.CanChangePasswordTo(strPassword))
    {
        m_Password.SetPassword(strPassword);
        m_pManager->MarkAsChanged(this);
    }
}

SString CAccount::GetPasswordHash()
{
    return m_Password.GetPasswordHash();
}

void CAccount::SetHttpPassAppend(const SString& strHttpPassAppend)
{
    m_strHttpPassAppend = strHttpPassAppend;
    m_pManager->MarkAsChanged(this);
}

CAccountData* CAccount::GetDataPointer(const std::string& strKey)
{
    return MapFind(m_Data, strKey);
}

std::shared_ptr<CLuaArgument> CAccount::GetData(const std::string& strKey)
{
    CAccountData* pData = GetDataPointer(strKey);
    auto          pResult = std::make_shared<CLuaArgument>();

    if (pData)
    {
        switch (pData->GetType())
        {
        case LUA_TBOOLEAN:
            pResult->ReadBool(strcmp(pData->GetStrValue().c_str(), "true") == 0);
            break;

        case LUA_TNUMBER:
            pResult->ReadNumber(strtod(pData->GetStrValue().c_str(), NULL));
            break;

        case LUA_TNIL:
            break;

        case LUA_TSTRING:
            pResult->ReadString(pData->GetStrValue());
            break;

        default:
            dassert(0); // It never should hit this, if so, something corrupted
            break;
        }
    }
    else
    {
        pResult->ReadBool(false);
    }
    return pResult;
}

// Return true if data was changed
bool CAccount::SetData(const std::string& strKey, const std::string& strValue, int iType)
{
    if (strValue == "false" && iType == LUA_TBOOLEAN)
    {
        if (HasData(strKey))
        {
            RemoveData(strKey);
            return true;
        }
    }
    else
    {
        CAccountData* pData = GetDataPointer(strKey);

        if (pData)
        {
            if (pData->GetType() != iType || pData->GetStrValue() != strValue)
            {
                pData->SetStrValue(strValue);
                pData->SetType(iType);
                return true;
            }
        }
        else
        {
            MapSet(m_Data, strKey, CAccountData(strKey, strValue, iType));
            return true;
        }
    }
    return false;
}

bool CAccount::HasData(const std::string& strKey)
{
    return MapContains(m_Data, strKey);
}

void CAccount::RemoveData(const std::string& strKey)
{
    MapRemove(m_Data, strKey);
}

//
// Authorized aerial stuff
//
// Account serial usage is only recorded for accounts that require serial authorization,
// and is only loaded when required.
void CAccount::EnsureLoadedSerialUsage()
{
    if (!m_bLoadedSerialUsage)
    {
        m_bLoadedSerialUsage = true;
        m_pManager->LoadAccountSerialUsage(this);
    }
}

bool CAccount::HasLoadedSerialUsage()
{
    return m_bLoadedSerialUsage;
}

std::vector<CAccount::SSerialUsage>& CAccount::GetSerialUsageList()
{
    EnsureLoadedSerialUsage();
    return m_SerialUsageList;
}

CAccount::SSerialUsage* CAccount::GetSerialUsage(const SString& strSerial)
{
    EnsureLoadedSerialUsage();
    for (auto& info : m_SerialUsageList)
    {
        if (info.strSerial == strSerial)
            return &info;
    }
    return nullptr;
}

//
// Check if the supplied serial had been authorized for this account
//
bool CAccount::IsSerialAuthorized(const SString& strSerial)
{
    SSerialUsage* pInfo = GetSerialUsage(strSerial);
    if (pInfo)
    {
        return pInfo->IsAuthorized();
    }
    return false;
}

//
// Check if the supplied IP was last used by an authorized serial
//
bool CAccount::IsIpAuthorized(const SString& strIp)
{
    EnsureLoadedSerialUsage();
    for (auto& info : m_SerialUsageList)
    {
        if (info.strLastLoginIp == strIp && info.IsAuthorized())
            return true;
    }
    return false;
}

//
// Mark pending serial as authorized for this account
//
bool CAccount::AuthorizeSerial(const SString& strSerial, const SString& strWho)
{
    SSerialUsage* pInfo = GetSerialUsage(strSerial);
    if (pInfo)
    {
        if (!pInfo->IsAuthorized())
        {
            pInfo->tAuthDate = time(nullptr);
            pInfo->strAuthWho = strWho;
            m_pManager->MarkAsChanged(this);
            return true;
        }
    }
    return false;
}

//
// Unconditionally remove usage info for a serial
//
bool CAccount::RemoveSerial(const SString& strSerial)
{
    EnsureLoadedSerialUsage();
    for (auto iter = m_SerialUsageList.begin(); iter != m_SerialUsageList.end(); ++iter)
    {
        SSerialUsage& info = *iter;
        if (info.strSerial == strSerial)
        {
            iter = m_SerialUsageList.erase(iter);
            m_pManager->MarkAsChanged(this);
            return true;
        }
    }
    return false;
}

//
// Cleanup unauthorized serials
//
void CAccount::RemoveUnauthorizedSerials()
{
    EnsureLoadedSerialUsage();
    for (auto iter = m_SerialUsageList.begin(); iter != m_SerialUsageList.end();)
    {
        SSerialUsage& info = *iter;
        if (!info.IsAuthorized())
            iter = m_SerialUsageList.erase(iter);
        else
            ++iter;
    }
    m_pManager->MarkAsChanged(this);
}

//
// If serial not already present, add for possible authorization
//
bool CAccount::AddSerialForAuthorization(const SString& strSerial, const SString& strIp)
{
    SSerialUsage* pInfo = GetSerialUsage(strSerial);
    if (!pInfo)
    {
        // Only one new serial at a time, so remove all other unauthorized serials for this account
        RemoveUnauthorizedSerials();

        SSerialUsage info;
        info.strSerial = strSerial;
        info.strAddedIp = strIp;
        info.tAddedDate = time(nullptr);
        info.tAuthDate = 0;
        info.tLastLoginDate = 0;
        info.tLastLoginHttpDate = 0;

        // First one doesn't require authorization
        if (m_SerialUsageList.size() == 0)
        {
            info.tAuthDate = time(nullptr);
        }
        m_SerialUsageList.push_back(info);
        m_pManager->MarkAsChanged(this);
        return true;
    }
    return false;
}

//
// Called when the player has successful logged in
//
void CAccount::OnLoginSuccess(const SString& strSerial, const SString& strIp)
{
    SSerialUsage* pInfo = GetSerialUsage(strSerial);
    if (pInfo)
    {
        pInfo->strLastLoginIp = strIp;
        pInfo->tLastLoginDate = time(nullptr);

        // On successful login, delete all other unauthorized serials for this account
        RemoveUnauthorizedSerials();
    }
    m_strIP = strIp;
    m_strSerial = strSerial;
    m_pManager->MarkAsChanged(this);
}

//
// Called when the player has successful logged in via the http interface
//
void CAccount::OnLoginHttpSuccess(const SString& strIp)
{
    EnsureLoadedSerialUsage();
    for (auto& info : m_SerialUsageList)
    {
        if (info.strLastLoginIp == strIp && info.IsAuthorized())
        {
            info.tLastLoginHttpDate = time(nullptr);
            m_pManager->MarkAsChanged(this);
        }
    }
}
