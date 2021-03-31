/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once
#include <optional>
#include <variant>
#include <array>

class CVector2D;
class CVector;
class CVector4D;

namespace js
{
    inline void Push(CV8FunctionCallbackBase* JS, float value) { JS->Return(value); }
    inline void Push(CV8FunctionCallbackBase* JS, std::string value) { JS->Return(value); }
    inline void Push(CV8FunctionCallbackBase* JS, nullptr_t) { JS->ReturnUndefined(); }
    inline void Push(CV8FunctionCallbackBase* JS, std::function<void(CV8AsyncContextBase*)> value) { JS->ReturnPromise(value);
    }
}
