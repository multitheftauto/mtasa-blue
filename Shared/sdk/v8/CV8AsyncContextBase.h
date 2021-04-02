// Resolving promise
// Content of lambda used in promise is executed by worker thread!
// All methods in this class are safe to use inside promise lambda.
class CV8AsyncContextBase
{
public:
    // fulfilling promise with `undefined` value
    virtual void Resolve() = 0;
    // fulfilling promise with `string` value
    virtual void Resolve(std::string value) = 0;
};
