#ifndef DISCOVERY_H
#define DISCOVERY_H

#include "stdsoap2.h"

void* DiscoveryThread(void *param);

class Discovery
{
public:
    //Discovery();
    static Discovery* GetDiscoveryInstance();
    ~Discovery();
    void Init();
    void Start();
    void Stop();
    struct soap* GetSOAP();
    void GetUUID(char *uuid, short num);
    void GetAddr(char *addr, short num);
    int  GetPort();
    bool GetThreadFlag();
    void SetThreadFlag(bool flag);
    void SendHello();
    void SendBye();

private:
    Discovery();
    static Discovery* s_pDiscovery;
    struct soap* m_pSoap;

public:
    static char s_aUUID[46];
    static char s_aServiceAddress[16];
    static short s_nServicePort;
    static char s_aXaddrs[64];
    static char s_aTypes[28];
    static char s_aScopes[512];

    pthread_t _t;
    bool m_bThreadFlag;

};

#endif // DISCOVERY_H
