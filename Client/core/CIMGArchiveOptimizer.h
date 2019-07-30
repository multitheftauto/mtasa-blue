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
    void         WriteTXD(RwTexDictionary* pTxdDictionary, CIMGArchiveFile* pDFFArchiveFile);
    void         OptimizeDFFFile(CIMGArchiveFile* pDFFArchiveFile);
    void         OptimizeIMGArchiveFiles(std::vector<CIMGArchiveFile>* imgArchiveFiles);
    void         InsertImgArchiveFilesToOutputContainer(std::vector<CIMGArchiveFile>* imgArchiveFiles, std::vector<CIMGArchiveFile*>& imgArchiveFilesOutput);
    unsigned int GetTotalPossibleCustomOutputFiles(std::vector<CIMGArchiveFile>* imgArchiveFiles);
    void         FlushDFFOptimizationDataToFile(const char* filePath);
    void         ReadDFFOptimizationInfoFiles();

private:

    bool OnImgGenerateClick(CGUIElement* pElement);

    CRenderWare*  m_pRenderWare; 
    CGUIElement * m_pWindow;
    CGUILabel*    m_pImgSpaceRequiredLabel;
    CIDELoader    m_IdeLoader;
    std::vector<CIMGArchiveFile> m_ImgArchiveCustomFiles;
    CIMGArchive                  m_gt3IMgArchive;
    std::vector<SString>         m_outputIMgFilePaths;
    CDFFModelOptimizationInfo    m_dffOptimizationInfo;

    RwTexDictionary* m_pVehicleTxdDictionary;

    // n megabytes * 1024 * 1024
    const unsigned int imgReadWriteOperationSizeInBytes = 20 * 1024 * 1024;
};
