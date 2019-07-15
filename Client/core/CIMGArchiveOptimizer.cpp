#include "StdInc.h"
#include "CIMGArchive.h"
#include "CIDELoader.h"
#include "CTextureAtlas.h"
#include "CDXTexture.h"
#include <xatlas.h>
#include "xatlas_repack.h"
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

const std::array<SString, 212> arrDffFilesToIgnoreNames = {
    "admiral.dff",  "alpha.dff",    "ambulan.dff",  "androm.dff",   "artict1.dff",  "artict2.dff",  "artict3.dff",  "at400.dff",    "bagboxa.dff",
    "bagboxb.dff",  "baggage.dff",  "bandito.dff",  "banshee.dff",  "barracks.dff", "beagle.dff",   "benson.dff",   "bf400.dff",    "bfinject.dff",
    "bike.dff",     "blade.dff",    "blistac.dff",  "bloodra.dff",  "bmx.dff",      "bobcat.dff",   "boxburg.dff",  "boxville.dff", "bravura.dff",
    "broadway.dff", "buccanee.dff", "buffalo.dff",  "bullet.dff",   "burrito.dff",  "bus.dff",      "cabbie.dff",   "caddy.dff",    "cadrona.dff",
    "camper.dff",   "cargobob.dff", "cement.dff",   "cheetah.dff",  "clover.dff",   "club.dff",     "coach.dff",    "coastg.dff",   "combine.dff",
    "comet.dff",    "copbike.dff",  "copcarla.dff", "copcarru.dff", "copcarsf.dff", "copcarvg.dff", "cropdust.dff", "dft30.dff",    "dinghy.dff",
    "dodo.dff",     "dozer.dff",    "dumper.dff",   "duneride.dff", "elegant.dff",  "elegy.dff",    "emperor.dff",  "enforcer.dff", "esperant.dff",
    "euros.dff",    "faggio.dff",   "farmtr1.dff",  "fbiranch.dff", "fbitruck.dff", "fcr900.dff",   "feltzer.dff",  "firela.dff",   "firetruk.dff",
    "flash.dff",    "flatbed.dff",  "forklift.dff", "fortune.dff",  "freeway.dff",  "freibox.dff",  "freiflat.dff", "freight.dff",  "glendale.dff",
    "glenshit.dff", "greenwoo.dff", "hermes.dff",   "hotdog.dff",   "hotknife.dff", "hotrina.dff",  "hotrinb.dff",  "hotring.dff",  "hunter.dff",
    "huntley.dff",  "hustler.dff",  "hydra.dff",    "infernus.dff", "intruder.dff", "jester.dff",   "jetmax.dff",   "journey.dff",  "kart.dff",
    "landstal.dff", "launch.dff",   "leviathn.dff", "linerun.dff",  "majestic.dff", "manana.dff",   "marquis.dff",  "maverick.dff", "merit.dff",
    "mesa.dff",     "monster.dff",  "monstera.dff", "monsterb.dff", "moonbeam.dff", "mower.dff",    "mrwhoop.dff",  "mtbike.dff",   "mule.dff",
    "nebula.dff",   "nevada.dff",   "newsvan.dff",  "nrg500.dff",   "oceanic.dff",  "packer.dff",   "patriot.dff",  "pcj600.dff",   "peren.dff",
    "petro.dff",    "petrotr.dff",  "phoenix.dff",  "picador.dff",  "pizzaboy.dff", "polmav.dff",   "pony.dff",     "predator.dff", "premier.dff",
    "previon.dff",  "primo.dff",    "quad.dff",     "raindanc.dff", "rancher.dff",  "rcbandit.dff", "rcbaron.dff",  "rccam.dff",    "rcgoblin.dff",
    "rcraider.dff", "rctiger.dff",  "rdtrain.dff",  "reefer.dff",   "regina.dff",   "remingtn.dff", "rhino.dff",    "rnchlure.dff", "romero.dff",
    "rumpo.dff",    "rustler.dff",  "sabre.dff",    "sadler.dff",   "sadlshit.dff", "sanchez.dff",  "sandking.dff", "savanna.dff",  "seaspar.dff",
    "securica.dff", "sentinel.dff", "shamal.dff",   "skimmer.dff",  "slamvan.dff",  "solair.dff",   "sparrow.dff",  "speeder.dff",  "squalo.dff",
    "stafford.dff", "stallion.dff", "stratum.dff",  "streak.dff",   "streakc.dff",  "stretch.dff",  "stunt.dff",    "sultan.dff",   "sunrise.dff",
    "supergt.dff",  "swatvan.dff",  "sweeper.dff",  "tahoma.dff",   "tampa.dff",    "taxi.dff",     "topfun.dff",   "tornado.dff",  "towtruck.dff",
    "tractor.dff",  "tram.dff",     "trash.dff",    "tropic.dff",   "tug.dff",      "tugstair.dff", "turismo.dff",  "uranus.dff",   "utility.dff",
    "utiltr1.dff",  "vcnmav.dff",   "vincent.dff",  "virgo.dff",    "voodoo.dff",   "vortex.dff",   "walton.dff",   "washing.dff",  "wayfarer.dff",
    "willard.dff",  "windsor.dff",  "yankee.dff",   "yosemite.dff", "zr350.dff"};

