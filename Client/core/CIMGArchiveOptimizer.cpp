#include "StdInc.h"
#include "CIMGArchive.h"
#include "CIDELoader.h"
#include "C3DModelOptimizer.h"
/*
GUIEditor.window[1] = guiCreateWindow(499, 255, 366, 177, "Optimize gta3.img", false)
guiWindowSetSizable(GUIEditor.window[1], false)

GUIEditor.label[1] = guiCreateLabel(11, 28, 349, 57, , false, GUIEditor.window[1])
guiSetFont(GUIEditor.label[1], "default-bold-small")
guiLabelSetHorizontalAlign(GUIEditor.label[1], "left", true)

GUIEditor.button[1] = guiCreateButton(112, 108, 125, 47, "GENERATE", false, GUIEditor.window[1])
*/

CIMGArchiveOptimizer::CIMGArchiveOptimizer()
{
    CreateGUI();
}

void CIMGArchiveOptimizer::CreateGUI()
{
    CVector2D resolution = CCore::GetSingleton().GetGUI()->GetResolution();
    CVector2D windowSize = CVector2D(499, 255);
    CGUI*     pManager = g_pCore->GetGUI();

    CGUIWindow* pWindow = reinterpret_cast<CGUIWindow*>(pManager->CreateWnd(NULL, _("IMG OPTIMIZER")));
    pWindow->SetCloseButtonEnabled(true);
    pWindow->SetMovable(true);
    pWindow->SetPosition((resolution - windowSize) / 2);
    pWindow->SetSize(CVector2D(366, 177));
    pWindow->SetSizingEnabled(false);
    pWindow->SetAlwaysOnTop(true);
    pWindow->BringToFront();
    pWindow->SetVisible(false);
    m_pWindow = pWindow;

    SString strImgSpaceRequiredText =
        "A custom optimized gta3.img file needs to be generated in your models folder. \
                Please make sure you have the required space. The size of the custom gta3.img will be a little bigger \
                than your current one. ";

    m_pImgSpaceRequiredLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pWindow, _(strImgSpaceRequiredText)));
    m_pImgSpaceRequiredLabel->SetPosition(CVector2D(11, 28));
    m_pImgSpaceRequiredLabel->SetSize(CVector2D(349, 57));
    m_pImgSpaceRequiredLabel->SetFont("default-bold-small");
    m_pImgSpaceRequiredLabel->SetHorizontalAlign(CGUI_ALIGN_LEFT);

    CGUIButton* pImgGenerateButton = reinterpret_cast<CGUIButton*>(pManager->CreateButton(m_pWindow, _("GENERATE")));
    pImgGenerateButton->SetClickHandler(GUI_CALLBACK(&CIMGArchiveOptimizer::OnImgGenerateClick, this));
    pImgGenerateButton->SetSize(CVector2D(125, 47));
    pImgGenerateButton->SetPosition(CVector2D(112, 108));
    pImgGenerateButton->SetZOrderingEnabled(false);
}

void CIMGArchiveOptimizer::SetVisible(bool bVisible)
{
    if (bVisible)
    {
        if (!m_pWindow->IsVisible())
        {
            std::printf("yeah, making it visible\n");
            m_pWindow->SetVisible(bVisible);
            m_pWindow->SetVisible(bVisible);
            m_pWindow->BringToFront();
            m_pWindow->SetZOrderingEnabled(false);
        }
    }
    else
    {
        m_pWindow->SetVisible(bVisible);
    }
}

DWORD GetActualFileSize(DWORD blockSize)
{
    const uint64_t IMG_BLOCK_SIZE = 2048;
    blockSize = (blockSize + IMG_BLOCK_SIZE - 1) & ~(IMG_BLOCK_SIZE - 1);            // Round up to block size
    return blockSize;
}

void OptimizeTXDFile(CIMGArchiveFile* newFile)
{
    CRenderWare* pRenderWare = g_pCore->GetGame()->GetRenderWare();
    auto         RwTexDictionaryStreamGetSize = (unsigned int(__cdecl*)(RwTexDictionary * dict))0x804930;

    RwTexDictionary* pTxdDictionary = pRenderWare->ReadTXD(nullptr, newFile->fileByteBuffer, false);
    if (pTxdDictionary)
    {
        // std::printf("txd loaded\n");
        unsigned int txdSize = RwTexDictionaryStreamGetSize(pTxdDictionary);

        // free the memory
        CBuffer().Swap(newFile->fileByteBuffer);

        newFile->actualFileSize = GetActualFileSize(txdSize);
        newFile->fileEntry.usSize = newFile->actualFileSize / 2048;
        std::printf("txd file: %s  | new txd size: %d | actualFileSize: %d | capacity: %d\n", newFile->fileEntry.fileName, txdSize,
                    (int)newFile->actualFileSize, newFile->fileByteBuffer.GetCapacity());

        // newFile->fileByteBuffer.SetSize(newFile->actualFileSize);
        // pRenderWare->WriteTXD(newFile->fileByteBuffer.GetData(), newFile->actualFileSize, pTxdDictionary);

        pRenderWare->DestroyTXD(pTxdDictionary);
    }
    else
    {
        std::printf("failed to read %s\n", newFile->fileEntry.fileName);
    }
}

