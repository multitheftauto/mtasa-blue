#include "StdInc.h"
#include "CIMGArchive.h"
#include "CIDELoader.h"

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
    CGUI* pManager = g_pCore->GetGUI();

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

    SString strImgSpaceRequiredText = "A custom optimized gta3.img file needs to be generated in your models folder. \
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


/*
CRenderWare* pRenderWare = g_pCore->GetGame()->GetRenderWare();
RwTexDictionary* pTxdDictionary = pRenderWare->ReadTXD("C:\\Users\\danish\\Desktop\\gtaTxdDff\\Infernus\\infernus.txd", CBuffer());
pRenderWare->WriteTXD("C:\\Users\\danish\\Desktop\\gtaTxdDff\\Infernus\\infernus2.txd", pTxdDictionary);

std::vector <CTextureInfo> vecTexturesInfo;

SString theDirPath = "C:\\Program Files (x86)\\NVIDIA Corporation\\Texture Atlas Tools\\DEMOS\\Direct3D9\\bin\\release\\";
vecTexturesInfo.emplace_back(CTextureInfo(theDirPath + "grass.png", 256, 256)); //grass.png
vecTexturesInfo.emplace_back(CTextureInfo(theDirPath + "nvidia_cloth.png", 512, 512)); //nvidia_cloth.png
vecTexturesInfo.emplace_back(CTextureInfo(theDirPath + "1d_debug.png", 128, 128)); //1d_debug.png
vecTexturesInfo.emplace_back(CTextureInfo(theDirPath + "shine.png", 64, 64)); //shine.png
vecTexturesInfo.emplace_back(CTextureInfo(theDirPath + "mtasa.png", 800, 600)); //mtasa.png

auto pTextureInfo = g_pCore->CreateTextureAtlas(vecTexturesInfo);
*/


const std::array<SString, 214> arrDffFilesToIgnoreNames = {
    "admiral.dff",
    "alpha.dff",
    "ambulan.dff",
    "androm.dff",
    "artict1.dff",
    "artict2.dff",
    "artict3.dff",
    "at400.dff",
    "bagboxa.dff",
    "bagboxb.dff",
    "baggage.dff",
    "bandito.dff",
    "banshee.dff",
    "barracks.dff",
    "beagle.dff",
    "benson.dff",
    "bf400.dff",
    "bfinject.dff",
    "bike.dff",
    "blade.dff",
    "blistac.dff",
    "bloodra.dff",
    "bloodrb.dff",
    "bmx.dff",
    "bobcat.dff",
    "boxburg.dff",
    "boxville.dff",
    "bravura.dff",
    "broadway.dff",
    "buccanee.dff",
    "buffalo.dff",
    "bullet.dff",
    "burrito.dff",
    "bus.dff",
    "cabbie.dff",
    "caddy.dff",
    "cadrona.dff",
    "camper.dff",
    "cargobob.dff",
    "cement.dff",
    "cheetah.dff",
    "clover.dff",
    "club.dff",
    "coach.dff",
    "coastg.dff",
    "combine.dff",
    "comet.dff",
    "copbike.dff",
    "copcarla.dff",
    "copcarru.dff",
    "copcarsf.dff",
    "copcarvg.dff",
    "cropdust.dff",
    "dft30.dff",
    "dinghy.dff",
    "dodo.dff",
    "dozer.dff",
    "dumper.dff",
    "duneride.dff",
    "elegant.dff",
    "elegy.dff",
    "emperor.dff",
    "enforcer.dff",
    "esperant.dff",
    "euros.dff",
    "faggio.dff",
    "farmtr1.dff",
    "fbiranch.dff",
    "fbitruck.dff",
    "fcr900.dff",
    "feltzer.dff",
    "firela.dff",
    "firetruk.dff",
    "flash.dff",
    "flatbed.dff",
    "forklift.dff",
    "fortune.dff",
    "freeway.dff",
    "freibox.dff",
    "freiflat.dff",
    "freight.dff",
    "glendale.dff",
    "glenshit.dff",
    "greenwoo.dff",
    "hermes.dff",
    "hotdog.dff",
    "hotknife.dff",
    "hotrina.dff",
    "hotrinb.dff",
    "hotring.dff",
    "hunter.dff",
    "huntley.dff",
    "hustler.dff",
    "hydra.dff",
    "infernus.dff",
    "intruder.dff",
    "jester.dff",
    "jetmax.dff",
    "journey.dff",
    "kart.dff",
    "landstal.dff",
    "launch.dff",
    "leviathn.dff",
    "linerun.dff",
    "majestic.dff",
    "manana.dff",
    "marquis.dff",
    "maverick.dff",
    "merit.dff",
    "mesa.dff",
    "monster.dff",
    "monstera.dff",
    "monsterb.dff",
    "moonbeam.dff",
    "mower.dff",
    "mrwhoop.dff",
    "mtbike.dff",
    "mule.dff",
    "nebula.dff",
    "nevada.dff",
    "newsvan.dff",
    "nrg500.dff",
    "oceanic.dff",
    "packer.dff",
    "patriot.dff",
    "pcj600.dff",
    "peren.dff",
    "petro.dff",
    "petrotr.dff",
    "phoenix.dff",
    "picador.dff",
    "pizzaboy.dff",
    "polmav.dff",
    "pony.dff",
    "predator.dff",
    "premier.dff",
    "previon.dff",
    "primo.dff",
    "quad.dff",
    "raindanc.dff",
    "rancher.dff",
    "rcbandit.dff",
    "rcbaron.dff",
    "rccam.dff",
    "rcgoblin.dff",
    "rcraider.dff",
    "rctiger.dff",
    "rdtrain.dff",
    "rdtraint.dff",
    "reefer.dff",
    "regina.dff",
    "remingtn.dff",
    "rhino.dff",
    "rnchlure.dff",
    "romero.dff",
    "rumpo.dff",
    "rustler.dff",
    "sabre.dff",
    "sadler.dff",
    "sadlshit.dff",
    "sanchez.dff",
    "sandking.dff",
    "savanna.dff",
    "seaspar.dff",
    "securica.dff",
    "sentinel.dff",
    "shamal.dff",
    "skimmer.dff",
    "slamvan.dff",
    "solair.dff",
    "sparrow.dff",
    "speeder.dff",
    "squalo.dff",
    "stafford.dff",
    "stallion.dff",
    "stratum.dff",
    "streak.dff",
    "streakc.dff",
    "stretch.dff",
    "stunt.dff",
    "sultan.dff",
    "sunrise.dff",
    "supergt.dff",
    "swatvan.dff",
    "sweeper.dff",
    "tahoma.dff",
    "tampa.dff",
    "taxi.dff",
    "topfun.dff",
    "tornado.dff",
    "towtruck.dff",
    "tractor.dff",
    "tram.dff",
    "trash.dff",
    "tropic.dff",
    "tug.dff",
    "tugstair.dff",
    "turismo.dff",
    "uranus.dff",
    "utility.dff",
    "utiltr1.dff",
    "vcnmav.dff",
    "vincent.dff",
    "virgo.dff",
    "voodoo.dff",
    "vortex.dff",
    "walton.dff",
    "washing.dff",
    "wayfarer.dff",
    "willard.dff",
    "windsor.dff",
    "yankee.dff",
    "yosemite.dff",
    "zr350.dff" };