std::set<unsigned int> setOfIgnoredDffNameHashes;

const std::array<SString, 212> arrTXDFilesToIgnoreNames = {
    "admiral.txd",  "alpha.txd",    "ambulan.txd",  "androm.txd",   "artict1.txd",  "artict2.txd",  "artict3.txd",  "at400.txd",    "bagboxa.txd",
    "bagboxb.txd",  "baggage.txd",  "bandito.txd",  "banshee.txd",  "barracks.txd", "beagle.txd",   "benson.txd",   "bf400.txd",    "bfinject.txd",
    "bike.txd",     "blade.txd",    "blistac.txd",  "bloodra.txd",  "bmx.txd",      "bobcat.txd",   "boxburg.txd",  "boxville.txd", "bravura.txd",
    "broadway.txd", "buccanee.txd", "buffalo.txd",  "bullet.txd",   "burrito.txd",  "bus.txd",      "cabbie.txd",   "caddy.txd",    "cadrona.txd",
    "camper.txd",   "cargobob.txd", "cement.txd",   "cheetah.txd",  "clover.txd",   "club.txd",     "coach.txd",    "coastg.txd",   "combine.txd",
    "comet.txd",    "copbike.txd",  "copcarla.txd", "copcarru.txd", "copcarsf.txd", "copcarvg.txd", "cropdust.txd", "dft30.txd",    "dinghy.txd",
    "dodo.txd",     "dozer.txd",    "dumper.txd",   "duneride.txd", "elegant.txd",  "elegy.txd",    "emperor.txd",  "enforcer.txd", "esperant.txd",
    "euros.txd",    "faggio.txd",   "farmtr1.txd",  "fbiranch.txd", "fbitruck.txd", "fcr900.txd",   "feltzer.txd",  "firela.txd",   "firetruk.txd",
    "flash.txd",    "flatbed.txd",  "forklift.txd", "fortune.txd",  "freeway.txd",  "freibox.txd",  "freiflat.txd", "freight.txd",  "glendale.txd",
    "glenshit.txd", "greenwoo.txd", "hermes.txd",   "hotdog.txd",   "hotknife.txd", "hotrina.txd",  "hotrinb.txd",  "hotring.txd",  "hunter.txd",
    "huntley.txd",  "hustler.txd",  "hydra.txd",    "infernus.txd", "intruder.txd", "jester.txd",   "jetmax.txd",   "journey.txd",  "kart.txd",
    "landstal.txd", "launch.txd",   "leviathn.txd", "linerun.txd",  "majestic.txd", "manana.txd",   "marquis.txd",  "maverick.txd", "merit.txd",
    "mesa.txd",     "monster.txd",  "monstera.txd", "monsterb.txd", "moonbeam.txd", "mower.txd",    "mrwhoop.txd",  "mtbike.txd",   "mule.txd",
    "nebula.txd",   "nevada.txd",   "newsvan.txd",  "nrg500.txd",   "oceanic.txd",  "packer.txd",   "patriot.txd",  "pcj600.txd",   "peren.txd",
    "petro.txd",    "petrotr.txd",  "phoenix.txd",  "picador.txd",  "pizzaboy.txd", "polmav.txd",   "pony.txd",     "predator.txd", "premier.txd",
    "previon.txd",  "primo.txd",    "quad.txd",     "raindanc.txd", "rancher.txd",  "rcbandit.txd", "rcbaron.txd",  "rccam.txd",    "rcgoblin.txd",
    "rcraider.txd", "rctiger.txd",  "rdtrain.txd",  "reefer.txd",   "regina.txd",   "remingtn.txd", "rhino.txd",    "rnchlure.txd", "romero.txd",
    "rumpo.txd",    "rustler.txd",  "sabre.txd",    "sadler.txd",   "sadlshit.txd", "sanchez.txd",  "sandking.txd", "savanna.txd",  "seaspar.txd",
    "securica.txd", "sentinel.txd", "shamal.txd",   "skimmer.txd",  "slamvan.txd",  "solair.txd",   "sparrow.txd",  "speeder.txd",  "squalo.txd",
    "stafford.txd", "stallion.txd", "stratum.txd",  "streak.txd",   "streakc.txd",  "stretch.txd",  "stunt.txd",    "sultan.txd",   "sunrise.txd",
    "supergt.txd",  "swatvan.txd",  "sweeper.txd",  "tahoma.txd",   "tampa.txd",    "taxi.txd",     "topfun.txd",   "tornado.txd",  "towtruck.txd",
    "tractor.txd",  "tram.txd",     "trash.txd",    "tropic.txd",   "tug.txd",      "tugstair.txd", "turismo.txd",  "uranus.txd",   "utility.txd",
    "utiltr1.txd",  "vcnmav.txd",   "vincent.txd",  "virgo.txd",    "voodoo.txd",   "vortex.txd",   "walton.txd",   "washing.txd",  "wayfarer.txd",
    "willard.txd",  "windsor.txd",  "yankee.txd",   "yosemite.txd", "zr350.txd"};

