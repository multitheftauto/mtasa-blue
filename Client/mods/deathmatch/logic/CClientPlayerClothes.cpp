/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientPlayerClothes.cpp
 *  PURPOSE:     Player entity clothes manager
 *
 *****************************************************************************/

#include <StdInc.h>

static const SPlayerClothingType g_clothesNames[PLAYER_CLOTHING_SLOTS] = {
    {"Torso"},           {"Hair"},     {"Legs"},       {"Shoes"},       {"Left Upper Arm"}, {"Left Lower Arm"}, {"Right Upper Arm"},
    {"Right Lower Arm"}, {"Back Top"}, {"Left Chest"}, {"Right Chest"}, {"Stomach"},        {"Lower Back"},     {"Extra1"},
    {"Extra2"},          {"Extra3"},   {"Extra4"},     {"Suit"}};

SFixedArray<std::vector<SPlayerClothing>, PLAYER_CLOTHING_SLOTS> CClientPlayerClothes::m_DefaultClothes
{
    //Torso Clothing
    std::vector<SPlayerClothing> {
        {"player_torso", "torso"}, {"vestblack", "vest"}, {"vest", "vest"}, {"tshirt2horiz", "tshirt2"}, {"tshirtwhite", "tshirt"},
        {"tshirtilovels", "tshirt"}, {"tshirtblunts", "tshirt"}, {"shirtbplaid", "shirtb"}, {"shirtbcheck", "shirtb"},
        {"field", "field"}, {"tshirterisyell", "tshirt"}, {"tshirterisorn", "tshirt"}, {"trackytop2eris", "trackytop1"},
        {"bbjackrim", "bbjack"}, {"bballjackrstar", "bbjack"}, {"baskballdrib", "baskball"}, {"baskballrim", "baskball"},
        {"sixtyniners", "tshirt"}, {"bandits", "baseball"}, {"tshirtprored", "tshirt"}, {"tshirtproblk", "tshirt"},
        {"trackytop1pro", "trackytop1"}, {"hockeytop", "sweat"}, {"bbjersey", "sleevt"}, {"shellsuit", "trackytop1"},
        {"tshirtheatwht", "tshirt"}, {"tshirtbobomonk", "tshirt"}, {"tshirtbobored", "tshirt"}, {"tshirtbase5", "tshirt"},
        {"tshirtsuburb", "tshirt"}, {"hoodyamerc", "hoodya"}, {"hoodyabase5", "hoodya"}, {"hoodyarockstar", "hoodya"},
        {"wcoatblue", "wcoat"}, {"coach", "coach"}, {"coachsemi", "coach"}, {"sweatrstar", "sweat"}, {"hoodyAblue", "hoodyA"},
        {"hoodyAblack", "hoodyA"}, {"hoodyAgreen", "hoodyA"}, {"sleevtbrown", "sleevt"}, {"shirtablue", "shirta"}, {"shirtayellow", "shirta"},
        {"shirtagrey", "shirta"}, {"shirtbgang", "shirtb"}, {"tshirtzipcrm", "tshirt"}, {"tshirtzipgry", "tshirt"}, {"denimfade", "denim"},
        {"bowling", "hawaii"}, {"hoodjackbeige", "hoodjack"}, {"baskballloc", "baskball"}, {"tshirtlocgrey", "tshirt"},
        {"tshirtmaddgrey", "tshirt"}, {"tshirtmaddgrn", "tshirt"}, {"suit1grey", "suit1"}, {"suit1blk", "suit1"}, {"leather", "leather"},
        {"painter", "painter"}, {"hawaiiwht", "hawaii"}, {"hawaiired", "hawaii"}, {"sportjack", "trackytop1"}, {"suit1red", "suit1"},
        {"suit1blue", "suit1"}, {"suit1yellow", "suit1"}, {"suit2grn", "suit2"}, {"tuxedo", "suit2"}, {"suit1gang", "suit1"}, {"letter", "sleevt"}
    },

    //Hair Clothing
    std::vector<SPlayerClothing> {
        {"player_face", "head"}, {"hairblond", "head"}, {"hairred", "head"}, {"hairblue", "head"}, {"hairgreen", "head"}, {"hairpink", "head"},
        {"bald", "head"}, {"baldbeard", "head"}, {"baldtash", "head"}, {"baldgoatee", "head"}, {"highfade", "head"}, {"highafro", "highafro"},
        {"wedge", "wedge"}, {"slope", "slope"}, {"jhericurl", "jheri"}, {"cornrows", "cornrows"}, {"cornrowsb", "cornrows"}, {"tramline", "tramline"},
        {"groovecut", "groovecut"}, {"mohawk", "mohawk"}, {"mohawkblond", "mohawk"}, {"mohawkpink", "mohawk"},
        {"mohawkbeard", "mohawk"}, {"afro", "afro"}, {"afrotash", "afro"}, {"afrobeard", "afro"}, {"afroblond", "afro"}, {"flattop", "flattop"},
        {"elvishair", "elvishair"}, {"beard", "head"}, {"tash", "head"}, {"goatee", "head"}, {"afrogoatee", "afro"}
    },

    //Legs Clothing
    std::vector<SPlayerClothing> {
        {"player_legs", "legs"}, {"worktrcamogrn", "worktr"}, {"worktrcamogry", "worktr"}, {"worktrgrey", "worktr"},
        {"worktrkhaki", "worktr"}, {"tracktr", "tracktr"}, {"tracktreris", "tracktr"}, {"jeansdenim", "jeans"},
        {"legsblack", "legs"}, {"legsheart", "legs"}, {"biegetr", "chinosb"}, {"tracktrpro", "tracktr"},
        {"tracktrwhstr", "tracktr"}, {"tracktrblue", "tracktr"}, {"tracktrgang", "tracktr"}, {"bbshortwht", "boxingshort"},
        {"boxshort", "boxingshort"}, {"bbshortred", "boxingshort"}, {"shellsuittr", "tracktr"}, {"shortsgrey", "shorts"},
        {"shortskhaki", "shorts"}, {"chongergrey", "chonger"}, {"chongergang", "chonger"}, {"chongerred", "chonger"},
        {"chongerblue", "chonger"}, {"shortsgang", "shorts"}, {"denimsgang", "jeans"}, {"denimsred", "jeans"},
        {"chinosbiege", "chinosb"}, {"chinoskhaki", "chinosb"}, {"cutoffchinos", "shorts"}, {"cutoffchinosblue", "shorts"},
        {"chinosblack", "chinosb"}, {"chinosblue", "chinosb"}, {"leathertr", "leathertr"}, {"leathertrchaps", "leathertr"},
        {"suit1trgrey", "suit1tr"}, {"suit1trblk", "suit1tr"}, {"cutoffdenims", "shorts"}, {"suit1trred", "suit1tr"},
        {"suit1trblue", "suit1tr"}, {"suit1tryellow", "suit1tr"}, {"suit1trgreen", "suit1tr"}, {"suit1trblk2", "suit1tr"},
        {"suit1trgang", "suit1tr"}
    },

    //Shoes Clothing
    std::vector<SPlayerClothing> {
        {"foot", "feet"}, {"cowboyboot2", "biker"}, {"bask2semi", "bask1"}, {"bask1eris", "bask1"}, {"sneakerbincgang", "sneaker"}, {"sneakerbincblu", "sneaker"},
        {"sneakerbincblk", "sneaker"}, {"sandal", "flipflop"}, {"sandalsock", "flipflop"}, {"flipflop", "flipflop"}, {"hitop", "bask1"}, {"convproblk", "conv"},
        {"convproblu", "conv"}, {"convprogrn", "conv"}, {"sneakerprored", "sneaker"}, {"sneakerproblu", "sneaker"}, {"sneakerprowht", "sneaker"},
        {"bask1prowht", "bask1"}, {"bask1problk", "bask1"}, {"boxingshoe", "biker"}, {"convheatblk", "conv"}, {"convheatred", "conv"}, {"convheatorn", "conv"},
        {"sneakerheatwht", "sneaker"}, {"sneakerheatgry", "sneaker"}, {"sneakerheatblk", "sneaker"}, {"bask2heatwht", "bask1"}, {"bask2heatband", "bask1"},
        {"timbergrey", "bask1"}, {"timberred", "bask1"}, {"timberfawn", "bask1"}, {"timberhike", "bask1"}, {"cowboyboot", "biker"}, {"biker", "biker"},
        {"snakeskin", "biker"}, {"shoedressblk", "shoe"}, {"shoedressbrn", "shoe"}, {"shoespatz", "shoe"}
    },

    //Left Upper Arm Clothing
    std::vector<SPlayerClothing> {
        {"4weed", "4WEED"}, {"4rip", "4RIP"}, {"4spider", "4SPIDER"}
    },

    //Left Lower Arm Clothing
    std::vector<SPlayerClothing> {
        {"5gun", "5GUN"}, {"5cross", "5CROSS"}, {"5cross2", "5CROSS2"},
        {"5cross3", "5CROSS3"}
    },

    //Right Upper Arm Clothing
    std::vector<SPlayerClothing> {
        {"6aztec", "6AZTEC"}, {"6crown", "6CROWN"}, {"6clown", "6CLOWN"},
        {"6africa", "6AFRICA"}
    },

    //Right LowerA rm Clothing
    std::vector<SPlayerClothing> {
        {"7cross", "7CROSS"}, {"7cross2", "7CROSS2"}, {"7cross3", "7CROSS3"},
        {"7mary", "7MARY"}
    },

    //Back Top Clothing
    std::vector<SPlayerClothing> {
        {"8sa", "8SA"}, {"8sa2", "8SA2"}, {"8sa3", "8SA3"},
        {"8westside", "8WESTSD"}, {"8santos", "8SANTOS"},
        {"8poker", "8POKER"}, {"8gun", "8GUN"}
    },

    //Left Chest Clothing
    std::vector<SPlayerClothing> {
        {"9crown", "9CROWN"}, {"9gun", "9GUN"}, {"9gun2", "9GUN2"},
        {"9homeboy", "9HOMBY"}, {"9bullet", "9BULLT"},
        {"9rasta", "9RASTA"}
    },

    //Right Chest Clothing
    std::vector<SPlayerClothing> {
        {"10ls", "10LS"}, {"10ls2", "10LS2"}, {"10ls3", "10LS3"},
        {"10ls4", "10LS4"}, {"10ls5", "10LS5"}, {"10og", "10OG"},
        {"10weed", "10WEED"}
    },

    //Stomach Clothing
    std::vector<SPlayerClothing> {
        {"11grove", "11GROVE"}, {"11grove2", "11GROV2"}, {"11grove3", "11GROV3"}, {"11dice", "11DICE"},
        {"11dice2", "11DICE2"}, {"11jail", "11JAIL"}, {"11godsgift", "11GGIFT"}
    },

    //Lower Back Clothing
    std::vector<SPlayerClothing> {
        {"12angels", "12ANGEL"}, {"12mayabird", "12MAYBR"}, {"12dagger", "12DAGER"},
        {"12bandit", "12BNDIT"}, {"12cross7", "12CROSS"}, {"12mayaface", "12MYFAC"},
    },

    //Extra1 Clothing
    std::vector<SPlayerClothing> {
        {"dogtag", "neck"}, {"neckafrica", "neck"}, {"stopwatch", "neck"}, {"necksaints", "neck"}, {"neckhash", "neck"}, {"necksilver", "neck2"},
        {"neckgold", "neck2"}, {"neckropes", "neck2"}, {"neckropeg", "neck2"}, {"neckls", "neck"}, {"neckdollar", "neck"}, {"neckcross", "neck"}
    },

    //Extra2 Clothing
    std::vector<SPlayerClothing> {
        {"watchpink", "watch"}, {"watchyellow", "watch"}, {"watchpro", "watch"}, {"watchpro2", "watch"}, {"watchsub1", "watch"},
        {"watchsub2", "watch"}, {"watchzip1", "watch"}, {"watchzip2", "watch"}, {"watchgno", "watch"}, {"watchgno2", "watch"},
        {"watchcro", "watch"}, {"watchcro2", "watch"}
    },

    //Extra3 Clothing
    std::vector<SPlayerClothing> {
        {"groucho", "grouchos"}, {"zorro", "zorromask"}, {"eyepatch", "eyepatch"},
        {"glasses01", "glasses01"}, {"glasses04", "glasses04"}, {"bandred3", "bandmask"},
        {"bandblue3", "bandmask"}, {"bandgang3", "bandmask"}, {"bandblack3", "bandmask"},
        {"glasses01dark", "glasses01"}, {"glasses04dark", "glasses04"}, {"glasses03", "glasses03"},
        {"glasses03red", "glasses03"}, {"glasses03blue", "glasses03"}, {"glasses03dark", "glasses03"},
        {"glasses05dark", "glasses03"}, {"glasses05", "glasses03"}
    },

    //Extra4 Clothing
    std::vector<SPlayerClothing> {
        {"bandred", "bandana"}, {"bandblue", "bandana"}, {"bandgang", "bandana"}, {"bandblack", "bandana"}, {"bandred2", "bandknots"},
        {"bandblue2", "bandknots"}, {"bandblack2", "bandknots"}, {"bandgang2", "bandknots"}, {"capknitgrn", "capknit"}, {"captruck", "captruck"},
        {"cowboy", "cowboy"}, {"hattiger", "cowboy"}, {"helmet", "helmet"}, {"moto", "moto"}, {"boxingcap", "boxingcap"}, {"hockey", "hockeymask"}, {"capgang", "cap"},
        {"capgangback", "capback"}, {"capgangside", "capside"}, {"capgangover", "capovereye"}, {"capgangup", "caprimup"}, {"bikerhelmet", "bikerhelmet"},
        {"capred", "cap"}, {"capredback", "capback"}, {"capredside", "capside"}, {"capredover", "capovereye"}, {"capredup", "caprimup"}, {"capblue", "cap"},
        {"capblueback", "capback"}, {"capblueside", "capside"}, {"capblueover", "capovereye"}, {"capblueup", "caprimup"}, {"skullyblk", "skullycap"},
        {"skullygrn", "skullycap"}, {"hatmancblk", "hatmanc"}, {"hatmancplaid", "hatmanc"}, {"capzip", "cap"}, {"capzipback", "capback"}, {"capzipside", "capside"},
        {"capzipover", "capovereye"}, {"capzipup", "caprimup"}, {"beretred", "beret"}, {"beretblk", "beret"}, {"capblk", "cap"}, {"capblkback", "capback"},
        {"capblkside", "capside"}, {"capblkover", "capovereye"}, {"capblkup", "caprimup"}, {"trilbydrk", "trilby"}, {"trilbylght", "trilby"},
        {"bowler", "bowler"}, {"bowlerred", "bowler"}, {"bowlerblue", "bowler"}, {"bowleryellow", "bowler"}, {"boater", "boater"}, {"bowlergang", "bowler"},
        {"boaterblk", "boater"}
    },

    // Suit Clothing
    std::vector<SPlayerClothing> {
        {"gimpleg", "gimpleg"}, {"valet", "valet"}, {"countrytr", "countrytr"}, {"croupier", "valet"},
        {"policetr", "policetr"}, {"balaclava", "balaclava"}, {"pimptr", "pimptr"},
        {"garageleg", "garagetr"}, {"medictr", "medictr"}
    }
};

