/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/


struct CTickRateSettings
{
public:
    CTickRateSettings ( void )
    {
        iPureSync = 100;
        iLightSync = 1500;
        iCamSync = 500;
        iPedSync = 500;
        iUnoccupiedVehicle = 400;
        iObjectSync = 500;
        iKeySyncRotation = 100;
        iKeySyncAnalogMove = 100;
    }

    int iPureSync;
    int iLightSync;
    int iCamSync;
    int iPedSync;
    int iUnoccupiedVehicle;
    int iObjectSync;
    int iKeySyncRotation;
    int iKeySyncAnalogMove;
};

extern CTickRateSettings g_TickRateSettings;
