#include "StdInc.h"

CPerformanceRecorder::CPerformanceRecorder()
{
    m_samples = json_object_new_array();
}

CPerformanceRecorder::~CPerformanceRecorder()
{
}

void CPerformanceRecorder::Start()
{
    m_bRecordPerformance = true;
}

void CPerformanceRecorder::Stop()
{
    m_bRecordPerformance = false;
}

void CPerformanceRecorder::Clear()
{
    // m_strStream.clear();
    m_start = GetTimeUs();
}

void CPerformanceRecorder::EnterScope()
{
    m_stackSamples.push(json_object_new_object());
}

void CPerformanceRecorder::ExitScope()
{
    assert(m_stackSamples.size() > 0);
    json_object* top = m_stackSamples.top();
    json_object_array_add(m_samples, m_stackSamples.top());
    m_stackSamples.pop();
}

json_object* CPerformanceRecorder::GetSampleObject()
{
    assert(m_stackSamples.size() > 0);
    return m_stackSamples.top();
}

std::string CPerformanceRecorder::GetResult()
{
    int flags = JSON_C_TO_STRING_PLAIN;
    return json_object_to_json_string_ext(m_samples, flags);
}

CPerformanceRecorder::FunctionSample::FunctionSample(const char* name) : Sample(name)
{
}

CPerformanceRecorder::FunctionSample::~FunctionSample()
{
#ifdef MTA_CLIENT
    json_object* object = g_pClientGame->GetPerformanceRecorder()->GetSampleObject();
#else
    json_object* object = g_pGame->GetPerformanceRecorder()->GetSampleObject();
#endif
    json_object_object_add(object, "name", json_object_new_string(("Native function: " + m_name).c_str()));
    json_object_object_add(object, "cat", json_object_new_string("function"));
}

CPerformanceRecorder::EventSample::EventSample(const char* name) : Sample(name)
{
}

CPerformanceRecorder::EventSample::~EventSample()
{
#ifdef MTA_CLIENT
    json_object* object = g_pClientGame->GetPerformanceRecorder()->GetSampleObject();
#else
    json_object* object = g_pGame->GetPerformanceRecorder()->GetSampleObject();
#endif
    json_object_object_add(object, "name", json_object_new_string(("Event: " + m_name).c_str()));
    json_object_object_add(object, "cat", json_object_new_string("event"));
}

CPerformanceRecorder::Sample::Sample(const char* name) : m_name(name), m_startTime(GetTimeUs())
{
#ifdef MTA_CLIENT
    m_enabled = g_pClientGame->GetPerformanceRecorder()->m_bRecordPerformance;
    g_pClientGame->GetPerformanceRecorder()->EnterScope();
#else
    m_enabled = g_pGame->GetPerformanceRecorder()->m_bRecordPerformance;
    g_pGame->GetPerformanceRecorder()->EnterScope();
#endif
}

void CPerformanceRecorder::Sample::SetArg(const char* szKey, const char* value)
{
    if (!m_enabled)
        return;

#ifdef MTA_CLIENT
    json_object* object = g_pClientGame->GetPerformanceRecorder()->GetSampleObject();
#else
    json_object* object = g_pGame->GetPerformanceRecorder()->GetSampleObject();
#endif
    json_object* args = json_object_object_get(object, "args");
    if (!args)
    {
        args = json_object_new_object();
        json_object_object_add(object, "args", args);
    }

    json_object_object_add(args, szKey, json_object_new_string(value));
}

void CPerformanceRecorder::Sample::SetArg(const char* szKey, int value)
{
    if (!m_enabled)
        return;

#ifdef MTA_CLIENT
    json_object* object = g_pClientGame->GetPerformanceRecorder()->GetSampleObject();
#else
    json_object* object = g_pGame->GetPerformanceRecorder()->GetSampleObject();
#endif
    json_object* args = json_object_object_get(object, "args");
    if (!args)
    {
        args = json_object_new_object();
        json_object_object_add(object, "args", args);
    }

    json_object_object_add(args, szKey, json_object_new_int(value));
}

CPerformanceRecorder::Sample::~Sample()
{
    if (!m_enabled)
        return;

#ifdef MTA_CLIENT
    json_object* object = g_pClientGame->GetPerformanceRecorder()->GetSampleObject();
#else
    json_object* object = g_pGame->GetPerformanceRecorder()->GetSampleObject();
#endif

    if (!json_object_object_get(object, "name"))
        json_object_object_add(object, "name", json_object_new_string(m_name.c_str()));
    if (!json_object_object_get(object, "cat"))
        json_object_object_add(object, "cat", json_object_new_string("default"));

    json_object_object_add(object, "ph", json_object_new_string("X"));
    json_object_object_add(object, "ts", json_object_new_int(m_startTime));
    json_object_object_add(object, "dur", json_object_new_int(GetTimeUs() - m_startTime));
    json_object_object_add(object, "tid", json_object_new_int(getpid()));

#ifdef MTA_CLIENT
    g_pClientGame->GetPerformanceRecorder()->ExitScope();
#else
    g_pGame->GetPerformanceRecorder()->ExitScope();
#endif
}
