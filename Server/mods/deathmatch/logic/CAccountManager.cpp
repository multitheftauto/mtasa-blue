/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CAccountManager.cpp
 *  PURPOSE:     User account manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CAccountManager.h"
#include "CGame.h"
#include "CDatabaseManager.h"
#include "CRegistry.h"
#include "CMainConfig.h"
#include "CIdArray.h"
#include "CAccessControlListManager.h"
#include "Utils.h"
#include "CMapManager.h"

CAccountManager::CAccountManager(const SString& strDbPathFilename)
    : m_AccountProtect(6, 30000, 60000 * 1)            // Max of 6 attempts per 30 seconds, then 1 minute ignore
{
    m_llLastTimeSaved = GetTickCount64_();
    m_bChangedSinceSaved = false;
    m_iAccounts = 1;
    m_pDatabaseManager = g_pGame->GetDatabaseManager();
    m_strDbPathFilename = strDbPathFilename;
    m_hDbConnection = INVALID_DB_HANDLE;

    // Load internal.db
    ReconnectToDatabase();

    // Check if new installation
    CRegistryResult result;
    m_pDatabaseManager->QueryWithResultf(m_hDbConnection, &result, "SELECT name FROM sqlite_master WHERE type='table' AND name='accounts'");
    bool bNewInstallation = (result->nRows == 0);

    // Create all our tables (Don't echo the results)
    m_pDatabaseManager->Execf(m_hDbConnection, "CREATE TABLE IF NOT EXISTS accounts (id INTEGER PRIMARY KEY, name TEXT, password TEXT, ip TEXT, serial TEXT)");
    m_pDatabaseManager->Execf(m_hDbConnection,
                              "CREATE TABLE IF NOT EXISTS userdata (id INTEGER PRIMARY KEY, userid INTEGER, key TEXT, value TEXT, type INTEGER)");
    m_pDatabaseManager->Execf(m_hDbConnection,
                              "CREATE TABLE IF NOT EXISTS serialusage (id INTEGER PRIMARY KEY, userid INTEGER, "
                              "serial TEXT, "
                              "added_ip TEXT, "
                              "added_date INTEGER, "
                              "auth_who INTEGER, "
                              "auth_date INTEGER, "
                              "last_login_ip TEXT, "
                              "last_login_date INTEGER, "
                              "last_login_http_date INTEGER )");
    m_pDatabaseManager->Execf(m_hDbConnection, "CREATE INDEX IF NOT EXISTS IDX_SERIALUSAGE_USERID on serialusage(userid)");
    m_pDatabaseManager->Execf(m_hDbConnection, "CREATE UNIQUE INDEX IF NOT EXISTS IDX_SERIALUSAGE_USERID_SERIAL_U on serialusage(userid,serial)");

    // Check if unique index on accounts exists
    m_pDatabaseManager->QueryWithResultf(m_hDbConnection, &result, "SELECT name FROM sqlite_master WHERE type='index' AND name='IDX_ACCOUNTS_NAME_U'");
    if (result->nRows == 0)
    {
        // Need to add unique index on accounts
        if (!bNewInstallation)
            CLogger::LogPrintNoStamp("Updating accounts table...\n");

        // Make sure we have a non-unique index to speed up the duplication removal
        m_pDatabaseManager->Execf(m_hDbConnection, "CREATE INDEX IF NOT EXISTS IDX_ACCOUNTS_NAME on accounts(name)");
        // Remove any duplicate name entries
        m_pDatabaseManager->Execf(m_hDbConnection,
                                  "DELETE FROM accounts WHERE rowid in "
                                  " (SELECT A.rowid"
                                  " FROM accounts A, accounts B"
                                  " WHERE A.rowid > B.rowid AND A.name = B.name)");
        // Remove non-unique index
        m_pDatabaseManager->Execf(m_hDbConnection, "DROP INDEX IF EXISTS IDX_ACCOUNTS_NAME");
        // Add unique index
        m_pDatabaseManager->Execf(m_hDbConnection, "CREATE UNIQUE INDEX IF NOT EXISTS IDX_ACCOUNTS_NAME_U on accounts(name)");
    }

    // Check if unique index on userdata exists
    m_pDatabaseManager->QueryWithResultf(m_hDbConnection, &result, "SELECT name FROM sqlite_master WHERE type='index' AND name='IDX_USERDATA_USERID_KEY_U'");
    if (result->nRows == 0)
    {
        // Need to add unique index on userdata
        if (!bNewInstallation)
            CLogger::LogPrintNoStamp("Updating userdata table...\n");

        // Make sure we have a non-unique index to speed up the duplication removal
        m_pDatabaseManager->Execf(m_hDbConnection, "CREATE INDEX IF NOT EXISTS IDX_USERDATA_USERID_KEY on userdata(userid,key)");
        // Remove any duplicate userid+key entries
        m_pDatabaseManager->Execf(m_hDbConnection,
                                  "DELETE FROM userdata WHERE rowid in "
                                  " (SELECT A.rowid"
                                  " FROM userdata A, userdata B"
                                  " WHERE A.rowid > B.rowid AND A.userid = B.userid AND A.key = B.key)");
        // Remove non-unique index
        m_pDatabaseManager->Execf(m_hDbConnection, "DROP INDEX IF EXISTS IDX_USERDATA_USERID_KEY");
        // Add unique index
        m_pDatabaseManager->Execf(m_hDbConnection, "CREATE UNIQUE INDEX IF NOT EXISTS IDX_USERDATA_USERID_KEY_U on userdata(userid,key)");
    }

    // Ensure old indexes are removed
    m_pDatabaseManager->Execf(m_hDbConnection, "DROP INDEX IF EXISTS IDX_ACCOUNTS_NAME");
    m_pDatabaseManager->Execf(m_hDbConnection, "DROP INDEX IF EXISTS IDX_USERDATA_USERID");
    m_pDatabaseManager->Execf(m_hDbConnection, "DROP INDEX IF EXISTS IDX_USERDATA_USERID_KEY");

    // Check if httppass has been added yet
    m_pDatabaseManager->QueryWithResultf(m_hDbConnection, &result, "PRAGMA table_info(accounts)");
    if (ListContains(result->ColNames, "httppass") == false)
    {
        m_pDatabaseManager->Execf(m_hDbConnection, "ALTER TABLE accounts ADD COLUMN httppass TEXT");
    }
}

