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
    g_pGame->GetPerformanceRecorder()->m_strStream << '[';
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

CPerformanceRecorder::Sample::Sample(const char* name) : m_szName(name), m_start(GetTimeUs())
{
    m_pObject = json_object_new_object();
}
//
//{"name": "aaaaaaaaaaa", "cat": "PERF", "ph": "B", "pid": 22630, "tid": 22630, "ts": 829, "cname": "foo"},
//{"name": "aaaaaaaaaaa", "cat": "PERF", "ph": "E", "pid": 22630, "tid": 22630, "ts": 2000, "cname": "foo"},
void CPerformanceRecorder::Sample::Set(const char* szKey, const char* value)
{
    if (m_pObject)
    {
        json_object_object_add(m_pObject, szKey, json_object_new_string(value));
    }
}
CPerformanceRecorder::Sample::~Sample()
{
    if (m_pObject)
    {
        json_object* m_pObjectStart = json_object_new_object();
        json_object_object_add(m_pObjectStart, "name", json_object_new_string(m_szName));
        json_object_object_add(m_pObjectStart, "cat", json_object_new_string("PERF"));
        json_object_object_add(m_pObjectStart, "ph", json_object_new_string("B"));
        json_object_object_add(m_pObjectStart, "ts", json_object_new_int(m_start));
        json_object_object_add(m_pObjectStart, "pid", json_object_new_int(GetCurrentThreadId()));


        Set("name", m_szName);
        Set("cat", "PERF");
        Set("ph", "E");
        json_object_object_add(m_pObject, "ts", json_object_new_int(GetTimeUs()));
        json_object_object_add(m_pObject, "pid", json_object_new_int(GetCurrentThreadId()));

        int flags = JSON_C_TO_STRING_PLAIN;

        g_pGame->GetPerformanceRecorder()->m_strStream << json_object_to_json_string_ext(m_pObjectStart, flags) << ',';
        g_pGame->GetPerformanceRecorder()->m_strStream << json_object_to_json_string_ext(m_pObject, flags) << ',';
        json_object_put(m_pObject);
        json_object_put(m_pObjectStart);
    }

    int a = 5;
}
