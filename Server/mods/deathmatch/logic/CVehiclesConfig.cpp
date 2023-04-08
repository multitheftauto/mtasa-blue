#include "StdInc.h"
#include "CVehiclesConfig.h"
#include "CLogger.h"
#include "models/CModelVehicle.h"
#include "models/CModelManager.h"
#include <logic/CVehicle.h>
#include "CGame.h"
#include "CHandlingManager.h"

CVehiclesConfig::CVehiclesConfig() : CXMLConfig(nullptr)
{
}

bool CVehiclesConfig::Load()
{
    // Eventually destroy the previously loaded xml
    if (m_pFile)
    {
        delete m_pFile;
        m_pFile = nullptr;
    }

    // Load the XML
    m_pFile = g_pServerInterface->GetXML()->CreateXML(GetFileName().c_str());
    if (!m_pFile)
    {
        CLogger::ErrorPrintf("Error loading config file\n");
        return false;
    }

    // Parse it
    if (!m_pFile->Parse())
    {
        SString strParseErrorDesc;
        m_pFile->GetLastError(strParseErrorDesc);
        CLogger::ErrorPrintf("Error parsing %s - %s\n", *ExtractFilename(GetFileName()), *strParseErrorDesc);
        return false;
    }

    // Grab the XML root node
    CXMLNode* pRootNode = m_pFile->GetRootNode();
    if (!pRootNode)
    {
        CLogger::ErrorPrintf("Missing root node ('config')\n");
        return false;
    }

    RegisterVehicles(pRootNode);

    return true;
}

bool CVehiclesConfig::Save()
{
    // Not implement
    return false;
}

void CVehiclesConfig::RegisterVehicles(CXMLNode* pRootNode)
{
    for (auto it = pRootNode->ChildrenBegin(); it != pRootNode->ChildrenEnd(); ++it)
    {
        CXMLNode*       pNode = *it;
        CXMLAttributes& pAttibites = pNode->GetAttributes();

        CXMLAttribute*     pAttribute;
        uint32_t           uiModelID;
        SModelVehicleDefs* sModelData = new SModelVehicleDefs();
        std::string_view   strType;
        std::string_view   strVarianType;

        pAttribute = pAttibites.Find("modelID");
        if (pAttribute)
            uiModelID = atoi(pAttribute->GetValue().c_str());
        else
            continue;

        pAttribute = pAttibites.Find("name");
        if (pAttribute)
        {
            std::string_view strName = pAttribute->GetValue();
            const char*      pName = new char[strName.size()];
            sModelData->strVehicleName = pName;
        }

        pAttribute = pAttibites.Find("type");
        if (pAttribute)
            strType = pAttribute->GetValue();

        pAttribute = pAttibites.Find("variationType");
        if (pAttribute)
            strVarianType = pAttribute->GetValue();

        pAttribute = pAttibites.Find("variantsCount");
        if (pAttribute)
            sModelData->uiVariantsCount = atoi(pAttribute->GetValue().c_str());

        pAttribute = pAttibites.Find("attributes");
        if (pAttribute)
            sModelData->cAttributes = atoi(pAttribute->GetValue().c_str());

        pAttribute = pAttibites.Find("maxPassngers");
        if (pAttribute)
            sModelData->uiMaxPassengers = atoi(pAttribute->GetValue().c_str());

        pAttribute = pAttibites.Find("hasDoors");
        if (pAttribute)
            sModelData->bHasDoors = (pAttribute->GetValue().compare("true") == 0);

        if (strType.compare("car") == 0)
        {
            sModelData->eVehicleModelType = eVehicleType::CAR;
        }
        else if (strType.compare("mtruck") == 0)
        {
            sModelData->eVehicleModelType = eVehicleType::MONSTERTRUCK;
        }
        else if (strType.compare("heli") == 0)
        {
            sModelData->eVehicleModelType = eVehicleType::HELI;
        }
        else if (strType.compare("boat") == 0)
        {
            sModelData->eVehicleModelType = eVehicleType::BOAT;
        }
        else if (strType.compare("trailer") == 0)
        {
            sModelData->eVehicleModelType = eVehicleType::TRAILER;
        }
        else if (strType.compare("train") == 0)
        {
            sModelData->eVehicleModelType = eVehicleType::TRAIN;
        }
        else if (strType.compare("plane") == 0)
        {
            sModelData->eVehicleModelType = eVehicleType::PLANE;
        }
        else if (strType.compare("bike") == 0)
        {
            sModelData->eVehicleModelType = eVehicleType::BIKE;
        }
        else if (strType.compare("quad") == 0)
        {
            sModelData->eVehicleModelType = eVehicleType::QUADBIKE;
        }
        else if (strType.compare("bmx") == 0)
        {
            sModelData->eVehicleModelType = eVehicleType::BMX;
        }
        else
        {
            continue;
        }

        if (strVarianType.compare("nrg") == 0)
        {
            sModelData->eVariationType = eVehicleVariationType::NRG;
        }
        else if (strVarianType.compare("caddy") == 0)
        {
            sModelData->eVariationType = eVehicleVariationType::CADDY;
        }
        else if (strVarianType.compare("slamvan") == 0)
        {
            sModelData->eVariationType = eVehicleVariationType::SLAMVAN;
        }
        else
        {
            sModelData->eVariationType = eVehicleVariationType::DEFAULT;
        }

        CModelVehicle* pModelData = new CModelVehicle(uiModelID, *sModelData);
        g_pGame->GetModelManager()->RegisterModel(pModelData);
    }
}