CAccountManager::~CAccountManager()
{
    // Save everything
    Save(true);
    // Delete our save file
    m_pDatabaseManager->Disconnect(m_hDbConnection);
    RemoveAll();
}

void CAccountManager::ReconnectToDatabase()
{
    if (m_hDbConnection != INVALID_DB_HANDLE)
    {
        m_pDatabaseManager->Disconnect(m_hDbConnection);
    }

    // Load internal.db
    SString strOptions;
#ifdef WITH_ACCOUNT_QUERY_LOGGING
    g_pGame->GetDatabaseManager()->SetLogLevel(EJobLogLevel::ALL, g_pGame->GetConfig()->GetDbLogFilename());
    SetOption<CDbOptionsMap>(strOptions, "log", 1);
    SetOption<CDbOptionsMap>(strOptions, "tag", "accounts");
#endif
    SetOption<CDbOptionsMap>(strOptions, "queue", DB_SQLITE_QUEUE_NAME_INTERNAL);
    m_hDbConnection = m_pDatabaseManager->Connect("sqlite", PathConform(m_strDbPathFilename), "", "", strOptions);
}

void CAccountManager::DoPulse()
{
    // Save it only once in a while whenever something has changed
    if (m_bChangedSinceSaved && GetTickCount64_() > m_llLastTimeSaved + 15000)
    {
        // Save it
        Save();
    }
}

bool CAccountManager::Load()
{
    // Create a registry result
    CRegistryResult result;
    // Select all our required information from the accounts database
    m_pDatabaseManager->QueryWithResultf(m_hDbConnection, &result, "SELECT id,name,password,ip,serial,httppass from accounts");

    // Initialize all our variables
    m_iAccounts = 0;
    bool         bNeedsVacuum = false;
    CElapsedTime activityTimer;
    bool         bOutputFeedback = false;
    for (CRegistryResultIterator iter = result->begin(); iter != result->end(); ++iter)
    {
        const CRegistryResultRow& row = *iter;
        // Fill User ID, Name & Password (Required data)
        int     iUserID = static_cast<int>(row[0].nVal);
        SString strName = (const char*)row[1].pVal;
        SString strPassword = (const char*)row[2].pVal;
        SString strIP = (const char*)row[3].pVal;
        SString strSerial = (const char*)row[4].pVal;
        SString strHttpPassAppend = (const char*)row[5].pVal;

        // Check for overlong names and incorrect escapement
        bool bRemoveAccount = false;
        bool bChanged = false;
        if (strName.length() > MAX_USERNAME_LENGTH)
        {
            // Try to repair name
            if (strName.length() <= 256)
            {
                strName = strName.Replace("\"\"", "\"", true).substr(0, MAX_USERNAME_LENGTH);
                bChanged = true;
            }

            // If name gone doolally or account with this name already exists, remove account
            if (strName.length() > 256 || Get(strName))
            {
                bNeedsVacuum = true;
                bRemoveAccount = true;
                CLogger::LogPrintf("Removed duplicate or damaged account for %s\n", strName.substr(0, 64).c_str());
            }
        }

        // Check for disallowed account names
        if (strName == "*****" || strName == CONSOLE_ACCOUNT_NAME)
            bRemoveAccount = true;

        // Do account remove if required
        if (bRemoveAccount)
        {
            m_pDatabaseManager->Execf(m_hDbConnection, "DELETE FROM accounts WHERE id=?", SQLITE_INTEGER, iUserID);
            m_pDatabaseManager->Execf(m_hDbConnection, "DELETE FROM userdata WHERE userid=?", SQLITE_INTEGER, iUserID);
            m_pDatabaseManager->Execf(m_hDbConnection, "DELETE FROM serialusage WHERE userid=?", SQLITE_INTEGER, iUserID);
            continue;
        }

        // Create a new account with the specified information
        CAccount* pAccount = g_pGame->GetAccountManager()->AddPlayerAccount(strName, strPassword, iUserID, strIP, strSerial, strHttpPassAppend);

        if (bChanged)
            pAccount->SetChanged(bChanged);
        m_iAccounts = std::max(m_iAccounts, iUserID);

        // Feedback for the user
        if (activityTimer.Get() > 5000)
        {
            activityTimer.Reset();
            bOutputFeedback = true;
            CLogger::LogPrintf("Reading accounts %d/%d\n", m_List.size(), result->nRows);
        }
    }
    if (bOutputFeedback)
        CLogger::LogPrintf("Reading accounts done.\n");
    if (bNeedsVacuum)
        m_pDatabaseManager->Execf(m_hDbConnection, "VACUUM");

    // Save any upgraded accounts
    {
        CElapsedTime activityTimer;
        bool         bOutputFeedback = false;
        uint         uiSaveCount = 0;
        for (CMappedAccountList::const_iterator iter = m_List.begin(); iter != m_List.end(); iter++)
        {
            CAccount* pAccount = *iter;
            if (pAccount->IsRegistered() && pAccount->HasChanged() && !pAccount->IsConsoleAccount())
            {
                uiSaveCount++;
                Save(pAccount, false);
                // Feedback for the user
                if (activityTimer.Get() > 5000)
                {
                    activityTimer.Reset();
                    bOutputFeedback = true;
                    CLogger::LogPrintf("Saving upgraded accounts %d\n", uiSaveCount);
                }
            }
        }

        if (uiSaveCount > 100)
        {
            bOutputFeedback = true;
            CLogger::LogPrintf("Finishing accounts upgrade...\n");
            for (uint i = 0; i < 10; i++)
            {
                Sleep(10);
                m_pDatabaseManager->DoPulse();
            }
        }

        if (bOutputFeedback)
            CLogger::LogPrintf("Completed accounts upgrade.\n");
    }

    return true;
}

