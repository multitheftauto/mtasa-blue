/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CPedModelInfo.h
*  PURPOSE:		Ped model information interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CPedModelInfo_H
#define __CPedModelInfo_H

#include "CModelInfo.h"

typedef unsigned long AssocGroupId;

class CPedModelInfo : public CModelInfo
{
public:
    virtual void							SetMotionAnimGroup	( AssocGroupId animGroup ) = 0;
};

#endif