// This represents GTA's 1 clothing block
SFixedArray<const SPlayerClothing*, PLAYER_CLOTHING_SLOTS>          CClientPlayerClothes::m_GlobalClothes;
SFixedArray<std::list<SPlayerClothing>, PLAYER_CLOTHING_SLOTS>      CClientPlayerClothes::m_NewClothes;
bool                                                                CClientPlayerClothes::m_bStaticInit = true;
bool                                                                CClientPlayerClothes::m_bHasClothesChanged = false;

CClientPlayerClothes::CClientPlayerClothes(CClientPed* pPlayerModel)
{
    m_pPlayerModel = pPlayerModel;
    memset(&m_Clothes[0], 0, sizeof(m_Clothes));

    if (m_bStaticInit)
    {
        m_bStaticInit = false;
        memset(&m_GlobalClothes[0], 0, sizeof(m_GlobalClothes));
    }
}

CClientPlayerClothes::~CClientPlayerClothes()
{
    RemoveAll(false);
}

const SPlayerClothing* CClientPlayerClothes::GetClothing(unsigned char ucType)
{
    if (ucType < PLAYER_CLOTHING_SLOTS)
    {
        return m_Clothes[ucType];
    }

    return NULL;
}

void CClientPlayerClothes::AddClothes(const char* szTexture, const char* szModel, unsigned char ucType, bool bAddToModel)
{
    if (ucType >= PLAYER_CLOTHING_SLOTS)
        return;

    const SPlayerClothing* pClothing = GetClothing(szTexture, szModel, ucType);
    if (pClothing && pClothing != m_Clothes[ucType])
    {
        RemoveClothes(ucType, bAddToModel);
        m_Clothes[ucType] = pClothing;

        if (bAddToModel)
        {
            InternalAddClothes(pClothing, ucType);
        }
    }
}

