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
    static bool Convert(Local<Object> object, CVector2D& vector);

private:
    static void ConstructorCall(const FunctionCallbackInfo<Value>& info);

    static void GetX(Local<Name> property, const PropertyCallbackInfo<Value>& info);
    static void SetX(Local<Name> property, Local<Value> value, const PropertyCallbackInfo<void>& info);
    static void GetY(Local<Name> property, const PropertyCallbackInfo<Value>& info);
    static void SetY(Local<Name> property, Local<Value> value, const PropertyCallbackInfo<void>& info);

    static float MethodGetLength(CV8FunctionCallback& info, Local<Object> self, CVector2D* value);
    static float MethodDotProduct(CV8FunctionCallback& info, Local<Object> self, CVector2D* value);
};
