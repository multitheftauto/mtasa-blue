#if defined(_WIN32) && !(defined(__GNUC__)  || defined(__GCCXML__)) && !defined(_RAKNET_LIB) && defined(_RAKNET_DLL)
#define RAK_DLL_EXPORT __declspec(dllexport)
#else
#define RAK_DLL_EXPORT  
#endif
