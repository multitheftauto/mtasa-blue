#include <StdInc.h>
#include "../game_sa/CAnimBlendSequenceSA.h"

CClientIFP::CClientIFP ( class CClientManager* pManager, ElementID ID ) : CClientEntity ( ID )
{
    // Init
    m_pManager = pManager;
    SetTypeName ( "IFP" );
    m_pIFPAnimations = std::make_shared < CIFPAnimations > ();
    m_pAnimManager = g_pGame->GetAnimManager ( );
    m_bVersion1    = false; 
    m_u32Hashkey   = 0;
}

CClientIFP::~CClientIFP ( void )
{
}

bool CClientIFP::LoadIFP ( const char* szFilePath, const SString & strBlockName )
{
    printf ("\nCClientIFP::LoadIFP: szFilePath %s\n szBlockName: %s\n\n", szFilePath, strBlockName.c_str());
    
    m_strBlockName = strBlockName;
    m_pVecAnimations = &m_pIFPAnimations->vecAnimations;

    if ( LoadIFPFile ( szFilePath ) )
    {
        m_u32Hashkey = g_pGame->GetKeyGen()->GetUppercaseKey ( strBlockName );
        return true;
    }
    return false;
}

bool CClientIFP::LoadIFPFile ( const char * szFilePath )
{
    CreateLoader ( szFilePath );

    if ( LoadFile ( ) )
    {
        printf("IfpLoader: File loaded. Parsing it now.\n");
        
        char Version [ 4 ];
        ReadBytes ( Version, sizeof ( Version ) );

        bool bAnp3 = strncmp ( Version, "ANP3", sizeof ( Version ) ) == 0;
        bool bAnp2 = strncmp ( Version, "ANP2", sizeof ( Version ) ) == 0;
        if ( bAnp2 || bAnp3 )
        {
            m_bVersion1 = false;
            ReadIFPVersion2 ( bAnp3 );
        }
        else
        {
            m_bVersion1 = true;
            ReadIFPVersion1 ( );
        }

        // We are unloading the data because we don't need to read it anymore. 
        // This function does not unload IFP, to unload ifp, use destroyElement from lua
        UnloadFile ( );
    }
    else
    {
        printf("ERROR: LoadIFPFile: failed to load file.\n");
        return false;
    }

    printf("Exiting LoadIFPFile function\n");

    return true;
}

void CClientIFP::ReadIFPVersion1 (  )
{ 
    IFP_INFO Info;
    ReadHeaderVersion1 ( Info );

    m_pVecAnimations->resize ( Info.Entries );
    for ( auto it = m_pVecAnimations->begin(); it != m_pVecAnimations->end(); ++it ) 
    {
        ReadAnimationNameVersion1 ( it->Name );
        printf("Animation Name: %s  \n", it->Name.c_str ( ) );
      
        IFP_DGAN Dgan;
        ReadDgan ( Dgan );

        std::unique_ptr < CAnimBlendHierarchy > pAnimationHierarchy = m_pAnimManager->GetCustomAnimBlendHierarchy ( &it->Hierarchy );
        InitializeAnimationHierarchy ( pAnimationHierarchy, it->Name, Dgan.Info.Entries );
        it->pSequencesMemory  = ( char * ) AllocateSequencesMemory ( pAnimationHierarchy );
        
        pAnimationHierarchy->SetSequences ( reinterpret_cast < CAnimBlendSequenceSAInterface * > ( it->pSequencesMemory + 4 ) );
 
        *(DWORD *)it->pSequencesMemory = ReadSequencesWithDummies ( pAnimationHierarchy );
        PreProcessAnimationHierarchy ( pAnimationHierarchy );
    }
}

void CClientIFP::ReadIFPVersion2 ( bool bAnp3 )
{
    IFPHeaderV2 Header;
    ReadBuffer < IFPHeaderV2 > ( &Header );

    m_pVecAnimations->resize ( Header.TotalAnimations );
    for ( auto it = m_pVecAnimations->begin(); it != m_pVecAnimations->end(); ++it ) 
    {
        Animation AnimationNode;
        ReadAnimationHeaderVersion2 ( AnimationNode, bAnp3 );

        it->Name = AnimationNode.Name;
        printf("Animation Name: %s    \n", AnimationNode.Name );

        auto pAnimationHierarchy = m_pAnimManager->GetCustomAnimBlendHierarchy ( &it->Hierarchy );
        InitializeAnimationHierarchy ( pAnimationHierarchy, AnimationNode.Name, AnimationNode.TotalObjects );

        it->pSequencesMemory  = ( char * ) AllocateSequencesMemory ( pAnimationHierarchy );
        pAnimationHierarchy->SetSequences ( reinterpret_cast < CAnimBlendSequenceSAInterface * > ( it->pSequencesMemory + 4 ) );
 
        *(DWORD *)it->pSequencesMemory = ReadSequencesWithDummies ( pAnimationHierarchy );
        PreProcessAnimationHierarchy ( pAnimationHierarchy );
    }
}

WORD CClientIFP::ReadSequencesWithDummies ( std::unique_ptr < CAnimBlendHierarchy > & pAnimationHierarchy )
{
    std::map < DWORD, CAnimBlendSequenceSAInterface > MapOfSequences;
    WORD wUnknownSequences = ReadSequences ( pAnimationHierarchy, MapOfSequences );
        
    CopySequencesWithDummies ( pAnimationHierarchy, MapOfSequences );

    WORD cSequences = m_kcIFPSequences + wUnknownSequences;

    // As we need support for all 32 bones, we must change the total sequences count
    pAnimationHierarchy->SetNumSequences ( cSequences );
    return cSequences;
}

