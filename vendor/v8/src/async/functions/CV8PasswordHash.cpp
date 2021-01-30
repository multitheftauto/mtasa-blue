#include "StdInc.h"
#include "async/functions/CV8PasswordHash.h"

void CV8PasswordHash::Run(CV8DelegateBase* delegate)
{
    delegate->Resolve(SharedUtil::BcryptHash(m_strPassword, "", m_iCost));
}

void CV8PasswordHash::Cancel()
{

}