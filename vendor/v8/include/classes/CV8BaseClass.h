using namespace v8;

#include <assert.h>

class CV8BaseClass
{
public:
    static bool ConstructorCallCheck(const FunctionCallbackInfo<Value>& info);
};