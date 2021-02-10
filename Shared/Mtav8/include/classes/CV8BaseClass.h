#include <assert.h>

using namespace v8;

class CV8FunctionCallback;
class JavascriptWrapper;

class CV8BaseClass
{
public:
    enum EInternalFieldPurpose
    {
        TypeOfClass, // Value is one of value from EClass
        PointerToValue, // Values is a pointer to external value.
        Count,
    };

    enum class EClass
    {
        Invalid,            // To distinguish between nullptr and actual class.
        Vector2,
        Vector3,
        Vector4,
        Matrix,
    };

    template <typename R, typename T>
    using MethodCallbackFunc = R (*)(CV8FunctionCallback& info, Local<Object> self, T* value);

    template <typename R, typename T>
    static Local<FunctionTemplate> MethodCallback(MethodCallbackFunc<R, T> callbackFunc)
    {
        Isolate*                isolate = Isolate::GetCurrent();
        Local<FunctionTemplate> functionTemplate = FunctionTemplate::New(
            isolate,
            [](const FunctionCallbackInfo<Value>& info) {
                Local<External>          data = info.Data().As<External>();
                MethodCallbackFunc<R, T> cb = (MethodCallbackFunc<R, T>)data->Value();
                Local<Object>            self = info.Holder();
                Local<External>          wrap = Local<External>::Cast(self->GetInternalField(EInternalFieldPurpose::PointerToValue));
                void*                    ptr = wrap->Value();
                assert(ptr);
                T*                  value = static_cast<T*>(ptr);
                CV8FunctionCallback args(info);

                try
                {
                    if constexpr (std::is_same<R, void>())
                    {
                        cb(args, self, value);
                        info.GetReturnValue().SetUndefined();
                    }
                    else
                    {
                        R result = cb(args, self, value);

                        if constexpr (std::is_same<R, bool>())
                        {
                            info.GetReturnValue().Set(result);
                        }
                        else if constexpr (std::is_same<R, float>())
                        {
                            info.GetReturnValue().Set(result);
                        }
                        else if constexpr (std::is_same<R, CVector>())
                        {
                            info.GetReturnValue().Set(CV8Vector3D::New(result).ToLocalChecked());
                        }
                        else
                            info.GetReturnValue().SetUndefined();
                    }
                }
                catch (const std::invalid_argument& ex)
                {
                    info.GetIsolate()->ThrowException(CV8Utils::ToV8String(ex.what()));
                    info.GetReturnValue().SetUndefined();
                    return;
                }
            },
            External::New(isolate, callbackFunc));
        return functionTemplate;
    }

    static bool ConstructorCallCheck(const FunctionCallbackInfo<Value>& info);

    static void GarbageCollect(int index);

    template <typename T>
    static T* CreateGarbageCollected(Local<Object> object)
    {
        Isolate* isolate = Isolate::GetCurrent();
        assert(isolate);
        T* value = Allocate<T>(isolate);

        //AttachGC(object, sizeof(T));
        return value;
    }

    template <typename T>
    static T* Allocate(Isolate* isolate)
    {
        ArrayBuffer::Allocator* allocator = isolate->GetArrayBufferAllocator();

        T* data = (T*)allocator->Allocate(sizeof(T));
        isolate->AdjustAmountOfExternalAllocatedMemory(sizeof(T));
        return data;
    }

    // Freeing memory used by internal data at index 1 for this object
    static void AttachGC(Isolate* isolate, Local<Object> object);

    template <typename R, typename T>
    static void AddMethod(Local<ObjectTemplate> objectTemplate, const char* name, MethodCallbackFunc<R, T> callback)
    {
        objectTemplate->Set(CV8Utils::ToV8String(name), MethodCallback<R, T>(callback));
    }
};

class JavascriptWrapper
{

private:
    static std::map<int, JavascriptWrapper*> m_pGlobals;
    static int                               i;

    Global<Object> wrapper;

    static void weakCallbackForObjectHolder(const WeakCallbackInfo<JavascriptWrapper>& data)
    {
        delete data.GetParameter();
    }

    int m_pId;
    void* m_pExternalData;

public:
    static int GetGlobalsCount();

    ~JavascriptWrapper()
    {
        delete m_pExternalData;
        m_pGlobals.erase(m_pId);
    }

    JavascriptWrapper(Isolate* pIsolate, Local<Object> object) : m_pId(i)
    {
        wrapper.Reset(pIsolate, object);
        Local<External> wrap = Local<External>::Cast(object->GetInternalField(CV8BaseClass::EInternalFieldPurpose::PointerToValue));
        m_pExternalData = wrap->Value();
        wrapper.SetWeak(this, weakCallbackForObjectHolder, WeakCallbackType::kParameter);

        m_pGlobals[i] = this;
        i++;
    }
};
