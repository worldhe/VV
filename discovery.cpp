#include "discovery.h"
#include "gsoap/onvif/core/onvifdiscovery/plugin/wsddapi.h"
#include "gsoap/onvif/core/onvifdiscovery/onvifdiscoverywsddService.h"
#include "gsoap/onvif/core/onvifdiscovery/wsdd.nsmap"

//组播地址
#define ADDR "239.255.255.250"
//组播端口
#define PORT 3702

Discovery::Discovery()
    :p_soap(nullptr)
    ,p_uuid(nullptr)
    ,c_addr(ADDR)
    ,i_port(PORT)
    ,_t(0)
    ,b_threadFlag(true)
{

}

Discovery::~Discovery()
{
    if (this->p_uuid != nullptr)
    {
        delete this->p_uuid;
    }

    if (this->p_soap != nullptr)
    {
        soap_destroy(this->p_soap);
        soap_end(this->p_soap);
        soap_free(this->p_soap);
        delete this->p_soap;
        this->p_soap = nullptr;
    }
}

void Discovery::Init()
{
    this->p_soap = soap_new1(SOAP_IO_UDP);
    this->p_uuid = (char*)soap_wsa_rand_uuid(this->p_soap);
}

void Discovery::Start()
{
    this->SetThreadFlag(true);
    int ret = pthread_create(&this->_t, nullptr, DiscoveryThread, this);
    if (ret)
    {
        printf("Failed to create discovery thread. errno = %d : %s\n", errno, strerror(errno));
    }
}

void Discovery::Stop()
{
    this->SetThreadFlag(false);
}

void* DiscoveryThread(void *param)
{
    pthread_detach(pthread_self());
    Discovery *discovery = (Discovery *)param;
    struct soap* soap = discovery->GetSOAP();
    int port = discovery->GetPort();
    char addr[16];
    discovery->GetAddr(addr, sizeof (addr));
    if (soap == nullptr || strlen(addr) == 0)
        goto RETURN;

    //reuse address
    soap->bind_flags = SO_REUSEADDR;

    //bind
    if (!soap_valid_socket(soap_bind(soap, NULL, port, 100)))
    {
        soap_print_fault(soap, stderr);
        printf("Failed to bind port.\n");
        goto RETURN;
    }

    //optionally join a multicast group
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(addr);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(soap->socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)))
    {
        printf("Failed to set socket options. errno = %d:%s\n", errno, strerror(errno));
        goto RETURN;
    }

    while(discovery->GetThreadFlag())
    {
        printf("ONVIF discovery is listening. %s:%d\n", addr, port);
        //listen
        int ret = soap_wsdd_listen(soap, 1); // listen for messages for 1 s
        if (ret)
        {
            soap_print_fault(soap, stderr);
            printf("Failed to listen.\n");
            goto RETURN;
        }
    }

RETURN:
    pthread_exit(nullptr);
}

void Discovery::GetUUID(char* uuid, short num)
{
    //标准uuid是45个字符组成
    if (this->p_uuid == nullptr || num < 46)
    {
        printf("Failed to obtain UUID.\n");
        return;
    }
    memset(uuid, '\0', num);
    strncpy(uuid, this->p_uuid, strlen(this->p_uuid));
}

void Discovery::GetAddr(char *addr, short num)
{
    //标准addr是15字符组成
    if (num < 16)
    {
        printf("1.2\n");
        printf("Failed to obtain addr.\n");
        return;
    }
    memset(addr, '\0', num);
    strncpy(addr, this->c_addr, strlen(this->c_addr));
}

int Discovery::GetPort()
{
    return this->i_port;
}

bool Discovery::GetThreadFlag()
{
    return this->b_threadFlag;
}

void Discovery::SetThreadFlag(bool flag)
{
    this->b_threadFlag = flag;
}

struct soap* Discovery::GetSOAP()
{
    return this->p_soap;
}

//onvifdiscoverywsddService.h
int wsddService::Hello(struct wsdd__HelloType *wsdd__Hello)
{
    return SOAP_OK;
}