WORD CClientIFP::ReadSequences ( std::unique_ptr < CAnimBlendHierarchy > & pAnimationHierarchy, std::map < DWORD, CAnimBlendSequenceSAInterface > & MapOfSequences )
{
    if ( m_bVersion1 )
    {
        return ReadSequencesVersion1 ( pAnimationHierarchy, MapOfSequences );
    }
    return ReadSequencesVersion2 ( pAnimationHierarchy, MapOfSequences );
}

WORD CClientIFP::ReadSequencesVersion1 ( std::unique_ptr < CAnimBlendHierarchy > & pAnimationHierarchy, std::map < DWORD, CAnimBlendSequenceSAInterface > & MapOfSequences )
{
    WORD wUnknownSequences = 0;
    for (size_t SequenceIndex = 0; SequenceIndex < pAnimationHierarchy->GetNumSequences ( ); SequenceIndex++)
    {
        IFP_ANIM Anim;
        int32_t iBoneID = ReadSequenceVersion1 ( Anim );
        if ( Anim.Frames == 0 )
        {
            continue;
        }

        CAnimBlendSequenceSAInterface AnimationSequence;
        CAnimBlendSequenceSAInterface * pAnimationSequenceInterface = &AnimationSequence;

        bool bUnknownSequence = iBoneID == -1;
        if (bUnknownSequence)
        {
            size_t UnkownSequenceIndex  = m_kcIFPSequences + wUnknownSequences;
            pAnimationSequenceInterface = pAnimationHierarchy->GetSequence ( UnkownSequenceIndex );
            wUnknownSequences ++;
        }
        
        std::unique_ptr < CAnimBlendSequence > pAnimationSequence = m_pAnimManager->GetCustomAnimBlendSequence ( pAnimationSequenceInterface );
        InitializeAnimationSequence ( pAnimationSequence, Anim.Name, iBoneID );
    
        IFP_FrameType iFrameType = ReadKfrm ( );
        if ( ( ReadSequenceKeyFrames ( pAnimationSequence, iFrameType, Anim.Frames ) ) && ( !bUnknownSequence ) )
        {
            MapOfSequences [ iBoneID ] = *pAnimationSequence->GetInterface();
        }
    }
    return wUnknownSequences;
}

WORD CClientIFP::ReadSequencesVersion2 ( std::unique_ptr < CAnimBlendHierarchy > & pAnimationHierarchy, std::map < DWORD, CAnimBlendSequenceSAInterface > & MapOfSequences )
{
    WORD wUnknownSequences = 0;
    for (size_t SequenceIndex = 0; SequenceIndex  < pAnimationHierarchy->GetNumSequences ( ); SequenceIndex++)
    {
        Object ObjectNode;
        ReadSequenceVersion2 ( ObjectNode );

        bool bUnknownSequence = ObjectNode.BoneID == -1;

        CAnimBlendSequenceSAInterface AnimationSequence;
        CAnimBlendSequenceSAInterface * pAnimationSequenceInterface = &AnimationSequence;
        if (bUnknownSequence)
        {
            size_t UnkownSequenceIndex  = m_kcIFPSequences + wUnknownSequences;
            pAnimationSequenceInterface = pAnimationHierarchy->GetSequence ( UnkownSequenceIndex );
            wUnknownSequences ++;
        }

        std::unique_ptr < CAnimBlendSequence > pAnimationSequence = m_pAnimManager->GetCustomAnimBlendSequence ( pAnimationSequenceInterface );
        InitializeAnimationSequence ( pAnimationSequence, ObjectNode.Name, ObjectNode.BoneID );

        IFP_FrameType iFrameType = static_cast < IFP_FrameType > ( ObjectNode.FrameType );
        if ( ( ReadSequenceKeyFrames ( pAnimationSequence, iFrameType, ObjectNode.TotalFrames ) ) && ( !bUnknownSequence ) )
        {
            MapOfSequences [ ObjectNode.BoneID ] = *pAnimationSequence->GetInterface();
        }
    }
    return wUnknownSequences;
}

int32_t CClientIFP::ReadSequenceVersion1 ( IFP_ANIM & Anim )
{
    IFP_CPAN Cpan;
    ReadBuffer < IFP_CPAN > ( &Cpan );
    RoundSize ( Cpan.Base.Size );
    ReadBytes ( &Anim, sizeof ( IFP_BASE ) );
    RoundSize ( Anim.Base.Size );
    ReadBytes ( &Anim.Name, Anim.Base.Size );

    int32_t iBoneID = -1;
    if ( Anim.Base.Size == 0x2C )
    {
        iBoneID = Anim.Next;
    }

    std::string BoneName = ConvertStringToMapKey ( Anim.Name );
    iBoneID = GetBoneIDFromName ( BoneName );

    std::string strCorrectBoneName = GetCorrectBoneNameFromName ( BoneName );
    strncpy ( Anim.Name, strCorrectBoneName.c_str ( ), strCorrectBoneName.size ( ) + 1 );
    return iBoneID;
}

