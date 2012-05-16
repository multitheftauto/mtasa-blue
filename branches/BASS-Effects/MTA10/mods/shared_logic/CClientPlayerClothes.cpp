/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPlayerClothes.cpp
*  PURPOSE:     Player entity clothes manager
*  DEVELOPERS:  Jax <>
*               Christian Myhre Lundheim <>
*
*****************************************************************************/

#include <StdInc.h>

static const SPlayerClothingType g_clothesNames [ PLAYER_CLOTHING_SLOTS ] =
{ { "Torso" }, { "Hair" }, { "Legs" }, { "Shoes" }, { "Left Upper Arm" },
{ "Left Lower Arm" }, { "Right Upper Arm" }, { "Right Lower Arm" },
{ "Back Top" }, { "Left Chest" }, { "Right Chest" }, { "Stomach" },
{ "Lower Back" }, { "Extra1" }, { "Extra2" }, { "Extra3" }, { "Extra4" }, 
{ "Suit" } };

static const SPlayerClothing g_TorsoClothing [ TORSO_CLOTHING_MAX + 1 ] =
{ { "player_torso", "torso" }, { "vestblack", "vest" }, { "vest", "vest" },
{ "tshirt2horiz", "tshirt2" }, { "tshirtwhite", "tshirt" },
{ "tshirtilovels", "tshirt" }, { "tshirtblunts", "tshirt" },
{ "shirtbplaid", "shirtb" }, { "shirtbcheck", "shirtb" }, { "field", "field" },
{ "tshirterisyell", "tshirt" }, { "tshirterisorn", "tshirt" },
{ "trackytop2eris", "trackytop1" }, { "bbjackrim", "bbjack" },
{ "bballjackrstar", "bbjack" }, { "baskballdrib", "baskball" },
{ "baskballrim", "baskball" }, { "sixtyniners", "tshirt" },
{ "bandits", "baseball" }, { "tshirtprored", "tshirt" },
{ "tshirtproblk", "tshirt" }, { "trackytop1pro", "trackytop1" },
{ "hockeytop", "sweat" }, { "bbjersey", "sleevt" },
{ "shellsuit", "trackytop1" }, { "tshirtheatwht", "tshirt" },
{ "tshirtbobomonk", "tshirt" }, { "tshirtbobored", "tshirt" },
{ "tshirtbase5", "tshirt" }, { "tshirtsuburb", "tshirt" },
{ "hoodyamerc", "hoodya" }, { "hoodyabase5", "hoodya" },
{ "hoodyarockstar", "hoodya" }, { "wcoatblue", "wcoat" }, { "coach", "coach" },
{ "coachsemi", "coach" }, { "sweatrstar", "sweat" }, { "hoodyAblue", "hoodyA" },
{ "hoodyAblack", "hoodyA" }, { "hoodyAgreen", "hoodyA" },
{ "sleevtbrown", "sleevt" }, { "shirtablue", "shirta" },
{ "shirtayellow", "shirta" }, { "shirtagrey", "shirta" },
{ "shirtbgang", "shirtb" }, { "tshirtzipcrm", "tshirt" },
{ "tshirtzipgry", "tshirt" }, { "denimfade", "denim" }, { "bowling", "hawaii" },
{ "hoodjackbeige", "hoodjack" }, { "baskballloc", "baskball" },
{ "tshirtlocgrey", "tshirt" }, { "tshirtmaddgrey", "tshirt" },
{ "tshirtmaddgrn", "tshirt" }, { "suit1grey", "suit1" },
{ "suit1blk", "suit1" }, { "leather", "leather" }, { "painter", "painter" },
{ "hawaiiwht", "hawaii" }, { "hawaiired", "hawaii" },
{ "sportjack", "trackytop1" }, { "suit1red", "suit1" }, { "suit1blue", "suit1" },
{ "suit1yellow", "suit1" }, { "suit2grn", "suit2" }, { "tuxedo", "suit2" },
{ "suit1gang", "suit1" }, { "letter", "sleevt" }, { NULL, NULL } };

