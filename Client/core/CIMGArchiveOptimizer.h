#pragma once
#include "CIDELoader.h"
#include "CDFFModelOptimizationInfo.h"

class CIMGArchiveOptimizer
{
public:
    CIMGArchiveOptimizer();
    void CreateGUI();
    void SetVisible(bool bVisible);

    SDFFDescriptor* GetDFFDescriptor(CIMGArchiveFile* pDFFArchiveFile);
    RwTexDictionary* ReadTextureDictionary(SDFFDescriptor* pDFFDescriptor, unsigned int& defaultTXDSizeInBytes);
    void         WriteDFF(RpClump* pClump, CIMGArchiveFile* pDFFArchiveFile);
    void         WriteTXD(RwTexDictionary* pTxdDictionary, SDFFDescriptor* pDFFDescriptor);
    bool         OptimizeDFFFile(CIMGArchiveFile* pDFFArchiveFile);
    void         OptimizeIMGArchiveFiles(std::vector<CIMGArchiveFile>* imgArchiveFiles);
    CIMGArchiveFile* CreateOutputImgArchiveFile(unsigned int actualFileSize, unsigned int imgReadWriteOperationSizeInBytes);
    void         FlushDFFOptimizationDataToFile(const char* filePath);
    void         ReadDFFOptimizationInfoFiles();
    void         CreateNextOutputIMGFile();
    bool         CreateOutputDirectories();

private:

    bool OnImgGenerateClick(CGUIElement* pElement);

    CRenderWare*  m_pRenderWare; 
    CGUIElement * m_pWindow;
    CGUILabel*    m_pImgSpaceRequiredLabel;
    CIDELoader    m_IdeLoader;
    std::vector<CIMGArchiveFile> m_ImgArchiveCustomFiles;
    CIMGArchive                  m_gt3IMgArchive;
    CIMGArchive                  m_outputIMGArchive;
    std::vector<SString>         m_outputIMgFilePaths;
    CDFFModelOptimizationInfo    m_dffOptimizationInfo;

    RwTexDictionary* m_pVehicleTxdDictionary;

    // n megabytes * 1024 * 1024
    const unsigned int imgReadWriteOperationSizeInBytes = 20 * 1024 * 1024;
    size_t             m_imgFilesWrittenCount;
    const SString      m_outputIMGFileFormat = "proxy_gta3%d.img";
    const SString      m_atlasTxdSuffixWithoutExtension = "_atlas";
    const SString      m_outputFolder = "OptimizedFiles";
    const SString      m_ideOutputFolder = m_outputFolder + "\\IDE";
    const SString      m_OptimizedModelInfosFileName ="completeWithoutVehicleModsModels.gmoi";
};
