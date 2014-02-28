/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CVehicleModelInfoSA.h
*  PURPOSE:     Vehicle model manipulation and creation
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CVehicleModelInfoSA_H
#define __CVehicleModelInfoSA_H

#define MAX_COMPONENTS          15
#define MAX_PASSENGERS          8

#include <CQuat.h>

class CVehicleComponentPlacementSA
{
public:
    CVehicleComponentPlacementSA()
    {
        m_id = -1;
    }

    CVector                         m_offset;       // 0
    CQuat                           m_quat;         // 12
    int                             m_id;           // 28
};

class CVehicleComponentInfoSAInterface
{
public:
                                    CVehicleComponentInfoSAInterface( void );
                                    ~CVehicleComponentInfoSAInterface( void );

    void*   operator new( size_t );
    void    operator delete( void *ptr );

    void                            AddAtomic( RpAtomic *atomic );

    CVector                         m_seatOffset[MAX_COMPONENTS];   // 0
    CVehicleComponentPlacementSA    m_info[MAX_COMPONENTS + 3];     // 180
    RpAtomic*                       m_atomics[MAX_DOORS];           // 756
    char                            m_atomicCount;                  // 780
    BYTE                            m_pad;                          // 781
    unsigned short                  m_unknown4;                     // 782
    unsigned int                    m_usageFlags;                   // 784
};

#include "CVehicleModelInfoSA.render.h"
#include "CVehicleModelInfoSA.config.h"

class CVehicleModelInfoSAInterface : public CClumpModelInfoSAInterface
{
public:
                                    CVehicleModelInfoSAInterface    ( void );
                                    ~CVehicleModelInfoSAInterface   ( void );

    eModelType                      GetModelType                    ( void );
    void                            Init                            ( void );
    void                            DeleteRwObject                  ( void );
    RpClump*                        CreateRwObject                  ( void );
    void                            SetAnimFile                     ( const char *name );
    void                            ConvertAnimFileIndex            ( void );
    int                             GetAnimFileIndex                ( void );
    void                            SetClump                        ( RpClump *clump );

    void __thiscall                 RegisterRenderCallbacks         ( void );
    void __thiscall                 Setup                           ( void );
    void                            SetComponentFlags               ( RwFrame *frame, unsigned int flags );
    void                            RegisterRoot                    ( void );
    void                            SetupMateria                    ( void );
    void                            InitNameplate                   ( void );

    void                            AssignPaintjob                  ( unsigned short txdId );
    unsigned short                  GetNumberOfValidPaintjobs       ( void ) const;

    void __thiscall                 SetPlateText                    ( const char *plateText );
    const char* __thiscall          GetPlateText                    ( void ) const;

    void __thiscall                 SetRenderColor                  ( unsigned char color1, unsigned char color2, unsigned char color3, unsigned char color4 );

    eVehicleType                    GetVehicleType                  ( void )        { return vehicleType; }

    RpMaterial*                     pPlateMaterial;         // 36
    char                            plateText[8];           // 40
    BYTE                            pad3;                   // 48
    unsigned char                   plateDesign;            // 49
    char                            name[10];               // 50
    eVehicleType                    vehicleType;            // 60
    float                           frontWheelScale;        // 64
    float                           rearWheelScale;         // 68
    unsigned short                  wheelModel;             // 72
    unsigned short                  handlingID;             // 74
    unsigned char                   numberOfDoors;          // 76
    unsigned char                   vehicleList;            // 77
    unsigned char                   flags;                  // 78
    unsigned char                   wheelUpgrade;           // 79
    unsigned short                  usageCounter;           // 80
    unsigned short                  frequency;              // 82
    unsigned int                    componentFlags;         // 84
    float                           steerAngle;             // 88
    CVehicleComponentInfoSAInterface*   componentInfo;      // 92
    BYTE                            pad4[84];               // 96
    BYTE                            padUpgrade[380];        // 180
    RpMaterial*                     bodyMaterials[16];      // 560
    BYTE                            pad6[96];               // 624
    unsigned short                  numberOfColors;         // 720
    unsigned char   color1, color2, color3, color4;         // 722
    unsigned short                  upgradeTypes[18];       // 726
    unsigned short                  paintjobTypes[5];       // 762, http://www.gtaforums.com/index.php?showtopic=209878
    int                             animFileIndex;          // 772
};

#endif