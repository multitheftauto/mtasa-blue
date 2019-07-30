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
    m_pVehicleTxdDictionary = nullptr;
    m_pRenderWare = g_pCore->GetGame()->GetRenderWare();
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

SDFFDescriptor* CIMGArchiveOptimizer::GetDFFDescriptor(CIMGArchiveFile* pDFFArchiveFile)
{
    const unsigned int uiDFFNameHash = HashString(pDFFArchiveFile->fileEntry.fileName);
    SDFFDescriptor*    pDFFDescriptor = m_IdeLoader.GetDFFDescriptor(uiDFFNameHash);
    if (!pDFFDescriptor)
    {
        std::printf("couldn't find dff descriptor for '%s'\n", pDFFArchiveFile->fileEntry.fileName);
        return nullptr;
    }

    // Do we really need to optimize vehicle mods? I think no.
    if (IsVehiclMODModel(pDFFDescriptor->GetModelID()))
    {
        return nullptr;
    }

    return pDFFDescriptor;
}

RwTexDictionary* CIMGArchiveOptimizer::ReadTextureDictionary(SDFFDescriptor* pDFFDescriptor, unsigned int& defaultTXDSizeInBytes)
{
    STXDDescriptor*  pTXDDescriptor = pDFFDescriptor->GetTXDDescriptor();
    RwTexDictionary* pTxdDictionary = pTXDDescriptor->GetTextureDictionary();
    if (!pTxdDictionary)
    {
        STXDImgArchiveInfo* pTXDImgArchiveInfo = pTXDDescriptor->GetTXDImgArchiveInfo();
        if (pTXDImgArchiveInfo->usSize == 0)
        {
            return nullptr;
        }
        CIMGArchiveFile* txdArchiveFile = new CIMGArchiveFile();
        if (m_gt3IMgArchive.GetFileByTXDImgArchiveInfo(pTXDImgArchiveInfo, *txdArchiveFile))
        {
            defaultTXDSizeInBytes = txdArchiveFile->fileByteBuffer.GetSize();
            pTxdDictionary = m_pRenderWare->ReadTXD(nullptr, txdArchiveFile->fileByteBuffer, false);
            if (IsVehicleModel(pDFFDescriptor->GetModelID()))
            {
                m_pRenderWare->CopyTexturesFromDictionary(pTxdDictionary, m_pVehicleTxdDictionary);
            }
        }
        else
        {
            printf("Failed to read TXD for dff\n");
        }
        delete txdArchiveFile;
    }

    return pTxdDictionary;
}

void CIMGArchiveOptimizer::WriteDFF(RpClump* pClump, CIMGArchiveFile* pDFFArchiveFile)
{
    auto RpClumpStreamGetSize = (unsigned int(__cdecl*)(RpClump*))0x74A5E0;

    SString strPathOfGeneratedDff = "dffs\\";

    unsigned int clumpSize = RpClumpStreamGetSize(pClump);

    // there's still an issue with size of empty extension headers of 12 bytes for clump
    clumpSize += 24;

    CIMGArchiveFile& dffArchiveFile = m_ImgArchiveCustomFiles.emplace_back();
    dffArchiveFile.fileEntry = pDFFArchiveFile->fileEntry;
    dffArchiveFile.actualFileSize = GetActualFileSize(clumpSize);
    dffArchiveFile.fileEntry.usSize = dffArchiveFile.actualFileSize / 2048;
    dffArchiveFile.fileByteBuffer.SetSize(dffArchiveFile.actualFileSize);
    void* pDFFData = dffArchiveFile.fileByteBuffer.GetData();
    // m_pRenderWare->WriteDFF(pDFFData, dffArchiveFile.actualFileSize, pClump);

    // Remove this line later
    m_pRenderWare->WriteDFF(strPathOfGeneratedDff + dffArchiveFile.fileEntry.fileName, pClump);
}

