class CVector2D;
class CVector;

using namespace v8;

class CV8Vector3D : public CV8BaseClass
{
public:
    inline static EClass      m_eClass = EClass::Vector3;
    inline static const char* m_szName = "Vector3";

    // Adds new class definition to the context
    static Handle<FunctionTemplate> CreateTemplate(Local<Context> context, Handle<FunctionTemplate> parent);
    static MaybeLocal<Object>       New(CVector vector);
    static bool                     Convert(Local<Object> object, CVector& vector);

private:
    static bool ConstructorCall(CV8FunctionCallback& info, Local<Object> object, CVector* value);

    static float CV8Vector3D::GetZ(CVector* internalValue);
    static void  SetZ(CVector* internalValue, float value);

    static float MethodGetLength(CV8FunctionCallback& info, Local<Object> self, CVector* value);
    static float MethodGetLengthSquared(CV8FunctionCallback& info, Local<Object> self, CVector* value);
    static void  MethodCrossProduct(CV8FunctionCallback& info, Local<Object> self, CVector* value);
    static void  MethodDotProduct(CV8FunctionCallback& info, Local<Object> self, CVector* value);
    static void  MethodNormalize(CV8FunctionCallback& info, Local<Object> self, CVector* value);
};