void CClientPlayerClothes::InternalAddClothes(const SPlayerClothing* pClothing, unsigned char ucType)
{
    if (m_pPlayerModel)
    {
        CPlayerPed* pPlayerPed = m_pPlayerModel->GetGamePlayer();
        if (pPlayerPed)
        {
            if (pClothing && !IsEmptyClothing(pClothing, ucType))
            {
                pPlayerPed->SetClothesTextureAndModel(pClothing->texture.c_str(), pClothing->model.c_str(), ucType);
            }
            else
            {
                pPlayerPed->SetClothesTextureAndModel(NULL, NULL, ucType);
            }
            // Update our static clothing block
            m_GlobalClothes[ucType] = pClothing;
        }
    }
}

bool CClientPlayerClothes::RemoveClothes(unsigned char ucType, bool bRemoveFromModel)
{
    if (ucType >= PLAYER_CLOTHING_SLOTS)
        return false;

    // Do we have any set clothes on this slot?
    if (m_Clothes[ucType])
    {
        // Can we replace them with empty-type clothes (eg: player_torso)
        if (HasEmptyClothing(ucType))
        {
            const std::vector<const SPlayerClothing*> pGroup = GetClothingGroup(ucType);
            m_Clothes[ucType] = pGroup.at(0);
        }
        else
        {
            m_Clothes[ucType] = NULL;
        }

        // Remove them from the model now?
        if (bRemoveFromModel)
        {
            InternalAddClothes(NULL, ucType);
        }

        return true;
    }
    return false;
}

