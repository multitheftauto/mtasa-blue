#pragma once
#include "CIDELoader.h"
#include "CDFFModelOptimizationInfo.h"

class CIMGArchiveOptimizer
{
public:
    CIMGArchiveOptimizer();
    void CreateGUI();
    void SetVisible(bool bVisible);

    void            GetNextAtlasTxdName(SString& atlasTxdName);

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
    bool             AreGta3ImgFileNamesValid();
    void             FreeGeneratorAllocatedMemory();


private:
    bool OnImgGenerateClick(CGUIElement* pElement);

    CRenderWare*  m_pRenderWare; 
    CGUIElement * m_pWindow;
    CGUILabel*    m_pImgSpaceRequiredLabel;
    CIDELoader    m_IdeLoader;
    CIMGArchive                  m_gt3IMgArchive;
    CIMGArchive                  m_outputIMGArchive;
    CDFFModelOptimizationInfo    m_dffOptimizationInfo;

    RwTexDictionary* m_pVehicleTxdDictionary;

    // n megabytes * 1024 * 1024
    const unsigned int imgReadWriteOperationSizeInBytes = 20 * 1024 * 1024;

    // Just to avoid having TXD files with the same names
    size_t             m_txdNameUniqueNumericSuffix;
    size_t             m_imgFilesWrittenCount;
    const SString      m_outputIMGFileNameFormat = "proxy_gta3_%d.img";
    const SString      m_atlasTxdPrefixWithoutExtension = "atlas_";
    const SString      m_atlasTxdFileNameFormat = m_atlasTxdPrefixWithoutExtension + "%u";
    const SString      m_outputFolder = "OptimizedFiles";
    const SString      m_ideOutputFolder = m_outputFolder + "\\IDE";
    const SString      m_OptimizedModelInfosFileName ="completeWithoutVehicleModsModels.gmoi";
};
