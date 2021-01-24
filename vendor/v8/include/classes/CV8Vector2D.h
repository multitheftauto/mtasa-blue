class CVector2D;

class CV8Vector2D : public CMtaReference<CVector2D>
{
public:
    static void CreateTemplate(v8::Local<v8::Context> context);

    //static v8::Handle<v8::Object> New(CVector2D vector2d, v8::Isolate* isolate);

private:
    static void ConstructorCall(const v8::FunctionCallbackInfo<v8::Value>& info);

    static void GetX(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetX(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetY(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetY(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
};