bool IsVehicleModel(const int modelID)
{
    return modelID >= 400 && modelID <= 611;
}

bool IsVehiclMODModel(const int modelID)
{
    return modelID >= 1000 && modelID <= 1193;
}

static RwTexDictionary* g_pVehicleTxdDictionary = nullptr;

// Seriously, this should be refactored later, lol
class CTempClass
{
public:
    CTempClass() {}

    std::vector<CIMGArchiveFile> imgArchiveCustomFiles;
};

bool OptimizeDFFFile(size_t modelIndexInImg, CIMGArchive* pIMgArchive, CTempClass* ptempClass, CIMGArchiveFile* pDFFArchiveFile, CIDELoader& ideLoader)
{
    CRenderWare* pRenderWare = g_pCore->GetGame()->GetRenderWare();
    auto         RpClumpStreamGetSize = (unsigned int(__cdecl*)(RpClump*))0x74A5E0;
    auto         RwTexDictionaryStreamGetSize = (unsigned int(__cdecl*)(RwTexDictionary * dict))0x804930;

    bool bOptimizedDFFFile = false;

    /*
    // REMOVE LATER
    int         modelID = 0;
    const char* pStrDFFName = "des_cluckinlod.dff";            //"infernus.dff";
    memcpy(pDFFArchiveFile->fileEntry.fileName, pStrDFFName, strlen(pStrDFFName) + 1);

    RwTexDictionary* pTxdDictionary = pRenderWare->ReadTXD("lod_countryn.txd", CBuffer(), false);
    // REMOVE END
    */

    const unsigned int uiDFFNameHash = HashString(pDFFArchiveFile->fileEntry.fileName);
    SDFFDescriptor* pDFFDescriptor = ideLoader.GetDFFDescriptor(uiDFFNameHash);
    if (!pDFFDescriptor)
    {
        std::printf("couldn't find dff descriptor for '%s'\n", pDFFArchiveFile->fileEntry.fileName);
        return bOptimizedDFFFile;
    }

    ///*
    STXDDescriptor* pTXDDescriptor = pDFFDescriptor->GetTXDDescriptor();

    // Do we really need to optimize vehicle mods? I think no.
    if (IsVehiclMODModel(pDFFDescriptor->GetModelID()))
    {
        return false;
    }

    RwTexDictionary* pTxdDictionary = pTXDDescriptor->GetTextureDictionary();
    if (!pTxdDictionary)
    {
        STXDImgArchiveInfo* pTXDImgArchiveInfo = pTXDDescriptor->GetTXDImgArchiveInfo();
        if (pTXDImgArchiveInfo->usSize == 0)
        {
            return bOptimizedDFFFile;
        }
        CIMGArchiveFile* txdArchiveFile = new CIMGArchiveFile();
        if (pIMgArchive->GetFileByTXDImgArchiveInfo(pTXDImgArchiveInfo, *txdArchiveFile))
        {
            pTxdDictionary = pRenderWare->ReadTXD(nullptr, txdArchiveFile->fileByteBuffer, false);
        }
        else
        {
            printf("Failed to read TXD for dff '%s'\n", pDFFArchiveFile->fileEntry.fileName);
        }
        delete txdArchiveFile;
    }
    //*/

    // /*
    int modelID = pDFFDescriptor->GetModelID();
    if (IsVehicleModel(modelID))
    {
        pRenderWare->CopyTexturesFromDictionary(pTxdDictionary, g_pVehicleTxdDictionary);
    }
    // */

    pRenderWare->SetCurrentDFFWriteModelID(modelID);
    pRenderWare->SetCurrentReadDFFWithoutReplacingCOL(true);

    printf("\nabout to read: %s | size: %u | modelIndexInImg: %u\n\n", (char*)pDFFArchiveFile->fileEntry.fileName,pDFFArchiveFile->actualFileSize, modelIndexInImg);

        bool     bLoadCollision = IsVehicleModel(modelID);
    RwBuffer buffer = {pDFFArchiveFile->GetData(), pDFFArchiveFile->actualFileSize};
    //RpClump* pClump = pRenderWare->ReadDFF(pDFFArchiveFile->fileEntry.fileName, CBuffer(), modelID, bLoadCollision, pTxdDictionary);
    RpClump* pClump = pRenderWare->ReadDFF(pDFFArchiveFile->fileEntry.fileName, buffer, modelID, bLoadCollision, pTxdDictionary);
    pRenderWare->SetCurrentReadDFFWithoutReplacingCOL(false);
    if (pClump)
    {
        SString strPathOfGeneratedDff = "dffs\\";
        // RwTexDictionary* pAtlasTxdDictionary = nullptr;

        C3DModelOptimizer* modelOptimizer = new C3DModelOptimizer(pClump, pTxdDictionary);
        RwTexDictionary*   pAtlasTxdDictionary = modelOptimizer->GetAtlasTexDictionary();
        if (!pAtlasTxdDictionary)
        {
            delete modelOptimizer;
            modelOptimizer = nullptr;
            std::printf("failed to CREATE TXD atlas :(\n");
            if (bLoadCollision)
            {
                pRenderWare->DeleteReadDFFCollisionModel();
            }

            pRenderWare->DestroyDFF(pClump);
            pRenderWare->TxdForceUnload(0, true, pTxdDictionary);
            return bOptimizedDFFFile;
        }

        delete modelOptimizer;
        modelOptimizer = nullptr;
        //*/

        unsigned int clumpSize = RpClumpStreamGetSize(pClump);

        // there's still an issue with size of empty extension headers of 12 bytes for clump
        clumpSize += 24;

        CIMGArchiveFile& dffArchiveFile = ptempClass->imgArchiveCustomFiles.emplace_back();
        dffArchiveFile.fileEntry = pDFFArchiveFile->fileEntry;
        dffArchiveFile.actualFileSize = GetActualFileSize(clumpSize);
        dffArchiveFile.fileEntry.usSize = dffArchiveFile.actualFileSize / 2048;
        dffArchiveFile.fileByteBuffer.SetSize(dffArchiveFile.actualFileSize);
        void* pDFFData = dffArchiveFile.fileByteBuffer.GetData();
        //pRenderWare->WriteDFF(pDFFData, dffArchiveFile.actualFileSize, pClump);

        // Remove this line later
         pRenderWare->WriteDFF(strPathOfGeneratedDff + dffArchiveFile.fileEntry.fileName, pClump);

        ///*
        if (pAtlasTxdDictionary)
        {
            std::string fileName = pDFFArchiveFile->fileEntry.fileName;
            size_t      lastindex = fileName.find_last_of(".");
            std::string fileNameWithoutExtension = fileName.substr(0, lastindex);
            std::string strTXDFileName = fileNameWithoutExtension + ".txd";
            const char* pTXDName = strTXDFileName.c_str();

            printf("txd name: %s\n", strTXDFileName.c_str());

            unsigned int txdSize = RwTexDictionaryStreamGetSize(pAtlasTxdDictionary);

            // there's still an issue with size of empty extension headers of 12 bytes for TXD
            txdSize += 24;

            CIMGArchiveFile& txdArchiveFile = ptempClass->imgArchiveCustomFiles.emplace_back();
            memcpy(txdArchiveFile.fileEntry.fileName, pTXDName, strlen(pTXDName) + 1);

            txdArchiveFile.actualFileSize = GetActualFileSize(txdSize);
            txdArchiveFile.fileEntry.usSize = txdArchiveFile.actualFileSize / 2048;
            txdArchiveFile.fileByteBuffer.SetSize(txdArchiveFile.actualFileSize);
            void* pTXDData = txdArchiveFile.fileByteBuffer.GetData();
            //pRenderWare->WriteTXD(pTXDData, txdArchiveFile.actualFileSize, pAtlasTxdDictionary);
            // Remove this line later
            pRenderWare->WriteTXD(strPathOfGeneratedDff + pTXDName, pAtlasTxdDictionary);
        }
        //*/
        if (bLoadCollision)
        {
            pRenderWare->DeleteReadDFFCollisionModel();
        }

        pRenderWare->DestroyDFF(pClump);
        pRenderWare->TxdForceUnload(0, true, pAtlasTxdDictionary);

        bOptimizedDFFFile = true;
    }
    else
    {
        std::printf("failed to read %s | modelIndexInImg: %d\n", pDFFArchiveFile->fileEntry.fileName, modelIndexInImg);
    }

    pRenderWare->TxdForceUnload(0, true, pTxdDictionary);

    return bOptimizedDFFFile;
}