void CClientPlayerClothes::AddAllToModel()
{
    if (m_pPlayerModel)
    {
        CPlayerPed* pPlayerPed = m_pPlayerModel->GetGamePlayer();
        if (pPlayerPed)
        {
            for (unsigned char ucType = 0; ucType < PLAYER_CLOTHING_SLOTS; ucType++)
            {
                const SPlayerClothing* pPrevious = m_GlobalClothes[ucType];
                const SPlayerClothing* pCurrent = m_Clothes[ucType];
                if (pCurrent)
                {
                    if (!pPrevious || pPrevious != pCurrent)
                    {
                        InternalAddClothes(pCurrent, ucType);
                    }
                }
                else if (pPrevious)
                {
                    InternalAddClothes(NULL, ucType);
                }
            }
        }
    }
}

void CClientPlayerClothes::RemoveAll(bool bRemoveFromModel)
{
    for (unsigned char ucType = 0; ucType < PLAYER_CLOTHING_SLOTS; ucType++)
    {
        RemoveClothes(ucType, bRemoveFromModel);
    }
}

void CClientPlayerClothes::DefaultClothes(bool bAddToModel)
{
    RemoveAll();

    AddClothes("vestblack", "vest", 0, bAddToModel);
    AddClothes("JEANSDENIM", "JEANS", 2, bAddToModel);
    AddClothes("SNEAKERBINCBLK", "SNEAKER", 3, bAddToModel);
    AddClothes("PLAYER_FACE", "HEAD", 1, bAddToModel);
}

