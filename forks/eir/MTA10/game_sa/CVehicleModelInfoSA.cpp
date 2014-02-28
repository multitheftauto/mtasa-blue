/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CVehicleModelInfoSA.cpp
*  PURPOSE:     Vehicle model info
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "gamesa_renderware.h"

/*
    CVehicleModelInfoSAInterface

    This class represents model information for every in-game vehicle.
    It stores things like seat offsets and rotations, vehicle type,
    maximum number of passengers, paintjob txd ids, etc.

    The virtual interface is nearly completely researched so that we may
    start adding native vehicle models very soon. Nevertheless, we may just
    use this interface for reference if there are issues related to vehicle
    model replacement. We may be interested to replace the GTA:SA code with 
    these new functions since it'd be more stable
    (CVehicleModelInfoSAInterface::Setup).
*/

CVehicleComponentInfoPool **ppVehicleComponentInfoPool = (CVehicleComponentInfoPool**)0x00B4E680;


CVehicleModelInfoSAInterface::CVehicleModelInfoSAInterface( void )
{
    Init(); // ???
}

CVehicleModelInfoSAInterface::~CVehicleModelInfoSAInterface( void )
{

}

/*=========================================================
    CVehicleModelInfoSAInterface::GetModelType

    Purpose:
        Returns the interface type of this model info.
        ( -> MODEL_VEHICLE )
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C7650
=========================================================*/
eModelType CVehicleModelInfoSAInterface::GetModelType( void )
{
    return MODEL_VEHICLE;
}

/*=========================================================
    CVehicleModelInfoSAInterface::Init

    Purpose:
        Initializes this interface for first time usage.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C7630
=========================================================*/
void CVehicleModelInfoSAInterface::Init( void )
{
    CClumpModelInfoSAInterface::Init();

    this->vehicleType = (eVehicleType)0xFFFFFFFF;
    this->wheelModel = 0xFFFF;
    this->steerAngle = 1000.0f;
}

/*=========================================================
    CVehicleModelInfoSAInterface::DeleteRwObject

    Purpose:
        Deletes or dereferences all RenderWare resources associated
        with this vehicle model info.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C9890
=========================================================*/
void CVehicleModelInfoSAInterface::DeleteRwObject( void )
{
    if ( componentInfo )
    {
        delete componentInfo;
        componentInfo = NULL;
    }

    CClumpModelInfoSAInterface::DeleteRwObject();
}

/*=========================================================
    CVehicleModelInfoSAInterface::CreateRwObject

    Purpose:
        Creates an instance of the RenderWare clump associated
        with this vehicle model info. The cloned clump will be
        independent from this model info at best.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C9680
=========================================================*/
RpClump* CVehicleModelInfoSAInterface::CreateRwObject( void )
{
    RpClump *clump = (RpClump*)CClumpModelInfoSAInterface::CreateRwObject();

    // TODO

    return clump;
}

/*=========================================================
    CVehicleModelInfoSAInterface::SetAnimFile

    Arguments:
        name - name of the animation file
    Purpose:
        Temporarily stores the animation file name into the
        anim block id member.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C7670
=========================================================*/
void CVehicleModelInfoSAInterface::SetAnimFile( const char *name )
{
    char *anim;

    if ( strcmp( name, "null" ) == 0 )
        return;

    anim = (char*)malloc( strlen( name ) + 1 );

    strcpy(anim, name);

    // this is one nasty hack
    animFileIndex = (int)anim;
}

/*=========================================================
    CVehicleModelInfoSAInterface::ConvertAnimFileIndex

    Purpose:
        Converts the anim file path pointer at the anim block
        index member to a real anim block index, undoing the
        hack.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C76D0
=========================================================*/
void CVehicleModelInfoSAInterface::ConvertAnimFileIndex( void )
{
    int animBlock;

    if ( animFileIndex == -1 )
        return;

    animBlock = pGame->GetAnimManager()->GetAnimBlockIndex( (const char*)this->m_animBlock );

    free( (void*)animFileIndex );

    // Yeah, weird
    animFileIndex = animBlock;
}