void CClientIFP::ReadSequenceVersion2 ( Object & ObjectNode )
{
    ReadBuffer < Object > ( &ObjectNode );
    std::string BoneName = ConvertStringToMapKey ( ObjectNode.Name );
    std::string strCorrectBoneName;
    if (ObjectNode.BoneID == -1)
    {
        ObjectNode.BoneID = GetBoneIDFromName ( BoneName );
        strCorrectBoneName = GetCorrectBoneNameFromName ( BoneName );
    }
    else
    {
        strCorrectBoneName = GetCorrectBoneNameFromID ( ObjectNode.BoneID );
        if ( strCorrectBoneName.size ( ) == 0 )
        {
            strCorrectBoneName = GetCorrectBoneNameFromName ( BoneName );
        }
    }

    strncpy ( ObjectNode.Name, strCorrectBoneName.c_str(), strCorrectBoneName.size() +1 );
}

bool CClientIFP::ReadSequenceKeyFrames ( std::unique_ptr < CAnimBlendSequence > & pAnimationSequence, IFP_FrameType iFrameType, int32_t iFrames )
{
    size_t iCompressedFrameSize = GetSizeOfCompressedFrame ( iFrameType );
    if ( iCompressedFrameSize )
    {
        BYTE * pKeyFrames = m_pAnimManager->AllocateKeyFramesMemory ( iCompressedFrameSize * iFrames ); 
        pAnimationSequence->SetKeyFrames ( iFrames, IsKeyFramesTypeRoot ( iFrameType ), m_kbAllKeyFramesCompressed, pKeyFrames );
        ReadKeyFramesAsCompressed ( pAnimationSequence, iFrameType, iFrames );
        return true;
    }
    return false;
}

void CClientIFP::ReadHeaderVersion1 ( IFP_INFO & Info )
{
    uint32_t OffsetEOF;
    ReadBuffer < uint32_t > ( &OffsetEOF );
    RoundSize ( OffsetEOF );
    ReadBytes ( &Info, sizeof ( IFP_BASE ) );
    RoundSize ( Info.Base.Size );
    ReadBytes ( &Info.Entries, Info.Base.Size );
}

void CClientIFP::ReadAnimationNameVersion1 ( SString & strAnimationName )
{
    IFP_NAME Name;
    ReadBuffer < IFP_NAME > ( &Name );
    RoundSize ( Name.Base.Size );

    char szAnimationName [ 24 ];
    ReadCString ( szAnimationName, Name.Base.Size );
    strAnimationName = szAnimationName;
}

void CClientIFP::ReadDgan ( IFP_DGAN & Dgan )
{
    ReadBytes ( &Dgan, sizeof ( IFP_BASE ) * 2 );
    RoundSize ( Dgan.Base.Size );
    RoundSize ( Dgan.Info.Base.Size );
    ReadBytes ( &Dgan.Info.Entries, Dgan.Info.Base.Size );
}

CClientIFP::IFP_FrameType CClientIFP::ReadKfrm ( void )
{
    IFP_KFRM Kfrm;
    ReadBuffer < IFP_KFRM > ( &Kfrm );
    return GetFrameTypeFromFourCC ( Kfrm.Base.FourCC );
}

void CClientIFP::ReadAnimationHeaderVersion2 ( Animation & AnimationNode, bool bAnp3 )
{
    ReadCString ( ( char * ) &AnimationNode.Name, sizeof ( Animation::Name ) );
    ReadBuffer < int32_t > ( &AnimationNode.TotalObjects );
    if ( bAnp3 )
    {
        ReadBuffer < int32_t > ( &AnimationNode.FrameSize );
        ReadBuffer < int32_t > ( &AnimationNode.isCompressed );
    }
}

void CClientIFP::ReadKeyFramesAsCompressed ( std::unique_ptr < CAnimBlendSequence > & pAnimationSequence, IFP_FrameType iFrameType, int32_t iFrames )
{
    switch ( iFrameType )
    { 
        case IFP_FrameType::KRTS:
        {
            ReadKrtsFramesAsCompressed ( pAnimationSequence, iFrames );
            break;
        }
        case IFP_FrameType::KRT0:
        {
            ReadKrt0FramesAsCompressed ( pAnimationSequence, iFrames );
            break;
        }
        case IFP_FrameType::KR00:
        {
            ReadKr00FramesAsCompressed ( pAnimationSequence, iFrames );
            break;
        }
        case IFP_FrameType::KR00_COMPRESSED:
        {
            ReadCompressedFrames < IFP_Compressed_KR00 >  ( pAnimationSequence, iFrames );
            break;
        }
        case IFP_FrameType::KRT0_COMPRESSED:
        {
            ReadCompressedFrames < IFP_Compressed_KRT0 >  ( pAnimationSequence, iFrames );
            break;
        }
    }
}