std::set <unsigned int> setOfIgnoredDffNameHashes;

void AddIgnoredDffHashesToSet()
{
    for (auto& dffName : arrDffFilesToIgnoreNames)
    {
        setOfIgnoredDffNameHashes.insert(HashString(dffName.ToLower()));
    }
}

DWORD GetActualFileSize(DWORD blockSize)
{
    const uint64_t IMG_BLOCK_SIZE = 2048;
    blockSize = (blockSize + IMG_BLOCK_SIZE - 1) & ~(IMG_BLOCK_SIZE - 1); // Round up to block size
    return blockSize;
}

void OptimizeTXDFile(CIMGArchiveFile* newFile)
{
    CRenderWare* pRenderWare = g_pCore->GetGame()->GetRenderWare();
    auto RwTexDictionaryStreamGetSize = (unsigned int(__cdecl*)(RwTexDictionary *dict))0x804930;

    RwTexDictionary* pTxdDictionary = pRenderWare->ReadTXD(nullptr, newFile->fileByteBuffer, false);
    if (pTxdDictionary)
    {
       // std::printf("txd loaded\n");
        unsigned int txdSize = RwTexDictionaryStreamGetSize(pTxdDictionary);

        //free the memory
        CBuffer().Swap(newFile->fileByteBuffer);

        newFile->actualFileSize = GetActualFileSize(txdSize);
        newFile->fileEntry->usSize = newFile->actualFileSize / 2048;
        std::printf("txd file: %s  | new txd size: %d | actualFileSize: %d | capacity: %d\n",
            newFile->fileEntry->fileName, txdSize, (int)newFile->actualFileSize, newFile->fileByteBuffer.GetCapacity());

        newFile->fileByteBuffer.SetSize(newFile->actualFileSize);
        pRenderWare->WriteTXD(newFile->fileByteBuffer.GetData(), newFile->actualFileSize, pTxdDictionary);

        pRenderWare->DestroyTXD(pTxdDictionary);
    }
    else
    {
        std::printf("failed to read %s\n", newFile->fileEntry->fileName);
    }
}

extern SString g_CurrentDFFBeingGeneratedFileName;

