#include "StdInc.h"
#include <queue>

using namespace v8;

static std::queue<std::string> modulesListName;            // Todo, get rid of this list

class InspectorClientImpl : public v8_inspector::V8InspectorClient
{
public:
    InspectorClientImpl() {}
};

class ChannelImpl : public v8_inspector::V8Inspector::Channel
{
public:
    ChannelImpl(Isolate* pIsolate) : m_pIsolate(pIsolate) {}
    void sendResponse(int callId, std::unique_ptr<v8_inspector::StringBuffer> message) { int a = 5; }
    void sendNotification(std::unique_ptr<v8_inspector::StringBuffer> message)
    {
        v8::HandleScope handle_scope(m_pIsolate);
        auto stringView = message->string();
        int  length = static_cast<int>(stringView.length());
        Local<String> mess =
            (stringView.is8Bit()
                 ? v8::String::NewFromOneByte(m_pIsolate, reinterpret_cast<const uint8_t*>(stringView.characters8()), v8::NewStringType::kNormal, length)
                 : v8::String::NewFromTwoByte(m_pIsolate, reinterpret_cast<const uint16_t*>(stringView.characters16()), v8::NewStringType::kNormal, length))
                .ToLocalChecked();

        String::Utf8Value utf8(m_pIsolate, mess);
        std::string       str(*utf8);

        printf("[SEND NOTIFICATION] %s\n", str.c_str());
    }
    void                                flushProtocolNotifications() { int b = 5; }
    std::unique_ptr<v8_inspector::V8InspectorSession> connect(int contextGroupId, Channel*, v8_inspector::StringView state) { int b = 5; };
    std::unique_ptr<v8_inspector::V8StackTrace>         createStackTrace(v8::Local<v8::StackTrace>) { int b = 5; }
    std::unique_ptr<v8_inspector::V8StackTrace>         captureStackTrace(bool fullStack) { int c = 5; }

private:
    Isolate* m_pIsolate;
};

CV8Isolate::CV8Isolate(const CV8* pCV8, std::string& originResource) : m_pCV8(pCV8)
{
    m_createParams.array_buffer_allocator = ArrayBuffer::Allocator::NewDefaultAllocator();
    m_pIsolate = Isolate::New(m_createParams);
    m_strOriginResource = originResource;
    m_pIsolate->SetMicrotasksPolicy(v8::MicrotasksPolicy::kExplicit);

    m_pIsolate->SetData(0, this);
}

void CV8Isolate::DoTaskPulse()
{
    for (auto const& promise : m_vecPromises)
    {
        if (promise)
            promise->Execute();
    }
}

void CV8Isolate::DoPulse()
{   
    bool anyPromiseHasResolved = false;
    for (int i = m_vecPromises.size() - 1; i >= 0; --i)
    {
        const auto& promise = m_vecPromises[i];
        if (promise->FulFill())
        {
            m_vecPromises.erase(m_vecPromises.begin() + i);
            anyPromiseHasResolved = true;
        }
    }
    if (anyPromiseHasResolved)
        m_pIsolate->PerformMicrotaskCheckpoint();
}

void CV8Isolate::ReportException(v8::TryCatch* pTryCatch)
    {
    String::Utf8Value      utf8(m_pIsolate, pTryCatch->Exception());
    v8::Local<v8::Message> message = pTryCatch->Message();
    int                    line = message->GetLineNumber(m_pIsolate->GetCurrentContext()).FromJust();
    int                    column = message->GetEndColumn(m_pIsolate->GetCurrentContext()).FromJust();
    printf("%s: '%s' at %i:%i\n", m_strOriginResource.c_str(), *utf8, line, column);
}

MaybeLocal<Module> CV8Isolate::InstantiateModule(Local<Context> context, Local<String> specifier, Local<FixedArray> import_assertions, Local<Module> referrer)
{
    String::Utf8Value  importName(context->GetIsolate(), specifier);
    CV8Module*         pModule = CV8::GetModuleByName(*importName);
    MaybeLocal<Module> module;
    if (!pModule)
        return module;

    modulesListName.push(*importName);

    Local<String> moduleName = String::NewFromUtf8(context->GetIsolate(), *importName).ToLocalChecked();
    auto          exports = pModule->GetExports(context->GetIsolate());
    module = Module::CreateSyntheticModule(context->GetIsolate(), moduleName, exports, [](Local<Context> context, Local<Module> module) {
        CV8Isolate* pThisIsolate = (CV8Isolate*)context->GetIsolate()->GetData(0);
        return pThisIsolate->InitializeModuleExports(context, module);
    });
    Local<Module> checkedModule;
    if (!module.ToLocal(&checkedModule))
    {
        printf("error?");
        return module;
    }

    return checkedModule;
}