void CAccountManager::Save(CAccount* pAccount, bool bCheckForErrors)
{
    SString      strName = pAccount->GetName();
    SString      strPassword = pAccount->GetPasswordHash();
    SString      strHttpPassAppend = pAccount->GetHttpPassAppend();
    SString      strIP = pAccount->GetIP();
    SString      strSerial = pAccount->GetSerial();
    unsigned int iID = pAccount->GetID();

    m_pDatabaseManager->Execf(m_hDbConnection, "INSERT OR IGNORE INTO accounts (id, name, ip, serial, password) VALUES(?,?,?,?,?)", SQLITE_INTEGER, iID,
                              SQLITE_TEXT, strName.c_str(), SQLITE_TEXT, strIP.c_str(), SQLITE_TEXT, strSerial.c_str(), SQLITE_TEXT, strPassword.c_str());

    SString strQuery;
    strQuery += m_pDatabaseManager->PrepareStringf(m_hDbConnection, "UPDATE accounts SET ip=?", SQLITE_TEXT, *strIP);
    if (!strSerial.empty())
        strQuery += m_pDatabaseManager->PrepareStringf(m_hDbConnection, ", serial=?", SQLITE_TEXT, *strSerial);
    strQuery += m_pDatabaseManager->PrepareStringf(m_hDbConnection, ", name=?, password=?, httppass=? WHERE id=?", SQLITE_TEXT, *strName, SQLITE_TEXT,
                                                   *strPassword, SQLITE_TEXT, *strHttpPassAppend, SQLITE_INTEGER, iID);

    if (bCheckForErrors)
    {
        m_pDatabaseManager->QueryWithCallback(m_hDbConnection, StaticDbCallback, this, strQuery);
    }
    else
    {
        m_pDatabaseManager->Exec(m_hDbConnection, strQuery);
    }

    SaveAccountSerialUsage(pAccount);

    // Set changed since saved to false
    pAccount->SetChanged(false);
}

void CAccountManager::Save(bool bForce)
{
    if (m_bChangedSinceSaved || bForce)
    {
        // Attempted save now
        m_bChangedSinceSaved = false;

        for (auto pAccount : m_List)
        {
            if (pAccount->IsRegistered() && pAccount->HasChanged() && !pAccount->IsConsoleAccount())
            {
                Save(pAccount);
            }
        }
    }
}

bool CAccountManager::IntegrityCheck()
{
    // Check database integrity
    {
        CRegistryResult result;
        bool            bOk = m_pDatabaseManager->QueryWithResultf(m_hDbConnection, &result, "PRAGMA integrity_check");

        // Get result as a string
        SString strResult;
        if (result->nRows && result->nColumns)
        {
            CRegistryResultCell& cell = result->Data.front()[0];
            if (cell.nType == SQLITE_TEXT)
                strResult = std::string((const char*)cell.pVal, cell.nLength - 1);
        }

        // Process result
        if (!bOk || !strResult.BeginsWithI("ok"))
        {
            CLogger::ErrorPrintf("%s", *strResult);
            CLogger::ErrorPrintf("%s\n", *m_pDatabaseManager->GetLastErrorMessage());
            CLogger::ErrorPrintf("Errors were encountered loading '%s' database\n", *ExtractFilename(PathConform("internal.db")));
            CLogger::ErrorPrintf("Maybe now is the perfect time to panic.\n");
            CLogger::ErrorPrintf("See - http://wiki.multitheftauto.com/wiki/fixdb\n");
            CLogger::ErrorPrintf("************************\n");
            // Allow server to continue
        }
    }

    // Check can update file
    {
        m_pDatabaseManager->Execf(m_hDbConnection, "DROP TABLE IF EXISTS write_test");
        m_pDatabaseManager->Execf(m_hDbConnection, "CREATE TABLE IF NOT EXISTS write_test (id INTEGER PRIMARY KEY, value INTEGER)");
        m_pDatabaseManager->Execf(m_hDbConnection, "INSERT OR IGNORE INTO write_test (id, value) VALUES(1,2)");
        bool bOk = m_pDatabaseManager->QueryWithResultf(m_hDbConnection, NULL, "UPDATE write_test SET value=3 WHERE id=1");
        if (!bOk)
        {
            CLogger::ErrorPrintf("%s\n", *m_pDatabaseManager->GetLastErrorMessage());
            CLogger::ErrorPrintf("Errors were encountered updating '%s' database\n", *ExtractFilename(PathConform("internal.db")));
            CLogger::ErrorPrintf("Database might have incorrect file permissions, or locked by another process, or damaged.\n");
            CLogger::ErrorPrintf("See - http://wiki.multitheftauto.com/wiki/fixdb\n");
            CLogger::ErrorPrintf("************************\n");
            return false;
        }
        m_pDatabaseManager->Execf(m_hDbConnection, "DROP TABLE write_test");
    }

    // Do compact if required
    if (g_pGame->GetConfig()->ShouldCompactInternalDatabases())
    {
        CLogger::LogPrintf("Compacting accounts database '%s' ...\n", *ExtractFilename(PathConform("internal.db")));

        CRegistryResult result;
        bool            bOk = m_pDatabaseManager->QueryWithResultf(m_hDbConnection, &result, "VACUUM");

        // Get result as a string
        SString strResult;
        if (result->nRows && result->nColumns)
        {
            CRegistryResultCell& cell = result->Data.front()[0];
            if (cell.nType == SQLITE_TEXT)
                strResult = std::string((const char*)cell.pVal, cell.nLength - 1);
        }

        // Process result
        if (!bOk)
        {
            CLogger::ErrorPrintf("%s", *strResult);
            CLogger::ErrorPrintf("%s\n", *m_pDatabaseManager->GetLastErrorMessage());
            CLogger::ErrorPrintf("Errors were encountered compacting '%s' database\n", *ExtractFilename(PathConform("internal.db")));
            CLogger::ErrorPrintf("Maybe now is the perfect time to panic.\n");
            CLogger::ErrorPrintf("See - http://wiki.multitheftauto.com/wiki/fixdb\n");
            CLogger::ErrorPrintf("************************\n");
            // Allow server to continue
        }
    }
    return true;
}