void CClientIFP::ReadKrtsFramesAsCompressed ( std::unique_ptr < CAnimBlendSequence > & pAnimationSequence, int32_t TotalFrames )
{
    for ( int32_t FrameIndex = 0; FrameIndex < TotalFrames; FrameIndex++ )
    {
        IFP_Compressed_KRT0 * CompressedKrt0 = static_cast < IFP_Compressed_KRT0 * > ( pAnimationSequence->GetKeyFrame ( FrameIndex, sizeof ( IFP_Compressed_KRT0 ) ) );
        IFP_KRTS Krts;
        ReadBuffer < IFP_KRTS > ( &Krts );

        CompressedKrt0->Rotation.X    = static_cast < int16_t > ( ( ( -Krts.Rotation.X ) * 4096.0f ) );
        CompressedKrt0->Rotation.Y    = static_cast < int16_t > ( ( ( -Krts.Rotation.Y ) * 4096.0f ) );
        CompressedKrt0->Rotation.Z    = static_cast < int16_t > ( ( ( -Krts.Rotation.Z ) * 4096.0f ) );
        CompressedKrt0->Rotation.W    = static_cast < int16_t > ( ( Krts.Rotation.W  * 4096.0f ) );

        CompressedKrt0->Time          = static_cast < int16_t > ( ( Krts.Time * 60.0f + 0.5f ) );

        CompressedKrt0->Translation.X = static_cast < int16_t > ( ( Krts.Translation.X * 1024.0f ) );
        CompressedKrt0->Translation.Y = static_cast < int16_t > ( ( Krts.Translation.Y * 1024.0f ) );
        CompressedKrt0->Translation.Z = static_cast < int16_t > ( ( Krts.Translation.Z * 1024.0f ) );
    }
}

void CClientIFP::ReadKrt0FramesAsCompressed ( std::unique_ptr < CAnimBlendSequence > & pAnimationSequence, int32_t TotalFrames )
{
    for ( int32_t FrameIndex = 0; FrameIndex < TotalFrames; FrameIndex++ )
    {
        IFP_Compressed_KRT0 * CompressedKrt0 = static_cast < IFP_Compressed_KRT0 * > ( pAnimationSequence->GetKeyFrame ( FrameIndex, sizeof ( IFP_Compressed_KRT0 ) ) );
        IFP_KRT0 Krt0;
        ReadBuffer < IFP_KRT0 > ( &Krt0 );

        CompressedKrt0->Rotation.X = static_cast < int16_t > ( ( ( -Krt0.Rotation.X ) * 4096.0f ) );
        CompressedKrt0->Rotation.Y = static_cast < int16_t > ( ( ( -Krt0.Rotation.Y ) * 4096.0f ) );
        CompressedKrt0->Rotation.Z = static_cast < int16_t > ( ( ( -Krt0.Rotation.Z ) * 4096.0f ) );
        CompressedKrt0->Rotation.W = static_cast < int16_t > ( ( Krt0.Rotation.W  * 4096.0f ) );

        CompressedKrt0->Time = static_cast < int16_t > ( ( Krt0.Time * 60.0f + 0.5f ) );

        CompressedKrt0->Translation.X = static_cast < int16_t > ( ( Krt0.Translation.X * 1024.0f ) ); 
        CompressedKrt0->Translation.Y = static_cast < int16_t > ( ( Krt0.Translation.Y * 1024.0f ) );
        CompressedKrt0->Translation.Z = static_cast < int16_t > ( ( Krt0.Translation.Z * 1024.0f ) );
    }
}

void CClientIFP::ReadKr00FramesAsCompressed ( std::unique_ptr < CAnimBlendSequence > & pAnimationSequence, int32_t TotalFrames )
{
    for ( int32_t FrameIndex = 0; FrameIndex < TotalFrames; FrameIndex++ )
    {
        IFP_Compressed_KR00 * CompressedKr00 = static_cast < IFP_Compressed_KR00 * > ( pAnimationSequence->GetKeyFrame ( FrameIndex, sizeof ( IFP_Compressed_KR00 ) ) );
        IFP_KR00 Kr00;
        ReadBuffer < IFP_KR00 > ( &Kr00 );

        CompressedKr00->Rotation.X = static_cast < int16_t > ( ( ( -Kr00.Rotation.X ) * 4096.0f ) );
        CompressedKr00->Rotation.Y = static_cast < int16_t > ( ( ( -Kr00.Rotation.Y ) * 4096.0f ) );
        CompressedKr00->Rotation.Z = static_cast < int16_t > ( ( ( -Kr00.Rotation.Z ) * 4096.0f ) );
        CompressedKr00->Rotation.W = static_cast < int16_t > ( ( Kr00.Rotation.W  * 4096.0f ) );

        CompressedKr00->Time = static_cast < int16_t > ( (Kr00.Time * 60.0f + 0.5f) );
    }
}

size_t CClientIFP::GetSizeOfCompressedFrame ( IFP_FrameType iFrameType )
{
    switch ( iFrameType )
    { 
        case IFP_FrameType::KRTS:
        {
            return sizeof ( IFP_Compressed_KRT0 );
        }
        case IFP_FrameType::KRT0:
        {
            return sizeof ( IFP_Compressed_KRT0 );
        }
        case IFP_FrameType::KR00:
        {
            return sizeof ( IFP_Compressed_KR00 );
        }
        case IFP_FrameType::KR00_COMPRESSED:
        {
            return sizeof ( IFP_Compressed_KR00 );
        }
        case IFP_FrameType::KRT0_COMPRESSED:
        {
            return sizeof ( IFP_Compressed_KRT0 );
        }
    }
    return 0;
}

void CClientIFP::InitializeAnimationHierarchy ( std::unique_ptr < CAnimBlendHierarchy > & pAnimationHierarchy, const char * szAnimationName, const int32_t iSequences )
{
    pAnimationHierarchy->Initialize ( );
    pAnimationHierarchy->SetName ( szAnimationName );
    pAnimationHierarchy->SetNumSequences ( iSequences );
    pAnimationHierarchy->SetAnimationBlockID ( 0 ); 
    pAnimationHierarchy->SetRunningCompressed ( m_kbAllKeyFramesCompressed );
}

