#ifndef __XFIREGAMECLIENT_H__
#define __XFIREGAMECLIENT_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
**  XfireIsLoaded()
**
**  returns 1 if application can talk to Xfire, 0 otherwise
*/
int XfireIsLoaded();

/*
**  XfireSetCustomGameDataA()
**
**  ANSI version to tell xfire of custom game data
*/
int XfireSetCustomGameDataA(int num_keys, const char **keys, const char **values);

/*
**  XfireSetCustomGameDataA()
**
**  UNICODE version to tell xfire of custom game data
*/
int XfireSetCustomGameDataW(int num_keys, const wchar_t **keys, const wchar_t **values);

/*
**  XfireSetCustomGameDataUTF8()
**
**  UTF8 version to tell xfire of custom game data
*/
int XfireSetCustomGameDataUTF8(int num_keys, const char **keys, const char **values);

#ifdef UNICODE
#define XfireSetCustomGameData XfireSetCustomGameDataW
#else
#define XfireSetCustomGameData XfireSetCustomGameDataA
#endif


#ifdef __cplusplus
}
#endif

#endif /* __XFIREGAMECLIENT_H__ */
