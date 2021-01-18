// https://storage.googleapis.com/chromium-v8/official/canary/v8-win32-rel-latest.json
// https://storage.googleapis.com/chromium-v8/official/canary/v8-${fileName}-rel-${version}.zip
// https://storage.googleapis.com/chromium-v8/official/canary/v8-win32-rel-8.9.276.zip
// https://chromium.googlesource.com/v8/v8.git/+archive/roll.tar.gz

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/v8.h"
#include "include/libplatform/libplatform.h"
#include "SharedUtil.Defines.h"
void Foo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int len = args.Length();
    int a = 5;
}

int testV8(/*int argc, char* argv[]*/)
    {
    // Initialize V8.
    v8::V8::InitializeICUDefaultLocation("");
    v8::V8::InitializeExternalStartupData("");
    //v8::V8::InitializeICUDefaultLocation(argv[0]);
    //v8::V8::InitializeExternalStartupData(argv[0]);

    std::unique_ptr<v8::Platform> platform = v8::platform::NewSingleThreadedDefaultPlatform();
    v8::V8::InitializePlatform(platform.get());
    v8::V8::Initialize();
    // Create a new Isolate and make it the current one.
    v8::Isolate::CreateParams create_params;


    create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    v8::Isolate* isolate = v8::Isolate::New(create_params);
    {
        v8::Isolate::Scope isolate_scope(isolate);


        // Create a stack-allocated handle scope.
        v8::HandleScope handle_scope(isolate);
        // Create a new context.
        //v8::Local<v8::Context> context = v8::Context::New(isolate);
        // Enter the context for compiling and running the hello world script.

        v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
        global->Set(isolate, "foo", v8::FunctionTemplate::New(isolate, Foo));
        v8::Local<v8::Context>        context = v8::Context::New(isolate, NULL, global);
        v8::Context::Scope context_scope(context);

        // Create a string containing the JavaScript source code.
        v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate, "foo(1,'two')", v8::NewStringType::kNormal).ToLocalChecked();
        v8::Local<v8::String> resourceName = v8::String::NewFromUtf8(isolate, "Foo resource", v8::NewStringType::kNormal).ToLocalChecked();
        // Compile the source code.
        v8::ScriptOrigin      origin(resourceName);
        v8::Local<v8::Script> script = v8::Script::Compile(context, source, &origin).ToLocalChecked();
        // Run the script to get the result.
        v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();
        // Convert the result to an UTF8 string and print it.
        v8::String::Utf8Value utf8(isolate, result);
        printf("%s\n", *utf8);
    }
    // Dispose the isolate and tear down V8.
    isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
    delete create_params.array_buffer_allocator;
    return 0;
}

MTAEXPORT int InitServer(int argc, char* argv[])
{
    return testV8();
}