/*=========================================================
    CVehicleModelInfoSAInterface::GetAnimFileIndex

    Purpose:
        Returns the model info anim block index. If none is
        associated, -1 is returned.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C7660
=========================================================*/
int CVehicleModelInfoSAInterface::GetAnimFileIndex( void )
{
    return animFileIndex;
}

/*=========================================================
    CVehicleModelInfoSAInterface::SetClump

    Arguments:
        clump - new clump which shall be associated with this
                model info
    Purpose:
        Assigns a new clump RenderWare object with this vehicle
        model info and sets it up for distribution by
        CreateRwObject. The distributed model has atomics which
        are selected from the given clump by CComponentHierarchySAInterface
        model construction information.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C95C0
=========================================================*/
void CVehicleModelInfoSAInterface::SetClump( RpClump *clump )
{
    componentInfo = new CVehicleComponentInfoSAInterface;

    CClumpModelInfoSAInterface::SetClump( clump );

    RegisterRenderCallbacks();

    // Correctly assign vehicle atomics
    AssignAtomics( ((CComponentHierarchySAInterface**)0x008A7740)[ GetVehicleType() ] );

    RegisterRoot();

    Setup();

    SetupMateria();

    InitNameplate();
}

/*=========================================================
    CVehicleComponentInfoSAInterface::constructor

    Purpose:
        Constructs a vehicle component information container.
        In this structure component positions and rotations are stored.
        Components are prepared so that they can be easily applied to
        vehicles.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C8D60
=========================================================*/
CVehicleComponentInfoSAInterface::CVehicleComponentInfoSAInterface( void )
{
    // We have no atomics in the beginning
    memset( m_atomics, 0, sizeof(m_atomics) );

    m_atomicCount = 0;

    m_unknown4 = 0;
    m_usageFlags = 0;
}

/*=========================================================
    CVehicleComponentInfoSAInterface::destructor

    Purpose:
        Destroys all resources which were associated with
        this component information structure.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C7410
=========================================================*/
CVehicleComponentInfoSAInterface::~CVehicleComponentInfoSAInterface( void )
{
    // Destroy atomic and frame resources
    for ( unsigned char n = 0; n < m_atomicCount; n++ )
    {
        RpAtomic *atomic = m_atomics[n];
        RwFrame *frame = atomic->parent;

        RpAtomicDestroy( atomic );
        RwFrameDestroy( frame );
    }
}

// The vehicle component interface is allocated in a pool.
// It means that GTA:SA can have as many vehicle models loaded at a
// time as it has pool slots.
void* CVehicleComponentInfoSAInterface::operator new( size_t )
{
    return (*ppVehicleComponentInfoPool)->Allocate();
}

void CVehicleComponentInfoSAInterface::operator delete( void *ptr )
{
    (*ppVehicleComponentInfoPool)->Free( (CVehicleComponentInfoSAInterface*)ptr );
}

/*=========================================================
    CVehicleComponentInfoSAInterface::AddAtomic

    Arguments:
        atomic - atomic to add to the storage
    Purpose:
        Appends an atomic to this vehicle component structure.
        It supports a maximum of 6 atomics.
    Note:
        This function is inlined into CVehicleModelInfoSAInterface::Setup.
        The GTA:SA function does not check for the maximum (0x004C9027).
=========================================================*/
void CVehicleComponentInfoSAInterface::AddAtomic( RpAtomic *atomic )
{
    if ( m_atomicCount == 6 )
        return;

    m_atomics[ m_atomicCount++ ] = atomic;
}

/*=========================================================
    RwFrameChildBaseHierarchy

    Arguments:
        child - child frame of the root frame, whose object hierarchy
                should be root-based
    Purpose:
        Scans through all children of a frame's hierarchy and
        reparents all their objects to the given root frame.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C8E30
=========================================================*/
static bool RwFrameChildBaseHierarchy( RwFrame *child, RwFrame *root )
{
    child->ForAllChildren( RwFrameChildBaseHierarchy, root );

    // Add all objects to the root
    LIST_FOREACH_BEGIN( RwObjectFrame, child->objects.root, lFrame )
        item->AddToFrame( root );
    LIST_FOREACH_END
    return true;
}

