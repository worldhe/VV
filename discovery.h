#ifndef DISCOVERY_H
#define DISCOVERY_H

#include "stdsoap2.h"

void* DiscoveryThread(void *param);

class Discovery
{
public:
    Discovery();
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

private:
    struct soap* p_soap;
    char* p_uuid;
    char c_addr[16];
    int i_port;

    pthread_t _t;
    bool b_threadFlag;
};

#endif // DISCOVERY_H
