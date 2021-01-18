#include "StdInc.h"

using namespace v8;

CV8Isolate::CV8Isolate(std::string& originResource)
{
    m_createParams.array_buffer_allocator = ArrayBuffer::Allocator::NewDefaultAllocator();
    m_pIsolate = Isolate::New(m_createParams);
    m_strOriginResource = originResource;
}

void CV8Isolate::ReportException(v8::Isolate* isolate, v8::TryCatch* tryCatch)
{
    String::Utf8Value      utf8(isolate, tryCatch->Exception());
    v8::Local<v8::Message> message = tryCatch->Message();
    int line = message->GetLineNumber(isolate->GetCurrentContext()).FromJust();
    int column = message->GetEndColumn(isolate->GetCurrentContext()).FromJust();
    printf("%s: '%s' at %i:%i\n", m_strOriginResource.c_str(), *utf8, line, column);
}

void CV8Isolate::RunCode(std::string& code)
{
    Isolate::Scope isolate_scope(m_pIsolate);
    // Create a stack-allocated handle scope.
    HandleScope handle_scope(m_pIsolate);

    // Create a new context.
    // Local<Context> context = Context::New(isolate);
    // Enter the context for compiling and running the hello world script.

    Local<ObjectTemplate> global = ObjectTemplate::New(m_pIsolate);
    //global->Set(isolate, "foo", FunctionTemplate::New(isolate, Foo));
    Local<Context> context = Context::New(m_pIsolate, nullptr, global);
    Context::Scope context_scope(context);

    // Create a string containing the JavaScript source code.
    Local<String> source = String::NewFromUtf8(m_pIsolate, code.c_str(), NewStringType::kNormal).ToLocalChecked();
    Local<String> resourceName = String::NewFromUtf8(m_pIsolate, m_strOriginResource.c_str(), NewStringType::kNormal).ToLocalChecked();
    // Compile the source code.
    ScriptOrigin  origin(resourceName);
    v8::TryCatch          tryCatch(m_pIsolate);
    v8::Local<v8::Script> script;
    if (!Script::Compile(context, source, &origin).ToLocal(&script))
    {
        ReportException(m_pIsolate, &tryCatch);
        return;
    }
    // Run the script to get the result.
    Local<Value> result = script->Run(context).ToLocalChecked();
    // Convert the result to an UTF8 string and print it.
    String::Utf8Value utf8(m_pIsolate, result);
    printf("%s\n", *utf8);
}

CV8Isolate::~CV8Isolate()
{
    m_pIsolate->Dispose();

    delete m_createParams.array_buffer_allocator;
}