void CIMGArchiveOptimizer::WriteTXD(RwTexDictionary* pTxdDictionary, CIMGArchiveFile* pDFFArchiveFile)
{
    auto RwTexDictionaryStreamGetSize = (unsigned int(__cdecl*)(RwTexDictionary * dict))0x804930;

    SString strPathOfGeneratedDff = "dffs\\";

    std::string fileName = pDFFArchiveFile->fileEntry.fileName;
    size_t      lastindex = fileName.find_last_of(".");
    std::string fileNameWithoutExtension = fileName.substr(0, lastindex);
    std::string strTXDFileName = fileNameWithoutExtension + ".txd";
    const char* pTXDName = strTXDFileName.c_str();

    unsigned int txdSize = RwTexDictionaryStreamGetSize(pTxdDictionary);

    // there's still an issue with size of empty extension headers of 12 bytes for TXD
    txdSize += 24;

    CIMGArchiveFile& txdArchiveFile = m_ImgArchiveCustomFiles.emplace_back();
    memcpy(txdArchiveFile.fileEntry.fileName, pTXDName, strlen(pTXDName) + 1);

    txdArchiveFile.actualFileSize = GetActualFileSize(txdSize);
    txdArchiveFile.fileEntry.usSize = txdArchiveFile.actualFileSize / 2048;
    txdArchiveFile.fileByteBuffer.SetSize(txdArchiveFile.actualFileSize);
    void* pTXDData = txdArchiveFile.fileByteBuffer.GetData();
    // m_pRenderWare->WriteTXD(pTXDData, txdArchiveFile.actualFileSize, pTxdDictionary);
    // Remove this line later
    m_pRenderWare->WriteTXD(strPathOfGeneratedDff + pTXDName, pTxdDictionary);
}

void CIMGArchiveOptimizer::OptimizeDFFFile(CIMGArchiveFile* pDFFArchiveFile)
{
    /*
    // REMOVE LATER
    //int         modelID = 0;
    const char* pStrDFFName = "a51_jetroom.dff";            //"infernus.dff";
    memcpy(pDFFArchiveFile->fileEntry.fileName, pStrDFFName, strlen(pStrDFFName) + 1);

    RwTexDictionary* pTxdDictionary = m_pRenderWare->ReadTXD("a51.txd", CBuffer(), false);
    // REMOVE END
    */

    SOptimizedDFF* pOptimizedDFF = m_dffOptimizationInfo.GetOptimizedDFF(pDFFArchiveFile->fileEntry.fileName);
    if (!pOptimizedDFF)
    {
        return;
    }

    SDFFDescriptor*  pDFFDescriptor = GetDFFDescriptor(pDFFArchiveFile);
    /*
    int             modelID = 0;
    if (pDFFDescriptor && (modelID = pDFFDescriptor->GetModelID()), (modelID != 16682))
    {
        return;
    }*/

    unsigned int     defaultTXDSizeInBytes = 0;
    RwTexDictionary* pTxdDictionary = pDFFDescriptor ? ReadTextureDictionary(pDFFDescriptor, defaultTXDSizeInBytes) : nullptr;
    if (!pTxdDictionary)
    {
        return;
    }

    int modelID = pDFFDescriptor->GetModelID();
    m_pRenderWare->SetCurrentDFFWriteModelID(modelID);
    m_pRenderWare->SetCurrentReadDFFWithoutReplacingCOL(true);

    printf("\nabout to read: %s | size: %u \n\n", (char*)pDFFArchiveFile->fileEntry.fileName, pDFFArchiveFile->actualFileSize);

    bool bLoadCollision = IsVehicleModel(modelID);
    // RpClump* pClump = m_pRenderWare->ReadDFF(pDFFArchiveFile->fileEntry.fileName, CBuffer(), modelID, bLoadCollision, pTxdDictionary);
    RwBuffer buffer = {pDFFArchiveFile->GetData(), pDFFArchiveFile->actualFileSize};
    RpClump* pClump = m_pRenderWare->ReadDFF(pDFFArchiveFile->fileEntry.fileName, buffer, modelID, bLoadCollision, pTxdDictionary);

    m_pRenderWare->SetCurrentReadDFFWithoutReplacingCOL(false);

    if (pClump)
    {
        // This should be set to false when generating atlases
        bool               bDontLoadTextures = false;
        C3DModelOptimizer* modelOptimizer = new C3DModelOptimizer(pClump, defaultTXDSizeInBytes, bDontLoadTextures);
        modelOptimizer->SetOptimizationInfo(pOptimizedDFF);
        // modelOptimizer->OutputClumpAsOBJ();
        if (modelOptimizer->Optimize())
        {
            ///*
            // RwTexDictionary* pAtlasTxdDictionary = modelOptimizer->CreateTXDAtlas();
            // assert(pAtlasTxdDictionary != nullptr);
            //*/

            //SOptimizedDFF& dffModelOptimizationInfo = m_dffOptimizationInfo.InsertDFF(pDFFArchiveFile->fileEntry.fileName);
            //assert(modelOptimizer->GetModelOptimizationInfo(dffModelOptimizationInfo) != false);

            // WriteDFF(pClump, pDFFArchiveFile);
            // WriteTXD(pAtlasTxdDictionary, pDFFArchiveFile);

            // m_pRenderWare->TxdForceUnload(0, true, pAtlasTxdDictionary);
        }
        delete modelOptimizer;
        modelOptimizer = nullptr;
    }
    else
    {
        std::printf("failed to read %s\n", pDFFArchiveFile->fileEntry.fileName);
    }

    if (pClump && bLoadCollision)
    {
        m_pRenderWare->DeleteReadDFFCollisionModel();
    }

    m_pRenderWare->DestroyDFF(pClump);
    m_pRenderWare->TxdForceUnload(0, true, pTxdDictionary);
}