CAccount* CAccountManager::Get(const char* szName, const char* szPassword, bool bCaseSensitive)
{
    if (szName && szName[0])
    {
        std::vector<CAccount*> results;
        m_List.FindAccountMatches(&results, szName, bCaseSensitive);

        if (!bCaseSensitive)
        {
            CAccount* pFirstMatchAccount = nullptr;

            for (CAccount* pAccount : results)
            {
                if (!pAccount->IsRegistered())
                    continue;

                if (szPassword && !pAccount->IsPassword(szPassword))
                    continue;

                if (pAccount->GetName() == szName)
                {
                    return pAccount;
                }
                else if (!pFirstMatchAccount)
                {
                    pFirstMatchAccount = pAccount;
                }
            }

            return pFirstMatchAccount;
        }

        for (CAccount* pAccount : results)
        {
            if (pAccount->IsRegistered())
                return pAccount;
        }
    }

    return nullptr;
}

CAccount* CAccountManager::GetAccountFromScriptID(uint uiScriptID)
{
    CAccount* pAccount = (CAccount*)CIdArray::FindEntry(uiScriptID, EIdClass::ACCOUNT);
    dassert(!pAccount || ListContains(m_List, pAccount));
    return pAccount;
}

// Return the name of an account which already exists, and is using the same name with one or more letters in a different case
SString CAccountManager::GetActiveCaseVariation(const SString& strName)
{
    // Check for exact match already existing
    if (Get(strName))
        return "";

    std::vector<CAccount*> results;
    // Case insensitive search to find all variations
    m_List.FindAccountMatches(&results, strName, false);
    for (uint i = 0; i < results.size(); i++)
    {
        CAccount* pAccount = results[i];
        if (pAccount->IsRegistered() && pAccount->GetName() != strName)
        {
            return pAccount->GetName();
        }
    }
    return "";
}

void CAccountManager::RemoveFromList(CAccount* pAccount)
{
    m_List.remove(pAccount);
}

void CAccountManager::ChangingName(CAccount* pAccount, const SString& strOldName, const SString& strNewName)
{
    m_List.ChangingName(pAccount, strOldName, strNewName);
}

void CAccountManager::MarkAsChanged(CAccount* pAccount)
{
    if (pAccount->IsRegistered() && !pAccount->IsConsoleAccount())
    {
        m_bChangedSinceSaved = true;
        pAccount->SetChanged(true);
    }
}

void CAccountManager::RemoveAll()
{
    DeletePointersAndClearList(m_List);
}

bool CAccountManager::LogIn(CClient* pClient, CClient* pEchoClient, const std::string& strAccountName, const char* szPassword)
{
    // Is he already logged in?
    if (pClient->IsRegistered())
    {
        if (pEchoClient)
            pEchoClient->SendEcho("login: You are already logged in");
        return false;
    }

    if (pClient->GetClientType() != CClient::CLIENT_PLAYER)
    {
        if (pEchoClient)
            pEchoClient->SendEcho("login: Only players can log in");
        return false;
    }

    // Get the players details
    CPlayer* pPlayer = static_cast<CPlayer*>(pClient);
    SString  strPlayerName = pPlayer->GetNick();
    SString  strPlayerIP = pPlayer->GetSourceIP();
    SString  strPlayerSerial = pPlayer->GetSerial();

    std::string strSlicedAccountName = strAccountName.substr(0, MAX_USERNAME_LENGTH);
    const char* szAccountName = strSlicedAccountName.c_str();

    if (!IsValidAccountName(szAccountName))
    {
        if (pEchoClient)
            pEchoClient->SendEcho("login: Invalid account name provided");
        CLogger::AuthPrintf("LOGIN: %s tried to log in with an invalid account name (IP: %s  Serial: %s)\n", szAccountName, strPlayerIP.c_str(),
                            strPlayerSerial.c_str());
        m_AccountProtect.AddConnect(strPlayerIP.c_str());
        return false;
    }

    if (m_AccountProtect.IsFlooding(strPlayerIP.c_str()))
    {
        if (pEchoClient)
            pEchoClient->SendEcho("login: Account locked");
        CLogger::AuthPrintf("LOGIN: Ignoring %s trying to log in as '%s' (IP: %s  Serial: %s)\n", strPlayerName.c_str(), szAccountName, strPlayerIP.c_str(),
                            strPlayerSerial.c_str());
        return false;
    }

    // Grab the account on his nick if any
    CAccount* pAccount = g_pGame->GetAccountManager()->Get(szAccountName);
    if (!pAccount)
    {
        if (pEchoClient)
            pEchoClient->SendEcho(SString("login: No known account for '%s'", szAccountName).c_str());
        CLogger::AuthPrintf("LOGIN: %s tried to log in as '%s' (Unknown account) (IP: %s  Serial: %s)\n", strPlayerName.c_str(), szAccountName,
                            strPlayerIP.c_str(), strPlayerSerial.c_str());
        return false;
    }

    if (pAccount->GetClient())
    {
        if (pEchoClient)
            pEchoClient->SendEcho(SString("login: Account for '%s' is already in use", szAccountName).c_str());
        return false;
    }
    if (!IsValidPassword(szPassword) || !pAccount->IsPassword(szPassword))
    {
        if (pEchoClient)
            pEchoClient->SendEcho(SString("login: Invalid password for account '%s'", szAccountName).c_str());
        CLogger::AuthPrintf("LOGIN: %s tried to log in as '%s' with an invalid password (IP: %s  Serial: %s)\n", strPlayerName.c_str(), szAccountName,
                            strPlayerIP.c_str(), strPlayerSerial.c_str());
        m_AccountProtect.AddConnect(strPlayerIP.c_str());
        return false;
    }

    // Check serial authorization
    if (IsAuthorizedSerialRequired(pAccount))
    {
        pAccount->AddSerialForAuthorization(strPlayerSerial, strPlayerIP);
        if (!pAccount->IsSerialAuthorized(strPlayerSerial))
        {
            if (pEchoClient)
                pEchoClient->SendEcho(
                    SString("login: Serial pending authorization for account '%s' - See https:"
                            "//mtasa.com/authserial",
                            szAccountName));
            CLogger::AuthPrintf("LOGIN: %s tried to log in as '%s' with an unauthorized serial (IP: %s  Serial: %s)\n", *strPlayerName, szAccountName,
                                *strPlayerIP, *strPlayerSerial);
            CLogger::AuthPrintf(
                "LOGIN: See https:"
                "//mtasa.com/authserial\n");
            return false;
        }
    }

    // Log him in
    CAccount* pCurrentAccount = pClient->GetAccount();
    pClient->SetAccount(pAccount);
    pAccount->SetClient(pClient);

    // Call the onPlayerLogin script event
    CLuaArguments Arguments;
    Arguments.PushAccount(pCurrentAccount);
    Arguments.PushAccount(pAccount);
    Arguments.PushBoolean(false);            // was bAutoLogin
    if (!pPlayer->CallEvent("onPlayerLogin", Arguments))
    {
        // DENIED!
        pClient->SetAccount(pCurrentAccount);
        pAccount->SetClient(NULL);
        return false;
    }

    // Success is here
    pAccount->OnLoginSuccess(strPlayerSerial, strPlayerIP);

    SString strGroupList =
        SString::Join(", ", g_pGame->GetACLManager()->GetObjectGroupNames(pAccount->GetName(), CAccessControlListGroupObject::OBJECT_TYPE_USER));
    CLogger::AuthPrintf("LOGIN: (%s) %s successfully logged in as '%s' (IP: %s  Serial: %s)\n", strGroupList.c_str(), pClient->GetNick(),
                        pAccount->GetName().c_str(), strPlayerIP.c_str(), strPlayerSerial.c_str());

    // Tell the player
    if (pEchoClient)
    {
        pEchoClient->SendEcho("login: You successfully logged in");
    }

    // Delete the old account if it was a guest account
    if (!pCurrentAccount->IsRegistered())
        delete pCurrentAccount;

    return true;
}

