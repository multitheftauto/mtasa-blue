using namespace v8;

#include <assert.h>

class CV8BaseClass
{
public:
    // Adds new class definition to the context
    static void CreateTemplate(Local<Context> context) { assert(false); }

    static bool ConstructorCallCheck(const FunctionCallbackInfo<Value>& info);
};