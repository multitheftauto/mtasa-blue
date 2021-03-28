class CV8Class : public CV8ClassBase
{
public:
    CV8Class(std::string name, uint16_t classId) : m_name(name), m_classId(classId) {}

    Handle<FunctionTemplate> Initialize(CV8Isolate* pIsolate);

    void SetConstructorFunction(std::function<void*(CV8FunctionCallbackBase&, void*)> func) { m_constructorFunc = func; };
    // Fast fail purpose
    void SetParametersCount(size_t count) { m_parametersCount = count; }
    void SetSizeOf(size_t size) { m_sizeOf = size; }

    void AttachGC(Isolate* isolate, Local<Object> object, uint16_t classId);

    // Allocates memory of size of underlying class
    void* Allocate(Isolate* isolate);

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

    void SetConstructor(Handle<FunctionTemplate> objectTemplate)
    {
        objectTemplate->SetCallHandler(
            [](const FunctionCallbackInfo<Value>& info) {
                Isolate*        pV8Isolate = Isolate::GetCurrent();
                HandleScope     scope(pV8Isolate);
                Local<External> data = info.Data().As<External>();
                CV8Class*       thisClass = (CV8Class*)data->Value();

                /*std::function<CVector2D*(float, float)> asdf = *(std::function<CVector2D*(float, float)>*)asd;
                CVector2D*                              aa = asdf(2.0f, 3.40f);*/
                if (info.Length() != thisClass->GetParametersCount())
                {
                    pV8Isolate->ThrowException(CV8Utils::ToV8String("Error"));
                    return;
                }

                pV8Isolate->Enter();

                std::function<void*(CV8FunctionCallbackBase&, void*)> constructionFunction = thisClass->GetConstrutorFunction();
                CV8FunctionCallback                                   funcCallback(info);
                void*                                                 wrappedClass = constructionFunction(funcCallback, thisClass->Allocate(pV8Isolate));
                CVector2D*                                            test = (CVector2D*)wrappedClass;

                Local<Object> wrapper = info.Holder();

                wrapper->SetInternalField(CV8::EInternalFieldPurpose::TypeOfClass, CV8Utils::ToV8Number((double)thisClass->GetClassId()));
                wrapper->SetInternalField(CV8::EInternalFieldPurpose::PointerToValue, External::New(pV8Isolate, wrappedClass));

                thisClass->AttachGC(pV8Isolate, wrapper, thisClass->GetClassId());

                info.GetReturnValue().Set(wrapper);

                pV8Isolate->Exit();
                return;
                //}

                //// Here we are expecting exception to be thrown
                // pV8Isolate->Exit();
            },
            External::New(Isolate::GetCurrent(), this));
    }

    void AddAccessor(std::string name, float (*getter)(void*), void(*setter)(void*, float))
    {
        m_floatAccessors[name] = {getter, setter};
    }

    void SetAccessors(Local<ObjectTemplate> objectTemplate);

    template <typename T>
    constexpr void SetAccessor(Local<ObjectTemplate> objectTemplate, std::string name, T(*getter)(void*), void (*setter)(void*, T))
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

                Local<Array>            array = info.Data().As<Array>();
                Local<External>         externalData = array->Get(info.GetIsolate()->GetCurrentContext(), 0).ToLocalChecked().As<External>();
                CVector2D*              asd = (CVector2D*)(pointerToValue);
                auto getter = (T(*)(void*))externalData->Value();
                T    result = getter(pointerToValue);
                auto                    n = typeid(T).name();
                info.GetReturnValue().SetUndefined();
                if constexpr (std::is_same_v<T, void>)
                {
                    info.GetReturnValue().SetUndefined();
                }
                else if constexpr (std::is_same_v<T, float>)
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

                Local<Array>                  data = info.Data().As<Array>();
                Local<External>               externalData = data->Get(isolate->GetCurrentContext(), 1).ToLocalChecked().As<External>();

                auto setter = (void (*)(void*, T))(externalData->Value());
                if constexpr (std::is_same_v<T, double>)
                {
                    setter(pointerToValue, value->NumberValue(isolate->GetCurrentContext()).ToChecked());
                }
                else if constexpr (std::is_same_v<T, float>)
                {
                    setter(pointerToValue, (float)value->NumberValue(isolate->GetCurrentContext()).ToChecked());
                }
                else
                {
                    static_assert(0);
                }
            },
            arr);
    };

    uint16_t                                              GetClassId() const { return m_classId; }
    std::function<void*(CV8FunctionCallbackBase&, void*)> GetConstrutorFunction() const { return m_constructorFunc; }
    size_t                                                GetParametersCount() const { return m_parametersCount; }

private:
    size_t                                                m_parametersCount;
    size_t                                                m_sizeOf;
    std::string                                           m_name;
    uint16_t                                              m_classId;
    std::function<void*(CV8FunctionCallbackBase&, void*)> m_constructorFunc;

    std::map<std::string, std::pair<float (*)(void*), void (*)(void*, float)>> m_floatAccessors;
};
