class CVector2D;
class CV8FunctionCallback;

using namespace v8;

class CV8Vector2D : public CV8BaseClass
{
public:
    inline static EClass      m_eClass = EClass::Vector2;
    inline static const char* m_szName = "Vector2";
    // Adds new class definition to the context

    static Handle<FunctionTemplate> CreateTemplate(Local<Context> context);

    static MaybeLocal<Object> New(CVector2D vector);
    static bool               Convert(Local<Object> object, CVector2D& vector);

private:
    static bool ConstructorCall(CV8FunctionCallback& info, Local<Object> object, CVector2D* value);

    static float GetX(CVector2D* internalValue);
    static void  SetX(CVector2D* internalValue, float value);
    static float GetY(CVector2D* internalValue);
    static void  SetY(CVector2D* internalValue, float value);

    static float MethodGetLength(CV8FunctionCallback& info, Local<Object> self, CVector2D* value);
    static float MethodGetLengthSquared(CV8FunctionCallback& info, Local<Object> self, CVector2D* value);
    static bool  MethodNormalize(CV8FunctionCallback& info, Local<Object> self, CVector2D* value);
    static float MethodDotProduct(CV8FunctionCallback& info, Local<Object> self, CVector2D* value);
};