//onvifdiscoverywsddService.h
int wsddService::Bye(struct wsdd__ByeType *wsdd__Bye)
{
    return SOAP_OK;
}

//onvifdiscoverywsddService.h
int wsddService::Probe(struct wsdd__ProbeType *wsdd__Probe)
{
    return SOAP_OK;
}

//onvifdiscoverywsddService.h
int wsddService::ProbeMatches(struct wsdd__ProbeMatchesType *wsdd__ProbeMatches)
{
    return SOAP_OK;
}

//onvifdiscoverywsddService.h
int wsddService::Resolve(struct wsdd__ResolveType *wsdd__Resolve)
{
    return SOAP_OK;
}

//onvifdiscoverywsddService.h
int wsddService::ResolveMatches(struct wsdd__ResolveMatchesType *wsdd__ResolveMatches)
{
    return SOAP_OK;
}

//wsddapi.h
void wsdd_event_Hello(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int MetadataVersion)
{

}

//wsddapi.h
void wsdd_event_Bye(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int *MetadataVersion)
{

}

//wsddapi.h
soap_wsdd_mode wsdd_event_Probe(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *Types, const char *Scopes, const char *MatchBy, struct wsdd__ProbeMatchesType *matches)
{
    return SOAP_WSDD_ADHOC;
}

//wsddapi.h
void wsdd_event_ProbeMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ProbeMatchesType *matches)
{

}

//wsddapi.h
soap_wsdd_mode wsdd_event_Resolve(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *EndpointReference, struct wsdd__ResolveMatchType *match)
{
    return SOAP_WSDD_ADHOC;
}

//wsddapi.h
void wsdd_event_ResolveMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ResolveMatchType *match)
{

}

//wsddapi.h
SOAP_FMAC5 int SOAP_FMAC6 soap_send___wsdd__Hello(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct wsdd__HelloType *wsdd__Hello)
{

}

//wsddapi.h
SOAP_FMAC5 int SOAP_FMAC6 soap_recv___wsdd__Hello(struct soap *soap, struct __wsdd__Hello *)
{

}

//wsddapi.h
SOAP_FMAC5 int SOAP_FMAC6 soap_send___wsdd__Bye(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct wsdd__ByeType *wsdd__Bye)
{

}

//wsddapi.h
SOAP_FMAC5 int SOAP_FMAC6 soap_recv___wsdd__Bye(struct soap *soap, struct __wsdd__Bye *)
{

}

//wsddapi.h
SOAP_FMAC5 int SOAP_FMAC6 soap_send___wsdd__Probe(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct wsdd__ProbeType *wsdd__Probe)
{

}

//wsddapi.h
SOAP_FMAC5 int SOAP_FMAC6 soap_recv___wsdd__Probe(struct soap *soap, struct __wsdd__Probe *)
{

}

//wsddapi.h
SOAP_FMAC5 int SOAP_FMAC6 soap_send___wsdd__ProbeMatches(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct wsdd__ProbeMatchesType *wsdd__ProbeMatches)
{

}

//wsddapi.h
SOAP_FMAC5 int SOAP_FMAC6 soap_recv___wsdd__ProbeMatches(struct soap *soap, struct __wsdd__ProbeMatches *)
{

}

//wsddapi.h
SOAP_FMAC5 int SOAP_FMAC6 soap_send___wsdd__Resolve(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct wsdd__ResolveType *wsdd__Resolve)
{

}

//wsddapi.h
SOAP_FMAC5 int SOAP_FMAC6 soap_recv___wsdd__Resolve(struct soap *soap, struct __wsdd__Resolve *)
{

}

//wsddapi.h
SOAP_FMAC5 int SOAP_FMAC6 soap_send___wsdd__ResolveMatches(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct wsdd__ResolveMatchesType *wsdd__ResolveMatches)
{

}

//wsddapi.h
SOAP_FMAC5 int SOAP_FMAC6 soap_recv___wsdd__ResolveMatches(struct soap *soap, struct __wsdd__ResolveMatches *)
{

}