void CClientIFP::InitializeAnimationSequence ( std::unique_ptr < CAnimBlendSequence > & pAnimationSequence, const char * szName, const int32_t iBoneID )
{
    pAnimationSequence->Initialize ( );
    pAnimationSequence->SetName ( szName );
    pAnimationSequence->SetBoneTag ( iBoneID );
}

void CClientIFP::PreProcessAnimationHierarchy ( std::unique_ptr < CAnimBlendHierarchy > & pAnimationHierarchy )
{
    if ( !pAnimationHierarchy->isRunningCompressed ( ) )
    {
        pAnimationHierarchy->RemoveQuaternionFlips ( );
        pAnimationHierarchy->CalculateTotalTime ( );
    }  
}

void CClientIFP::CopySequencesWithDummies ( std::unique_ptr < CAnimBlendHierarchy > & pAnimationHierarchy, std::map < DWORD, CAnimBlendSequenceSAInterface > & mapOfSequences )
{
    for (size_t SequenceIndex = 0; SequenceIndex < m_kcIFPSequences; SequenceIndex++)
    {
        std::string  BoneName = m_karrstrBoneNames[SequenceIndex];
        DWORD        BoneID   = m_karruBoneIds[SequenceIndex];
            
        CAnimBlendSequenceSAInterface * pAnimationSequenceInterface = pAnimationHierarchy->GetSequence ( SequenceIndex );
        auto it = mapOfSequences.find ( BoneID );
        if ( it != mapOfSequences.end ( ) )
        {
            memcpy ( pAnimationSequenceInterface, &it->second, sizeof ( CAnimBlendSequenceSAInterface ) );
        }
        else
        {
            std::unique_ptr < CAnimBlendSequence > pAnimationSequence = m_pAnimManager->GetCustomAnimBlendSequence ( pAnimationSequenceInterface );
            InsertAnimationDummySequence ( pAnimationSequence, BoneName, BoneID );
        }
    }
}

BYTE * CClientIFP::AllocateSequencesMemory ( std::unique_ptr < CAnimBlendHierarchy > & pAnimationHierarchy )
{
    const WORD cMaxSequences = m_kcIFPSequences + pAnimationHierarchy->GetNumSequences ( );
    return static_cast < BYTE * > ( operator new ( 12 * cMaxSequences + 4 ) ); 
}

CClientIFP::IFP_FrameType CClientIFP::GetFrameTypeFromFourCC ( const char * szFourCC )
{
    if ( strncmp ( szFourCC, "KRTS", 4 ) == 0)
    {
        return IFP_FrameType::KRTS;
    }
    else if ( strncmp ( szFourCC, "KRT0", 4 ) == 0)
    {
        return IFP_FrameType::KRT0;
    }
    else if ( strncmp ( szFourCC, "KR00", 4 ) == 0)
    {
        return IFP_FrameType::KR00;
    }

    return IFP_FrameType::UNKNOWN_FRAME;
}


void CClientIFP::InsertAnimationDummySequence ( std::unique_ptr < CAnimBlendSequence > & pAnimationSequence, std::string & BoneName, DWORD & dwBoneID )
{
    pAnimationSequence->Initialize ( );
    pAnimationSequence->SetName ( BoneName.c_str ( ) );
    pAnimationSequence->SetBoneTag ( dwBoneID );

    bool bKeyFrameCompressed = true;
    bool bRootBone = dwBoneID == BoneType::NORMAL;
    bool bHasTranslationValues = false;

    // We only need 1 dummy key frame to make the animation work
    const size_t cKeyFrames = 1;
    // KR00 is child key frame and KRT0 is Root key frame
    size_t FrameSize = sizeof ( IFP_Compressed_KR00 );

    if ( bRootBone )
    {
        // This key frame will have translation values.
        FrameSize = sizeof ( IFP_Compressed_KRT0 );
        bHasTranslationValues = true;
    }

    const size_t FramesDataSizeInBytes = FrameSize * cKeyFrames;
    BYTE * pKeyFrames = m_pAnimManager->AllocateKeyFramesMemory ( FramesDataSizeInBytes );
    pAnimationSequence->SetKeyFrames ( cKeyFrames, bHasTranslationValues, bKeyFrameCompressed, pKeyFrames );
    CopyDummyKeyFrameByBoneID ( pKeyFrames, dwBoneID );
}