static const SPlayerClothing g_HairClothing [ HAIR_CLOTHING_MAX + 1 ] =
{ { "player_face", "head" }, { "hairblond", "head" }, { "hairred", "head" },
{ "hairblue", "head" }, { "hairgreen", "head" }, { "hairpink", "head" },
{ "bald", "head" }, { "baldbeard", "head" }, { "baldtash", "head" },
{ "baldgoatee", "head" }, { "highfade", "head" }, { "highafro", "highafro" },
{ "wedge", "wedge" }, { "slope", "slope" }, { "jhericurl", "jheri" },
{ "cornrows", "cornrows" }, { "cornrowsb", "cornrows" },
{ "tramline", "tramline" }, { "groovecut", "groovecut" },
{ "mohawk", "mohawk" }, { "mohawkblond", "mohawk" },
{ "mohawkpink", "mohawk" }, { "mohawkbeard", "mohawk" }, { "afro", "afro" },
{ "afrotash", "afro" }, { "afrobeard", "afro" }, { "afroblond", "afro" },
{ "flattop", "flattop" }, { "elvishair", "elvishair" }, { "beard", "head" },
{ "tash", "head" }, { "goatee", "head" }, { "afrogoatee", "afro" }, { NULL, NULL } };

static const SPlayerClothing g_LegsClothing [ LEGS_CLOTHING_MAX + 1 ] =
{ { "player_legs", "legs" }, { "worktrcamogrn", "worktr" },
{ "worktrcamogry", "worktr" }, { "worktrgrey", "worktr" }, { "worktrkhaki", "worktr" },
{ "tracktr", "tracktr" }, { "tracktreris", "tracktr" },
{ "jeansdenim", "jeans" }, { "legsblack", "legs" }, { "legsheart", "legs" },
{ "biegetr", "chinosb" }, { "tracktrpro", "tracktr" },
{ "tracktrwhstr", "tracktr" }, { "tracktrblue", "tracktr" },
{ "tracktrgang", "tracktr" }, { "bbshortwht", "boxingshort" },
{ "boxshort", "boxingshort" }, { "bbshortred", "boxingshort" },
{ "shellsuittr", "tracktr" }, { "shortsgrey", "shorts" },
{ "shortskhaki", "shorts" }, { "chongergrey", "chonger" },
{ "chongergang", "chonger" }, { "chongerred", "chonger" },
{ "chongerblue", "chonger" }, { "shortsgang", "shorts" },
{ "denimsgang", "jeans" }, { "denimsred", "jeans" },
{ "chinosbiege", "chinosb" }, { "chinoskhaki", "chinosb" },
{ "cutoffchinos", "shorts" }, { "cutoffchinosblue", "shorts" },
{ "chinosblack", "chinosb" }, { "chinosblue", "chinosb" },
{ "leathertr", "leathertr" }, { "leathertrchaps", "leathertr" },
{ "suit1trgrey", "suit1tr" }, { "suit1trblk", "suit1tr" },
{ "cutoffdenims", "shorts" }, { "suit1trred", "suit1tr" },
{ "suit1trblue", "suit1tr" }, { "suit1tryellow", "suit1tr" },
{ "suit1trgreen", "suit1tr" }, { "suit1trblk2", "suit1tr" },
{ "suit1trgang", "suit1tr" }, { NULL, NULL } };

static const SPlayerClothing g_ShoesClothing [ SHOES_CLOTHING_MAX + 1 ] =
{ { "foot", "feet" }, { "cowboyboot2", "biker" }, { "bask2semi", "bask1" },
{ "bask1eris", "bask1" }, { "sneakerbincgang", "sneaker" },
{ "sneakerbincblu", "sneaker" }, { "sneakerbincblk", "sneaker" },
{ "sandal", "flipflop" }, { "sandalsock", "flipflop" },
{ "flipflop", "flipflop" }, { "hitop", "bask1" }, { "convproblk", "conv" },
{ "convproblu", "conv" }, { "convprogrn", "conv" },
{ "sneakerprored", "sneaker" }, { "sneakerproblu", "sneaker" },
{ "sneakerprowht", "sneaker" }, { "bask1prowht", "bask1" },
{ "bask1problk", "bask1" }, { "boxingshoe", "biker" },
{ "convheatblk", "conv" }, { "convheatred", "conv" },
{ "convheatorn", "conv" }, { "sneakerheatwht", "sneaker" },
{ "sneakerheatgry", "sneaker" }, { "sneakerheatblk", "sneaker" },
{ "bask2heatwht", "bask1" }, { "bask2heatband", "bask1" },
{ "timbergrey", "bask1" }, { "timberred", "bask1" },
{ "timberfawn", "bask1" }, { "timberhike", "bask1" },
{ "cowboyboot", "biker" }, { "biker", "biker" }, { "snakeskin", "biker" },
{ "shoedressblk", "shoe" }, { "shoedressbrn", "shoe" }, { "shoespatz", "shoe" },
{ NULL, NULL } };

