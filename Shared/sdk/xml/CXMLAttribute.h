/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/xml/CXMLAttribute.h
 *  PURPOSE:     XML attribute interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include "CXMLCommon.h"
#include <string>

class CXMLAttribute : public CXMLCommon
{
public:
    virtual ~CXMLAttribute() = default;

    virtual const std::string GetName() const = 0;
    virtual const std::string GetValue() const = 0;

    virtual void SetValue(const std::string& value) = 0;
    virtual void SetValue(const char* value) = 0;
    virtual void SetValue(bool value) = 0;
    virtual void SetValue(int value) = 0;
    virtual void SetValue(unsigned int value) = 0;
    virtual void SetValue(float value) = 0;
};
