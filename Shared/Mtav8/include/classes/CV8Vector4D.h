class CVector4D;
class CVector2D;
class CVector;

using namespace v8;

class CV8Vector4D : public CV8BaseClass
{
public:
    inline static EClass      m_eClass = EClass::Vector3;
    inline static const char* m_szName = "Vector4";

    // Adds new class definition to the context
    static Handle<FunctionTemplate> CreateTemplate(Local<Context> context, Handle<FunctionTemplate> parent);

    // Equivalent to "new Vector4(x,y,z,w)" in js
    static MaybeLocal<Object>       New(CVector4D vector);
    static bool                     Convert(Local<Object> object, CVector4D& vector);

private:
    static void ConstructorCall(const FunctionCallbackInfo<Value>& info);

    static void GetW(Local<Name> property, const PropertyCallbackInfo<Value>& info);

    static void SetW(Local<Name> property, Local<Value> value, const PropertyCallbackInfo<void>& info);

    static void MethodGetLength(const FunctionCallbackInfo<Value>& info);
};
