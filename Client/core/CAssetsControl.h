/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CAssetsControl.cpp
 *  PURPOSE:     Header file for the assets class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

class CAssetsControl
{
public:
    CAssetsControl();
    ~CAssetsControl();
    DWORD GetAmbientColor();

private:
    DWORD ambientColor;            // argb ambient color
};
