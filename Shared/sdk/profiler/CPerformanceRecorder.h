#pragma once

#include <stack>

class CPerformanceRecorder
{
    friend class Sample;

public:
    CPerformanceRecorder();
    ~CPerformanceRecorder();

    void         Start();
    void         Stop();
    void         Clear();
    void         EnterScope();
    void         ExitScope();
    json_object* GetSampleObject();

    bool        IsDuringPerformanceRecording() const { return m_bRecordPerformance; }
    std::string GetResult();

    class Sample
    {
    public:
        Sample(const char* name);
        virtual ~Sample();

        void SetArg(const char* szKey, const char* value);
        void SetArg(const char* szKey, int value);
        void SetArg(const char* szKey, size_t value);
        void SetArg(const char* szKey, bool value);

        void Exit();

    protected:
        std::string m_name;
        bool        m_enabled;

    private:
        TIMEUS m_startTime = 0;
    };

    class FunctionSample : public Sample
    {
    public:
        FunctionSample(const char* name);
        ~FunctionSample();
    };

    class EventSample : public Sample
    {
    public:
        EventSample(const char* name);
        ~EventSample();
    };

private:
    bool   m_bRecordPerformance = false;
    TIMEUS m_start;

    std::stack<json_object*> m_stackSamples;
    json_object*             m_samples;
};