void OptimizeDFFFile(CIMGArchive* pIMgArchive, CIMGArchiveFile* newFile, CIDELoader& ideLoader)
{
    CRenderWare* pRenderWare = g_pCore->GetGame()->GetRenderWare();
    auto RpClumpStreamGetSize = (unsigned int(__cdecl*)(RpClump *))0x74A5E0;

    /*
    // REMOVE THIS LATER
    char theDFFName[] = "tcelawcuntunb.dff";
    memcpy(newFile->fileEntry->fileName, theDFFName, strlen(theDFFName) + 1);
    // REMOVE END
    */

    const unsigned int uiDFFNameHash = HashString(newFile->fileEntry->fileName);
    auto it = setOfIgnoredDffNameHashes.find(uiDFFNameHash);
    if (it != setOfIgnoredDffNameHashes.end())
    {
        return; // ignore it
    }

    STXDDescriptor* pTXDDescriptor = ideLoader.GetTXDDescriptorFromDFFName(uiDFFNameHash);
    if (!pTXDDescriptor)
    {
        return;
    }

    //std::printf("GetTXDDescriptorFromDFFName failed\n");
    //RwTexDictionary* pTxdDictionary = nullptr;
    RwTexDictionary* pTxdDictionary = pTXDDescriptor->GetTextureDictionary();
    if (!pTxdDictionary)
    {
        STXDImgArchiveInfo* pTXDImgArchiveInfo = pTXDDescriptor->GetTXDImgArchiveInfo();
        CIMGArchiveFile* pTXDArchiveFile = pIMgArchive->GetFileByTXDImgArchiveInfo(pTXDImgArchiveInfo);
        pTxdDictionary = pRenderWare->ReadTXD(nullptr, pTXDArchiveFile->fileByteBuffer, false);
        pTXDDescriptor->SetTextureDictionary(pTxdDictionary);
        delete pTXDArchiveFile;
          
    }


    //RpClump* pClump = pRenderWare->ReadDFF(theDFFName, CBuffer(), 0, false, pTxdDictionary);
    RpClump* pClump = pRenderWare->ReadDFF(newFile->fileEntry->fileName, newFile->fileByteBuffer, 0, false, pTxdDictionary);
    if (pClump)
    {
        unsigned int clumpSize = RpClumpStreamGetSize(pClump);

        // there's still an issue with size of empty extension headers of 12 btytes for clump
        clumpSize += 24;

        //free the memory
        CBuffer().Swap(newFile->fileByteBuffer);

        newFile->actualFileSize = GetActualFileSize(clumpSize);
        newFile->fileEntry->usSize = newFile->actualFileSize / 2048;
        std::printf("dff file: %s  | new dff size: %u | actualFileSize: %d | capacity: %d\n",
            newFile->fileEntry->fileName, clumpSize, (int)newFile->actualFileSize, newFile->fileByteBuffer.GetCapacity());

        newFile->fileByteBuffer.SetSize(newFile->actualFileSize);
        void* pData = newFile->fileByteBuffer.GetData();
        pRenderWare->WriteDFF(pData, newFile->actualFileSize, pClump);

        //SString strPathOfGeneratedDff = "dffs\\";
        //pRenderWare->WriteDFF(strPathOfGeneratedDff + newFile->fileEntry->fileName, pClump);

        pRenderWare->DestroyDFF(pClump);
        if (pTXDDescriptor)
        {
            pTXDDescriptor->RemoveDFFNameFromSet(uiDFFNameHash);
        }
    }
    else
    {
        std::printf("failed to read %s\n", newFile->fileEntry->fileName);
    }
}

bool CIMGArchiveOptimizer::OnImgGenerateClick(CGUIElement* pElement)
{
    std::printf("Generate button pressed\n");

    CRenderWare* pRenderWare = g_pCore->GetGame()->GetRenderWare();

    CIDELoader ideLoader;
    
    CIMGArchive* newIMgArchive = new CIMGArchive("models\\gta3.img", IMG_FILE_READ);
    newIMgArchive->ReadEntries();

    ideLoader.AddTXDDFFInfoToMaps(newIMgArchive);

    AddIgnoredDffHashesToSet();


    CIMGArchive* newIMgArchiveOut = new CIMGArchive("proxy_test_gta3.img", IMG_FILE_WRITE);
    std::vector<CIMGArchiveFile*> imgArchiveFiles;
    for (DWORD i = 0; i < newIMgArchive->GetFileCount(); i++) // newIMgArchive->GetFileCount()
    {
        CIMGArchiveFile* newFile = newIMgArchive->GetFileByID(i);
        if (newFile != NULL)
        {
            SString strFileName = newFile->fileEntry->fileName;
            strFileName = strFileName.ToLower();

            pRenderWare->SetCurrentDFFBeingGeneratedFileName(strFileName);

            const char * pFileName = strFileName.c_str();
            memcpy(newFile->fileEntry->fileName, pFileName, strlen(pFileName)+1);

            SString strFileExtension = strFileName.substr(strFileName.find_last_of(".") + 1);
            
            // check if it's a txd file
            if (strFileExtension == "txd")
            {
                //OptimizeTXDFile(newFile);
            }
            else if (strFileExtension == "dff")
            {
               
                OptimizeDFFFile(newIMgArchive, newFile, ideLoader);
            }
            
            imgArchiveFiles.push_back(newFile);
        }
    }

    newIMgArchiveOut->WriteEntries(imgArchiveFiles);

    //delete newFile;
    delete newIMgArchive;
    delete newIMgArchiveOut;
    

    return true;
}

