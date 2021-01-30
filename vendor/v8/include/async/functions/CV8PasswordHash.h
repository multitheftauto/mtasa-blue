#include "../CV8AsyncFunction.h"

class CV8PasswordHash : public CV8AsyncFunction
{
public:
    CV8PasswordHash(std::string password, int cost) : m_strPassword(password), m_iCost(cost) {
    }

    void Run(CV8DelegateBase* delegate);
    void Cancel();

private:
    std::string m_strPassword;
    int m_iCost;
};