std::set<unsigned int> setOfIgnoredTXDNameHashes;

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
        newFile->fileEntry->usSize = newFile->actualFileSize / 2048;
        std::printf("txd file: %s  | new txd size: %d | actualFileSize: %d | capacity: %d\n", newFile->fileEntry->fileName, txdSize,
                    (int)newFile->actualFileSize, newFile->fileByteBuffer.GetCapacity());

        // newFile->fileByteBuffer.SetSize(newFile->actualFileSize);
        // pRenderWare->WriteTXD(newFile->fileByteBuffer.GetData(), newFile->actualFileSize, pTxdDictionary);

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
bool VectorNormalize(RwV3d* pIn, RwV3d* pOut)
{
    float len = (float)(std::sqrt(pIn->x * pIn->x + pIn->y * pIn->y + pIn->z * pIn->z));
    if (len)
    {
        pOut->x = pIn->x / len;
        pOut->y = pIn->y / len;
        pOut->z = pIn->z / len;
        return true;
    }
    return false;
}

void GetTextures(RpClump* pClump, std::vector<uint32_t>& textures, std::vector<CDXTexture>& texturesCache)
{
    CRenderWare* pRenderWare = g_pCore->GetGame()->GetRenderWare();

    std::vector<RpAtomic*> outAtomicList;
    pRenderWare->GetClumpAtomicList(pClump, outAtomicList);

    std::set<unsigned int> setOfTextureNameHashes;

    for (uint32_t geometryIndex = 0; geometryIndex < outAtomicList.size(); geometryIndex++)
    {
        RpAtomic*   pAtomic = outAtomicList[geometryIndex];
        RpGeometry* pGeometry = pAtomic->geometry;
        for (int i = 0; i < pGeometry->materials.entries; i++)
        {
            RpMaterial* pMaterial = pGeometry->materials.materials[i];

            if (!pMaterial->texture)
            {
                printf("GetTextures: No texture\n");
                textures.emplace_back(UINT32_MAX);
                continue;
            }

            const unsigned int textureNameHash = HashString(pMaterial->texture->name);
            auto               it = setOfTextureNameHashes.find(textureNameHash);
            if (it == setOfTextureNameHashes.end())
            {
                textures.emplace_back(texturesCache.size());
                CDXTexture& dxTexture = texturesCache.emplace_back(pMaterial);
                dxTexture.LockTexture(D3DLOCK_READONLY);

                setOfTextureNameHashes.insert(textureNameHash);

                printf("GetTextures: added texture to cache: %s\n", pMaterial->texture->name);
            }
            else
            {
                unsigned int index = std::distance(setOfTextureNameHashes.begin(), it);
                textures.emplace_back(textures[index]);

                printf("GetTextures: Added index to `textures` as tex exists: %s\n", pMaterial->texture->name);
            }
        }
    }
}

void GetVerticesToMaterial(RpClump* pClump, std::vector<uint16_t>** vertexToMaterial)
{
    CRenderWare* pRenderWare = g_pCore->GetGame()->GetRenderWare();

    std::vector<RpAtomic*> outAtomicList;
    pRenderWare->GetClumpAtomicList(pClump, outAtomicList);

    size_t totalVertices = 0;
    for (uint32_t geometryIndex = 0; geometryIndex < outAtomicList.size(); geometryIndex++)
    {
        RpAtomic*   pAtomic = outAtomicList[geometryIndex];
        RpGeometry* pGeometry = pAtomic->geometry;
        totalVertices += static_cast<size_t>(pGeometry->vertices_size);
    }

    *vertexToMaterial = new std::vector<uint16_t>(totalVertices);

    size_t FirstVertexIndex = 0;
    size_t FirstMaterialIndex = 0;
    for (uint32_t geometryIndex = 0; geometryIndex < outAtomicList.size(); geometryIndex++)
    {
        RpAtomic*   pAtomic = outAtomicList[geometryIndex];
        RpGeometry* pGeometry = pAtomic->geometry;
        for (int i = 0; i < pGeometry->triangles_size; i++)
        {
            RpTriangle& triangle = pGeometry->triangles[i];
            for (int j = 0; j < 3; j++)
            {
                int vertexIndex = triangle.vertIndex[j];

                unsigned short materialIndex = triangle.matIndex + FirstMaterialIndex;
                (*vertexToMaterial)->at(vertexIndex + FirstVertexIndex) = materialIndex;
            }
        }

        FirstVertexIndex += pGeometry->vertices_size;
        FirstMaterialIndex += pGeometry->materials.entries;
    }
}

void GetVerticesPositionsAndUVs(RpClump* pClump, std::vector<Vector3>& vertices, std::vector<Vector2>& uvs)
{
    CRenderWare* pRenderWare = g_pCore->GetGame()->GetRenderWare();

    std::vector<RpAtomic*> outAtomicList;
    pRenderWare->GetClumpAtomicList(pClump, outAtomicList);

    size_t totalVertices = 0;
    size_t FirstVertexIndex = 0;
    for (uint32_t geometryIndex = 0; geometryIndex < outAtomicList.size(); geometryIndex++)
    {
        RpAtomic*             pAtomic = outAtomicList[geometryIndex];
        RwTextureCoordinates* pTextureCoordinateArray = pAtomic->geometry->texcoords[0];
        RwV3d*                pVertices = pAtomic->geometry->morphTarget->verts;
        for (int i = 0; i < pAtomic->geometry->vertices_size; i++)
        {
            vertices[FirstVertexIndex + i] = Vector3(pVertices[i].x, pVertices[i].y, pVertices[i].z);

            RwTextureCoordinates& textureCoords = pTextureCoordinateArray[i];

            // Flip V for xatlas :)
            float theV = 1.0f - textureCoords.v;
            uvs[FirstVertexIndex + i] = Vector2(textureCoords.u, theV);
        }

        FirstVertexIndex += pAtomic->geometry->vertices_size;
    }
}

