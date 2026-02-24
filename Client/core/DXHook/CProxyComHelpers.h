/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/DXHook/CProxyComHelpers.h
 *  PURPOSE:     Shared COM interface helper functions for proxy classes
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "ComPtrValidation.h"

template <typename T>
void ReleaseInterface(T*& pointer, int errorCode, const char* context = nullptr)
{
    if (!pointer)
        return;

    T* heldPointer = pointer;

    const bool valid = IsValidComInterfacePointer(pointer, ComPtrValidation::ValidationMode::ForceRefresh);

    if (!valid)
    {
        SString label;
        label = context ? context : "ReleaseInterface";
        SString message;
        message.Format("%s: skipping Release on invalid COM pointer %p", label.c_str(), heldPointer);
        AddReportLog(errorCode, message, 5);
        ComPtrValidation::Invalidate(heldPointer);
        pointer = nullptr;
        return;
    }

    heldPointer->Release();
    pointer = nullptr;
}

template <typename T>
void ReleaseInterface(T*& pointer)
{
    if (pointer)
    {
        pointer->Release();
        pointer = nullptr;
    }
}

template <typename T>
void ReplaceInterface(T*& destination, T* source, int releaseErrorCode, int replaceErrorCode, const char* context = nullptr)
{
    if (destination == source)
        return;

    ReleaseInterface(destination, releaseErrorCode, context);

    if (source && !IsValidComInterfacePointer(source, ComPtrValidation::ValidationMode::ForceRefresh))
    {
        SString label;
        label = context ? context : "ReplaceInterface";
        SString message;
        message.Format("%s: refusing to assign invalid COM pointer %p", label.c_str(), source);
        AddReportLog(replaceErrorCode, message, 5);
        destination = nullptr;
        return;
    }

    destination = source;
    if (destination)
        destination->AddRef();
}

template <typename T>
void ReplaceInterface(T*& destination, T* source)
{
    if (destination == source)
        return;

    if (destination)
    {
        // Cached validation. All tracked objects are held via AddRef,
        // so an external release cannot free them while our ref is live.
        if (!ComPtrValidation::Validate(destination))
        {
            AddReportLog(8799, SString("ReplaceInterface: skipping Release on stale COM pointer %p", destination), 5);
            ComPtrValidation::Invalidate(destination);
        }
        else
        {
            destination->Release();
        }
        destination = nullptr;
    }

    destination = source;
    if (destination)
        destination->AddRef();
}

template <typename T>
void ReplaceInterface(T*& destination, T* source, const char* context)
{
    constexpr int GENERIC_COM_ERROR = 8799;
    ReplaceInterface(destination, source, GENERIC_COM_ERROR, GENERIC_COM_ERROR, context);
}
