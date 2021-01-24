class CVector2D;

using namespace v8;

class CV8Vector2D : public CV8BaseClass
{
public:
    inline static const char* m_szName = "Vector2";
    // Adds new class definition to the context

    static void CreateTemplate(Local<Context> context);

    //static Handle<Object> New(CVector2D vector2d, Isolate* isolate);

private:
    static void ConstructorCall(const FunctionCallbackInfo<Value>& info);

    static void GetX(Local<Name> property, const PropertyCallbackInfo<Value>& info);
    static void SetX(Local<Name> property, Local<Value> value, const PropertyCallbackInfo<void>& info);
    static void GetY(Local<Name> property, const PropertyCallbackInfo<Value>& info);
    static void SetY(Local<Name> property, Local<Value> value, const PropertyCallbackInfo<void>& info);

    static void MethodGetLength(const FunctionCallbackInfo<Value>& info);
};