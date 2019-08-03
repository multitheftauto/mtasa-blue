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
    m_imgFilesWrittenCount = 0;
    m_txdNameUniqueNumericSuffix = 0;
}

void CIMGArchiveOptimizer::GetNextAtlasTxdName(SString& atlasTxdName)
{
    atlasTxdName.Format(m_atlasTxdFileNameFormat.c_str(), m_txdNameUniqueNumericSuffix);
    m_txdNameUniqueNumericSuffix++;
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
        // std::printf("couldn't find dff descriptor for '%s'\n", pDFFArchiveFile->fileEntry.fileName);
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

    DWORD            actualFileSize = GetActualFileSize(clumpSize);
    CIMGArchiveFile* dffArchiveFile = CreateOutputImgArchiveFile(actualFileSize, imgReadWriteOperationSizeInBytes);
    dffArchiveFile->fileEntry = pDFFArchiveFile->fileEntry;
    dffArchiveFile->actualFileSize = actualFileSize;
    dffArchiveFile->fileEntry.usSize = dffArchiveFile->actualFileSize / 2048;
    dffArchiveFile->fileByteBuffer.SetSize(dffArchiveFile->actualFileSize);
    void* pDFFData = dffArchiveFile->fileByteBuffer.GetData();
    m_pRenderWare->WriteDFF(pDFFData, dffArchiveFile->actualFileSize, pClump);

    // Remove this line later
    // m_pRenderWare->WriteDFF(strPathOfGeneratedDff + dffArchiveFile->fileEntry.fileName, pClump);
}

void CIMGArchiveOptimizer::WriteTXD(RwTexDictionary* pTxdDictionary, SDFFDescriptor* pDFFDescriptor)
{
    auto RwTexDictionaryStreamGetSize = (unsigned int(__cdecl*)(RwTexDictionary * dict))0x804930;

    SString strPathOfGeneratedDff = "dffs\\";

    SString strTXDFileName;
    GetNextAtlasTxdName(strTXDFileName);
    const char* pTXDNameWithoutExtension = strTXDFileName.c_str();
    memcpy(pDFFDescriptor->m_pModelObject->txdName, pTXDNameWithoutExtension, strlen(pTXDNameWithoutExtension) + 1);

    strTXDFileName += ".txd";

    const char* pTXDName = strTXDFileName.c_str();

    unsigned int txdSize = RwTexDictionaryStreamGetSize(pTxdDictionary);

    // there's still an issue with size of empty extension headers of 12 bytes for TXD
    txdSize += 24;

    DWORD            actualFileSize = GetActualFileSize(txdSize);
    CIMGArchiveFile* txdArchiveFile = CreateOutputImgArchiveFile(actualFileSize, imgReadWriteOperationSizeInBytes);
    memcpy(txdArchiveFile->fileEntry.fileName, pTXDName, strlen(pTXDName) + 1);
    txdArchiveFile->actualFileSize = actualFileSize;
    txdArchiveFile->fileEntry.usSize = txdArchiveFile->actualFileSize / 2048;
    txdArchiveFile->fileByteBuffer.SetSize(txdArchiveFile->actualFileSize);
    void* pTXDData = txdArchiveFile->fileByteBuffer.GetData();
    m_pRenderWare->WriteTXD(pTXDData, txdArchiveFile->actualFileSize, pTxdDictionary);

    // Remove this line later
    // m_pRenderWare->WriteTXD(strPathOfGeneratedDff + pTXDName, pTxdDictionary);
}