MaybeLocal<Value> CV8Isolate::InitializeModuleExports(Local<Context> context, Local<Module> module)
{
    const char* name = modulesListName.front().c_str();
    CV8Module*  pModule = CV8::GetModuleByName(name);
    for (auto const& pair : pModule->GetFunctions())
    {
        Local<Value> value = External::New(context->GetIsolate(), pair.second);

        FunctionCallback callback = [](const FunctionCallbackInfo<Value>& args) {
            v8::HandleScope         handle_scope(args.GetIsolate());
            v8::Local<v8::External> ext = args.Data().As<v8::External>();
            CV8Isolate*             pThisIsolate = (CV8Isolate*)args.GetIsolate()->GetData(0);
            void (*func)(CV8FunctionCallbackBase*) = static_cast<void (*)(CV8FunctionCallbackBase*)>(ext->Value());
            CV8FunctionCallback callback(args);
            func(&callback);
        };

        module->SetSyntheticModuleExport(context->GetIsolate(), String::NewFromUtf8(context->GetIsolate(), pair.first).ToLocalChecked(),
                                         v8::Function::New(context, callback, value).ToLocalChecked());
    }
    modulesListName.pop();
    return True(context->GetIsolate());
}

void CV8Isolate::TestMess(std::string mess)
{
    v8_inspector::StringView view((uint8_t*)mess.c_str(), mess.length() + 1);
    m_pSession->dispatchProtocolMessage(view);
}

void CV8Isolate::RunCode(std::string& code, bool bAsModule)
{
    Isolate::Scope isolate_scope(m_pIsolate);
    // Create a stack-allocated handle scope.
    HandleScope handle_scope(m_pIsolate);

    Local<ObjectTemplate> global = ObjectTemplate::New(m_pIsolate);
    Local<Context> context = Context::New(m_pIsolate, nullptr, global);

    Context::Scope context_scope(context);
    Handle<FunctionTemplate> vector2dTemplate = CV8Vector2D::CreateTemplate(context);
    Handle<FunctionTemplate> vector3dTemplate = CV8Vector3D::CreateTemplate(context, vector2dTemplate);
    Handle<FunctionTemplate> matrixTemplate = CV8Matrix::CreateTemplate(context);

    // Create a string containing the JavaScript source code.
    Local<String> source = String::NewFromUtf8(m_pIsolate, code.c_str(), NewStringType::kNormal).ToLocalChecked();
    Local<String> resourceName = String::NewFromUtf8(m_pIsolate, m_strOriginResource.c_str(), NewStringType::kNormal).ToLocalChecked();
    // Compile the source code.

    {
        v8::TryCatch compileTryCatch(m_pIsolate);
        if (bAsModule)
        {
            ScriptOrigin origin(resourceName, 0, 0, false, -1, Local<Value>(), false, false, true);

            ScriptCompiler::Source compilerSource(source, origin);
            Local<Module>          module;
            if (!ScriptCompiler::CompileModule(m_pIsolate, &compilerSource).ToLocal(&module))
            {
                ReportException(&compileTryCatch);
                return;
            }
            for (int i = 0; i < module->GetModuleRequestsLength(); i++)
            {
                Local<ModuleRequest> request = module->GetModuleRequests()->Get(context, i).As<ModuleRequest>();

                v8::Maybe<bool> result = module->InstantiateModule(context, InstantiateModule);
                if (result.IsNothing())
                {
                    v8::String::Utf8Value str(context->GetIsolate(), request->GetSpecifier());
                    printf("Failed to import module %s\n", *str);
                }
            }
            if (Module::Status::kInstantiated == module->GetStatus())
            {
                v8::TryCatch evaluateTryCatch(m_pIsolate);
                Local<Value> returnValue;
                if (!module->Evaluate(context).ToLocal(&returnValue))
                {
                    ReportException(&evaluateTryCatch);
                    return;
                }
            }
        }
        else
        {
            ScriptOrigin  origin(resourceName);
            Local<Script> script;

            if (!Script::Compile(context, source, &origin).ToLocal(&script))
            {
                ReportException(&compileTryCatch);
                return;
            }
        }
    }

    // v8_inspector::V8InspectorClient* client = new v8_inspector::V8InspectorClient();
    // std::unique_ptr<v8_inspector::V8Inspector> inspector = v8_inspector::V8Inspector::create(m_pIsolate, client);

    // if (result.IsEmpty())
    // Local<Value> result = script->Run(context).ToLocalChecked();
    //// Convert the result to an UTF8 string and print it.
    // String::Utf8Value utf8(m_pIsolate, result);
    // printf("%s\n", *utf8);

    if (true)
    {
        m_pClient = std::make_unique<InspectorClientImpl>();

        m_pInspector = std::move(v8_inspector::V8Inspector::create(m_pIsolate, m_pClient.get()));

        std::string                         test("TEST MTA 123123");
        m_pChannel = std::make_unique<ChannelImpl>(m_pIsolate);
        v8_inspector::StringView              view((uint8_t*)test.c_str(), test.length() + 1);
        v8_inspector::StringView              ctx_name((uint8_t*)test.c_str(), test.length() + 1);

        m_pSession = m_pInspector->connect(1, m_pChannel.get(), view);

        m_pInspector->contextCreated(v8_inspector::V8ContextInfo(context, 1, ctx_name));

    }
}

CV8Isolate::~CV8Isolate()
{
    m_pIsolate->Dispose();

    delete m_createParams.array_buffer_allocator;
}