// 5 mb
const unsigned int imgReadWriteOperationSizeInBytes = 50 * 1024 * 1024;

bool CIMGArchiveOptimizer::OnImgGenerateClick(CGUIElement* pElement)
{
    std::printf("Generate button pressed\n");

    CRenderWare* pRenderWare = g_pCore->GetGame()->GetRenderWare();

    if (!g_pVehicleTxdDictionary)
    {
        g_pVehicleTxdDictionary = pRenderWare->ReadTXD("models\\generic\\vehicle.txd", CBuffer(), false);
        assert(g_pVehicleTxdDictionary != nullptr);
    }

    std::string outputFolder = "OptimizedFiles";
    if (CreateDirectory(outputFolder.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError())
    {
    }

    CIDELoader ideLoader;

    CIMGArchive* newIMgArchive = new CIMGArchive("models\\gta3.img", IMG_FILE_READ);
    newIMgArchive->ReadEntries();

    ideLoader.AddTXDDFFInfoToMaps(newIMgArchive);

    CIMGArchive* newIMgArchiveOut = new CIMGArchive("proxy_test_gta3.img", IMG_FILE_WRITE);

    std::vector<CIMGArchiveFile>& imgArchiveFiles = newIMgArchive->GetNextImgFiles(imgReadWriteOperationSizeInBytes);
    CTempClass*                   pMyTempClass = new CTempClass();
    std::vector<CIMGArchiveFile*> imgArchiveFilesOutput;

    unsigned int totalPossibleCustomOutputFiles = 0;
    for (auto& archiveFile : imgArchiveFiles)            // newIMgArchive->GetFileCount()
    {
        const unsigned int uiDFFNameHash = HashString(archiveFile.fileEntry.fileName);
        SDFFDescriptor*    pDFFDescriptor = ideLoader.GetDFFDescriptor(uiDFFNameHash);
        if (!pDFFDescriptor)
        {
            continue;
        }
        totalPossibleCustomOutputFiles++;
    }

    totalPossibleCustomOutputFiles *= 2;            // each DFF will have its own TXD, so multiply the amount by 2
    pMyTempClass->imgArchiveCustomFiles.reserve(totalPossibleCustomOutputFiles);

    size_t modelIndexInIMG = 527;
    //for (auto& archiveFile : imgArchiveFiles)
    //for (modelIndexInIMG = 0; modelIndexInIMG < imgArchiveFiles.size(); modelIndexInIMG++)
    while (modelIndexInIMG < std::min((size_t)528, imgArchiveFiles.size()))            // std::min((size_t)1400, imgArchiveFiles.size())
    {
        CIMGArchiveFile& archiveFile = imgArchiveFiles[modelIndexInIMG];
        SString strFileName = archiveFile.fileEntry.fileName;
        strFileName = strFileName.ToLower();

        pRenderWare->SetCurrentDFFBeingGeneratedFileName(strFileName);

        const char* pFileName = strFileName.c_str();
        memcpy(archiveFile.fileEntry.fileName, pFileName, strlen(pFileName) + 1);

        SString strFileExtension = strFileName.substr(strFileName.find_last_of(".") + 1);

        if (strFileExtension == "dff")
        {
            if (OptimizeDFFFile(modelIndexInIMG, newIMgArchive, pMyTempClass, &archiveFile, ideLoader))
            {
            }
        }

        modelIndexInIMG++;

        // REMOVE LATER
        //break;
        // REMOVE END
    }

    printf("\n[ABOUT TO WRITE] modelIndexInIMG: %u\n\n", modelIndexInIMG);
    for (auto& customArchiveFile : pMyTempClass->imgArchiveCustomFiles)
    {
        imgArchiveFilesOutput.emplace_back(&customArchiveFile);
    }

    for (auto& archiveFile : imgArchiveFiles)
    {
        bool         bFileWithNameExists = false;
        unsigned int archiveFileNameHash = HashString(archiveFile.fileEntry.fileName);
        for (auto& archiveFileOutput : imgArchiveFilesOutput)
        {
            if (archiveFileNameHash == HashString(archiveFileOutput->fileEntry.fileName))
            {
                printf("[Add File Output FAIL] File exists with properties: %s | size: %u\n", archiveFileOutput->fileEntry.fileName,
                       archiveFileOutput->actualFileSize);
                bFileWithNameExists = true;
                break;
            }
        }

        if (!bFileWithNameExists)
        {
            printf("[Add File Output OK] File doesn't exists: %s | size: %u\n", archiveFile.fileEntry.fileName, archiveFile.actualFileSize);
            imgArchiveFilesOutput.push_back(&archiveFile);
        }
    }
    newIMgArchiveOut->WriteEntries(imgArchiveFilesOutput);

    delete pMyTempClass;

    // delete newFile;
    delete newIMgArchive;
    delete newIMgArchiveOut;

    return true;
}
