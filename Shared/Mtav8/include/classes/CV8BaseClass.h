#include <assert.h>

using namespace v8;

class CV8FunctionCallback;

class CV8BaseClass
{
public:
    enum class EClass
    {
        Invalid, // To distinguish between nullptr and actual class.
        Vector2,
        Vector3,
        Vector4,
        Matrix,
    };

    template <typename R, typename T>
    using MethodCallbackFunc = R(*)(CV8FunctionCallback& info, Local<Object> self, T* value);

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
                Local<External>          wrap = Local<External>::Cast(self->GetInternalField(1));
                void*                    ptr = wrap->Value();
                assert(ptr);
                T*                       value = static_cast<T*>(ptr);
                CV8FunctionCallback      args(info);
                R                        result;
                try
                {
                    result = cb(args, self, value);
                }
                catch (const std::invalid_argument& ex)
                {
                    info.GetIsolate()->ThrowException(CV8Utils::ToV8String(ex.what()));
                    info.GetReturnValue().SetUndefined();
                    return;
                }

                if constexpr (std::is_same<R, bool>())
                {
                    info.GetReturnValue().Set(result);
                }else if constexpr (std::is_same<R, float>())
                {
                    info.GetReturnValue().Set(result);
                }
                else
                    info.GetReturnValue().SetUndefined();
            },
            External::New(isolate, callbackFunc));
        return functionTemplate;
    }

    static bool ConstructorCallCheck(const FunctionCallbackInfo<Value>& info);

    template<typename T>
    static T* CreateGarbageCollected(Local<Object> object)
    {
        Isolate* isolate = Isolate::GetCurrent();
        assert(isolate);
        T* value = Allocate<T>(isolate);
        AttachGC(object, sizeof(T));
        return value;
    }

    template <typename T>
    static T* Allocate(Isolate* isolate) {
        ArrayBuffer::Allocator* allocator = isolate->GetArrayBufferAllocator();

        T* data = (T*)allocator->Allocate(sizeof(T));
        isolate->AdjustAmountOfExternalAllocatedMemory(sizeof(T));
        return data;
    }

    static std::map<int, Global<Object>*> m_mapPersistents;
    static int                            i;

    // Freeing memory used by internal data at index 1 for this object
    // "blockSize" is a size of allocated memory
    static void AttachGC(Local<Object> object, size_t blockSize);

    template <typename R, typename T>
    static void AddMethod(Local<ObjectTemplate> objectTemplate, const char* name, MethodCallbackFunc<R, T> callback)
    {
        objectTemplate->Set(CV8Utils::ToV8String(name), MethodCallback<R, T>(callback));
    }
};
