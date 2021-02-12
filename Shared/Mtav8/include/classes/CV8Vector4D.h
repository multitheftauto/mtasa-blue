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
    static MaybeLocal<Object> New(CVector4D vector);
    static bool               Convert(Local<Object> object, CVector4D& vector);

private:
    static bool ConstructorCall(CV8FunctionCallback& info, Local<Object> object, CVector4D* value);

    static float GetW(CVector4D* internalValue);
    static void  SetW(CVector4D* internalValue, float value);

    static float MethodGetLength(CV8FunctionCallback& info, Local<Object> self, CVector4D* value);
    static float MethodGetLengthSquared(CV8FunctionCallback& info, Local<Object> self, CVector4D* value);
    static void  MethodNormalize(CV8FunctionCallback& info, Local<Object> self, CVector4D* value);
    static void  MethodDotProduct(CV8FunctionCallback& info, Local<Object> self, CVector4D* value);
};