bool CAccountManager::LogOut(CClient* pClient, CClient* pEchoClient)
{
    // Is he logged in?
    if (!pClient->IsRegistered())
    {
        if (pEchoClient)
            pEchoClient->SendEcho("logout: You were not logged in");
        return false;
    }

    if (pClient->GetClientType() != CClient::CLIENT_PLAYER)
    {
        if (pEchoClient)
            pEchoClient->SendEcho("logout: Only players can log out");
        return false;
    }
    CPlayer* pPlayer = static_cast<CPlayer*>(pClient);

    CAccount* pCurrentAccount = pClient->GetAccount();
    pCurrentAccount->SetClient(NULL);

    CAccount* pAccount = g_pGame->GetAccountManager()->AddGuestAccount(GUEST_ACCOUNT_NAME);
    pClient->SetAccount(pAccount);

    // Call our script event
    CLuaArguments Arguments;
    Arguments.PushAccount(pCurrentAccount);
    Arguments.PushAccount(pAccount);
    if (!pPlayer->CallEvent("onPlayerLogout", Arguments))
    {
        // DENIED!
        pClient->SetAccount(pCurrentAccount);
        pCurrentAccount->SetClient(pClient);
        delete pAccount;
        return false;
    }

    // Tell the console
    CLogger::AuthPrintf("LOGOUT: %s logged out as '%s'\n", pClient->GetNick(), pCurrentAccount->GetName().c_str());

    // Tell the player
    if (pEchoClient)
        pEchoClient->SendEcho("logout: You logged out");

    return true;
}

std::shared_ptr<CLuaArgument> CAccountManager::GetAccountData(CAccount* pAccount, const char* szKey)
{
    if (!pAccount->IsRegistered())
    {
        return pAccount->GetData(szKey);
    }

    // Check cache first
    if (pAccount->HasData(szKey))
    {
        return pAccount->GetData(szKey);
    }

    // Get the user ID
    int iUserID = pAccount->GetID();
    // create a new registry result for the query return
    CRegistryResult result;

    // Select the value and type from the database where the user is our user and the key is the required key
    m_pDatabaseManager->QueryWithResultf(m_hDbConnection, &result, "SELECT value,type from userdata where userid=? and key=? LIMIT 1", SQLITE_INTEGER, iUserID,
                                         SQLITE_TEXT, szKey);

    // Default result is nil
    auto pResult = std::make_shared<CLuaArgument>();

    // Do we have any results?
    if (result->nRows > 0)
    {
        const CRegistryResultRow& row = result->Data.front();

        const auto type = static_cast<int>(row[1].nVal);
        const auto value = (const char*)row[0].pVal;

        // Cache value for next get
        pAccount->SetData(szKey, value, type);

        // Account data is stored as text so we don't need to check what type it is just return it
        switch (type)
        {
            case LUA_TBOOLEAN:
                pResult->ReadBool(strcmp(value, "true") == 0);
                break;

            case LUA_TNUMBER:
                pResult->ReadNumber(strtod(value, NULL));
                break;

            case LUA_TNIL:
                break;

            case LUA_TSTRING:
                pResult->ReadString(value);
                break;

            default:
                dassert(0);            // It never should hit this, if so, something corrupted
                break;
        }
    }
    else
    {
        // No results
        pResult->ReadBool(false);
    }

    return pResult;
}

bool CAccountManager::SetAccountData(CAccount* pAccount, const char* szKey, const SString& strValue, int iType)
{
    if (iType != LUA_TSTRING && iType != LUA_TNUMBER && iType != LUA_TBOOLEAN && iType != LUA_TNIL)
        return false;

    if (!pAccount->IsRegistered())
    {
        pAccount->SetData(szKey, strValue, iType);
        return true;
    }

    // Set cache value for next get
    if (!pAccount->SetData(szKey, strValue, iType))
    {
        // If no change, skip DB update
        return true;
    }

    // Get the user ID
    int     iUserID = pAccount->GetID();
    SString strKey = szKey;

    // Does the user want to delete the data?
    if (strValue == "false" && iType == LUA_TBOOLEAN)
    {
        m_pDatabaseManager->Execf(m_hDbConnection, "DELETE FROM userdata WHERE userid=? AND key=?", SQLITE_INTEGER, iUserID, SQLITE_TEXT, strKey.c_str());
        return true;
    }

    m_pDatabaseManager->Execf(m_hDbConnection, "INSERT OR IGNORE INTO userdata (userid, key, value, type) VALUES(?,?,?,?)", SQLITE_INTEGER, pAccount->GetID(),
                              SQLITE_TEXT, strKey.c_str(), SQLITE_TEXT, strValue.c_str(), SQLITE_INTEGER, iType);
    m_pDatabaseManager->QueryWithCallbackf(m_hDbConnection, StaticDbCallback, this, "UPDATE userdata SET value=?, type=? WHERE userid=? AND key=?", SQLITE_TEXT,
                                           strValue.c_str(), SQLITE_INTEGER, iType, SQLITE_INTEGER, iUserID, SQLITE_TEXT, strKey.c_str());
    return true;
}