void GetVertexIndices(RpClump* pClump, std::vector<uint32_t>** vertexIndices)
{
    CRenderWare* pRenderWare = g_pCore->GetGame()->GetRenderWare();

    std::vector<RpAtomic*> outAtomicList;
    pRenderWare->GetClumpAtomicList(pClump, outAtomicList);

    size_t totalIndices = 0;
    for (uint32_t geometryIndex = 0; geometryIndex < outAtomicList.size(); geometryIndex++)
    {
        RpAtomic*   pAtomic = outAtomicList[geometryIndex];
        RpGeometry* pGeometry = pAtomic->geometry;

        totalIndices += (pGeometry->triangles_size * 3);
    }

    *vertexIndices = new std::vector<uint32_t>(totalIndices);

    size_t FirstIndex = 0;
    size_t FirstVertexIndex = 0;
    for (uint32_t geometryIndex = 0; geometryIndex < outAtomicList.size(); geometryIndex++)
    {
        RpAtomic*   pAtomic = outAtomicList[geometryIndex];
        RpGeometry* pGeometry = pAtomic->geometry;
        int         triangleIndex = 0;
        uint32_t    totalGeometryIndices = pGeometry->triangles_size * 3;
        for (uint32_t f = 0; f < totalGeometryIndices; f += 3, triangleIndex++)
        {
            RpTriangle& triangle = pGeometry->triangles[triangleIndex];
            for (int j = 0; j < 3; j++)
            {
                uint32_t vertexIndex = triangle.vertIndex[j] + FirstVertexIndex;
                (*vertexIndices)->at(FirstIndex + f + j) = vertexIndex;
            }
        }

        FirstIndex += totalGeometryIndices;
        FirstVertexIndex += pGeometry->vertices_size;
    }
}

void GetFaceMaterials(RpClump* pClump, std::vector<uint32_t>** faceMaterials)
{
    CRenderWare* pRenderWare = g_pCore->GetGame()->GetRenderWare();

    std::vector<RpAtomic*> outAtomicList;
    pRenderWare->GetClumpAtomicList(pClump, outAtomicList);

    size_t totalTriangles = 0;
    for (uint32_t geometryIndex = 0; geometryIndex < outAtomicList.size(); geometryIndex++)
    {
        RpAtomic*   pAtomic = outAtomicList[geometryIndex];
        RpGeometry* pGeometry = pAtomic->geometry;
        totalTriangles += pGeometry->triangles_size;
    }

    *faceMaterials = new std::vector<uint32_t>(totalTriangles);

    size_t FirstIndex = 0;
    size_t FirstMaterialIndex = 0;
    for (uint32_t geometryIndex = 0; geometryIndex < outAtomicList.size(); geometryIndex++)
    {
        RpAtomic*   pAtomic = outAtomicList[geometryIndex];
        RpGeometry* pGeometry = pAtomic->geometry;
        int         triangleIndex = 0;
        for (uint32_t triangleIndex = 0; triangleIndex < pGeometry->triangles_size; triangleIndex++)
        {
            RpTriangle& triangle = pGeometry->triangles[triangleIndex];

            unsigned int materialIndex = triangle.matIndex + FirstMaterialIndex;
            (*faceMaterials)->at(FirstIndex + triangleIndex) = materialIndex;
        }

        FirstIndex += pGeometry->triangles_size;
        FirstMaterialIndex += pGeometry->materials.entries;
    }
}