static unsigned int gTotalModelsToOptimize = 0;

void CIMGArchiveOptimizer::OptimizeIMGArchiveFiles(std::vector<CIMGArchiveFile>* imgArchiveFiles)
{
    size_t modelIndexInIMG = 0;
    while (modelIndexInIMG < imgArchiveFiles->size())            // std::min((size_t)1400, imgArchiveFiles.size())
    {
        CIMGArchiveFile& archiveFile = imgArchiveFiles->at(modelIndexInIMG);
        SString          strFileName = archiveFile.fileEntry.fileName;
        strFileName = strFileName.ToLower();

        m_pRenderWare->SetCurrentDFFBeingGeneratedFileName(strFileName);

        const char* pFileName = strFileName.c_str();
        memcpy(archiveFile.fileEntry.fileName, pFileName, strlen(pFileName) + 1);

        SString strFileExtension = strFileName.substr(strFileName.find_last_of(".") + 1);
        if (strFileExtension == "dff")
        {
            OptimizeDFFFile(&archiveFile);
            // break;
        }

        modelIndexInIMG++;

        gTotalModelsToOptimize++;
    }
}

unsigned int CIMGArchiveOptimizer::GetTotalPossibleCustomOutputFiles(std::vector<CIMGArchiveFile>* imgArchiveFiles)
{
    unsigned int totalPossibleCustomOutputFiles = 0;
    for (size_t archiveFileIndex = 0; archiveFileIndex < imgArchiveFiles->size(); archiveFileIndex++)
    {
        CIMGArchiveFile&   archiveFile = imgArchiveFiles->at(archiveFileIndex);
        const unsigned int uiDFFNameHash = HashString(archiveFile.fileEntry.fileName);
        SDFFDescriptor*    pDFFDescriptor = m_IdeLoader.GetDFFDescriptor(uiDFFNameHash);
        if (!pDFFDescriptor)
        {
            continue;
        }
        totalPossibleCustomOutputFiles++;
    }
    return totalPossibleCustomOutputFiles;
}