bool CAccountManager::CopyAccountData(CAccount* pFromAccount, CAccount* pToAccount)
{
    // list to store pFromAccount data to
    std::map<SString, CAccountData> copiedData;

    if (!pFromAccount->IsRegistered())            // is not registered account, retrieve data from memory
    {
        for (const auto& iter : *pFromAccount)
        {
            MapSet(copiedData, iter.second.GetKey(), CAccountData(iter.second.GetKey(), iter.second.GetStrValue(), iter.second.GetType()));
        }
    }
    else            // is registered account, retrieve from database
    {
        SString strKey;
        SString strValue;

        // Get the user ID of the from account
        int iUserID = pFromAccount->GetID();
        // create a new registry result for the from account query return value
        CRegistryResult result;

        m_pDatabaseManager->QueryWithResultf(m_hDbConnection, &result, "SELECT key,value,type from userdata where userid=?", SQLITE_INTEGER, iUserID);

        // Do we have any results?
        if (result->nRows > 0)
        {
            for (CRegistryResultIterator iter = result->begin(); iter != result->end(); ++iter)
            {
                const CRegistryResultRow& row = *iter;
                // Get our key
                strKey = (const char*)row[0].pVal;
                // Get our value
                strValue = (const char*)row[1].pVal;
                int iType = static_cast<int>(row[2].nVal);

                MapSet(copiedData, strKey, CAccountData(strKey, strValue, iType));
            }
        }
    }

    if (copiedData.size() > 0)            // got anything to copy?
    {
        std::map<SString, CAccountData>::iterator iter = copiedData.begin();

        for (; iter != copiedData.end(); iter++)
        {
            if (!pToAccount->IsRegistered())            // store to memory
            {
                pToAccount->SetData(iter->second.GetKey(), iter->second.GetStrValue(), iter->second.GetType());
            }
            else            // store to database
            {
                CRegistryResult subResult;

                m_pDatabaseManager->QueryWithResultf(m_hDbConnection, &subResult, "SELECT id,userid from userdata where userid=? and key=? LIMIT 1",
                                                     SQLITE_INTEGER, pToAccount->GetID(), SQLITE_TEXT, iter->second.GetKey().c_str());
                // If there is a key with this value update it otherwise insert it and store the return value in bRetVal
                if (subResult->nRows > 0)
                    m_pDatabaseManager->Execf(m_hDbConnection, "UPDATE userdata SET value=?, type=? WHERE userid=? AND key=?", SQLITE_TEXT,
                                              iter->second.GetStrValue().c_str(), SQLITE_INTEGER, iter->second.GetType(), SQLITE_INTEGER, pToAccount->GetID(),
                                              SQLITE_TEXT, iter->second.GetKey().c_str());
                else
                    m_pDatabaseManager->Execf(m_hDbConnection, "INSERT INTO userdata (userid, key, value, type) VALUES(?,?,?,?)", SQLITE_INTEGER,
                                              pToAccount->GetID(), SQLITE_TEXT, iter->second.GetKey().c_str(), SQLITE_TEXT, iter->second.GetStrValue().c_str(),
                                              SQLITE_INTEGER, iter->second.GetType());
            }
        }
        return true;
    }
    else
        return false;
}

bool CAccountManager::GetAllAccountData(CAccount* pAccount, lua_State* pLua)
{
    if (!pAccount->IsRegistered())
    {
        for (const auto& iter : *pAccount)
        {
            if (iter.second.GetType() == LUA_TNIL)
            {
                lua_pushstring(pLua, iter.second.GetKey().c_str());
                lua_pushnil(pLua);
                lua_settable(pLua, -3);
            }
            if (iter.second.GetType() == LUA_TBOOLEAN)
            {
                lua_pushstring(pLua, iter.second.GetKey().c_str());
                lua_pushboolean(pLua, iter.second.GetStrValue() == "true");
                lua_settable(pLua, -3);
            }
            if (iter.second.GetType() == LUA_TNUMBER)
            {
                lua_pushstring(pLua, iter.second.GetKey().c_str());
                lua_pushnumber(pLua, strtod(iter.second.GetStrValue().c_str(), NULL));
                lua_settable(pLua, -3);
            }
            else
            {
                lua_pushstring(pLua, iter.second.GetKey().c_str());
                lua_pushstring(pLua, iter.second.GetStrValue().c_str());
                lua_settable(pLua, -3);
            }
        }
        return true;
    }

    // Get the user ID
    int iUserID = pAccount->GetID();
    // create a new registry result for the query return
    CRegistryResult result;
    SString         strKey;

    // Select the value and type from the database where the user is our user and the key is the required key
    m_pDatabaseManager->QueryWithResultf(m_hDbConnection, &result, "SELECT key,value,type from userdata where userid=?", SQLITE_INTEGER, iUserID);

    // Do we have any results?
    if (result->nRows <= 0)
        return false;

    // Loop through until i is the same as the number of rows
    for (const auto& row : result->Data)
    {
        // Get our key
        strKey = reinterpret_cast<const char*>(row[0].pVal);
        // Get our type
        int iType = static_cast<int>(row[2].nVal);
        // Account data is stored as text so we don't need to check what type it is just return it
        if (iType == LUA_TNIL)
        {
            lua_pushstring(pLua, strKey);
            lua_pushnil(pLua);
            lua_settable(pLua, -3);
        }
        if (iType == LUA_TBOOLEAN)
        {
            SString strResult = (const char*)row[1].pVal;
            lua_pushstring(pLua, strKey);
            lua_pushboolean(pLua, strResult == "true");
            lua_settable(pLua, -3);
        }
        if (iType == LUA_TNUMBER)
        {
            lua_pushstring(pLua, strKey);
            lua_pushnumber(pLua, strtod((const char*)row[1].pVal, NULL));
            lua_settable(pLua, -3);
        }
        else
        {
            lua_pushstring(pLua, strKey);
            lua_pushstring(pLua, ((const char*)row[1].pVal));
            lua_settable(pLua, -3);
        }
    }
    return true;
}

