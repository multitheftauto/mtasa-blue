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

    //template <typename T>
    //static T* CreateGarbageCollected(Local<Object> object)
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
                Isolate*                                      pV8Isolate = Isolate::GetCurrent();
                HandleScope                                   scope(pV8Isolate);
                Local<External>                               data = info.Data().As<External>();
                CV8Class*                                     thisClass = (CV8Class*)data->Value();

                /*std::function<CVector2D*(float, float)> asdf = *(std::function<CVector2D*(float, float)>*)asd;
                CVector2D*                              aa = asdf(2.0f, 3.40f);*/
                if (info.Length() != thisClass->GetParametersCount())
                {
                    pV8Isolate->ThrowException(CV8Utils::ToV8String("Error"));
                    return;
                }

                pV8Isolate->Enter();

                std::function<void*(CV8FunctionCallbackBase&, void*)> constructionFunction = thisClass->GetConstrutorFunction();
                CV8FunctionCallback funcCallback(info);
                void* wrappedClass = constructionFunction(funcCallback, thisClass->Allocate(pV8Isolate));
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

    uint16_t                                       GetClassId() const { return m_classId; }
    std::function<void*(CV8FunctionCallbackBase&, void*)> GetConstrutorFunction() const { return m_constructorFunc; }
    size_t                                         GetParametersCount() const { return m_parametersCount; }

private:
    size_t                                        m_parametersCount;
    size_t                                        m_sizeOf;
    std::string                                   m_name;
    uint16_t                                      m_classId;
    std::function<void*(CV8FunctionCallbackBase&, void*)> m_constructorFunc;
};