void CIMGArchiveOptimizer::InsertImgArchiveFilesToOutputContainer(std::vector<CIMGArchiveFile>*  imgArchiveFiles,
                                                                  std::vector<CIMGArchiveFile*>& imgArchiveFilesOutput)
{
    for (size_t archiveFileIndex = 0; archiveFileIndex < imgArchiveFiles->size(); archiveFileIndex++)
    {
        CIMGArchiveFile& archiveFile = imgArchiveFiles->at(archiveFileIndex);
        bool             bFileWithNameExists = false;
        unsigned int     archiveFileNameHash = HashString(archiveFile.fileEntry.fileName);
        for (auto& archiveFileOutput : imgArchiveFilesOutput)
        {
            if (archiveFileNameHash == HashString(archiveFileOutput->fileEntry.fileName))
            {
                // printf("[Add File Output FAIL] File exists with properties: %s | size: %u\n", archiveFileOutput->fileEntry.fileName,
                //       archiveFileOutput->actualFileSize);
                bFileWithNameExists = true;
                break;
            }
        }

        if (!bFileWithNameExists)
        {
            // printf("[Add File Output OK] File doesn't exists: %s | size: %u\n", archiveFile.fileEntry.fileName, archiveFile.actualFileSize);
            imgArchiveFilesOutput.push_back(&archiveFile);
        }
    }
}

void CIMGArchiveOptimizer::FlushDFFOptimizationDataToFile(const char* filePath)
{
    m_dffOptimizationInfo.CreateTheFile(filePath, OPTIMIZE_FILE_WRITE);
    m_dffOptimizationInfo.WriteHeader();
    m_dffOptimizationInfo.WriteOptimizedDFFs();
    m_dffOptimizationInfo.CloseFile();
}

void CIMGArchiveOptimizer::ReadDFFOptimizationInfoFiles()
{
    SString outputFolder = "OptimizedFiles";

    m_dffOptimizationInfo.AllocateSpace(m_IdeLoader.GetMaximumOptimizableModelsCount());
    assert(m_dffOptimizationInfo.CreateTheFile(outputFolder + "\\completeWithoutVehicleModsModels.gmoi", OPTIMIZE_FILE_READ) != false);
    if (!m_dffOptimizationInfo.ReadOptimizedDFFs())
    {
        printf("ReadOptimizedDFFs failed\n");
    }
    m_dffOptimizationInfo.CloseFile();
    /*
    int totalFiles = 46;
    for (int dffModelInfosIndex = 0; dffModelInfosIndex < totalFiles; dffModelInfosIndex++)
    {
        char fileNameBuffer[100];
        sprintf(fileNameBuffer, "atlasesInfo%d.gmoi", dffModelInfosIndex);
        printf("reading %s\n", fileNameBuffer);
        assert(m_dffOptimizationInfo.CreateTheFile(outputFolder + "\\" + fileNameBuffer, OPTIMIZE_FILE_READ) != false);
        if (!m_dffOptimizationInfo.ReadOptimizedDFFs())
        {
            printf("ReadOptimizedDFFs failed\n");
        }
        m_dffOptimizationInfo.CloseFile();
    }

    CDFFModelOptimizationInfo theDffOptimizationInfo;
    theDffOptimizationInfo.AllocateSpace(20);
    assert(theDffOptimizationInfo.CreateTheFile(outputFolder + "\\atlasesInfoRemaining.gmoi", OPTIMIZE_FILE_READ) != false);
    if (!theDffOptimizationInfo.ReadOptimizedDFFs())
    {
        printf("ReadOptimizedDFFs failed\n");
    }
    theDffOptimizationInfo.CloseFile();

    for (unsigned int i = 0; i < theDffOptimizationInfo.GetOptimizedDFFCount(); i++)
    {
        SOptimizedDFF& optimizedDFF = theDffOptimizationInfo.GetOptimizedDFF(i);
        if (m_dffOptimizationInfo.GetOptimizedDFFByHash(optimizedDFF.GetDFFNameHash()))
        {
            printf("skipping a remaining DFF optimization info with texture count: %u\n", optimizedDFF.GetTotalOptimizedTextures());
            continue;
        }

        SOptimizedDFF& theNewOptimizedDFF = m_dffOptimizationInfo.InsertDFF(optimizedDFF.GetDFFNameHash(), optimizedDFF.GetTXDNameHash());

        for (unsigned int textureIndex = 0; textureIndex < theNewOptimizedDFF.GetTotalOptimizedTextures(); textureIndex++)
        {
            SOptimizedTexture& optimizedTexture = theNewOptimizedDFF.GetOptimizedTexture(textureIndex);
            theNewOptimizedDFF.Addtexture(optimizedTexture.m_textureNameHash, optimizedTexture.m_textureSizeWithinAtlasInBytes);
        }
    }
    */
}

