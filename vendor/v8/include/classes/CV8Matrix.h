class CVector2D;

using namespace v8;

class CV8Matrix : public CV8BaseClass
{
public:
    inline static EClass      m_eClass = EClass::Matrix;
    inline static const char* m_szName = "Matrix";

    // Adds new class definition to the context
    static Handle<FunctionTemplate> CV8Matrix::CreateTemplate(Local<Context> context);

private:
    static void ConstructorCall(const FunctionCallbackInfo<Value>& info);

    static void GetPosition(Local<Name> property, const PropertyCallbackInfo<Value>& info);
    static void SetPosition(Local<Name> property, Local<Value> value, const PropertyCallbackInfo<void>& info);
    static void GetRotation(Local<Name> property, const PropertyCallbackInfo<Value>& info);
    static void SetRotation(Local<Name> property, Local<Value> value, const PropertyCallbackInfo<void>& info);
    static void GetScale(Local<Name> property, const PropertyCallbackInfo<Value>& info);
    static void SetScale(Local<Name> property, Local<Value> value, const PropertyCallbackInfo<void>& info);
};