bool CClientPlayerClothes::HasEmptyClothing(unsigned char ucType)
{
    return (ucType == 0 || ucType == 1 || ucType == 2 || ucType == 3);
}

bool CClientPlayerClothes::IsEmptyClothing(const SPlayerClothing* pClothing, unsigned char ucType)
{
    if (!pClothing || ucType > 3) 
        return false;

    const std::vector<const SPlayerClothing*> pGroup = GetClothingGroup(ucType);

    if (pGroup.empty())
        return false;

    return pClothing == pGroup.at(0);
}

const char* CClientPlayerClothes::GetClothingName(unsigned char ucType)
{
    if (ucType < PLAYER_CLOTHING_SLOTS)
    {
        return g_clothesNames[ucType].szName;
    }
    return NULL;
}

std::vector<const SPlayerClothing*> CClientPlayerClothes::GetClothingGroup(unsigned char ucType)
{
    std::vector<const SPlayerClothing*> clothes;

    if (ucType < PLAYER_CLOTHING_SLOTS)
    {
        for (auto& clothing : m_DefaultClothes[ucType])
        {
            clothes.push_back(&clothing);
        }

        for (auto& clothing : m_NewClothes[ucType])
        {
            clothes.push_back(&clothing);
        }
    }

    return clothes;
}

