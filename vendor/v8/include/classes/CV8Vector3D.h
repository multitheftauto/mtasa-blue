class CVector2D;

using namespace v8;

class CV8Vector3D : public CV8BaseClass
{
public:
    inline static const char* m_szName = "Vector3";

    // Adds new class definition to the context
    static Handle<FunctionTemplate> CV8Vector3D::CreateTemplate(Local<Context> context, Handle<FunctionTemplate> parent);

private:
    static void ConstructorCall(const FunctionCallbackInfo<Value>& info);

    static void GetZ(Local<Name> property, const PropertyCallbackInfo<Value>& info);

    static void SetZ(Local<Name> property, Local<Value> value, const PropertyCallbackInfo<void>& info);

    static void MethodGetLength(const FunctionCallbackInfo<Value>& info);
};