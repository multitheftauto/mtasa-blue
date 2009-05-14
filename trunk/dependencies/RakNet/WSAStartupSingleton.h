#ifndef __WSA_STARTUP_SINGLETON_H
#define __WSA_STARTUP_SINGLETON_H

class WSAStartupSingleton
{
public:
	WSAStartupSingleton();
	~WSAStartupSingleton();
	static void AddRef(void);
	static void Deref(void);

protected:
	static int refCount;
};

#endif