void CreateGeometryMaterials(RpGeometry* pOriginalGeometry, RpGeometry* pNewGeometry, std::vector<RwTexture*>& atlasTextures, unsigned int atlasCount)
{
    auto RpMaterialCreate = (RpMaterial * (__cdecl*)())0x74D990;
    auto _rpMaterialListAlloc = (RpMaterial * *(__cdecl*)(RwUInt32 count))0x74E1C0;

    unsigned int totalMaterialWithoutTextures = 0;
    for (int i = 0; i < pOriginalGeometry->materials.entries; i++)
    {
        if (!pOriginalGeometry->materials.materials[i]->texture)
        {
            totalMaterialWithoutTextures++;
        }
    }

    size_t totalNewGeometryMaterials = totalMaterialWithoutTextures + atlasCount;
    pNewGeometry->materials.entries = totalNewGeometryMaterials;
    pNewGeometry->materials.unknown = totalNewGeometryMaterials;
    pNewGeometry->materials.materials = _rpMaterialListAlloc(totalNewGeometryMaterials);

    printf("done calling rpMaterialListAlloc with count: %u\n", totalNewGeometryMaterials);

    unsigned int materialWithoutTextureIndex = atlasCount;
    for (int i = 0; i < pOriginalGeometry->materials.entries; i++)
    {
        if (!pOriginalGeometry->materials.materials[i]->texture)
        {
            RpMaterial* pMaterial = pOriginalGeometry->materials.materials[i];
            pMaterial->refs++;
            pNewGeometry->materials.materials[materialWithoutTextureIndex] = pMaterial;
            printf("[INSERT] CreateGeometryMaterials: index: %u\n", materialWithoutTextureIndex);
            materialWithoutTextureIndex++;
        }
    }

    for (size_t i = 0; i < atlasTextures.size(); i++)
    {
        RpMaterial* pMaterial = RpMaterialCreate();
        // pMaterial->color = {0, 0, 255, 255}; // turns textures into blue during rendering
        pMaterial->texture = atlasTextures[i];
        pMaterial->refs++;
        pNewGeometry->materials.materials[i] = pMaterial;
        printf("[INSERT] CreateGeometryMaterials: index: %u | name: %s\n", i, pMaterial->texture->name);
    }
}

bool GetMaterialIndex(RpGeometry* pGeometry, RpMaterial* pMaterial, size_t* materialIndex)
{
    for (int i = 0; i < pGeometry->materials.entries; i++)
    {
        if (pGeometry->materials.materials[i] == pMaterial)
        {
            *materialIndex = i;
            return true;
        }
    }
    return false;
}

uint32_t GetBestAtlasMaxResolution(xatlas::Atlas* atlas, float texelsPerUnit)
{
    xatlas::PackOptions packOptions;
    packOptions.padding = 1;
    packOptions.texelsPerUnit = texelsPerUnit;
    xatlas::PackCharts(atlas, packOptions);

    uint32_t maxResolution = std::max(atlas->width, atlas->height);
    if (maxResolution % 2 != 0)
    {
        return maxResolution + 1;
    }
    return maxResolution;
}

bool IsResolutionTooBig(unsigned int bestAtlasResolution)
{
    // Implying we are using DXT1 format for compressing textures.
    // Example: assuming bestAtlasResolution is 2590
    // 5180x5180 with DXT1 = 12.7mb in TXD file
    // 5180 / 2 = 2590 (half of 5180) | so,  2590 / 204 = 12.7
    // 12.7 / (5180 / 2590) = 6.35mb (half of 12.7mb)

    const unsigned int sizePermittedInMBs = 5;
    unsigned int       atlasSizeInMBs = std::ceil(12.7 / (5180 / bestAtlasResolution));

    printf("atlasSizeInMBs: %u\n", atlasSizeInMBs);
    if (atlasSizeInMBs > sizePermittedInMBs)
    {
        return true;
    }
    return false;
}

RpGeometry* CreateAtlasRpGeometry(RpGeometry* pOriginalGeometry, int numVerts, int numTriangles, int format);