void CClientIFP::CopyDummyKeyFrameByBoneID ( BYTE * pKeyFrames, DWORD dwBoneID )
{
    switch ( dwBoneID )
    {
        case BoneType::NORMAL: // Normal or Root, both are same
        {     
            // This is a root frame. It contains translation as well, but it's compressed just like quaternion                                
            BYTE FrameData[16] = { 0x1F, 0x00, 0x00, 0x00, 0x53, 0x0B, 0x4D, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::PELVIS: 
        {
            BYTE FrameData[10] = { 0xB0, 0xF7, 0xB0, 0xF7, 0x55, 0xF8, 0xAB, 0x07, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::SPINE: 
        {
            BYTE FrameData[10] = { 0x0E, 0x00, 0x15, 0x00, 0xBE, 0xFF, 0xFF, 0x0F, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::SPINE1: 
        {
            BYTE FrameData[10] = { 0x29, 0x00, 0xD9, 0x00, 0xB5, 0x00, 0xF5, 0x0F, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::NECK: 
        {
            BYTE FrameData[10] = { 0x86, 0xFF, 0xB6, 0xFF, 0x12, 0x02, 0xDA, 0x0F, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::HEAD:
        {
            BYTE FrameData[10] = { 0xFA, 0x00, 0x0C, 0x01, 0x96, 0xFE, 0xDF, 0x0F, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::JAW:
        {
            BYTE FrameData[10] = { 0x00, 0x00, 0x00, 0x00, 0x2B, 0x0D, 0x16, 0x09, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::L_BROW:
        {
            BYTE FrameData[10] = { 0xC4, 0x00, 0xFF, 0xFE, 0x47, 0x09, 0xF8, 0x0C, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::R_BROW: 
        {
            BYTE FrameData[10] = { 0xA2, 0xFF, 0xF8, 0x00, 0x4F, 0x09, 0xF8, 0x0C, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::L_CLAVICLE:
        {
            BYTE FrameData[10] = { 0x7E, 0xF5, 0x82, 0x02, 0x37, 0xF4, 0x6A, 0xFF, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::L_UPPER_ARM: 
        {
            BYTE FrameData[10] = { 0xA8, 0x02, 0x6D, 0x09, 0x1F, 0xFD, 0x51, 0x0C, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::L_FORE_ARM: 
        {
            BYTE FrameData[10] = { 0x00, 0x00, 0x00, 0x00, 0x3A, 0xFE, 0xE6, 0x0F, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::L_HAND: 
        {
            BYTE FrameData[10] = { 0x9D, 0xF5, 0xBA, 0xFF, 0xEA, 0xFF, 0x29, 0x0C, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::L_FINGER: 
        {
            BYTE FrameData[10] = { 0xFF, 0xFF, 0x00, 0x00, 0xD8, 0x04, 0x3F, 0x0F, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::L_FINGER_01: 
        {
            BYTE FrameData[10] = { 0x00, 0x00, 0x00, 0x00, 0x54, 0x06, 0xB1, 0x0E, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::R_CLAVICLE:
        {
            BYTE FrameData[10] = { 0x0B, 0x0A, 0x3D, 0xFE, 0xBB, 0xF3, 0xCF, 0xFE, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::R_UPPER_ARM: 
        {
            BYTE FrameData[10] = { 0xF7, 0xFD, 0xED, 0xF6, 0xEB, 0xFD, 0xD9, 0x0C, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::R_FORE_ARM: 
        {
            BYTE FrameData[10] = { 0x00, 0x00, 0x00, 0x00, 0x42, 0xFF, 0xFB, 0x0F, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::R_HAND:
        {
            BYTE FrameData[10] = { 0xE9, 0x0B, 0x94, 0x00, 0x25, 0x02, 0x72, 0x0A, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::R_FINGER:
        {
            BYTE FrameData[10] = { 0x37, 0x00, 0xCB, 0xFF, 0x10, 0x09, 0x2E, 0x0D, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::R_FINGER_01:
        {
            BYTE FrameData[10] = { 0x00, 0x00, 0x00, 0x00, 0x60, 0x06, 0xAC, 0x0E, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::L_BREAST: 
        {
            BYTE FrameData[10] = { 0xC5, 0x01, 0x2B, 0x01, 0x53, 0x0A, 0x09, 0x0C, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::R_BREAST:
        {
            BYTE FrameData[10] = { 0x2F, 0xFF, 0xA5, 0xFF, 0xBA, 0x0A, 0xD6, 0x0B, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::BELLY:
        {
            BYTE FrameData[10] = { 0x00, 0x00, 0x00, 0x00, 0x20, 0x0B, 0x7F, 0x0B, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::L_THIGH: 
        {
            BYTE FrameData[10] = { 0x23, 0xFE, 0x44, 0xF0, 0x19, 0xFE, 0x25, 0x01, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::L_CALF: 
        {
            BYTE FrameData[10] = { 0x00, 0x00, 0x00, 0x00, 0x1E, 0xFC, 0x85, 0x0F, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::L_FOOT:
        {
            BYTE FrameData[10] = { 0xBB, 0xFE, 0x3E, 0xFF, 0xD2, 0x01, 0xD3, 0x0F, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::L_TOE_0: 
        {
            BYTE FrameData[10] = { 0x00, 0x00, 0x01, 0x00, 0x8D, 0x0B, 0x12, 0x0B, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::R_THIGH: 
        {
            BYTE FrameData[10] = { 0x0F, 0xFF, 0x19, 0xF0, 0x44, 0x01, 0xBB, 0x00, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::R_CALF:
        {
            BYTE FrameData[10] = { 0x00, 0x00, 0x00, 0x00, 0x64, 0xFD, 0xC9, 0x0F, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::R_FOOT:
        {
            BYTE FrameData[10] = { 0x11, 0x01, 0x9F, 0xFF, 0x9E, 0x01, 0xE0, 0x0F, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        case BoneType::R_TOE_0:
        {
            BYTE FrameData[10] = { 0x00, 0x00, 0x00, 0x00, 0x50, 0x0B, 0x4F, 0x0B, 0x00, 0x00 };
            memcpy ( pKeyFrames, FrameData, sizeof ( FrameData ) );
            break;
        }
        default:
        {
        }
    }
}

std::string CClientIFP::ConvertStringToMapKey ( const char * szString )
{
    std::string ConvertedString ( szString );
    std::transform ( ConvertedString.begin ( ), ConvertedString.end ( ), ConvertedString.begin ( ), ::tolower ); // convert the bone name to lowercase

    ConvertedString.erase ( std::remove(ConvertedString.begin(), ConvertedString.end(), ' '), ConvertedString.end ( ) ); // remove white spaces

    return ConvertedString;
}

constexpr void CClientIFP::RoundSize ( uint32_t & u32Size ) 
{ 
    if ( u32Size & 3 ) 
    { 
        u32Size += 4 - ( u32Size & 3 ); 
    }
}

constexpr bool CClientIFP::IsKeyFramesTypeRoot ( IFP_FrameType iFrameType )
{
    switch ( iFrameType )
    { 
        case IFP_FrameType::KR00:
        {
            return false;
        }
        case IFP_FrameType::KR00_COMPRESSED:
        {
            return false;
        }
    }
    return true;   
}

int32_t CClientIFP::GetBoneIDFromName ( std::string const& BoneName )
{
    if (BoneName == "root") return BoneType::NORMAL;
    if (BoneName == "normal") return BoneType::NORMAL;

   
    if (BoneName == "pelvis") return BoneType::PELVIS;
    if (BoneName == "spine") return BoneType::SPINE;
    if (BoneName == "spine1") return BoneType::SPINE1;
    if (BoneName == "neck") return BoneType::NECK; 
    if (BoneName == "head") return BoneType::HEAD;
    if (BoneName == "jaw") return BoneType::JAW; 
    if (BoneName == "lbrow") return BoneType::L_BROW;
    if (BoneName == "rbrow") return BoneType::R_BROW;
    if (BoneName == "bip01lclavicle") return BoneType::L_CLAVICLE;
    if (BoneName == "lupperarm") return BoneType::L_UPPER_ARM;
    if (BoneName == "lforearm") return BoneType::L_FORE_ARM;
    if (BoneName == "lhand") return BoneType::L_HAND;

    if (BoneName == "lfingers") return BoneType::L_FINGER;
    if (BoneName == "lfinger") return BoneType::L_FINGER;

    if (BoneName == "lfinger01") return BoneType::L_FINGER_01;
    if (BoneName == "bip01rclavicle") return BoneType::R_CLAVICLE;
    if (BoneName == "rupperarm") return BoneType::R_UPPER_ARM;
    if (BoneName == "rforearm") return BoneType::R_FORE_ARM;
    if (BoneName == "rhand") return BoneType::R_HAND;

    if (BoneName == "rfingers") return BoneType::R_FINGER;
    if (BoneName == "rfinger") return BoneType::R_FINGER;

    if (BoneName == "rfinger01") return BoneType::R_FINGER_01;
    if (BoneName == "lbreast") return BoneType::L_BREAST;
    if (BoneName == "rbreast") return BoneType::R_BREAST;
    if (BoneName == "belly") return BoneType::BELLY;
    if (BoneName == "lthigh") return BoneType::L_THIGH;
    if (BoneName == "lcalf") return BoneType::L_CALF;
    if (BoneName == "lfoot") return BoneType::L_FOOT;
    if (BoneName == "ltoe0") return BoneType::L_TOE_0;
    if (BoneName == "rthigh") return BoneType::R_THIGH;
    if (BoneName == "rcalf") return BoneType::R_CALF;
    if (BoneName == "rfoot") return BoneType::R_FOOT;
    if (BoneName == "rtoe0") return BoneType::R_TOE_0;

    // for GTA 3
    if (BoneName == "player") return BoneType::NORMAL;

    if (BoneName == "swaist") return BoneType::PELVIS;
    if (BoneName == "smid") return BoneType::SPINE;
    if (BoneName == "storso") return BoneType::SPINE1;
    if (BoneName == "shead") return BoneType::HEAD;

    if (BoneName == "supperarml") return BoneType::L_UPPER_ARM;
    if (BoneName == "slowerarml") return BoneType::L_FORE_ARM;
    if (BoneName == "supperarmr") return BoneType::R_UPPER_ARM;
    if (BoneName == "slowerarmr") return BoneType::R_FORE_ARM;

    if (BoneName == "srhand") return BoneType::R_HAND;
    if (BoneName == "slhand") return BoneType::L_HAND;

    if (BoneName == "supperlegr") return BoneType::R_THIGH;
    if (BoneName == "slowerlegr") return BoneType::R_CALF;
    if (BoneName == "sfootr") return BoneType::R_FOOT;

    if (BoneName == "supperlegl") return BoneType::L_THIGH;
    if (BoneName == "slowerlegl") return BoneType::L_CALF;
    if (BoneName == "sfootl") return BoneType::L_FOOT;

    return BoneType::UNKNOWN; 
}


std::string CClientIFP::GetCorrectBoneNameFromID ( int32_t & BoneID )
{
    if (BoneID == BoneType::NORMAL) return "Normal";
 
    if (BoneID == BoneType::PELVIS) return "Pelvis";
    if (BoneID == BoneType::SPINE) return "Spine";
    if (BoneID == BoneType::SPINE1) return "Spine1";
    if (BoneID == BoneType::NECK) return "Neck";
    if (BoneID == BoneType::HEAD) return "Head";
    if (BoneID == BoneType::JAW) return "Jaw";
    if (BoneID == BoneType::L_BROW) return "L Brow";
    if (BoneID == BoneType::R_BROW) return "R Brow";
    if (BoneID == BoneType::L_CLAVICLE) return "Bip01 L Clavicle";
    if (BoneID == BoneType::L_UPPER_ARM) return "L UpperArm";
    if (BoneID == BoneType::L_FORE_ARM) return "L ForeArm";
    if (BoneID == BoneType::L_HAND) return "L Hand";

    if (BoneID == BoneType::L_FINGER) return "L Finger";

    if (BoneID == BoneType::L_FINGER_01) return "L Finger01";
    if (BoneID == BoneType::R_CLAVICLE) return "Bip01 R Clavicle";
    if (BoneID == BoneType::R_UPPER_ARM) return "R UpperArm";
    if (BoneID == BoneType::R_FORE_ARM) return "R ForeArm";
    if (BoneID == BoneType::R_HAND) return "R Hand";

    if (BoneID == BoneType::R_FINGER) return "R Finger";

    if (BoneID == BoneType::R_FINGER_01) return "R Finger01";
    if (BoneID == BoneType::L_BREAST) return "L breast";
    if (BoneID == BoneType::R_BREAST) return "R breast";
    if (BoneID == BoneType::BELLY) return "Belly";
    if (BoneID == BoneType::L_THIGH) return "L Thigh";
    if (BoneID == BoneType::L_CALF) return "L Calf";
    if (BoneID == BoneType::L_FOOT) return "L Foot";
    if (BoneID == BoneType::L_TOE_0) return "L Toe0";
    if (BoneID == BoneType::R_THIGH) return "R Thigh";
    if (BoneID == BoneType::R_CALF) return "R Calf";
    if (BoneID == BoneType::R_FOOT) return "R Foot";
    if (BoneID == BoneType::R_TOE_0) return "R Toe0";
    
    return "";
}

std::string CClientIFP::GetCorrectBoneNameFromName ( std::string const& BoneName )
{

    if (BoneName == "root") return "Normal";
    if (BoneName == "normal") return "Normal";


    if (BoneName == "pelvis") return "Pelvis";
    if (BoneName == "spine") return "Spine";
    if (BoneName == "spine1") return "Spine1";
    if (BoneName == "neck") return "Neck";
    if (BoneName == "head") return "Head";
    if (BoneName == "jaw") return "Jaw";
    if (BoneName == "lbrow") return "L Brow";
    if (BoneName == "rbrow") return "R Brow";
    if (BoneName == "bip01lclavicle") return "Bip01 L Clavicle";
    if (BoneName == "lupperarm") return "L UpperArm";
    if (BoneName == "lforearm") return "L ForeArm";
    if (BoneName == "lhand") return "L Hand";

    if (BoneName == "lfingers") return "L Finger";
    if (BoneName == "lfinger") return "L Finger";

    if (BoneName == "lfinger01") return "L Finger01";
    if (BoneName == "bip01rclavicle") return "Bip01 R Clavicle";
    if (BoneName == "rupperarm") return "R UpperArm";
    if (BoneName == "rforearm") return "R ForeArm";
    if (BoneName == "rhand") return "R Hand";

    if (BoneName == "rfingers") return "R Finger";
    if (BoneName == "rfinger") return "R Finger";

    if (BoneName == "rfinger01") return "R Finger01";
    if (BoneName == "lbreast") return "L Breast";
    if (BoneName == "rbreast") return "R Breast";
    if (BoneName == "belly") return "Belly";
    if (BoneName == "lthigh") return "L Thigh";
    if (BoneName == "lcalf") return "L Calf";
    if (BoneName == "lfoot") return "L Foot";
    if (BoneName == "ltoe0") return "L Toe0";
    if (BoneName == "rthigh") return "R Thigh";
    if (BoneName == "rcalf") return "R Calf";
    if (BoneName == "rfoot") return "R Foot";
    if (BoneName == "rtoe0") return "R Toe0";
    
    // For GTA 3
    if (BoneName == "player") return "Normal";
    if (BoneName == "swaist") return "Pelvis";
    if (BoneName == "smid") return "Spine";
    if (BoneName == "storso") return "Spine1";
    if (BoneName == "shead") return "Head";

    if (BoneName == "supperarml") return "L UpperArm";
    if (BoneName == "slowerarml") return "L ForeArm";
    if (BoneName == "supperarmr") return "R UpperArm";
    if (BoneName == "slowerarmr") return "R ForeArm";

    if (BoneName == "srhand") return "R Hand";
    if (BoneName == "slhand") return "L Hand";
    if (BoneName == "supperlegr") return "R Thigh";
    if (BoneName == "slowerlegr") return "R Calf";
    if (BoneName == "sfootr") return "R Foot";
    if (BoneName == "supperlegl") return "L Thigh";
    if (BoneName == "slowerlegl") return "L Calf";
    if (BoneName == "sfootl") return "L Foot";

    return BoneName;
}

CAnimBlendHierarchySAInterface * CClientIFP::GetAnimationHierarchy ( const SString & strAnimationName )
{
    for ( auto it = m_pVecAnimations->begin(); it != m_pVecAnimations->end(); ++it ) 
    {
        if (strAnimationName.ToLower() == it->Name.ToLower())
        {
            return &it->Hierarchy;
        }
    }
    return nullptr;
}