bool CIMGArchiveOptimizer::OptimizeDFFFile(CIMGArchiveFile* pDFFArchiveFile)
{
    /*
    // REMOVE LATER
    //int         modelID = 0;
    const char* pStrDFFName = "a51_jetroom.dff";            //"infernus.dff";
    memcpy(pDFFArchiveFile->fileEntry.fileName, pStrDFFName, strlen(pStrDFFName) + 1);

    RwTexDictionary* pTxdDictionary = m_pRenderWare->ReadTXD("a51.txd", CBuffer(), false);
    // REMOVE END
    */

    bool           bDFFOptimized = false;
    SOptimizedDFF* pOptimizedDFF = m_dffOptimizationInfo.GetOptimizedDFF(pDFFArchiveFile->fileEntry.fileName);
    if (!pOptimizedDFF)
    {
        return bDFFOptimized;
    }

    SDFFDescriptor* pDFFDescriptor = GetDFFDescriptor(pDFFArchiveFile);
    if (!pDFFDescriptor)
    {
        return bDFFOptimized;
    }

    int modelID = pDFFDescriptor->GetModelID();
    /*
    if (pDFFDescriptor && (modelID != 3923))
    {
        return bDFFOptimized;
    }
    */

    unsigned int     defaultTXDSizeInBytes = 0;
    RwTexDictionary* pTxdDictionary = ReadTextureDictionary(pDFFDescriptor, defaultTXDSizeInBytes);
    if (!pTxdDictionary)
    {
        return bDFFOptimized;
    }

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
        if (!modelOptimizer->Optimize())
        {
            printf("\n[HEY] IGNORING %s DFF\n", pDFFArchiveFile->fileEntry.fileName);
        }
        else
        {
            ///*
            RwTexDictionary* pAtlasTxdDictionary = modelOptimizer->CreateTXDAtlas();
            assert(pAtlasTxdDictionary != nullptr);

            // SOptimizedDFF& dffModelOptimizationInfo = m_dffOptimizationInfo.InsertDFF(pDFFArchiveFile->fileEntry.fileName);
            // assert(modelOptimizer->GetModelOptimizationInfo(dffModelOptimizationInfo) != false);

            WriteDFF(pClump, pDFFArchiveFile);
            WriteTXD(pAtlasTxdDictionary, pDFFDescriptor);

            m_pRenderWare->DestroyTXDForcefully(pAtlasTxdDictionary, true);
            bDFFOptimized = true;
            //*/
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
    return bDFFOptimized;
}

static unsigned int gTotalModelsToOptimize = 0;

void CIMGArchiveOptimizer::OptimizeIMGArchiveFiles(std::vector<CIMGArchiveFile>* imgArchiveFiles)
{
    std::vector<CIMGArchiveFile>& imgArchiveFilesByReference = *imgArchiveFiles;
    size_t                        modelIndexInIMG = 0;
    // while (modelIndexInIMG < imgArchiveFiles->size())            // std::min((size_t)1400, imgArchiveFiles.size())
    for (auto& archiveFile : imgArchiveFilesByReference)
    {
        // CIMGArchiveFile& archiveFile = imgArchiveFiles->at(modelIndexInIMG);
        SString strFileName = archiveFile.fileEntry.fileName;
        strFileName = strFileName.ToLower();

        m_pRenderWare->SetCurrentDFFBeingGeneratedFileName(strFileName);

        const char* pFileName = strFileName.c_str();
        memcpy(archiveFile.fileEntry.fileName, pFileName, strlen(pFileName) + 1);

        SString strFileExtension = strFileName.substr(strFileName.find_last_of(".") + 1);
        if (strFileExtension == "dff")
        {
            // REMOVE LATER
            // OptimizeDFFFile(&archiveFile);
            // break;
            // REMOVE END
            ///*
            if (OptimizeDFFFile(&archiveFile))
            {
                continue;
            }
            //*/
        }

        // Any file which is not optmimized, it should be added to the new img file
        DWORD            actualFileSize = archiveFile.actualFileSize;
        CIMGArchiveFile* outputArchiveFile = CreateOutputImgArchiveFile(actualFileSize, imgReadWriteOperationSizeInBytes);
        *outputArchiveFile = archiveFile;

        outputArchiveFile->fileByteBuffer.SetSize(actualFileSize);
        void* pModelData = outputArchiveFile->fileByteBuffer.GetData();
        memcpy(pModelData, archiveFile.GetData(), outputArchiveFile->actualFileSize);

        outputArchiveFile->actualFileOffset = 0;
        CBuffer().Swap(archiveFile.fileByteBuffer);

        modelIndexInIMG++;

        gTotalModelsToOptimize++;
    }
}

CIMGArchiveFile* CIMGArchiveOptimizer::CreateOutputImgArchiveFile(unsigned int actualFileSize, unsigned int imgReadWriteOperationSizeInBytes)
{
    CIMGArchiveFile* pIMGArchiveFile = m_outputIMGArchive.InsertArchiveFile(actualFileSize, imgReadWriteOperationSizeInBytes);
    if (!pIMGArchiveFile)
    {
        m_outputIMGArchive.FlushEntriesToFile();
        m_outputIMGArchive.CloseFile();
        CreateNextOutputIMGFile();
        pIMGArchiveFile = m_outputIMGArchive.InsertArchiveFile(actualFileSize, imgReadWriteOperationSizeInBytes);
    }

    assert(pIMGArchiveFile != nullptr);
    return pIMGArchiveFile;
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
    m_dffOptimizationInfo.AllocateSpace(m_IdeLoader.GetMaximumOptimizableModelsCount());
    assert(m_dffOptimizationInfo.CreateTheFile(m_outputFolder + "\\" + m_OptimizedModelInfosFileName, OPTIMIZE_FILE_READ) != false);
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

void CIMGArchiveOptimizer::CreateNextOutputIMGFile()
{
    SString outputFilePath = m_outputFolder + "\\" + m_outputIMGFileNameFormat;
    SString outputFileName;
    outputFileName.Format(outputFilePath.c_str(), (int)m_imgFilesWrittenCount);
    assert(m_outputIMGArchive.CreateTheFile(outputFileName, IMG_FILE_WRITE) != false);
    m_imgFilesWrittenCount++;
}

bool CIMGArchiveOptimizer::CreateOutputDirectories()
{
    if (CreateDirectory(m_outputFolder.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError())
    {
        if (CreateDirectory(m_ideOutputFolder.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError())
        {
        }
        else
        {
            printf("failed to create directory %s\n", m_ideOutputFolder.c_str());
            return false;
        }
    }
    else
    {
        printf("failed to create directory %s\n", m_outputFolder.c_str());
        return false;
    }
    return true;
}

bool CIMGArchiveOptimizer::AreGta3ImgFileNamesValid()
{
    std::vector<EntryHeader>& archiveEntryHeaders = m_gt3IMgArchive.GetArchiveDirEntries();
    for (auto& entryHeader : archiveEntryHeaders)
    {
        SString fileName = entryHeader.fileName;
        if (fileName.find(m_atlasTxdPrefixWithoutExtension) != SString::npos)
        {
            printf("ERROR: archive File name '%s' in 'models\\gta3.img' contains text '%s'. Change it!\n", entryHeader.fileName,
                   m_atlasTxdPrefixWithoutExtension.c_str());
            return false;
        }
    }
    return true;
}

void CIMGArchiveOptimizer::FreeGeneratorAllocatedMemory()
{
    m_gt3IMgArchive.CloseFile();
    m_outputIMGArchive.CloseFile();

    m_IdeLoader.FreeMemory();
    m_gt3IMgArchive.FreeMemory();
    m_outputIMGArchive.FreeMemory();
    m_dffOptimizationInfo.FreeMemory();

    if (m_pVehicleTxdDictionary)
    {
        m_pRenderWare->TxdForceUnload(0, true, m_pVehicleTxdDictionary);
    }
}

bool CIMGArchiveOptimizer::OnImgGenerateClick(CGUIElement* pElement)
{
    std::printf("Generate button pressed\n");

    if (!m_gt3IMgArchive.CreateTheFile("models\\gta3.img", IMG_FILE_READ))
    {
        std::printf("loading gta3.img failed\n");
        return true;
    }

    unsigned int totalGta3ImgEntries = m_gt3IMgArchive.ReadEntries();
    if (!AreGta3ImgFileNamesValid())
    {
        FreeGeneratorAllocatedMemory();
        return true;
    }

    if (!m_pVehicleTxdDictionary)
    {
        m_pVehicleTxdDictionary = m_pRenderWare->ReadTXD("models\\generic\\vehicle.txd", CBuffer(), false);
        assert(m_pVehicleTxdDictionary != nullptr);
    }

    if (!CreateOutputDirectories())
    {
        FreeGeneratorAllocatedMemory();
        return true;
    }

    m_IdeLoader.LoadIDEFiles();

    m_IdeLoader.AddTXDDFFInfoToMaps(&m_gt3IMgArchive);

    unsigned int totalPossibleAtlasTXDFiles = m_IdeLoader.GetMaximumOptimizableModelsCount();            // every modified DFF will have its own TXD file
    m_outputIMGArchive.AllocateSpace(totalGta3ImgEntries + totalPossibleAtlasTXDFiles);
    CreateNextOutputIMGFile();

    ReadDFFOptimizationInfoFiles();

    int dffModelInfosIndex = 0;

    std::vector<CIMGArchiveFile>* imgArchiveFiles = m_gt3IMgArchive.GetNextImgFiles(imgReadWriteOperationSizeInBytes);

    char fileNameBuffer[100];
    while (imgArchiveFiles)
    {
        // m_dffOptimizationInfo.FreeMemory();
        // m_dffOptimizationInfo.AllocateSpace(totalPossibleCustomOutputFiles);

        OptimizeIMGArchiveFiles(imgArchiveFiles);

        // sprintf(fileNameBuffer, "atlasesInfo%d.gmoi", dffModelInfosIndex);
        // FlushDFFOptimizationDataToFile(outputFolder + "\\" + fileNameBuffer);

        imgArchiveFiles = m_gt3IMgArchive.GetNextImgFiles(imgReadWriteOperationSizeInBytes);
        dffModelInfosIndex++;

        // REMOVE THIS LATER
        // break;
        // REMOVE END
    }

    // Flush the last IMG file if any data is there.
    m_outputIMGArchive.FlushEntriesToFile();

    printf("\n\ngTotalModelsToOptimize: %u\n\n", gTotalModelsToOptimize);

    m_IdeLoader.WriteIDEFiles(m_ideOutputFolder);

    FreeGeneratorAllocatedMemory();
    return true;
}
