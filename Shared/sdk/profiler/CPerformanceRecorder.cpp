#include "StdInc.h"

CPerformanceRecorder::CPerformanceRecorder()
{
}

CPerformanceRecorder::~CPerformanceRecorder()
{
}

void CPerformanceRecorder::Start()
{
    m_bRecordPerformance = true;
#ifdef MTA_CLIENT
    g_pClientGame->GetPerformanceRecorder()->m_strStream
#else
    g_pGame->GetPerformanceRecorder()->m_strStream
#endif
    << '[';
}

void CPerformanceRecorder::Stop()
{
    m_bRecordPerformance = false;
}

void CPerformanceRecorder::Clear()
{
    m_strStream.clear();
    m_start = GetTimeUs();
}

std::string CPerformanceRecorder::GetResult()
{
    return m_strStream.str() + "]";
}

CPerformanceRecorder::FunctionSample::FunctionSample(const char* name) : Sample("")
{
    m_name = std::string("Native function: ") + name;
    m_category = "function";
}

CPerformanceRecorder::EventSample::EventSample(const char* name) : Sample("")
{
    m_name = std::string("Event: ") + name;
    m_category = "event";
}

CPerformanceRecorder::Sample::Sample(const char* name) : m_name(name), m_startTime(GetTimeUs())
{
#ifdef MTA_CLIENT
    if (g_pClientGame->GetPerformanceRecorder()->m_bRecordPerformance)
#else
    if (g_pGame->GetPerformanceRecorder()->m_bRecordPerformance)
#endif
        m_pObject = json_object_new_object();
}

void CPerformanceRecorder::Sample::SetArg(const char* szKey, const char* value)
{
    if (!m_pObjectArgs)
        m_pObjectArgs = json_object_new_object();

    json_object_object_add(m_pObjectArgs, szKey, json_object_new_string(value));
}

void CPerformanceRecorder::Sample::SetArg(const char* szKey, int value)
{
    if (!m_pObjectArgs)
        m_pObjectArgs = json_object_new_object();

    json_object_object_add(m_pObjectArgs, szKey, json_object_new_int(value));
}

CPerformanceRecorder::Sample::~Sample()
{
    if (m_pObject)
    {
        json_object* m_pObjectStart = json_object_new_object();
        json_object_object_add(m_pObjectStart, "name", json_object_new_string(m_name.c_str()));
        json_object_object_add(m_pObjectStart, "cat", json_object_new_string(m_category.c_str()));
        json_object_object_add(m_pObjectStart, "ph", json_object_new_string("B"));
        json_object_object_add(m_pObjectStart, "ts", json_object_new_int(m_startTime));
        json_object_object_add(m_pObjectStart, "pid", json_object_new_int(0));

        json_object_object_add(m_pObject, "name", json_object_new_string(m_name.c_str()));
        json_object_object_add(m_pObject, "cat", json_object_new_string(m_category.c_str()));
        json_object_object_add(m_pObject, "ph", json_object_new_string("E"));
        json_object_object_add(m_pObject, "ts", json_object_new_int(GetTimeUs()));
        json_object_object_add(m_pObject, "pid", json_object_new_int(0));

        if (m_pObjectArgs)
        {
            json_object_object_add(m_pObjectStart, "args", m_pObjectArgs);
        }

        int flags = JSON_C_TO_STRING_PLAIN;

#ifdef MTA_CLIENT
        g_pClientGame->GetPerformanceRecorder()->m_strStream
#else
        g_pGame->GetPerformanceRecorder()->m_strStream
#endif
                                                       << json_object_to_json_string_ext(m_pObjectStart, flags) << ','
                                                       << json_object_to_json_string_ext(m_pObject, flags) << ',';

        json_object_put(m_pObject);
        json_object_put(m_pObjectStart);
    }
}
