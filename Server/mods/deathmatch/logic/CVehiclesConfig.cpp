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
    m_pRootNode = nullptr;
}

bool CVehiclesConfig::Load()
{
    // Eventually destroy the previously loaded xml
    if (m_pFile)
    {
        delete m_pFile;
        m_pFile = NULL;
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
    m_pRootNode = m_pFile->GetRootNode();
    if (!m_pRootNode)
    {
        CLogger::ErrorPrintf("Missing root node ('config')\n");
        return false;
    }

    RegisterVehicles();
    return true;
}

bool CVehiclesConfig::Save()
{
    // Not implement
    return false;
}

void CVehiclesConfig::RegisterVehicles()
{
    for (auto it = m_pRootNode->ChildrenBegin(); it != m_pRootNode->ChildrenEnd(); ++it)
    {
        CXMLNode* pNode = *it;
        CXMLAttributes &pAttibites = pNode->GetAttributes();

        CXMLAttribute*     pAttribute;
        uint32_t           uiModelID;
        SModelVehicleDefs* sModelData = new SModelVehicleDefs();
        const char*        strType;
        const char*        strVarianType;

        pAttribute = pAttibites.Find("modelID");
        if (pAttribute)
            uiModelID = atoi(pAttribute->GetValue().c_str());
        else
            continue;

        pAttribute = pAttibites.Find("name");
        if (pAttribute)
            sModelData->strVehicleName = pAttribute->GetValue().c_str();

        pAttribute = pAttibites.Find("type");
        if (pAttribute)
            strType = pAttribute->GetValue().c_str();

        pAttribute = pAttibites.Find("variationType");
        if (pAttribute)
            strVarianType = pAttribute->GetValue().c_str();

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
            sModelData->bHasDoors = strcmp(pAttribute->GetValue().c_str(), "true") == 0;


        if (strcmp(strType, "car") == 0)
        {
            sModelData->eVehicleModelType = eVehicleType::CAR;
        }
        else if (strcmp(strType, "mtruck") == 0)
        {
            sModelData->eVehicleModelType = eVehicleType::MONSTERTRUCK;
        }
        else if (strcmp(strType, "heli") == 0)
        {
            sModelData->eVehicleModelType = eVehicleType::HELI;
        }
        else if (strcmp(strType, "boat") == 0)
        {
            sModelData->eVehicleModelType = eVehicleType::BOAT;
        }
        else if (strcmp(strType, "trailer") == 0)
        {
            sModelData->eVehicleModelType = eVehicleType::TRAILER;
        }
        else if (strcmp(strType, "train") == 0)
        {
            sModelData->eVehicleModelType = eVehicleType::TRAIN;
        }
        else if (strcmp(strType, "plane") == 0)
        {
            sModelData->eVehicleModelType = eVehicleType::PLANE;
        }
        else if (strcmp(strType, "bike") == 0)
        {
            sModelData->eVehicleModelType = eVehicleType::BIKE;
        }
        else if (strcmp(strType, "quad") == 0)
        {
            sModelData->eVehicleModelType = eVehicleType::QUADBIKE;
        }
        else if (strcmp(strType, "bmx") == 0)
        {
            sModelData->eVehicleModelType = eVehicleType::BMX;
        }

        else if (strcmp(strVarianType, "nrg") == 0)
        {
            sModelData->eVariationType = eVehicleVariationType::NRG;
        }
        else if (strcmp(strVarianType, "caddy") == 0)
        {
            sModelData->eVariationType = eVehicleVariationType::CADDY;
        }
        else if (strcmp(strVarianType, "slamvan") == 0)
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

