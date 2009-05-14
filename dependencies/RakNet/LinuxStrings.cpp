
#if (defined(__GNUC__)  || defined(__GCCXML__) || defined(_PS3)) && !defined(__WIN32)
#include <string.h>
int _stricmp(const char* s1, const char* s2)
{
	return strcasecmp(s1,s2);
}
int _strnicmp(const char* s1, const char* s2, size_t n)
{
	return strncasecmp(s1,s2,n);
}
char *_strlwr(char * str )
{
	if (str==0)
		return 0;
	int i=0;
	while (str[i])
	{
		if (str[i]>='A' && str[i]<='Z')
			str[i]+='a'-'A';
	}
	return str;
}
#endif
