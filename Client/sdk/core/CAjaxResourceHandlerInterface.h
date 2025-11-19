/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/core/AjaxResourceHandler.h
 *  PURPOSE:     Interface for CEF Handler for Ajax Requests with delayed results
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <vector>
#include "SString.h"

class CAjaxResourceHandlerInterface
{
public:
    virtual std::vector<SString>& GetGetData() = 0;
    virtual std::vector<SString>& GetPostData() = 0;
    virtual void                  SetResponse(SString data) = 0;
};
