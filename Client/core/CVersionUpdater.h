/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CVersionUpdater.h
 *  PURPOSE:     Header file for version update check and message dialog
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CVersionUpdaterInterface
{
public:
    virtual ~CVersionUpdaterInterface() {}
    virtual bool           SaveConfigToXML() = 0;
    virtual void           EnableChecking(bool bOn) = 0;
    virtual void           DoPulse() = 0;
    virtual void           InitiateUpdate(const SString& strType, const SString& strData, const SString& strHost) = 0;
    virtual bool           IsOptionalUpdateInfoRequired(const SString& strHost) = 0;
    virtual void           InitiateDataFilesFix() = 0;
    virtual void           InitiateManualCheck() = 0;
    virtual void           GetAseServerList(std::vector<SString>& outResult) = 0;
    virtual void           InitiateSidegradeLaunch(const SString& strVersion, const SString& strIp, ushort usPort, const SString& strName,
                                                   const SString& strPassword) = 0;
    virtual void           GetBrowseVersionMaps(std::map<SString, int>& outBlockedVersionMap, std::map<SString, int>& outAllowedVersionMap) = 0;
    virtual void           GetNewsSettings(SString& strOutOldestPost, uint& uiOutMaxHistoryLength) = 0;
    virtual const SString& GetDebugFilterString() = 0;
};

CVersionUpdaterInterface* GetVersionUpdater();
