#include "StdInc.h"
#include <queue>

using namespace v8;

static std::queue<std::string> modulesListName;            // Todo, get rid of this list

CV8Isolate::CV8Isolate(const CV8* pCV8, std::string& originResource) : m_pCV8(pCV8)
{
    m_createParams.array_buffer_allocator = ArrayBuffer::Allocator::NewDefaultAllocator();
    m_pIsolate = Isolate::New(m_createParams);
    m_strOriginResource = originResource;
    m_pIsolate->SetMicrotasksPolicy(v8::MicrotasksPolicy::kExplicit);
    m_pIsolate->SetData(0, this);
}

void CV8Isolate::DoPulse()
{   
    m_pIsolate->PerformMicrotaskCheckpoint();
}

class CMicrotask
{
public:
    CMicrotask(std::function<void(CV8Isolate*)> microtask, CV8Isolate* isolate) : m_microtask(microtask), m_isolate(isolate) {}
    std::function<void(CV8Isolate*)> m_microtask;
    CV8Isolate*                      m_isolate;
};

void CV8Isolate::EnqueueMicrotask(std::function<void(CV8Isolate*)> microtask)
{
    Locker lock(m_pIsolate);
    m_pIsolate->EnqueueMicrotask(
        [](void* data) {
            CMicrotask* microtask = (CMicrotask*)data;
            microtask->m_microtask(microtask->m_isolate);
            delete microtask;
    },
        new CMicrotask(microtask, this));
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

void CV8Isolate::AddPromise(std::unique_ptr<CV8Promise> pPromise)
{
    CV8Promise* promise = pPromise.get();
    m_pCV8->GetPlatform()->CallOnWorkerThread(std::make_unique<CV8Task>([promise]() { promise->Run(); }));
    m_vecPromises.push_back(std::move(pPromise));
}

MaybeLocal<Value> CV8Isolate::InitializeModuleExports(Local<Context> context, Local<Module> module)
{
    Locker      lock(m_pIsolate);
    const char* name = modulesListName.front().c_str();
    CV8Module*  pModule = CV8::GetModuleByName(name);
    for (auto const& pair : pModule->GetFunctions())
    {
        Local<Value> value = External::New(context->GetIsolate(), pair.second);

        FunctionCallback callback = [](const FunctionCallbackInfo<Value>& args) {
            Locker                  lock(args.GetIsolate());
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

void CV8Isolate::RunCode(std::string& code, bool bAsModule)
{
    Locker lock(m_pIsolate);
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
}

CV8Isolate::~CV8Isolate()
{
    delete m_createParams.array_buffer_allocator;
    m_pIsolate->Dispose();
}