/*=========================================================
    CVehicleModelInfoSAInterface::Setup

    Purpose:
        Takes the current clump RenderWare object and restructures
        it. The resulting model is optimized for in-game usage. It
        acquires seat placement information using frame-matrix
        transformation (matrix * vector), which is used to render
        the sitting player and interpolation to the seat. Special
        atomics (wheels, ...) get default rendering callbacks.
        The actual number of doors for this model is set by the
        amount of component frames which are marked as doors in
        the registry.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C8E60
    Note:
        The GTA:SA function made no distinctions between atomics,
        lights and cameras, resulting in a crash if lights or
        cameras were part of vehicle models. This has been fixed
        (GetFirstAtomic).
=========================================================*/
inline static void RwFrameGetAbsoluteTransformationBaseOffset( CVector& out, RwFrame *frame )
{
    out = frame->modelling.vPos;

    RwFrame *prevParent = frame->parent;

    // Transform until base frame is reached
    while ( prevParent )
    {
        RwFrame *parent = prevParent->parent;

        if ( !parent )
            break;

        prevParent->GetModelling().Transform( out, out );

        prevParent = parent;
    }
}

static __forceinline RpAtomic* RpAtomicCloneInherit( RpAtomic *orig, RpClump *base, RwFrame *hier )
{
    RpAtomic *clone = RpAtomicClone( orig );

    clone->AddToClump( base );
    clone->AddToFrame( hier );

    // The_GTA: usually all atomics in here were reset to default render callback.
    // This should not happen, as we have special rendering callbacks set to them.

    return clone;
}

void CVehicleModelInfoSAInterface::Setup( void )
{
    tHandlingDataSA *handling = &CHandlingManagerSA::m_OriginalHandlingData[ handlingID ];
    CComponentHierarchySAInterface *info = ((CComponentHierarchySAInterface**)0x008A7740)[ GetVehicleType() ];
    RpAtomic *obj1 = NULL;
    RpAtomic *obj2 = NULL;

    numberOfDoors = 0;

    for ( info; info->m_name; info++ )
    {
        RwFrame *hier;

        if ( info->m_flags & (ATOMIC_HIER_FRONTSEAT | ATOMIC_HIER_SEAT | ATOMIC_HIER_UNKNOWN3) && ( hier = GetRwObject()->parent->FindFreeChildByName( info->m_name ) ) )
        {
            if ( info->m_flags & ATOMIC_HIER_FRONTSEAT )
            {
                // Position the component
                RwFrameGetAbsoluteTransformationBaseOffset( componentInfo->m_seatOffset[ info->m_frameHierarchy ], hier );

                // We do not require this hierarchy anymore
                RwFrameDestroy( hier );
            }
            else if ( info->m_flags & ATOMIC_HIER_UNKNOWN3 )
            {
                 CVehicleComponentPlacementSA& seat = componentInfo->m_info[ info->m_frameHierarchy ];

                 seat.m_offset = hier->GetPosition();

                 // Calculate the quat for rotation
                 seat.m_quat = CQuat( hier->modelling );

                 seat.m_id = hier->parent->hierarchyId;
            }
            else
            {
                RpAtomic *atomic = hier->GetFirstAtomic();

                atomic->RemoveFromClump();

                hier->Unlink();

                // Apply the component flags
                SetComponentFlags( hier, info->m_flags );

                // Append the atomic onto the component registry
                componentInfo->AddAtomic( atomic );
            }
        }

        if ( info->m_flags & (ATOMIC_HIER_UNKNOWN4 | ATOMIC_HIER_UNKNOWN5) && ( hier = GetRwObject()->parent->FindChildByHierarchy( info->m_frameHierarchy ) ) )
        {
            for ( hier; hier; hier = hier->GetFirstChild() )
            {
                RpAtomic *obj = hier->GetFirstAtomic();

                if ( !obj )
                    continue;

                if ( info->m_flags & ATOMIC_HIER_UNKNOWN4 )
                    obj1 = obj;
                else
                    obj2 = obj;

                break;
            }
        }
    }

    info = ((CComponentHierarchySAInterface**)0x008A7740)[ GetVehicleType() ];

    for (info; info->m_name; info++)
    {
        if ( info->m_flags & (ATOMIC_HIER_FRONTSEAT | ATOMIC_HIER_SEAT | ATOMIC_HIER_UNKNOWN3) )
            continue;

        RwFrame *hier = GetRwObject()->parent->FindChildByHierarchy( info->m_frameHierarchy );

        if ( !hier )
            continue;

        if ( info->m_flags & ATOMIC_HIER_DOOR )
            numberOfDoors++;

        if ( info->m_flags & 0x02 )
        {
            RpAtomic *primary = NULL;
            RpAtomic *secondary = NULL;

            // Put all objects into the base frame
            hier->ForAllChildren( RwFrameChildBaseHierarchy, hier );

            hier->Update();

            hier->FindComponentAtomics( &primary, &secondary );

            if ( primary && secondary )
            {
                secondary->SetRenderCallback( primary->renderCallback );

                componentInfo->m_usageFlags |= 1 << info->m_frameHierarchy;
            }
        }

        SetComponentFlags( hier, info->m_flags );

        if ( info->m_flags & (ATOMIC_HIER_UNKNOWN4 | 0x04) )
        {
            if ( !obj1 )
                continue;

            if ( !( info->m_flags & ATOMIC_HIER_UNKNOWN4 ) )
            {
                RpAtomic *clone = RpAtomicCloneInherit( obj1, GetRwObject(), hier );

                if ( info->m_frameHierarchy == 2 || info->m_frameHierarchy == 5 || !( handling->uiModelFlags & 0x20000000 ) )
                    continue;

                // Create a new rotation frame
                RwFrame *frame = RwFrameCreate();
                hier->Link( frame );    // Add the new frame to children of "hier"
                
                clone = RpAtomicCloneInherit( obj1, GetRwObject(), frame );

                frame->modelling.Identity();
                frame->modelling.vPos[0] = (float)(1.15 * -0.25);
            }
            else
            {
                // Put all objects into the base frame
                hier->ForAllChildren( RwFrameChildBaseHierarchy, hier );

                hier->Update();

                //obj1->SetRenderCallback( NULL );
            }
        }
        else if ( info->m_flags & ATOMIC_HIER_UNKNOWN6 )
        {
            if ( !obj2 )
                continue;

            RpAtomicCloneInherit( obj2, GetRwObject(), hier );
        }
    }
}