bool CIMGArchiveOptimizer::OnImgGenerateClick(CGUIElement* pElement)
{
    std::printf("Generate button pressed\n");

    if (!m_pVehicleTxdDictionary)
    {
        m_pVehicleTxdDictionary = m_pRenderWare->ReadTXD("models\\generic\\vehicle.txd", CBuffer(), false);
        assert(m_pVehicleTxdDictionary != nullptr);
    }

    SString outputFolder = "OptimizedFiles";
    if (CreateDirectory(outputFolder.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError())
    {
    }

    m_IdeLoader.LoadIDEFiles();

    if (!m_gt3IMgArchive.LoadIMGFile("models\\gta3.img", IMG_FILE_READ))
    {
        std::printf("loading gta3.img failed\n");
        return true;
    }

    m_gt3IMgArchive.ReadEntries();

    m_IdeLoader.AddTXDDFFInfoToMaps(&m_gt3IMgArchive);

    ReadDFFOptimizationInfoFiles();

    CIMGArchive* newIMgArchiveOut = new CIMGArchive("proxy_test_gta3.img", IMG_FILE_WRITE);

    int dffModelInfosIndex = 0;

    std::vector<CIMGArchiveFile>* imgArchiveFiles = m_gt3IMgArchive.GetNextImgFiles(imgReadWriteOperationSizeInBytes);

    char fileNameBuffer[100];
    while (imgArchiveFiles)
    {
        std::vector<CIMGArchiveFile*> imgArchiveFilesOutput;

        unsigned int totalPossibleCustomOutputFiles = GetTotalPossibleCustomOutputFiles(imgArchiveFiles);

        // m_dffOptimizationInfo.FreeMemory();
        // m_dffOptimizationInfo.AllocateSpace(totalPossibleCustomOutputFiles);

        // each DFF will have its own TXD, so multiply the amount by 2
        totalPossibleCustomOutputFiles *= 2;

        std::vector<CIMGArchiveFile>().swap(m_ImgArchiveCustomFiles);
        m_ImgArchiveCustomFiles.reserve(totalPossibleCustomOutputFiles);

        OptimizeIMGArchiveFiles(imgArchiveFiles);

        /*
        for (auto& customArchiveFile : m_ImgArchiveCustomFiles)
        {
            imgArchiveFilesOutput.emplace_back(&customArchiveFile);
        }

        InsertImgArchiveFilesToOutputContainer(imgArchiveFiles, imgArchiveFilesOutput);
        newIMgArchiveOut->WriteEntries(imgArchiveFilesOutput);
        */

        // sprintf(fileNameBuffer, "atlasesInfo%d.gmoi", dffModelInfosIndex);
        // FlushDFFOptimizationDataToFile(outputFolder + "\\" + fileNameBuffer);

        imgArchiveFiles = m_gt3IMgArchive.GetNextImgFiles(imgReadWriteOperationSizeInBytes);
        dffModelInfosIndex++;

        // REMOVE THIS LATER
        // break;
        // REMOVE END
    }

    printf("\n\ngTotalModelsToOptimize: %u\n\n", gTotalModelsToOptimize);
    delete newIMgArchiveOut;

    // Free memory
    m_IdeLoader.FreeMemory();
    m_gt3IMgArchive.FreeArchiveDirEntries();
    m_dffOptimizationInfo.FreeMemory();

    m_gt3IMgArchive.closeFile();
    m_pRenderWare->TxdForceUnload(0, true, m_pVehicleTxdDictionary);
    return true;
}