RwTexDictionary* CreateTXDAtlas(RpClump* pClump, std::vector<CTextureAtlas>& vecTextureAtlases)
{
    auto RpMaterialCreate = (RpMaterial * (__cdecl*)())0x74D990;
    auto RpGeometryDestroy = (void(__cdecl*)(RpGeometry * geometry))0x74CCC0;
    auto RpAtomicSetGeometry = (RpAtomic * (__cdecl*)(RpAtomic * atomic, RpGeometry * geometry, RwUInt32 flags))0x749D40;
    auto _rpMaterialListAlloc = (RpMaterial * *(__cdecl*)(RwUInt32 count))0x74E1C0;

    CRenderWare* pRenderWare = g_pCore->GetGame()->GetRenderWare();

    std::vector<CDXTexture> texturesCache;
    std::vector<uint32_t>   textures;

    GetTextures(pClump, textures, texturesCache);

    // Map vertices to materials so rasterization knows which texture to sample.
    std::vector<uint16_t>* vertexToMaterial = nullptr;
    GetVerticesToMaterial(pClump, &vertexToMaterial);

    // Denormalize UVs by scaling them by texture dimensions.
    std::vector<Vector2>* denormalizedUVs = new std::vector<Vector2>(vertexToMaterial->size());
    std::vector<Vector2>* uvs = new std::vector<Vector2>(vertexToMaterial->size());

    std::vector<Vector3>* vertices = new std::vector<Vector3>(vertexToMaterial->size());
    GetVerticesPositionsAndUVs(pClump, *vertices, *uvs);

    memcpy(denormalizedUVs->data(), uvs->data(), uvs->size() * sizeof(float) * 2);

    for (uint32_t i = 0; i < denormalizedUVs->size(); i++)
    {
        const uint16_t materialIndex = vertexToMaterial->at(i);
        CDXTexture*    dxTexture = nullptr;
        if (materialIndex != UINT16_MAX && textures[materialIndex] != UINT32_MAX)
        {
            dxTexture = &texturesCache[textures[materialIndex]];
        }

        if (dxTexture)
        {
            denormalizedUVs->at(i).x *= (float)dxTexture->GetWidth();
            denormalizedUVs->at(i).y *= (float)dxTexture->GetHeight();
        }
    }

    std::vector<RpAtomic*> outAtomicList;
    pRenderWare->GetClumpAtomicList(pClump, outAtomicList);

    std::vector<uint32_t>* vertexIndices = nullptr;
    GetVertexIndices(pClump, &vertexIndices);

    std::vector<uint32_t>* faceMaterials = nullptr;
    GetFaceMaterials(pClump, &faceMaterials);

    // Generate the atlas.
    xatlas::SetPrint(printf, true);
    xatlas::Atlas* atlas = xatlas::Create();

    size_t FirstIndex = 0;
    size_t FirstVertexIndex = 0;
    size_t FirstFaceIndex = 0;
    for (uint32_t geometryIndex = 0; geometryIndex < outAtomicList.size(); geometryIndex++)
    {
        RpAtomic*   pAtomic = outAtomicList[geometryIndex];
        RpGeometry* pGeometry = pAtomic->geometry;
        uint32_t    totalGeometryIndices = pGeometry->triangles_size * 3;

        xatlas::UvMeshDecl meshDecl;
        meshDecl.vertexCount = (uint32_t)pGeometry->vertices_size;
        meshDecl.vertexUvData = &denormalizedUVs->at(FirstVertexIndex);
        meshDecl.vertexStride = sizeof(Vector2);
        meshDecl.indexCount = totalGeometryIndices;
        meshDecl.indexData = &vertexIndices->at(FirstIndex);
        meshDecl.indexFormat = xatlas::IndexFormat::UInt32;
        meshDecl.indexOffset = -(int32_t)FirstVertexIndex;
        meshDecl.rotateCharts = false;
        meshDecl.faceMaterialData = &faceMaterials->at(FirstFaceIndex);
        xatlas::AddMeshError::Enum error = xatlas::AddUvMesh(atlas, meshDecl);
        if (error != xatlas::AddMeshError::Success)
        {
            xatlas::Destroy(atlas);
            printf("Error adding mesh %d: %s\n", (int)geometryIndex, xatlas::StringForEnum(error));
            return nullptr;
        }

        FirstIndex += totalGeometryIndices;
        FirstVertexIndex += pGeometry->vertices_size;
        FirstFaceIndex += pGeometry->triangles_size;
    }

    float    texelsPerUnit = 1.0f;
    uint32_t bestAtlasResolution = GetBestAtlasMaxResolution(atlas, texelsPerUnit);

    if (IsResolutionTooBig(bestAtlasResolution))
    {
    }
    unsigned int        atlasFindingAttempts = 1;

    xatlas::PackOptions packOptions;
    do
    {
        printf("Attempt #%u: finding atlas size where atlas can be within a single image\n", atlasFindingAttempts);
        packOptions.padding = 1;
        packOptions.texelsPerUnit = texelsPerUnit;
        packOptions.resolution = bestAtlasResolution;
        xatlas::PackCharts(atlas, packOptions);
        bestAtlasResolution += 34;
        atlasFindingAttempts++;
    } while (atlas->atlasCount > 1);
    
    printf("Copying texture data into atlas\n");

    CTextureAtlas& textureAtlas = CTextureAtlas(pClump, atlas, packOptions, *vertexToMaterial, texturesCache, textures, *uvs);

    // texturesCache.swap(std::vector<CDXTexture>());
    // textures.swap(std::vector<uint32_t>());

    std::string textureAtlasFileExtension = ".png";

    std::vector<RwTexture*> atlasTextures;
    atlasTextures.resize(textureAtlas.atlasDXTextures.size());
    char buffer[100];
    for (size_t i = 0; i < textureAtlas.atlasDXTextures.size(); i++)
    {
        CDXTexture& atlasTexture = textureAtlas.atlasDXTextures[i];
        printf("compressing atlas texture to DXT1: index = %u\n", i);
        assert(atlasTexture.Compress(D3DFMT_DXT1) != false);
        sprintf(buffer, "myAtlas%d", i);
        RwTexture* pTexture = atlasTexture.GetRwTexture();
        memcpy(pTexture->name, buffer, strlen(buffer) + 1);
        atlasTextures[i] = pTexture;
    }

    RwTexDictionary* pAtlasTexDictionary = pRenderWare->CreateTextureDictionary(atlasTextures);

    FirstVertexIndex = 0;
    unsigned int FirstMaterialIndex = 0;
    for (uint32_t geometryIndex = 0; geometryIndex < outAtomicList.size(); geometryIndex++)
    {
        RpAtomic*           pAtomic = outAtomicList[geometryIndex];
        RpGeometry*         pGeometry = pAtomic->geometry;
        const xatlas::Mesh& mesh = atlas->meshes[geometryIndex];

        RpGeometry* pNewGeometry = CreateAtlasRpGeometry(pGeometry, mesh.vertexCount, pGeometry->triangles_size, pGeometry->flags);

        CreateGeometryMaterials(pGeometry, pNewGeometry, atlasTextures, atlas->atlasCount);

        RwV3d*                destVertices = pNewGeometry->morphTarget->verts;
        RwV3d*                destNormals = pNewGeometry->morphTarget->normals;
        RwColor*              destColors = pNewGeometry->colors;
        RwTextureCoordinates* pDestTextureCoordinateArray = pNewGeometry->texcoords[0];

        RwV3d*   sourceVertices = pGeometry->morphTarget->verts;
        RwV3d*   sourceNormals = pGeometry->morphTarget->normals;
        RwColor* sourceColors = pGeometry->colors;

        for (uint32_t v = 0; v < mesh.vertexCount; v++)
        {
            const xatlas::Vertex& vertex = mesh.vertexArray[v];
            size_t                sourceVertexIndex = vertex.xref;            // + FirstVertexIndex;

            destVertices[sourceVertexIndex] = sourceVertices[sourceVertexIndex];

            if (sourceNormals)
            {
                destNormals[sourceVertexIndex] = sourceNormals[sourceVertexIndex];
            }
            if (sourceColors)
            {
                destColors[sourceVertexIndex] = sourceColors[sourceVertexIndex];
            }

            pDestTextureCoordinateArray[sourceVertexIndex] = {vertex.uv[0] / atlas->width, vertex.uv[1] / atlas->height};
        }

        size_t triangleIndex = 0;
        for (uint32_t f = 0; f < mesh.indexCount; f += 3, triangleIndex++)
        {
            uint16_t       materialIndex = UINT16_MAX;
            unsigned int   vertexXref = UINT32_MAX;
            unsigned short theVertexToMaterial = UINT16_MAX;

            int32_t atlasIndex = -1;
            for (uint32_t j = 0; j < 3; j++)
            {
                uint32_t              vertexIndex = mesh.indexArray[f + j];
                const xatlas::Vertex& v = mesh.vertexArray[vertexIndex];
                vertexXref = v.xref + FirstVertexIndex;
                atlasIndex = v.atlasIndex;            // The same for every vertex in the triangle.
                materialIndex = vertexToMaterial->at(vertexXref) - FirstMaterialIndex;
                theVertexToMaterial = vertexToMaterial->at(vertexXref);
                pNewGeometry->triangles[triangleIndex].vertIndex[j] = vertexIndex;
            }

            RpMaterial* pMaterial = pGeometry->materials.materials[materialIndex];
            if (!pMaterial->texture)
            {
                size_t materialIndex = 0;
                if (!GetMaterialIndex(pNewGeometry, pMaterial, &materialIndex))
                {
                    printf("failed to look for materialIndex: %u (in new geometry)\n", materialIndex);
                    printf("GetMaterialIndex failed\n");
                    return nullptr;
                }
                pNewGeometry->triangles[triangleIndex].matIndex = materialIndex;
            }
            else
            {
                if (atlasIndex < 0)
                {
                    printf("invalid atlasIndex for triangle: %u\n", triangleIndex);
                    return nullptr;
                }

                pNewGeometry->triangles[triangleIndex].matIndex = atlasIndex;
            }
        }

        FirstVertexIndex += pGeometry->vertices_size;
        FirstMaterialIndex += pGeometry->materials.entries;

        RpAtomicSetGeometry(pAtomic, pNewGeometry, 0);
    }
    return pAtlasTexDictionary;
}