/*=========================================================
    CVehicleModelInfoSAInterface::SetComponentFlags

    Arguments:
        frame - component of the vehicle model
        flags - properties to set to the component
    Purpose:
        Applies component hierarchy flags to the atomics of the
        vehicle component frame.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C7C10
    Note:
        This function has been optimized so that it does not
        rapidly call RwFrame::SetAtomicComponentFlags but rather
        collects all frames for a single call.
=========================================================*/
void CVehicleModelInfoSAInterface::SetComponentFlags( RwFrame *frame, unsigned int flags )
{
    tHandlingDataSA *handling = &CHandlingManagerSA::m_OriginalHandlingData[ handlingID ];
    unsigned short compFlags = 0;

    if ( flags & 0x1000 )
        compFlags |= 0x80;

    if ( flags & 0x400000 )
        compFlags |= 0x400;

    if ( flags & 0x40000 )
        compFlags |= 0x2000;

    if ( flags & 0x80 )
        compFlags |= 0x10;
    else if ( flags & 0x0100 && ( handling->uiModelFlags & 0x01 || !( flags & (0x20 | 0x40) ) ) )
        compFlags |= 0x20;
    else if ( flags & 0x20 )
        compFlags |= 0x04;
    else if ( flags & 0x40 )
        compFlags |= 0x08;

    if ( flags & 0x8000 && ( handling->uiModelFlags & 0x80000000 || flags & (0x20 | 0x40) ) )
        compFlags |= 0x8000;

    if ( flags & 0x2000 )
        compFlags |= 0x100;
    else if ( flags & 0x4000 )
        compFlags |= 0x200;

    if ( flags & 0x0400 )
        compFlags |= 0x40;

    if ( compFlags != 0 )
        frame->SetAtomicComponentFlags( compFlags );
}

