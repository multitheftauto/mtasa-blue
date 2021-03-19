#pragma once

class CPerformanceRecorder
{
    friend class Sample;

public:
    CPerformanceRecorder();
    ~CPerformanceRecorder();

    void Start();
    void Stop();
    void Clear();

    bool IsDuringPerformanceRecording() const { return m_bRecordPerformance; }
    std::string GetResult();

    class Sample
    {
    public:
        Sample(const char* name);
        ~Sample();

        void SetArg(const char* szKey, const char* value);
        void SetArg(const char* szKey, int value);

    protected:
        std::string m_name;
        std::string m_category = "default";
    private:
        TIMEUS       m_startTime = 0;
        json_object* m_pObject = nullptr;
        json_object* m_pObjectArgs = nullptr;
    };

    class FunctionSample : public Sample
    {
    public:
        FunctionSample(const char* name);
    };
    
    class EventSample : public Sample
    {
    public:
        EventSample(const char* name);
    };

private:
    bool m_bRecordPerformance = false;
    TIMEUS m_start;

    std::stringstream m_strStream;
};
