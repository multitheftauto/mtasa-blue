#include <assert.h>

using namespace v8;

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

    static bool ConstructorCallCheck(const FunctionCallbackInfo<Value>& info);

    template<typename T>
    static T* CreateGarbageCollected(Isolate* isolate, Local<Object> object)
    {
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
};