static const SPlayerClothing g_LeftUpperArmClothing [ LEFT_UPPER_ARM_CLOTHING_MAX + 1 ] =
{ { "4weed", "4WEED" }, { "4rip", "4RIP" }, { "4spider", "4SPIDER" }, { NULL, NULL } };

static const SPlayerClothing g_LeftLowerArmClothing [ LEFT_LOWER_ARM_CLOTHING_MAX + 1 ] =
{ { "5gun", "5GUN" }, { "5cross", "5CROSS" }, { "5cross2", "5CROSS2" },
{ "5cross3", "5CROSS3" }, { NULL, NULL } };

static const SPlayerClothing g_RightUpperArmClothing [ RIGHT_UPPER_ARM_CLOTHING_MAX + 1 ] =
{ { "6aztec", "6AZTEC" }, { "6crown", "6CROWN" }, { "6clown", "6CLOWN" },
{ "6africa", "6AFRICA" }, { NULL, NULL } };

static const SPlayerClothing g_RightLowerArmClothing [ RIGHT_LOWER_ARM_CLOTHING_MAX + 1 ] =
{ { "7cross", "7CROSS" }, { "7cross2", "7CROSS2" }, { "7cross3", "7CROSS3" },
{ "7mary", "7MARY" }, { NULL, NULL } };

static const SPlayerClothing g_BackTopClothing [ BACK_TOP_CLOTHING_MAX + 1 ] =
{ { "8sa", "8SA" }, { "8sa2", "8SA2" }, { "8sa3", "8SA3" },
{ "8westside", "8WESTSD" }, { "8santos", "8SANTOS" },
{ "8poker", "8POKER" }, { "8gun", "8GUN" }, { NULL, NULL } };

static const SPlayerClothing g_LeftChestClothing [ LEFT_CHEST_CLOTHING_MAX + 1 ] =
{ { "9crown", "9CROWN" }, { "9gun", "9GUN" }, { "9gun2", "9GUN2" },
{ "9homeboy", "9HOMBY" }, { "9bullet", "9BULLT" }, { "9rasta", "9RASTA" },
{ NULL, NULL } }; 

static const SPlayerClothing g_RightChestClothing [ RIGHT_CHEST_CLOTHING_MAX + 1 ] =
{ { "10ls", "10LS" }, { "10ls2", "10LS2" }, { "10ls3", "10LS3" },
{ "10ls4", "10LS4" }, { "10ls5", "10LS5" }, { "10og", "10OG" },
{ "10weed", "10WEED" }, { NULL, NULL } };

static const SPlayerClothing g_StomachClothing [ STOMACH_CLOTHING_MAX + 1 ] =
{ { "11grove", "11GROVE" }, { "11grove2", "11GROV2" },
{ "11grove3", "11GROV3" }, { "11dice", "11DICE" }, { "11dice2", "11DICE2" },
{ "11jail", "11JAIL" }, { "11godsgift", "11GGIFT" }, { NULL, NULL } };

static const SPlayerClothing g_LowerBackClothing [ LOWER_BACK_CLOTHING_MAX + 1 ] =
{ { "12angels", "12ANGEL" }, { "12mayabird", "12MAYBR" },
{ "12dagger", "12DAGER" }, { "12bandit", "12BNDIT" },
{ "12cross7", "12CROSS" }, { "12mayaface", "12MYFAC" }, { NULL, NULL } };

static const SPlayerClothing g_Extra1Clothing [ EXTRA1_CLOTHING_MAX + 1 ] =
{ { "dogtag", "neck" }, { "neckafrica", "neck" }, { "stopwatch", "neck" },
{ "necksaints", "neck" }, { "neckhash", "neck" }, { "necksilver", "neck2" },
{ "neckgold", "neck2" }, { "neckropes", "neck2" }, { "neckropeg", "neck2" },
{ "neckls", "neck" }, { "neckdollar", "neck" }, { "neckcross", "neck" },
{ NULL, NULL } };

