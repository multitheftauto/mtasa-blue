using namespace v8;

class CV8Task : public Task
{
public:
    CV8Task(std::function<void()> task) : m_task(task) {}

    ~CV8Task(){};
    void Run() { m_task(); };

private:
    std::function<void()> m_task;
    std::mutex         m_lock;
};