const SPlayerClothing* CClientPlayerClothes::GetClothing(const char* szTexture, const char* szModel, unsigned char ucType)
{
    if (!szTexture || !szModel || ucType >= PLAYER_CLOTHING_SLOTS)
        return nullptr;

    std::vector<const SPlayerClothing*> pGroup = GetClothingGroup(ucType);

    if (pGroup.empty())
        return nullptr;

    for (const auto& clothing : pGroup)
    {
        if (!stricmp(szTexture, clothing->texture.c_str()) && !stricmp(szModel, clothing->model.c_str()))
        {
            return clothing;
        }
    }

    return nullptr;
}

bool CClientPlayerClothes::IsValidModel(unsigned short usModel)
{
    return usModel >= CLOTHES_MODEL_ID_FIRST && usModel <= CLOTHES_MODEL_ID_LAST;
}

bool CClientPlayerClothes::AddClothingModel(const char* texture, const char* model, unsigned char clothingType)
{
    if (clothingType < PLAYER_CLOTHING_SLOTS)
    {
        if (texture == nullptr || model == nullptr)
            return false;

        std::string textureFile = std::string(texture) + ".txd";

        if (!g_pGame->GetRenderWare()->HasClothesFile(textureFile.c_str()))
            return false;

        std::string modelFile = std::string(model) + ".dff";

        if (!g_pGame->GetRenderWare()->HasClothesFile(modelFile.c_str()))
            return false;

        auto& clothes = m_NewClothes[clothingType];

        if (std::any_of(clothes.begin(), clothes.end(), [&](const SPlayerClothing& clothing) {
            return !stricmp(texture, clothing.texture.c_str()) && !stricmp(model, clothing.model.c_str());
        }))
        {
            return false;
        }

        clothes.push_back({texture, model});
        return true;
    }

    return false;
}

bool CClientPlayerClothes::RemoveClothingModel(const char* texture, const char* model, unsigned char clothingType)
{
    if (clothingType < PLAYER_CLOTHING_SLOTS)
    {
        if (texture == nullptr || model == nullptr)
            return false;

        auto& clothes = m_NewClothes[clothingType];

        auto it = std::find_if(clothes.begin(), clothes.end(),[&](const SPlayerClothing& clothing) {
            return !stricmp(texture, clothing.texture.c_str()) && !stricmp(model, clothing.model.c_str());
        });

        if (it == clothes.end())
            return false;

        clothes.erase(it);
        m_bHasClothesChanged = true;

        return true;
    }

    return false;
}

bool CClientPlayerClothes::HasClothesChanged()
{
    return m_bHasClothesChanged;
}

void CClientPlayerClothes::RefreshClothes()
{
    for (std::uint8_t clothingType = 0; clothingType < PLAYER_CLOTHING_SLOTS; clothingType++)
    {
       auto& clothes = m_NewClothes[clothingType];

       if (clothes.empty() && !m_bHasClothesChanged)
           continue;

       bool                   hasInvalidClothing = false;
       const SPlayerClothing* pCurrent = m_Clothes[clothingType];

       if (!m_bHasClothesChanged)
       {
           for (auto clothing = clothes.begin(); clothing != clothes.end();)
           {
               std::string fileTXD = clothing->texture + ".txd";
               std::string fileDFF = clothing->model + ".dff";

               if (!g_pGame->GetRenderWare()->HasClothesFile(fileTXD.c_str()) || !g_pGame->GetRenderWare()->HasClothesFile(fileDFF.c_str()))
               {
                   if (pCurrent && (pCurrent->texture == clothing->texture || pCurrent->model == clothing->model))
                   {
                        hasInvalidClothing = true;
                   }

                   clothing = clothes.erase(clothing);
               }
               else
                   ++clothing;
           }
       }

       if (pCurrent && !hasInvalidClothing && m_bHasClothesChanged)
       {
           const SPlayerClothing* pClothing = GetClothing(pCurrent->texture.c_str(), pCurrent->model.c_str(), clothingType);

           if (!pClothing)
           {
               hasInvalidClothing = true;
           }
       }

       if (hasInvalidClothing)
       {
           RemoveClothes(clothingType, true);
       }
    }

    m_bHasClothesChanged = false;
}