RpGeometry* CreateAtlasRpGeometry(RpGeometry* pOriginalGeometry, int numVerts, int numTriangles, int format)
{
    auto RpGeometryUnlock = (RpGeometry * (__cdecl*)(RpGeometry * geometry))0x74C800;
    auto RpGeometryCreate = (RpGeometry * (__cdecl*)(int numVerts, int numTriangles, int format))0x74CA90;
    auto _rpMaterialListCopy = (RpMaterials * (__cdecl*)(RpMaterials * matListOut, const RpMaterials* matListIn))0x74E1F0;

    CRenderWare* pRenderWare = g_pCore->GetGame()->GetRenderWare();

    RpGeometry* pGeometry = RpGeometryCreate(numVerts, numTriangles, format);

    if (!pGeometry)
    {
        printf(" failed to create geometry\n");
        return nullptr;
    }

    RpGeometryUnlock(pGeometry);

    if (pOriginalGeometry->triangles)
    {
        memcpy(pGeometry->triangles, pOriginalGeometry->triangles, sizeof(RpTriangle) * numTriangles);
    }

    _rpMaterialListCopy(&pGeometry->materials, &pOriginalGeometry->materials);

    pRenderWare->CopyGeometryPlugins(pGeometry, pOriginalGeometry);

    return pGeometry;
}

