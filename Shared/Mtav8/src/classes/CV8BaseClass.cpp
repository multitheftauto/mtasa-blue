#include "StdInc.h"

int CV8BaseClass::i;
std::map<int, Global<Object>*> CV8BaseClass::m_mapPersistents;

// Common checks
bool CV8BaseClass::ConstructorCallCheck(const FunctionCallbackInfo<Value>& info)
{
    if (!info.IsConstructCall())
    {
        info.GetIsolate()->ThrowException(String::NewFromUtf8(info.GetIsolate(), "Cannot call constructor as function").ToLocalChecked());
        return false;
    }
    return true;
}

void CV8BaseClass::AttachGC(Local<Object> object, size_t blockSize)
{
    CV8BaseClass::m_mapPersistents[CV8BaseClass::i] = new Global<Object>(Isolate::GetCurrent(), object);

    CV8BaseClass::m_mapPersistents[CV8BaseClass::i]->SetWeak(
        new int[]{CV8BaseClass::i, (int)blockSize},
        [](const WeakCallbackInfo<int>& info) {
            int* id = info.GetParameter();
            auto persistent = CV8BaseClass::m_mapPersistents[id[0]];
            persistent->ClearWeak();
            persistent->Reset();
            delete CV8BaseClass::m_mapPersistents.at(id[0]);
            CV8BaseClass::m_mapPersistents.erase(id[0]);
            info.GetIsolate()->GetArrayBufferAllocator()->Free(info.GetInternalField(1), id[1]);
            delete id;
        },
        WeakCallbackType::kFinalizer);
    i++;
}