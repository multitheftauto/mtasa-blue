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

        void Set(const char* szKey, const char* value);
    private:
        const char*  m_szName;
        TIMEUS       m_start;
        json_object* m_pObject = nullptr;
    };


private:
    bool m_bRecordPerformance = false;
    TIMEUS m_start;

    std::stringstream m_strStream;
};
