#include "StdInc.h"
#include "CIMGArchive.h"
#include "CIDELoader.h"
#include "CTextureAtlas.h"
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


const std::array<SString, 212> arrDffFilesToIgnoreNames = {
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


const std::array<SString, 212> arrTXDFilesToIgnoreNames = {
    "admiral.txd",
    "alpha.txd",
    "ambulan.txd",
    "androm.txd",
    "artict1.txd",
    "artict2.txd",
    "artict3.txd",
    "at400.txd",
    "bagboxa.txd",
    "bagboxb.txd",
    "baggage.txd",
    "bandito.txd",
    "banshee.txd",
    "barracks.txd",
    "beagle.txd",
    "benson.txd",
    "bf400.txd",
    "bfinject.txd",
    "bike.txd",
    "blade.txd",
    "blistac.txd",
    "bloodra.txd",
    "bmx.txd",
    "bobcat.txd",
    "boxburg.txd",
    "boxville.txd",
    "bravura.txd",
    "broadway.txd",
    "buccanee.txd",
    "buffalo.txd",
    "bullet.txd",
    "burrito.txd",
    "bus.txd",
    "cabbie.txd",
    "caddy.txd",
    "cadrona.txd",
    "camper.txd",
    "cargobob.txd",
    "cement.txd",
    "cheetah.txd",
    "clover.txd",
    "club.txd",
    "coach.txd",
    "coastg.txd",
    "combine.txd",
    "comet.txd",
    "copbike.txd",
    "copcarla.txd",
    "copcarru.txd",
    "copcarsf.txd",
    "copcarvg.txd",
    "cropdust.txd",
    "dft30.txd",
    "dinghy.txd",
    "dodo.txd",
    "dozer.txd",
    "dumper.txd",
    "duneride.txd",
    "elegant.txd",
    "elegy.txd",
    "emperor.txd",
    "enforcer.txd",
    "esperant.txd",
    "euros.txd",
    "faggio.txd",
    "farmtr1.txd",
    "fbiranch.txd",
    "fbitruck.txd",
    "fcr900.txd",
    "feltzer.txd",
    "firela.txd",
    "firetruk.txd",
    "flash.txd",
    "flatbed.txd",
    "forklift.txd",
    "fortune.txd",
    "freeway.txd",
    "freibox.txd",
    "freiflat.txd",
    "freight.txd",
    "glendale.txd",
    "glenshit.txd",
    "greenwoo.txd",
    "hermes.txd",
    "hotdog.txd",
    "hotknife.txd",
    "hotrina.txd",
    "hotrinb.txd",
    "hotring.txd",
    "hunter.txd",
    "huntley.txd",
    "hustler.txd",
    "hydra.txd",
    "infernus.txd",
    "intruder.txd",
    "jester.txd",
    "jetmax.txd",
    "journey.txd",
    "kart.txd",
    "landstal.txd",
    "launch.txd",
    "leviathn.txd",
    "linerun.txd",
    "majestic.txd",
    "manana.txd",
    "marquis.txd",
    "maverick.txd",
    "merit.txd",
    "mesa.txd",
    "monster.txd",
    "monstera.txd",
    "monsterb.txd",
    "moonbeam.txd",
    "mower.txd",
    "mrwhoop.txd",
    "mtbike.txd",
    "mule.txd",
    "nebula.txd",
    "nevada.txd",
    "newsvan.txd",
    "nrg500.txd",
    "oceanic.txd",
    "packer.txd",
    "patriot.txd",
    "pcj600.txd",
    "peren.txd",
    "petro.txd",
    "petrotr.txd",
    "phoenix.txd",
    "picador.txd",
    "pizzaboy.txd",
    "polmav.txd",
    "pony.txd",
    "predator.txd",
    "premier.txd",
    "previon.txd",
    "primo.txd",
    "quad.txd",
    "raindanc.txd",
    "rancher.txd",
    "rcbandit.txd",
    "rcbaron.txd",
    "rccam.txd",
    "rcgoblin.txd",
    "rcraider.txd",
    "rctiger.txd",
    "rdtrain.txd",
    "reefer.txd",
    "regina.txd",
    "remingtn.txd",
    "rhino.txd",
    "rnchlure.txd",
    "romero.txd",
    "rumpo.txd",
    "rustler.txd",
    "sabre.txd",
    "sadler.txd",
    "sadlshit.txd",
    "sanchez.txd",
    "sandking.txd",
    "savanna.txd",
    "seaspar.txd",
    "securica.txd",
    "sentinel.txd",
    "shamal.txd",
    "skimmer.txd",
    "slamvan.txd",
    "solair.txd",
    "sparrow.txd",
    "speeder.txd",
    "squalo.txd",
    "stafford.txd",
    "stallion.txd",
    "stratum.txd",
    "streak.txd",
    "streakc.txd",
    "stretch.txd",
    "stunt.txd",
    "sultan.txd",
    "sunrise.txd",
    "supergt.txd",
    "swatvan.txd",
    "sweeper.txd",
    "tahoma.txd",
    "tampa.txd",
    "taxi.txd",
    "topfun.txd",
    "tornado.txd",
    "towtruck.txd",
    "tractor.txd",
    "tram.txd",
    "trash.txd",
    "tropic.txd",
    "tug.txd",
    "tugstair.txd",
    "turismo.txd",
    "uranus.txd",
    "utility.txd",
    "utiltr1.txd",
    "vcnmav.txd",
    "vincent.txd",
    "virgo.txd",
    "voodoo.txd",
    "vortex.txd",
    "walton.txd",
    "washing.txd",
    "wayfarer.txd",
    "willard.txd",
    "windsor.txd",
    "yankee.txd",
    "yosemite.txd",
    "zr350.txd" };

std::set <unsigned int> setOfIgnoredTXDNameHashes;

void AddIgnoredDffHashesToSet()
{
    for (auto& dffName : arrDffFilesToIgnoreNames)
    {
        setOfIgnoredDffNameHashes.insert(HashString(dffName.ToLower()));
    }
}

void AddIgnoredTXDHashesToSet()
{
    for (auto& txdName : arrTXDFilesToIgnoreNames)
    {
        setOfIgnoredTXDNameHashes.insert(HashString(txdName.ToLower()));
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

        //newFile->fileByteBuffer.SetSize(newFile->actualFileSize);
        //pRenderWare->WriteTXD(newFile->fileByteBuffer.GetData(), newFile->actualFileSize, pTxdDictionary);

        pRenderWare->DestroyTXD(pTxdDictionary);
    }
    else
    {
        std::printf("failed to read %s\n", newFile->fileEntry->fileName);
    }
}


bool IsVehicleModel(const int modelID)
{
    return modelID >= 400 && modelID <= 611;
}

extern SString g_CurrentDFFBeingGeneratedFileName;


static RwTexDictionary* g_pVehicleTxdDictionary = nullptr;

// Normalize pIn vector into pOut
bool VectorNormalize(RwV3d *pIn, RwV3d *pOut)
{
    float len = (float)(std::sqrt(pIn->x*pIn->x + pIn->y*pIn->y + pIn->z*pIn->z));
    if (len)
    {
        pOut->x = pIn->x / len;
        pOut->y = pIn->y / len;
        pOut->z = pIn->z / len;
        return true;
    }
    return false;
}

bool ReMapGeometryUVs(FILE* file, int firstIndex, int meshID, RpGeometry* pGeometry, std::vector <CTextureAtlas>& vecTextureAtlases)
{
    RwV3d* pVertices = pGeometry->morphTarget->verts;
    //RwV3d* pNormals = pGeometry->morphTarget->normals;
    RwTextureCoordinates* pTextureCoordinateArray = pGeometry->texcoords[0];
    RwV3d normal;
    for (int i = 0; i < pGeometry->vertices_size; i++)
    {
        VectorNormalize(&pVertices[i], &normal);
        fprintf(file, "v %g %g %g\n", pVertices[i].x, pVertices[i].z,  pVertices[i].y);
        fprintf(file, "vn %g %g %g\n", normal.x, normal.z, normal.y);
        fprintf(file, "vt %g %g\n", pTextureCoordinateArray[i].u, pTextureCoordinateArray[i].v);
    }
    fprintf(file, "o mesh%03u\n", meshID);
    //fprintf(file, "usemtl repack_atlas\n");
    fprintf(file, "s off\n");

    RpTriangle* triangles = pGeometry->triangles;
    for (uint32_t f = 0; f < pGeometry->triangles_size; f++)
    {
        RpTriangle& triangle = triangles[f];
        fprintf(file, "f ");
        for (uint32_t j = 0; j < 3; j++) 
        {
            const uint32_t index = firstIndex + triangle.vertIndex[j] + 1; // 1-indexed
            fprintf(file, "%d/%d/%d%c", index, index, index, j == 2 ? '\n' : ' ');
        }
    }
    return true;
}

bool ReMapClumpUVs(RpClump* pClump, std::vector <CTextureAtlas>& vecTextureAtlases)
{
    auto RpGeometryLock = (RpGeometry *(__cdecl*)(RpGeometry * geometry, RwInt32 lockMode))0x74C7D0;
    auto RpGeometryUnlock = (RpGeometry *(__cdecl*)(RpGeometry * geometry))0x74C800;

    CRenderWare* pRenderWare = g_pCore->GetGame()->GetRenderWare();

    std::vector<RpAtomic*> outAtomicList;
    pRenderWare->GetClumpAtomicList(pClump, outAtomicList);

    const char* modelFilePath = "C:\\Users\\danish\\Desktop\\clump_output.obj";
    FILE* file;
    if (fopen_s(&file, modelFilePath, "w") != 0)
    {
        printf("ReMapClumpUVs: Failed to open file\n");
        return false;
    }

    int firstIndex = 0;
    int meshID = 0;
    for (auto& pAtomic: outAtomicList)
    { 
        RpGeometryLock(pAtomic->geometry, rpGEOMETRYLOCKALL);

        if (!ReMapGeometryUVs(file, firstIndex, meshID, pAtomic->geometry, vecTextureAtlases))
        {
            RpGeometryUnlock(pAtomic->geometry);
            return false;
        }

        meshID++;
        firstIndex += pAtomic->geometry->vertices_size;
        RpGeometryUnlock(pAtomic->geometry);
    }

    if (file)
    {
        fclose(file);
    }
    return true;
}

void SetAtlasesNames(std::vector <CTextureAtlas>& vecTextureAtlases, SString& strTxdName)
{
    size_t i = 0;
    for (auto& textureAtlas : vecTextureAtlases)
    {
        SString atlasName; 
        atlasName.Format("%s%d", strTxdName.c_str(), i);

        const char* strAtlasName = atlasName.c_str();
        memcpy(textureAtlas.GetAtlasTexture()->name, strAtlasName, strlen(strAtlasName) + 1);

        i++;
    }
}

RwTexDictionary* CreateTXDAtlas(std::vector <CTextureAtlas>& vecTextureAtlases, RwTexDictionary* pTxdDictionary, SString& strTxdName)
{
    auto WriteRaster = (bool(__cdecl*)(RwRaster *raster, char *filename))0x5A4150;

    CRenderWare* pRenderWare = g_pCore->GetGame()->GetRenderWare();

    std::vector<RwTexture*> outTextureList;
    pRenderWare->GetTxdTextures(outTextureList, pTxdDictionary);

    std::vector<RwTexture*> vecAtlasesTextures;
    do
    {
        CTextureAtlas& textureAtlas = vecTextureAtlases.emplace_back();

        for (auto it = outTextureList.begin(); it != outTextureList.end();)
        {
            RwRaster* raster = (*it)->raster;
            textureAtlas.AddTextureInfo(*it, raster->width, raster->height);
            it = outTextureList.erase(it);
        }

        eTextureAtlasErrorCodes textureReturn = textureAtlas.CreateAtlas();
        if (textureReturn == TEX_ATLAS_CANT_FIT_INTO_ATLAS)
        {
            if (!textureAtlas.RemoveTextureInfoTillSuccess(outTextureList))
            {
                return nullptr;
            }
        }
        else if (textureReturn != TEX_ATLAS_SUCCESS)
        {
            return nullptr;
        }

        vecAtlasesTextures.push_back(textureAtlas.GetAtlasTexture());

    } while (outTextureList.size () > 0);

    SetAtlasesNames(vecTextureAtlases, strTxdName);
    std::printf("Total Atlases Created: %u\n", vecTextureAtlases.size());

    return pRenderWare->CreateTextureDictionary(vecAtlasesTextures);
}

void OptimizeDFFFile(CIMGArchive* pIMgArchive, CIMGArchiveFile* newFile, CIDELoader& ideLoader)
{
    CRenderWare* pRenderWare = g_pCore->GetGame()->GetRenderWare();
    auto RpClumpStreamGetSize = (unsigned int(__cdecl*)(RpClump *))0x74A5E0;


    // REMOVE LATER
    const char* pStrDFFName = "cj_bag_reclaim.dff"; //"infernus.dff";
    memcpy(newFile->fileEntry->fileName, pStrDFFName, strlen(pStrDFFName) + 1);
    // REMOVE END


    const unsigned int uiDFFNameHash = HashString(newFile->fileEntry->fileName);

    RwTexDictionary* pTxdDictionary = pRenderWare->ReadTXD("cj_airprt.txd", CBuffer(), false);

    /*SDFFDescriptor* pDFFDescriptor = ideLoader.GetDFFDescriptor(uiDFFNameHash);
    if (!pDFFDescriptor)
    {
        std::printf("couldn't find dff descriptor for '%s'\n", newFile->fileEntry->fileName);
        return;
    }

    STXDDescriptor* pTXDDescriptor = pDFFDescriptor->GetTXDDescriptor();
    RwTexDictionary* pTxdDictionary = pTXDDescriptor->GetTextureDictionary();
    if (!pTxdDictionary)
    {
        STXDImgArchiveInfo* pTXDImgArchiveInfo = pTXDDescriptor->GetTXDImgArchiveInfo();
        if (pTXDImgArchiveInfo->usSize == 0)
        {
            return;
        }
        CIMGArchiveFile* pTXDArchiveFile = pIMgArchive->GetFileByTXDImgArchiveInfo(pTXDImgArchiveInfo);
        pTxdDictionary = pRenderWare->ReadTXD(nullptr, pTXDArchiveFile->fileByteBuffer, false);
        pTXDDescriptor->SetTextureDictionary(pTxdDictionary);
        delete pTXDArchiveFile;
    }
    */

    std::vector <CTextureAtlas> vecTextureAtlases;

    /*
    int modelID = pDFFDescriptor->GetModelID();
    */
    int modelID = 0; // pDFFDescriptor->GetModelID();
    if (IsVehicleModel(modelID))
    {
        pRenderWare->CopyTexturesFromDictionary(pTxdDictionary, g_pVehicleTxdDictionary);
    }
    else
    {
        // WE NEED To CHANGE THIS LOGIC LATER TO ALLOW OPTIMIZING OF VEHICLE MODELS AND TXD FILES
       /* SString txdName = "cj_airprt";
        RwTexDictionary* pAtlasTxdDictionary = CreateTXDAtlas(vecTextureAtlases, pTxdDictionary, txdName);
        if (!pAtlasTxdDictionary)
        {
            std::printf("failed to CREATE TXD atlas for TXD '%s' :(\n", txdName.c_str());
            return;
        }

        pRenderWare->WriteTXD("dffs\\myatlas.txd", pAtlasTxdDictionary);*/
    }
    
    pRenderWare->SetCurrentDFFWriteModelID(modelID);
    pRenderWare->SetCurrentReadDFFWithoutReplacingCOL(true);

    bool bLoadCollision = IsVehicleModel(modelID);
    RpClump* pClump = pRenderWare->ReadDFF(newFile->fileEntry->fileName, CBuffer(), modelID, bLoadCollision, pTxdDictionary);
    //RpClump* pClump = pRenderWare->ReadDFF(newFile->fileEntry->fileName, newFile->fileByteBuffer, modelID, bLoadCollision, pTxdDictionary);
    pRenderWare->SetCurrentReadDFFWithoutReplacingCOL(false);
    if (pClump)
    {
        if (!ReMapClumpUVs(pClump, vecTextureAtlases))
        {
            std::printf("FAILED to remap UVs in clump geometries for DFF '%s'\n", newFile->fileEntry->fileName);
            return;
        }

        unsigned int clumpSize =  RpClumpStreamGetSize(pClump);

        // there's still an issue with size of empty extension headers of 12 btytes for clump
        clumpSize += 24;

        //free the memory
        CBuffer().Swap(newFile->fileByteBuffer);

        newFile->actualFileSize = GetActualFileSize(clumpSize);
        newFile->fileEntry->usSize = newFile->actualFileSize / 2048;
        //std::printf("dff file: %s  | new dff size: %u | actualFileSize: %d | capacity: %d\n",
        //    newFile->fileEntry->fileName, clumpSize, (int)newFile->actualFileSize, newFile->fileByteBuffer.GetCapacity());

        newFile->fileByteBuffer.SetSize(newFile->actualFileSize);
        void* pData = newFile->fileByteBuffer.GetData();
        //pRenderWare->WriteDFF(pData, newFile->actualFileSize, pClump);
        
        SString strPathOfGeneratedDff = "dffs\\";
        pRenderWare->WriteDFF(strPathOfGeneratedDff + newFile->fileEntry->fileName, pClump);

        if (bLoadCollision)
        {
            pRenderWare->DeleteReadDFFCollisionModel();
        }

        pRenderWare->DestroyDFF(pClump);
        
        //pTXDDescriptor->RemoveDFFNameFromSet(uiDFFNameHash);
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

    if (!g_pVehicleTxdDictionary)
    {
        g_pVehicleTxdDictionary = pRenderWare->ReadTXD("models\\generic\\vehicle.txd", CBuffer(), false);
        assert(g_pVehicleTxdDictionary != nullptr);
    }

    /*SString txdName = "vehicle";
    CreateTXDAtlas(g_pVehicleTxdDictionary, txdName);*/

    CIDELoader ideLoader;
    
    CIMGArchive* newIMgArchive = new CIMGArchive("models\\gta3.img", IMG_FILE_READ);
    
    //CIMGArchive* newIMgArchive = new CIMGArchive("models\\vehiclesonly_gta3.img", IMG_FILE_READ);
    newIMgArchive->ReadEntries();

    ideLoader.AddTXDDFFInfoToMaps(newIMgArchive);

    AddIgnoredDffHashesToSet();
    AddIgnoredTXDHashesToSet();

    CIMGArchive* newIMgArchiveOut = new CIMGArchive("proxy_test_gta3.img", IMG_FILE_WRITE);
    std::vector<CIMGArchiveFile*> imgArchiveFiles;
    for (DWORD i = 0; i < 1; i++) // newIMgArchive->GetFileCount() crash if total file count is 13 for vehicles img (banshee.dff)
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