static const SPlayerClothing g_Extra2Clothing [ EXTRA2_CLOTHING_MAX + 1 ] =
{ { "watchpink", "watch" }, { "watchyellow", "watch" },
{ "watchpro", "watch" }, { "watchpro2", "watch" }, { "watchsub1", "watch" },
{ "watchsub2", "watch" }, { "watchzip1", "watch" }, { "watchzip2", "watch" },
{ "watchgno", "watch" }, { "watchgno2", "watch" }, { "watchcro", "watch" },
{ "watchcro2", "watch" }, { NULL, NULL } };

static const SPlayerClothing g_Extra3Clothing [ EXTRA3_CLOTHING_MAX + 1 ] =
{ { "groucho", "grouchos" }, { "zorro", "zorromask" },
{ "eyepatch", "eyepatch" }, { "glasses01", "glasses01" },
{ "glasses04", "glasses04" }, { "bandred3", "bandmask" },
{ "bandblue3", "bandmask" }, { "bandgang3", "bandmask" },
{ "bandblack3", "bandmask" }, { "glasses01dark", "glasses01" },
{ "glasses04dark", "glasses04" }, { "glasses03", "glasses03" },
{ "glasses03red", "glasses03" }, { "glasses03blue", "glasses03" },
{ "glasses03dark", "glasses03" }, { "glasses05dark", "glasses03" },
{ "glasses05", "glasses03" }, { NULL, NULL } };

static const SPlayerClothing g_Extra4Clothing [ EXTRA4_CLOTHING_MAX + 1 ] =
{ { "bandred", "bandana" }, { "bandblue", "bandana" },
{ "bandgang", "bandana" }, { "bandblack", "bandana" },
{ "bandred2", "bandknots" }, { "bandblue2", "bandknots" },
{ "bandblack2", "bandknots" }, { "bandgang2", "bandknots" },
{ "capknitgrn", "capknit" }, { "captruck", "captruck" },
{ "cowboy", "cowboy" }, { "hattiger", "cowboy" }, { "helmet", "helmet" },
{ "moto", "moto" }, { "boxingcap", "boxingcap" }, { "hockey", "hockeymask" },
{ "capgang", "cap" }, { "capgangback", "capback" }, { "capgangside", "capside" },
{ "capgangover", "capovereye" }, { "capgangup", "caprimup" },
{ "bikerhelmet", "bikerhelmet" }, { "capred", "cap" }, { "capredback", "capback" },
{ "capredside", "capside" }, { "capredover", "capovereye" },
{ "capredup", "caprimup" }, { "capblue", "cap" }, { "capblueback", "capback" },
{ "capblueside", "capside" }, { "capblueover", "capovereye" },
{ "capblueup", "caprimup" }, { "skullyblk", "skullycap" },
{ "skullygrn", "skullycap" }, { "hatmancblk", "hatmanc" },
{ "hatmancplaid", "hatmanc" }, { "capzip", "cap" }, { "capzipback", "capback" },
{ "capzipside", "capside" }, { "capzipover", "capovereye" },
{ "capzipup", "caprimup" }, { "beretred", "beret" }, { "beretblk", "beret" },
{ "capblk", "cap" }, { "capblkback", "capback" }, { "capblkside", "capside" },
{ "capblkover", "capovereye" }, { "capblkup", "caprimup" },
{ "trilbydrk", "trilby" }, { "trilbylght", "trilby" }, { "bowler", "bowler" },
{ "bowlerred", "bowler" }, { "bowlerblue", "bowler" },
{ "bowleryellow", "bowler" }, { "boater", "boater" }, { "bowlergang", "bowler" }, 
{ "boaterblk", "boater" }, { NULL, NULL } };

static const SPlayerClothing g_SuitClothing [ SUIT_CLOTHING_MAX + 1 ] =
{ { "gimpleg", "gimpleg" }, { "valet", "valet" }, { "countrytr", "countrytr" },
{ "croupier", "valet" }, { "policetr", "policetr" }, { "balaclava", "balaclava" },
{ "pimptr", "pimptr" }, { "garageleg", "garagetr" }, { "medictr", "medictr" },
{ NULL, NULL } };

// This represents GTA's 1 clothing block
SFixedArray < const SPlayerClothing*, PLAYER_CLOTHING_SLOTS > CClientPlayerClothes::m_GlobalClothes;
bool CClientPlayerClothes::m_bStaticInit = true;