/*=========================================================
    RwClumpAtomicSetupVehiclePipeline

    Arguments:
        child - atomic of the vehicle model
    Purpose:
        Determines the rendering pipeline for internal procedures.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005D5B30
    Note:
        We should analyze RpAtomicSetupVehiclePipeline someday.
=========================================================*/
static bool RwClumpAtomicSetupVehiclePipeline( RpAtomic *child, int )
{
    RpAtomicSetupVehiclePipeline( child );
    return true;
}

/*=========================================================
    CVehicleModelInfoSAInterface::RegisterRoot

    Purpose:
        Assigns the vehicle rendering pipeline to all atomics
        and creates a global vehicle root model info frame.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C8900
=========================================================*/
void CVehicleModelInfoSAInterface::RegisterRoot( void )
{
    // Make sure we render using the vehicle pipeline
    GetRwObject()->ForAllAtomics( RwClumpAtomicSetupVehiclePipeline, 0 );

    // Do not do stuff if we have a root already
    if ( *(RwFrame**)0x00B4E6B8 )
        return;

    RwFrame *frame;
    CVector normal( 1.0f, 0, 0 );

    frame = RwFrameCreate();

    *(RwFrame**)0x00B4E6B8 = frame;

    RwFrameOrient( frame, 60, 0, normal );

    // The vehicle root frame is not a child frame
    frame->Update();

    // Cache the matrix
    frame->GetLTM();
}

/*=========================================================
    CVehicleModelInfoSAInterface::SetupMateria

    Purpose:
        Collects atomic materials and destroys them. 
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C8BD0
=========================================================*/
void CVehicleModelInfoSAInterface::SetupMateria( void )
{
    RwPrefetch();
    {
        RpMaterials mats( 20 );

        GetRwObject()->FetchMateria( mats );

        for ( char n = 0; n < componentInfo->m_atomicCount; n++ )
            componentInfo->m_atomics[n]->FetchMateria( mats );
    }

    GetRwObject()->RemoveAtomicComponentFlags( 0x2000 );
}

/*=========================================================
    CVehicleModelInfoSAInterface::AssignPaintjob

    Arguments:
        txdId - CTxdInstanceSA pool offset of the one you want
                to add as paintjob dictionary
    Purpose:
        Registers another tex dictionary to this vehicle model
        info as possible paintjob dictionary. There is a maximum
        of 5 paintjobs. Called during the loading of IMG archives.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C86D0
=========================================================*/
void CVehicleModelInfoSAInterface::AssignPaintjob( unsigned short txdId )
{
    unsigned char n = 0;

    while ( paintjobTypes[n] != 0xFFFF && n++ < 4 );

    paintjobTypes[n] = txdId;
}

/*=========================================================
    CVehicleModelInfoSAInterface::GetNumberOfValidPaintjobs

    Purpose:
        Returns the number of active paintjob registrations.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C86B0
=========================================================*/
unsigned short CVehicleModelInfoSAInterface::GetNumberOfValidPaintjobs( void ) const
{
    unsigned int n;

    for ( n = 0; n < 4; n++ )
        if ( paintjobTypes[n] == 0xFFFF )
            break;
    
    return n;
}

/*=========================================================
    CVehicleModelInfoSAInterface::SetPlateText

    Arguments:
        plateText - the new plate text to assign to this
                    vehicle model info (can be NULL)
    Purpose:
        Attempts to assign a new plate text string to this
        vehicle model info.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C8980
=========================================================*/
void CVehicleModelInfoSAInterface::SetPlateText( const char *plateText )
{
    if ( !plateText )
    {
        // If no plate text given, we simply terminate the string.
        this->plateText[0] = '\0';
    }
    else
    {
        // Otherwise we copy it into our local buffer.
        strncpy( this->plateText, plateText, 8 );
    }
}

/*=========================================================
    CVehicleModelInfoSAInterface::GetPlateText

    Purpose:
        Returns a pointer to the plate text if it is valid.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C8970
=========================================================*/
const char* CVehicleModelInfoSAInterface::GetPlateText( void ) const
{
    return ( *plateText != 0 ) ? ( plateText ) : ( NULL );
}