void OptimizeDFFFile(CIMGArchive* pIMgArchive, CIMGArchiveFile* newFile, CIDELoader& ideLoader)
{
    CRenderWare* pRenderWare = g_pCore->GetGame()->GetRenderWare();
    auto         RpClumpStreamGetSize = (unsigned int(__cdecl*)(RpClump*))0x74A5E0;

    ///*
    // REMOVE LATER
    int         modelID = 0;
    const char* pStrDFFName = "cj_bag_reclaim.dff";            //"infernus.dff";
    memcpy(newFile->fileEntry->fileName, pStrDFFName, strlen(pStrDFFName) + 1);

    RwTexDictionary* pTxdDictionary = pRenderWare->ReadTXD("cj_airprt.txd", CBuffer(), false);
    // REMOVE END
    // */

    const unsigned int uiDFFNameHash = HashString(newFile->fileEntry->fileName);

    SDFFDescriptor* pDFFDescriptor = ideLoader.GetDFFDescriptor(uiDFFNameHash);
    if (!pDFFDescriptor)
    {
        std::printf("couldn't find dff descriptor for '%s'\n", newFile->fileEntry->fileName);
        return;
    }

    // RwTexDictionary* pTxdDictionary = nullptr;

    /*
   STXDDescriptor*  pTXDDescriptor = pDFFDescriptor->GetTXDDescriptor();
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
       delete pTXDArchiveFile;
   }
   */

    std::vector<CTextureAtlas> vecTextureAtlases;

    /*
    int modelID = pDFFDescriptor->GetModelID();
    if (IsVehicleModel(modelID))
    {
        pRenderWare->CopyTexturesFromDictionary(pTxdDictionary, g_pVehicleTxdDictionary);
    }
    */

    pRenderWare->SetCurrentDFFWriteModelID(modelID);
    pRenderWare->SetCurrentReadDFFWithoutReplacingCOL(true);

    bool     bLoadCollision = IsVehicleModel(modelID);
    RpClump* pClump = pRenderWare->ReadDFF(newFile->fileEntry->fileName, CBuffer(), modelID, bLoadCollision, pTxdDictionary);
    // RpClump* pClump = pRenderWare->ReadDFF(newFile->fileEntry->fileName, newFile->fileByteBuffer, modelID, bLoadCollision, pTxdDictionary);
    pRenderWare->SetCurrentReadDFFWithoutReplacingCOL(false);
    if (pClump)
    {
        SString strPathOfGeneratedDff = "dffs\\";

        RwTexDictionary* pAtlasTxdDictionary = CreateTXDAtlas(pClump, vecTextureAtlases);
        if (!pAtlasTxdDictionary)
        {
            std::printf("failed to CREATE TXD atlas :(\n");
            return;
        }

        pRenderWare->WriteTXD(strPathOfGeneratedDff + "myatlas.txd", pAtlasTxdDictionary);

        unsigned int clumpSize = RpClumpStreamGetSize(pClump);

        // there's still an issue with size of empty extension headers of 12 btytes for clump
        clumpSize += 24;

        // free the memory
        CBuffer().Swap(newFile->fileByteBuffer);

        newFile->actualFileSize = GetActualFileSize(clumpSize);
        newFile->fileEntry->usSize = newFile->actualFileSize / 2048;
        // std::printf("dff file: %s  | new dff size: %u | actualFileSize: %d | capacity: %d\n",
        //    newFile->fileEntry->fileName, clumpSize, (int)newFile->actualFileSize, newFile->fileByteBuffer.GetCapacity());

        newFile->fileByteBuffer.SetSize(newFile->actualFileSize);
        void* pData = newFile->fileByteBuffer.GetData();
        // pRenderWare->WriteDFF(pData, newFile->actualFileSize, pClump);

        pRenderWare->WriteDFF(strPathOfGeneratedDff + "mydff.dff", pClump);

        if (bLoadCollision)
        {
            pRenderWare->DeleteReadDFFCollisionModel();
        }

        pRenderWare->DestroyDFF(pClump);
        pRenderWare->DestroyTXD(pAtlasTxdDictionary);
    }
    else
    {
        std::printf("failed to read %s\n", newFile->fileEntry->fileName);
    }

    pRenderWare->DestroyTXD(pTxdDictionary);
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
    newIMgArchive->ReadEntries();

    ideLoader.AddTXDDFFInfoToMaps(newIMgArchive);

    AddIgnoredDffHashesToSet();
    AddIgnoredTXDHashesToSet();

    CIMGArchive*                  newIMgArchiveOut = new CIMGArchive("proxy_test_gta3.img", IMG_FILE_WRITE);
    std::vector<CIMGArchiveFile*> imgArchiveFiles;
    for (DWORD i = 0; i < 1; i++)            // newIMgArchive->GetFileCount()
    {
        CIMGArchiveFile* newFile = newIMgArchive->GetFileByID(i);
        if (newFile != NULL)
        {
            SString strFileName = newFile->fileEntry->fileName;
            strFileName = strFileName.ToLower();

            pRenderWare->SetCurrentDFFBeingGeneratedFileName(strFileName);

            const char* pFileName = strFileName.c_str();
            memcpy(newFile->fileEntry->fileName, pFileName, strlen(pFileName) + 1);

            SString strFileExtension = strFileName.substr(strFileName.find_last_of(".") + 1);

            // check if it's a txd file
            if (strFileExtension == "txd")
            {
                // OptimizeTXDFile(newFile);
            }
            else if (strFileExtension == "dff")
            {
                OptimizeDFFFile(newIMgArchive, newFile, ideLoader);
            }

            imgArchiveFiles.push_back(newFile);
        }
    }

    newIMgArchiveOut->WriteEntries(imgArchiveFiles);

    for (auto& pArchiveFile : imgArchiveFiles)
    {
        delete pArchiveFile;
    }

    // delete newFile;
    delete newIMgArchive;
    delete newIMgArchiveOut;

    return true;
}