CClientPlayerClothes::CClientPlayerClothes ( CClientPed* pPlayerModel )
{
    m_pPlayerModel = pPlayerModel;
    memset ( &m_Clothes[0], 0, sizeof ( m_Clothes ) );

    if ( m_bStaticInit )
    {
        m_bStaticInit = false;
        memset ( &m_GlobalClothes[0], 0, sizeof ( m_GlobalClothes ) );
    }
}


CClientPlayerClothes::~CClientPlayerClothes ( void )
{
    RemoveAll ( false );
}


const SPlayerClothing* CClientPlayerClothes::GetClothing ( unsigned char ucType )
{
    if ( ucType < PLAYER_CLOTHING_SLOTS )
    {
        return m_Clothes [ ucType ];
    }

    return NULL;
}


void CClientPlayerClothes::AddClothes ( const char* szTexture, const char* szModel, unsigned char ucType, bool bAddToModel )
{
    const SPlayerClothing * pClothing = GetClothing ( szTexture, szModel, ucType );
    if ( pClothing && pClothing != m_Clothes [ ucType ] )
    {
        RemoveClothes ( ucType, bAddToModel );
        m_Clothes [ ucType ] = pClothing;        

        if ( bAddToModel )
        {
            InternalAddClothes ( pClothing, ucType );
        }
    }
}


void CClientPlayerClothes::InternalAddClothes ( const SPlayerClothing * pClothing, unsigned char ucType )
{
    if ( g_pCore->GetDiagnosticDebug () == EDiagnosticDebug::FPS_6934 )
        return;

    if ( m_pPlayerModel )
    {
        CPlayerPed* pPlayerPed = m_pPlayerModel->GetGamePlayer ();
        if ( pPlayerPed )
        {
            if ( pClothing && !IsEmptyClothing ( pClothing, ucType ) )
            {
                pPlayerPed->SetClothesTextureAndModel ( pClothing->szTexture, pClothing->szModel, ucType );
            }
            else
            {
                pPlayerPed->SetClothesTextureAndModel ( NULL, NULL, ucType );
            }
            // Update our static clothing block
            m_GlobalClothes [ ucType ] = pClothing;
        }
    }
}


bool CClientPlayerClothes::RemoveClothes ( unsigned char ucType, bool bRemoveFromModel )
{
    // Do we have any set clothes on this slot?
    if ( m_Clothes [ ucType ] )
    {
        // Can we replace them with empty-type clothes (eg: player_torso)
        if ( HasEmptyClothing ( ucType ) )
        {
            const SPlayerClothing* pGroup = GetClothingGroup ( ucType );
            m_Clothes [ ucType ] = &pGroup [ 0 ];
        }
        else
        {
            m_Clothes [ ucType ] = NULL;
        }

        // Remove them from the model now?
        if ( bRemoveFromModel )
        {
            InternalAddClothes ( NULL, ucType );                        
        }        
        return true;
    }
    return false;
}


void CClientPlayerClothes::AddAllToModel ( void )
{
    if ( m_pPlayerModel )
    {
        CPlayerPed* pPlayerPed = m_pPlayerModel->GetGamePlayer ();
        if ( pPlayerPed )
        {         
            for ( unsigned char ucType = 0 ; ucType < PLAYER_CLOTHING_SLOTS ; ucType++ )
            {
                const SPlayerClothing* pPrevious = m_GlobalClothes [ ucType ];
                const SPlayerClothing* pCurrent = m_Clothes [ ucType ];
                if ( pCurrent )
                {
                    if ( !pPrevious || pPrevious != pCurrent )
                    {
                        InternalAddClothes ( pCurrent, ucType );
                    }
                }
                else if ( pPrevious )
                {
                    InternalAddClothes ( NULL, ucType );
                }
            }
        }
    }
}


void CClientPlayerClothes::RemoveAll ( bool bRemoveFromModel )
{
    for ( unsigned char ucType = 0 ; ucType < PLAYER_CLOTHING_SLOTS ; ucType++ )
    {
        RemoveClothes ( ucType, bRemoveFromModel );
    }
}


