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

#include <string>
#include <vector>

class CAjaxResourceHandlerInterface
{
public:
    virtual std::vector<std::string>& GetGetData() = 0;
    virtual std::vector<std::string>& GetPostData() = 0;
    virtual void                      SetResponse(const std::string& data) = 0;
};