void CAccountManager::GetAccountsBySerial(const SString& strSerial, std::vector<CAccount*>& outAccounts)
{
    Save();
    CRegistryResult result;
    m_pDatabaseManager->QueryWithResultf(m_hDbConnection, &result, "SELECT name FROM accounts WHERE serial = ?", SQLITE_TEXT, strSerial.c_str());

    for (const auto& row : result->Data)
    {
        CAccount* pAccount = Get((const char*)row[0].pVal);
        if (pAccount)
            outAccounts.push_back(pAccount);
    }
}

void CAccountManager::GetAccountsByIP(const SString& strIP, std::vector<CAccount*>& outAccounts)
{
    Save();
    CRegistryResult result;
    m_pDatabaseManager->QueryWithResultf(m_hDbConnection, &result, "SELECT name FROM accounts WHERE ip = ?", SQLITE_TEXT, strIP.c_str());

    for (const auto& row : result->Data)
    {
        CAccount* pAccount = Get((const char*)row[0].pVal);
        if (pAccount)
            outAccounts.push_back(pAccount);
    }
}

CAccount* CAccountManager::GetAccountByID(int ID)
{
    CRegistryResult result;
    m_pDatabaseManager->QueryWithResultf(m_hDbConnection, &result, "SELECT name FROM accounts WHERE id = ?", SQLITE_INTEGER, ID);

    for (const auto& row : result->Data)
    {
        return Get(reinterpret_cast<const char*>(row[0].pVal));
    }

    return nullptr;
}

void CAccountManager::GetAccountsByData(const SString& dataName, const SString& value, std::vector<CAccount*>& outAccounts)
{
    Save();
    CRegistryResult result;
    m_pDatabaseManager->QueryWithResultf(m_hDbConnection, &result,
                                         "SELECT acc.name FROM accounts acc, userdata dat WHERE dat.key = ? AND dat.value = ? AND dat.userid = acc.id",
                                         SQLITE_TEXT, dataName.c_str(), SQLITE_TEXT, value.c_str());

    for (const auto& row : result->Data)
    {
        CAccount* pAccount = Get((const char*)row[0].pVal);
        if (pAccount)
            outAccounts.push_back(pAccount);
    }
}

// Fires for all joining players
CAccount* CAccountManager::AddGuestAccount(const SString& strName)
{
    CAccount*     pAccount = new CAccount(this, EAccountType::Guest, strName);
    CLuaArguments Arguments;
    Arguments.PushAccount(pAccount);
    g_pGame->GetMapManager()->GetRootElement()->CallEvent("onAccountCreate", Arguments);
    return pAccount;
}

// Fires only when console is created
CAccount* CAccountManager::AddConsoleAccount(const SString& strName)
{
    CAccount*     pAccount = new CAccount(this, EAccountType::Console, strName);
    CLuaArguments Arguments;
    Arguments.PushAccount(pAccount);
    g_pGame->GetMapManager()->GetRootElement()->CallEvent("onAccountCreate", Arguments);
    return pAccount;
}

// Fires for all created player accounts
CAccount* CAccountManager::AddPlayerAccount(const SString& strName, const SString& strPassword, int iUserID, const SString& strIP, const SString& strSerial,
                                            const SString& strHttpPassAppend)
{
    CAccount* pAccount = new CAccount(this, EAccountType::Player, strName, strPassword, iUserID, strIP, strSerial, strHttpPassAppend);
    CLuaArguments Arguments;
    Arguments.PushAccount(pAccount);
    g_pGame->GetMapManager()->GetRootElement()->CallEvent("onAccountCreate", Arguments);
    return pAccount;
}

// Fires whenever "addaccount" or "addAccount" was executed
CAccount* CAccountManager::AddNewPlayerAccount(const SString& strName, const SString& strPassword)
{
    CAccount* pAccount = new CAccount(this, EAccountType::Player, strName, strPassword, ++m_iAccounts);
    Save(pAccount);
    CLuaArguments Arguments;
    Arguments.PushAccount(pAccount);
    g_pGame->GetMapManager()->GetRootElement()->CallEvent("onAccountCreate", Arguments);
    return pAccount;
}

bool CAccountManager::RemoveAccount(CAccount* pAccount)
{
    if (pAccount->IsConsoleAccount())
        return false;
    if (pAccount->IsRegistered())
    {
        int iUserID = pAccount->GetID();
        m_pDatabaseManager->Execf(m_hDbConnection, "DELETE FROM accounts WHERE id=?", SQLITE_INTEGER, iUserID);
        m_pDatabaseManager->Execf(m_hDbConnection, "DELETE FROM userdata WHERE userid=?", SQLITE_INTEGER, iUserID);
        m_pDatabaseManager->Execf(m_hDbConnection, "DELETE FROM serialusage WHERE userid=?", SQLITE_INTEGER, iUserID);
    }
    CLuaArguments Arguments;
    Arguments.PushAccount(pAccount);
    g_pGame->GetMapManager()->GetRootElement()->CallEvent("onAccountRemove", Arguments);
    delete pAccount;
    return true;
}

//
// Callback for some database queries.
//    This is only done to check for (and report) errors
//
void CAccountManager::StaticDbCallback(CDbJobData* pJobData, void* pContext)
{
    if (pJobData->stage == EJobStage::RESULT)
        ((CAccountManager*)pContext)->DbCallback(pJobData);
#ifdef MTA_DEBUG
    else
        CLogger::LogPrintf("DEBUGINFO: StaticDbCallback stage was %d for '%s'\n", pJobData->stage, *pJobData->GetCommandStringForLog());
#endif
}