void CClientPlayerClothes::DefaultClothes ( bool bAddToModel )
{
    RemoveAll ();

    AddClothes ( "vestblack", "vest", 0, bAddToModel );
    AddClothes ( "JEANSDENIM", "JEANS", 2, bAddToModel );
    AddClothes ( "SNEAKERBINCBLK", "SNEAKER", 3, bAddToModel );
    AddClothes ( "PLAYER_FACE", "HEAD", 1, bAddToModel );
}


bool CClientPlayerClothes::HasEmptyClothing ( unsigned char ucType )
{
    return ( ucType == 0 || ucType == 1 || ucType == 2 || ucType == 3 );
}


bool CClientPlayerClothes::IsEmptyClothing ( const SPlayerClothing * pClothing, unsigned char ucType )
{
    if ( pClothing )
    {
        if ( ucType <= 3 )
        {
            const SPlayerClothing* pGroup = GetClothingGroup ( ucType );
            if ( pClothing == &pGroup [ 0 ] )
            {
                return true;
            }
        }
    }
    return false;
}

const char* CClientPlayerClothes::GetClothingName ( unsigned char ucType )
{
    if ( ucType < PLAYER_CLOTHING_SLOTS )
    {
        return g_clothesNames [ ucType ].szName;
    }
    return NULL;
}


const SPlayerClothing* CClientPlayerClothes::GetClothingGroup ( unsigned char ucType )
{
    if ( ucType < PLAYER_CLOTHING_SLOTS )
    {
        switch ( ucType )
        {
            case 0: return g_TorsoClothing;         case 1: return g_HairClothing;
            case 2: return g_LegsClothing;          case 3: return g_ShoesClothing;
            case 4: return g_LeftUpperArmClothing;  case 5: return g_LeftLowerArmClothing;
            case 6: return g_RightUpperArmClothing; case 7: return g_RightLowerArmClothing;
            case 8: return g_BackTopClothing;       case 9: return g_LeftChestClothing;
            case 10: return g_RightChestClothing;   case 11: return g_StomachClothing;
            case 12: return g_LowerBackClothing;    case 13: return g_Extra1Clothing;
            case 14: return g_Extra2Clothing;       case 15: return g_Extra3Clothing;
            case 16: return g_Extra4Clothing;       case 17: return g_SuitClothing;
        }
    }

    return NULL;
}


const SPlayerClothing * CClientPlayerClothes::GetClothing ( const char* szTexture, const char* szModel, unsigned char ucType )
{
    if ( szTexture && szModel && ucType < PLAYER_CLOTHING_SLOTS )
    {
        const SPlayerClothing * pGroup = GetClothingGroup ( ucType );
        int iMax = GetClothingGroupMax ( ucType );
        for ( int i = 0 ; i < iMax ; i++ )
        {
            const SPlayerClothing * pClothing = &pGroup [ i ];
            if ( !stricmp ( szTexture, pClothing->szTexture ) &&
                 !stricmp ( szModel, pClothing->szModel ) )
            {
                return pClothing;
            }
        }
    }
    return NULL;
}


const int CClientPlayerClothes::GetClothingGroupMax ( unsigned char ucType )
{
    if ( ucType < PLAYER_CLOTHING_SLOTS )
    {
        switch ( ucType )
        {
            case 0: return TORSO_CLOTHING_MAX;              case 1: return HAIR_CLOTHING_MAX;
            case 2: return LEGS_CLOTHING_MAX;               case 3: return SHOES_CLOTHING_MAX;
            case 4: return LEFT_UPPER_ARM_CLOTHING_MAX;     case 5: return LEFT_LOWER_ARM_CLOTHING_MAX;
            case 6: return RIGHT_UPPER_ARM_CLOTHING_MAX;    case 7: return RIGHT_LOWER_ARM_CLOTHING_MAX;
            case 8: return BACK_TOP_CLOTHING_MAX;           case 9: return LEFT_CHEST_CLOTHING_MAX;
            case 10: return RIGHT_CHEST_CLOTHING_MAX;       case 11: return STOMACH_CLOTHING_MAX;
            case 12: return LOWER_BACK_CLOTHING_MAX;        case 13: return EXTRA1_CLOTHING_MAX;
            case 14: return EXTRA2_CLOTHING_MAX;            case 15: return EXTRA3_CLOTHING_MAX;
            case 16: return EXTRA4_CLOTHING_MAX;            case 17: return SUIT_CLOTHING_MAX;
        }
    }

    return 0;
}
