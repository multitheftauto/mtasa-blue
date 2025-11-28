/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from https://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CResourceTranslationItem.h"
#include "CResource.h"
#include "CResourceTranslationManager.h"
#include "CLogger.h"
#include <filesystem>

CResourceTranslationItem::CResourceTranslationItem(CResource* resource, const char* name, const char* src, CXMLAttributes* xmlAttributes)
    : CResourceFile(resource, name, src, xmlAttributes), m_isPrimary(false)
{
    m_type = RESOURCE_FILE_TYPE_TRANSLATION;
    m_language = ExtractLanguageFromName();
    
    if (xmlAttributes)
    {
        CXMLAttribute* primaryAttr = xmlAttributes->Find("primary");
        if (primaryAttr)
        {
            std::string primaryValue = primaryAttr->GetValue();
            m_isPrimary = (primaryValue == "true");
        }
    }
}

bool CResourceTranslationItem::Start()
{
    if (!std::filesystem::exists(m_strResourceFileName))
    {
        CLogger::ErrorPrintf("Translation file '%s' not found for resource '%s'\n", 
                            m_strShortName.c_str(), m_resource->GetName().c_str());
        return false;
    }

    if (m_resource->GetTranslationManager())
    {
        return m_resource->GetTranslationManager()->LoadTranslation(m_strResourceFileName, m_isPrimary);
    }

    return false;
}

bool CResourceTranslationItem::Stop()
{
    return true;
}

std::string CResourceTranslationItem::ExtractLanguageFromName() const
{
    std::filesystem::path path(m_strShortName);
    return path.stem().string();
}