void CAccountManager::DbCallback(CDbJobData* pJobData)
{
    if (!m_pDatabaseManager->QueryPoll(pJobData, 0))
    {
        CLogger::LogPrintf("ERROR: Something worrying happened in DbCallback '%s': %s.\n", *pJobData->GetCommandStringForLog(), *pJobData->result.strReason);
        return;
    }

    if (pJobData->result.status != EJobResult::FAIL)
        return;

    CLogger::LogPrintf("ERROR: While updating account with '%s': %s.\n", *pJobData->GetCommandStringForLog(), *pJobData->result.strReason);
    if (pJobData->result.strReason.ContainsI("missing database"))
    {
        // Try reconnection
        CLogger::LogPrintf("INFO: Reconnecting to accounts database\n");
        ReconnectToDatabase();
    }
}

//
// Check name is legal for an existing account
//
bool CAccountManager::IsValidAccountName(const SString& strName)
{
    return strName.length() >= MIN_USERNAME_LENGTH;
}

//
// Check password is legal for an existing account
//
bool CAccountManager::IsValidPassword(const SString& strPassword)
{
    return strPassword.length() >= MIN_PASSWORD_LENGTH;
}

//
// Check name is legal for a new account
//
bool CAccountManager::IsValidNewAccountName(const SString& strName)
{
    if (!IsValidAccountName(strName) || strName.length() > MAX_USERNAME_LENGTH)
        return false;

    // Extra restrictions for new account names
    if (strName == "*****")
        return false;

    return true;
}

//
// Check password is legal for a new account/password
//
bool CAccountManager::IsValidNewPassword(const SString& strPassword)
{
    if (!IsValidPassword(strPassword))
        return false;

    // Extra restrictions for new account passwords
    if (strPassword == "*****")
        return false;

    return true;
}

//
// Return true if account is member of a restricted group
//
bool CAccountManager::IsAuthorizedSerialRequired(CAccount* pAccount)
{
    for (const auto& strGroup : g_pGame->GetACLManager()->GetObjectGroupNames(pAccount->GetName(), CAccessControlListGroupObject::OBJECT_TYPE_USER))
    {
        if (g_pGame->GetConfig()->IsAuthSerialGroup(strGroup))
            return true;
    }
    return false;
}

//
// Return true if account and supplied IP is allowed by auth serial rules
//
bool CAccountManager::IsHttpLoginAllowed(CAccount* pAccount, const SString& strIp)
{
    return !g_pGame->GetConfig()->GetAuthSerialHttpEnabled() || g_pGame->GetConfig()->IsAuthSerialHttpIpException(strIp) ||
        !IsAuthorizedSerialRequired(pAccount) || pAccount->IsIpAuthorized(strIp);
}

//
// Load authorized serial list for the supplied account
//
void CAccountManager::LoadAccountSerialUsage(CAccount* pAccount)
{
    auto& outSerialUsageList = pAccount->GetSerialUsageList();
    outSerialUsageList.clear();

    CRegistryResult result;
    m_pDatabaseManager->QueryWithResultf(m_hDbConnection, &result,
                                         "SELECT "
                                         " serial"
                                         " ,added_ip"
                                         " ,added_date"
                                         " ,auth_who"
                                         " ,auth_date"
                                         " ,last_login_ip"
                                         " ,last_login_date"
                                         " ,last_login_http_date"
                                         " FROM serialusage"
                                         " WHERE userid=?",
                                         SQLITE_INTEGER, pAccount->GetID());

    for (const auto& row : result->Data)
    {
        outSerialUsageList.push_back(CAccount::SSerialUsage());
        CAccount::SSerialUsage& info = outSerialUsageList.back();
        info.strSerial = (const char*)row[0].pVal;
        info.strAddedIp = (const char*)row[1].pVal;
        info.tAddedDate = row[2].GetNumber<time_t>();
        info.strAuthWho = (const char*)row[3].pVal;
        info.tAuthDate = row[4].GetNumber<time_t>();
        info.strLastLoginIp = (const char*)row[5].pVal;
        info.tLastLoginDate = row[6].GetNumber<time_t>();
        info.tLastLoginHttpDate = row[7].GetNumber<time_t>();
    }
}

//
// Save authorized serial list (if it exists) for the supplied account
//
void CAccountManager::SaveAccountSerialUsage(CAccount* pAccount)
{
    if (!pAccount->HasLoadedSerialUsage())
        return;

    // Update/add active serials
    for (auto& info : pAccount->GetSerialUsageList())
    {
        m_pDatabaseManager->Execf(m_hDbConnection,
                                  "INSERT OR IGNORE INTO serialusage"
                                  " ("
                                  "  userid"
                                  " ,serial"
                                  " )"
                                  " VALUES(?,?)",
                                  SQLITE_INTEGER, pAccount->GetID(), SQLITE_TEXT, *info.strSerial);

        m_pDatabaseManager->QueryWithCallbackf(m_hDbConnection, StaticDbCallback, this,
                                               "UPDATE serialusage "
                                               " SET "
                                               "  added_ip=?"
                                               " ,added_date=?"
                                               " ,auth_who=?"
                                               " ,auth_date=?"
                                               " ,last_login_ip=?"
                                               " ,last_login_date=?"
                                               " ,last_login_http_date=?"
                                               " WHERE userid=? AND serial=?"
                                               // SET
                                               ,
                                               SQLITE_TEXT, *info.strAddedIp, SQLITE_INTEGER, (int)info.tAddedDate, SQLITE_TEXT, *info.strAuthWho,
                                               SQLITE_INTEGER, (int)info.tAuthDate, SQLITE_TEXT, *info.strLastLoginIp, SQLITE_INTEGER, (int)info.tLastLoginDate,
                                               SQLITE_INTEGER,
                                               (int)info.tLastLoginHttpDate
                                               // WHERE
                                               ,
                                               SQLITE_INTEGER, pAccount->GetID(), SQLITE_TEXT, *info.strSerial);
    }

    // Delete removed serials
    SString strQuery = m_pDatabaseManager->PrepareStringf(m_hDbConnection,
                                                          "DELETE FROM serialusage"
                                                          " WHERE"
                                                          " userid=?",
                                                          SQLITE_INTEGER, pAccount->GetID());

    for (auto& info : pAccount->GetSerialUsageList())
    {
        strQuery += m_pDatabaseManager->PrepareStringf(m_hDbConnection, " AND serial!=?", SQLITE_TEXT, *info.strSerial);
    }

    m_pDatabaseManager->QueryWithCallbackf(m_hDbConnection, StaticDbCallback, this, strQuery);
}
