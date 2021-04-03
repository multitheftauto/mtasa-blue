
class JavascriptWrapper
{
public:
    ~JavascriptWrapper() { delete m_pExternalData; }

    JavascriptWrapper(Isolate* pIsolate, Local<Object> object, uint16_t classId)
    {
        wrapper.Reset(pIsolate, object);
        Local<External> wrap = Local<External>::Cast(object->GetInternalField(CV8::EInternalFieldPurpose::PointerToValue));
        m_pExternalData = wrap->Value();
        wrapper.SetWeak(this, WeakCallbackForObjectHolder, WeakCallbackType::kParameter);
        wrapper.SetWrapperClassId(classId);
    }

private:
    static void WeakCallbackForObjectHolder(const WeakCallbackInfo<JavascriptWrapper>& data) { delete data.GetParameter(); }

    Global<Object> wrapper;
    void*          m_pExternalData;
};

class CV8ExportClass : public CV8ExportClassBase
{
public:
    CV8ExportClass(std::string name, uint16_t classId);

    Local<FunctionTemplate> Initialize(CV8Isolate* pV8Isolate);

    void SetConstructorFunction(std::function<void(CV8FunctionCallbackBase*)> func) { m_constructorFunc = func; };
    void SetMethodFunction(std::string name, std::function<void(CV8FunctionCallbackBase*)> func);
    void SetSizeOf(size_t size) { m_sizeOf = size; }

    void AttachGC(Isolate* isolate, Local<Object> object);

    // Allocates memory of size of underlying class
    void* Allocate(Isolate* isolate);

    void SetInheritance(uint16_t baseClass) { m_parentClassId = baseClass; }
    // template <typename T>
    // static T* CreateGarbageCollected(Local<Object> object)
    //{
    //    Isolate* isolate = Isolate::GetCurrent();
    //    assert(isolate);
    //    T* value = Allocate<T>(isolate);

    //    // AttachGC(object, sizeof(T));
    //    object->SetInternalField(EInternalFieldPurpose::PointerToValue, External::New(isolate, value));
    //    return value;
    //}

    void SetConstructor(Handle<FunctionTemplate> objectTemplate);
    void AddAccessor(std::string name, float (*getter)(void*), void (*setter)(void*, float));
    void AddAccessor(std::string name, unsigned char (*getter)(void*), void (*setter)(void*, unsigned char));

    void SetAccessors(Local<ObjectTemplate> objectTemplate);

    template <typename T>
    constexpr void SetAccessor(Local<ObjectTemplate> objectTemplate, std::string name, T (*getter)(void*), void (*setter)(void*, T))
    {
        Isolate*     isolate = Isolate::GetCurrent();
        auto         externalGetter = External::New(isolate, getter);
        auto         externalSetter = External::New(isolate, setter);
        Local<Array> arr = Array::New(isolate, 2);
        arr->Set(isolate->GetCurrentContext(), 0, externalGetter);
        arr->Set(isolate->GetCurrentContext(), 1, externalSetter);
        objectTemplate->SetAccessor(
            CV8Utils::ToV8String(name),
            [](Local<Name> property, const PropertyCallbackInfo<Value>& info) {
                Isolate*        isolate = info.GetIsolate();
                Local<Object>   self = info.Holder();
                Local<External> external = Local<External>::Cast(self->GetInternalField(CV8::EInternalFieldPurpose::PointerToValue));
                void*           pointerToValue = external->Value();
                assert(pointerToValue);

                Local<Array>    array = info.Data().As<Array>();
                Local<External> externalData = array->Get(info.GetIsolate()->GetCurrentContext(), 0).ToLocalChecked().As<External>();

                auto getter = (T(*)(void*))externalData->Value();
                T    result = getter(pointerToValue);
                auto n = typeid(T).name();
                info.GetReturnValue().SetUndefined();
                if constexpr (std::is_same_v<T, void>)
                {
                    info.GetReturnValue().SetUndefined();
                }
                else if constexpr (std::is_arithmetic_v<T>)
                {
                    info.GetReturnValue().Set(Number::New(isolate, result));
                }
                else if constexpr (std::is_same_v<T, bool>)
                {
                    info.GetReturnValue().Set(Boolean::New(isolate, result));
                }
                else if constexpr (std::is_same_v<T, CVector2D>)
                {
                    info.GetReturnValue().Set(CV8Vector2D::New(result).ToLocalChecked());
                }
                else if constexpr (std::is_same_v<T, CVector>)
                {
                    info.GetReturnValue().Set(CV8Vector3D::New(result).ToLocalChecked());
                }
                else if constexpr (std::is_same_v<T, CVector4D>)
                {
                    info.GetReturnValue().Set(CV8Vector4D::New(result).ToLocalChecked());
                }
                else
                    static_assert(0);
            },
            [](Local<Name> property, Local<Value> value, const PropertyCallbackInfo<void>& info) {
                Isolate*        isolate = info.GetIsolate();
                Local<Object>   self = info.Holder();
                Local<External> external = Local<External>::Cast(self->GetInternalField(CV8::EInternalFieldPurpose::PointerToValue));
                void*           pointerToValue = external->Value();

                Local<Array>    data = info.Data().As<Array>();
                Local<External> externalData = data->Get(isolate->GetCurrentContext(), 1).ToLocalChecked().As<External>();

                auto setter = (void (*)(void*, T))(externalData->Value());
                if constexpr (std::is_same_v<T, double>)
                {
                    setter(pointerToValue, value->NumberValue(isolate->GetCurrentContext()).ToChecked());
                }
                else if constexpr (std::is_same_v<T, float>)
                {
                    setter(pointerToValue, (float)value->NumberValue(isolate->GetCurrentContext()).ToChecked());
                }
                else if constexpr (std::is_same_v<T, unsigned char>)
                {
                    setter(pointerToValue, (unsigned char)value->NumberValue(isolate->GetCurrentContext()).ToChecked());
                }
                else
                {
                    static_assert(0);
                }
            },
            arr);
    };

    uint16_t                                      GetParentClassId() const { return m_parentClassId; }
    uint16_t                                      GetClassId() const { return m_classId; }
    std::string                                   GetName() const { return m_name; }
    std::function<void(CV8FunctionCallbackBase*)> GetConstrutorFunction() const { return m_constructorFunc; }

private:
    size_t                                        m_sizeOf;
    int                                           m_length;            // Accessor count
    std::string                                   m_name;
    uint16_t                                      m_classId = 0;
    uint16_t                                      m_parentClassId = 0;
    std::function<void(CV8FunctionCallbackBase*)> m_constructorFunc;

    std::map<std::string, std::pair<float (*)(void*), void (*)(void*, float)>>                 m_floatAccessors;
    std::map<std::string, std::pair<unsigned char (*)(void*), void (*)(void*, unsigned char)>> m_ucharAccessors;

    std::map<std::string, std::function<void(CV8FunctionCallbackBase*)>> m_methods;
};
