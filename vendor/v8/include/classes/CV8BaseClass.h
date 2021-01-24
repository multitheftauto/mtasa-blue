using namespace v8;

#include <assert.h>

class CV8BaseClass
{
public:
    // All static methods below must be implemented with this specification:

    // Adds new class definition to the context
    static void CreateTemplate(Local<Context> context) { assert(false); }

    static void ConstructorCall(const FunctionCallbackInfo<Value>& info) { assert(false); }
};