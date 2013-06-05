
#if defined(__GNUC__) && (__GNUC__ >= 3)

// gcc's hash(), used in the google hash maps, does not support pointers by default.
// Also gcc refuses to implicitly cast function pointers to void*... so we
// need a special case.

struct lua_State;
typedef int (*lua_CFunction) (lua_State *L);
class CPlayer;
class CResource;
class CLuaArguments;

namespace __gnu_cxx
{
    template <>
    struct hash < lua_State* >
    {
        size_t operator()( lua_State* const& pFunc ) const
        {
            return (size_t)pFunc;
        }
    };

    template <>
    struct hash < lua_CFunction >
    {
        size_t operator()( lua_CFunction const& pFunc ) const
        {
            return (size_t)pFunc;
        }
    };

    template <>
    struct hash < CPlayer* >
    {
        size_t operator()( CPlayer* const& pFunc ) const
        {
            return (size_t)pFunc;
        }
    };

    template <>
    struct hash < CResource* >
    {
        size_t operator()( CResource* const& pFunc ) const
        {
            return (size_t)pFunc;
        }
    };

    template <>
    struct hash < CLuaArguments* >
    {
        size_t operator()( const CLuaArguments* pArguments ) const
        {
            return (size_t)pArguments;
        }
    };

    template <>
    struct hash < const void* >
    {
        size_t operator()( const void* pArguments ) const
        {
            return (size_t)pArguments;
        }
    };
}

#endif
