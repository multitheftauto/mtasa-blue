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

    RpMaterial*                     m_plateMaterial;        // 36
    BYTE                            m_pad2[9];              // 40
    unsigned char                   m_plateDesign;          // 49
    char                            m_name[10];             // 50
    eVehicleType                    m_vehicleType;          // 60
    float                           m_frontWheelScale;      // 64
    float                           m_rearWheelScale;       // 68
    unsigned short                  m_wheelModel;           // 72
    unsigned short                  m_handlingID;           // 74
    unsigned char                   m_numberOfDoors;        // 76
    unsigned char                   m_vehicleList;          // 77
    unsigned char                   m_flags;                // 78
    unsigned char                   m_wheelUpgrade;         // 79
    unsigned short                  m_usageCounter;         // 80
    unsigned short                  m_frequency;            // 82
    unsigned int                    m_componentFlags;       // 84
    float                           m_steerAngle;           // 88
    CVehicleComponentInfoSAInterface*   m_componentInfo;    // 92
    BYTE                            m_pad4[84];             // 96
    BYTE                            m_padUpgrade[540];      // 180
    unsigned int                    m_numberOfColors;       // 720
    BYTE                            m_pad5[2];              // 724
    unsigned short                  m_upgradeTypes[18];     // 726
    unsigned short                  m_paintjobTypes[5];     // 762, http://www.gtaforums.com/index.php?showtopic=209878
    int                             m_animFileIndex;        // 772
};

#endif