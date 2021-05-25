#include "discovery.h"
#include "gsoap/onvif/core/onvifdiscovery/plugin/wsddapi.h"
#include "gsoap/onvif/core/onvifdiscovery/onvifdiscoverywsddService.h"
#include "gsoap/onvif/core/onvifdiscovery/wsdd.nsmap"

//组播地址
#define MULTICAST_ADDRESS "239.255.255.250"
//组播端口
#define MULTICAST_PORT 3702

//服务端口
#define SERVER_PORT_STR "10000"
#define SERVER_PORT_INT 10000

Discovery* Discovery::s_pDiscovery = nullptr;
char Discovery::s_aUUID[46];
char Discovery::s_aServiceAddress[16];
short Discovery::s_nServicePort;
char Discovery::s_aXaddrs[64];
char Discovery::s_aTypes[28];
char Discovery::s_aScopes[512];

Discovery::Discovery()
    :m_pSoap(nullptr)
    ,_t(0)
    ,m_bThreadFlag(true)
{

}

Discovery* Discovery::GetDiscoveryInstance()
{
    //多线程时需要考虑加锁，防止实例化多个Discovery对象。
    if (Discovery::s_pDiscovery == nullptr)
    {
        Discovery::s_pDiscovery = new Discovery();
    }
    return Discovery::s_pDiscovery;
}

Discovery::~Discovery()
{
    if (this->m_pSoap != nullptr)
    {
        soap_destroy(this->m_pSoap);
        soap_end(this->m_pSoap);
        soap_free(this->m_pSoap);
        delete this->m_pSoap;
        this->m_pSoap = nullptr;
    }
}

void Discovery::Init()
{
    this->m_pSoap = soap_new1(SOAP_IO_UDP);
    char* l_pUUIDTmp = (char*)soap_wsa_rand_uuid(this->m_pSoap);
    short l_nUUIDLen = strlen(l_pUUIDTmp);
    strncpy(Discovery::s_aUUID, l_pUUIDTmp, l_nUUIDLen);

    //需要动态获取Address，此处固化测试。
    char l_aServerAddressTmp[16] = "192.168.1.5";
    short l_nServerAddressLen = strlen(l_aServerAddressTmp);
    memset(Discovery::s_aServiceAddress, '\0', sizeof (Discovery::s_aServiceAddress));
    strncpy(Discovery::s_aServiceAddress, l_aServerAddressTmp, l_nServerAddressLen);

    Discovery::s_nServicePort = SERVER_PORT_INT;

    //初始化Xaddrs
    memset(Discovery::s_aXaddrs, '\0', sizeof (Discovery::s_aXaddrs));
    char l_aHead[] = "http://";
    char l_aColon[] = ":";
    char l_aEnd[] = "/onvif/device_service";
    strncpy(Discovery::s_aXaddrs, l_aHead, strlen(l_aHead));
    strncpy(Discovery::s_aXaddrs + strlen(l_aHead), Discovery::s_aServiceAddress, strlen(Discovery::s_aServiceAddress));
    strncpy(Discovery::s_aXaddrs + strlen(l_aHead) + strlen(Discovery::s_aServiceAddress), l_aColon, strlen(l_aColon));
    strncpy(Discovery::s_aXaddrs + strlen(l_aHead) + strlen(Discovery::s_aServiceAddress) + strlen(l_aColon), SERVER_PORT_STR, strlen(SERVER_PORT_STR));
    strncpy(Discovery::s_aXaddrs + strlen(l_aHead) + strlen(Discovery::s_aServiceAddress) + strlen(l_aColon) + strlen(SERVER_PORT_STR), l_aEnd, strlen(l_aEnd));

    //初始化Types
    char l_aTypes[] = "tdn:NetworkVideoTransmitter";
    memset(Discovery::s_aTypes, '\0', sizeof (Discovery::s_aTypes));
    strncpy(Discovery::s_aTypes, l_aTypes, strlen(l_aTypes));

    //初始化Scopes
    char l_aScopes[] = "onvif://www.onvif.org/Profile/Streaming onvif://www.onvif.org/hardware/NetworkVideoTransmitter onvif://www.onvif.org/name/NVT";
    memset(Discovery::s_aScopes, '\0', sizeof (Discovery::s_aScopes));
    strncpy(Discovery::s_aScopes, l_aScopes, strlen(l_aScopes));

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

    //发送Bye消息
    this->SendBye();

    //退出组播
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_ADDRESS);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(this->m_pSoap->socket, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq)))
    {
        printf("Failed to set socket options. errno = %d:%s\n", errno, strerror(errno));
    }

}

void* DiscoveryThread(void *param)
{
    pthread_detach(pthread_self());
    Discovery *discovery = (Discovery *)param;
    struct soap* soap = discovery->GetSOAP();
    if (soap == nullptr)
        goto RETURN;

    //地址复用
    soap->bind_flags = SO_REUSEADDR;

    //绑定
    if (!soap_valid_socket(soap_bind(soap, NULL, MULTICAST_PORT, 100)))
    {
        soap_print_fault(soap, stderr);
        printf("Failed to bind port.\n");
        goto RETURN;
    }

    //加入组播
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_ADDRESS);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(soap->socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)))
    {
        printf("Failed to set socket options. errno = %d:%s\n", errno, strerror(errno));
        goto RETURN;
    }

    //发送Hello消息
    discovery->SendHello();

    while(discovery->GetThreadFlag())
    {
        printf("ONVIF discovery is listening. %s:%d\n", MULTICAST_ADDRESS, MULTICAST_PORT);

        //监听
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

bool Discovery::GetThreadFlag()
{
    return this->m_bThreadFlag;
}

void Discovery::SetThreadFlag(bool flag)
{
    this->m_bThreadFlag = flag;
}

struct soap* Discovery::GetSOAP()
{
    return this->m_pSoap;
}

void Discovery::SendHello()
{
    struct soap* soap = soap_new1(SOAP_IO_UDP);
    int res = soap_wsdd_Hello(
        soap,
        SOAP_WSDD_ADHOC,
        "soap.udp://239.255.255.250:3702",
        soap_wsa_rand_uuid(soap),
        nullptr,
        Discovery::s_aUUID,
        Discovery::s_aTypes,
        Discovery::s_aScopes,
        nullptr,
        Discovery::s_aXaddrs,
        1);
    if (res != SOAP_OK)
    {
        soap_print_fault(soap, stderr);
    }

    soap_end(soap);
}

void Discovery::SendBye()
{
    struct soap* soap = soap_new1(SOAP_IO_UDP);
    int res = soap_wsdd_Bye(
        soap,
        SOAP_WSDD_ADHOC,
        "soap.udp://239.255.255.250:3702",
        soap_wsa_rand_uuid(soap),
        Discovery::s_aUUID,
        Discovery::s_aTypes,
        Discovery::s_aScopes,
        NULL,
        Discovery::s_aXaddrs,
        1);
    if (res != SOAP_OK)
    {
        soap_print_fault(soap, stderr);
    }

    soap_end(soap);
}

//onvifdiscoverywsddService.h
int wsddService::Hello(struct wsdd__HelloType *wsdd__Hello)
{
    return __wsdd__Hello(this->soap, wsdd__Hello);
}

//onvifdiscoverywsddService.h
int wsddService::Bye(struct wsdd__ByeType *wsdd__Bye)
{
    return __wsdd__Bye(this->soap, wsdd__Bye);
}

//onvifdiscoverywsddService.h
int wsddService::Probe(struct wsdd__ProbeType *wsdd__Probe)
{
    return __wsdd__Probe(this->soap, wsdd__Probe);
}

//onvifdiscoverywsddService.h
int wsddService::ProbeMatches(struct wsdd__ProbeMatchesType *wsdd__ProbeMatches)
{
    return __wsdd__ProbeMatches(this->soap, wsdd__ProbeMatches);
}

//onvifdiscoverywsddService.h
int wsddService::Resolve(struct wsdd__ResolveType *wsdd__Resolve)
{
    return __wsdd__Resolve(this->soap, wsdd__Resolve);
}

//onvifdiscoverywsddService.h
int wsddService::ResolveMatches(struct wsdd__ResolveMatchesType *wsdd__ResolveMatches)
{
    return __wsdd__ResolveMatches(this->soap, wsdd__ResolveMatches);
}

//wsddapi.h
void wsdd_event_Hello(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int MetadataVersion)
{
    //服务端不接收Hello事件
}

//wsddapi.h
void wsdd_event_Bye(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int *MetadataVersion)
{
    //服务端不接收Bye事件
}

//wsddapi.h
soap_wsdd_mode wsdd_event_Probe(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *Types, const char *Scopes, const char *MatchBy, struct wsdd__ProbeMatchesType *matches)
{
/*
    ODTT点击Discovery Devices按钮
    MessageID:uuid:b403c36b-6ddc-4058-9304-e2bb31e7a421
    ReplyTo:(null)
    Types:"http://www.onvif.org/ver10/device/wsdl":Device
    Scopes:

    MessageID:uuid:3a12a6e7-58b2-462d-b04c-a2866798f660
    ReplyTo:(null)
    Types:tdn:NetworkVideoTransmitter
    Scopes:
*/
    soap_wsdd_init_ProbeMatches(soap, matches);
    soap_wsdd_add_ProbeMatch(
        soap,                       //soap
        matches,                    //matches
        Discovery::s_aUUID,         //EndpointReference
        Discovery::s_aTypes,
        Discovery::s_aScopes,
        nullptr,                    //MatchBy
        Discovery::s_aXaddrs,       //XAddrs
        1                           //MetadataVersion
    );

    soap_wsdd_ProbeMatches(soap, nullptr, soap_wsa_rand_uuid(soap), MessageID, ReplyTo, matches);
    return SOAP_WSDD_ADHOC;
}

//wsddapi.h
void wsdd_event_ProbeMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ProbeMatchesType *matches)
{
    //服务端不接收ProbeMatches事件
}

//wsddapi.h
soap_wsdd_mode wsdd_event_Resolve(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *EndpointReference, struct wsdd__ResolveMatchType *match)
{
    soap_wsdd_ResolveMatches(
        soap,
        NULL,
        soap_wsa_rand_uuid(soap),
        MessageID,
        ReplyTo,
        EndpointReference,
        Discovery::s_aTypes,
        Discovery::s_aScopes,
        nullptr,
        Discovery::s_aXaddrs,
        1);
    return SOAP_WSDD_ADHOC;
}

//wsddapi.h
void wsdd_event_ResolveMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ResolveMatchType *match)
{
    //服务端不接收ResolveMatches事件
}

//wsddapi.h
SOAP_FMAC5 int SOAP_FMAC6 soap_send___wsdd__Hello(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct wsdd__HelloType *wsdd__Hello)
{
    struct __wsdd__Hello soap_tmp___wsdd__Hello;
    if (soap_action == NULL)
        soap_action = "http://schemas.xmlsoap.org/ws/2005/04/discovery/hello";
    soap_begin(soap);
    soap->encodingStyle = NULL;
    soap_tmp___wsdd__Hello.wsdd__Hello = wsdd__Hello;
    soap_serializeheader(soap);
    soap_serialize___wsdd__Hello(soap, &soap_tmp___wsdd__Hello);
    if (soap_begin_count(soap))
        return soap->error;
    if (soap->mode & SOAP_IO_LENGTH)
    {
        if (soap_envelope_begin_out(soap)
            || soap_putheader(soap)
            || soap_body_begin_out(soap)
            || soap_put___wsdd__Hello(soap, &soap_tmp___wsdd__Hello, "-wsdd:Hello", NULL)
            || soap_body_end_out(soap)
            || soap_envelope_end_out(soap))
            return soap->error;
    }
    if (soap_end_count(soap))
        return soap->error;
    if (soap_connect(soap, soap_extend_url(soap, soap_endpoint, NULL), soap_action)
        || soap_envelope_begin_out(soap)
        || soap_putheader(soap)
        || soap_body_begin_out(soap)
        || soap_put___wsdd__Hello(soap, &soap_tmp___wsdd__Hello, "-wsdd:Hello", NULL)
        || soap_body_end_out(soap)
        || soap_envelope_end_out(soap)
        || soap_end_send(soap))
        return soap_closesock(soap);

    return SOAP_OK;
}

//wsddapi.h
SOAP_FMAC5 int SOAP_FMAC6 soap_recv___wsdd__Hello(struct soap *soap, struct __wsdd__Hello *param)
{
    soap_default___wsdd__Hello(soap, param);
    soap_begin(soap);
    if (soap_begin_recv(soap)
        || soap_envelope_begin_in(soap)
        || soap_recv_header(soap)
        || soap_body_begin_in(soap))
        return soap_closesock(soap);
    soap_get___wsdd__Hello(soap, param, "-wsdd:Hello", NULL);
    if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
        soap->error = SOAP_OK;
    if (soap->error
        || soap_body_end_in(soap)
        || soap_envelope_end_in(soap)
        || soap_end_recv(soap))
        return soap_closesock(soap);
    return soap_closesock(soap);
}

//wsddapi.h
SOAP_FMAC5 int SOAP_FMAC6 soap_send___wsdd__Bye(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct wsdd__ByeType *wsdd__Bye)
{
    struct __wsdd__Bye soap_tmp___wsdd__Bye;
    if (soap_action == NULL)
        soap_action = "http://schemas.xmlsoap.org/ws/2005/04/discovery/Bye";
    soap_begin(soap);
    soap->encodingStyle = NULL;
    soap_tmp___wsdd__Bye.wsdd__Bye = wsdd__Bye;
    soap_serializeheader(soap);
    soap_serialize___wsdd__Bye(soap, &soap_tmp___wsdd__Bye);
    if (soap_begin_count(soap))
        return soap->error;
    if (soap->mode & SOAP_IO_LENGTH)
    {
        if (soap_envelope_begin_out(soap)
        || soap_putheader(soap)
        || soap_body_begin_out(soap)
        || soap_put___wsdd__Bye(soap, &soap_tmp___wsdd__Bye, "-wsdd:Bye", NULL)
        || soap_body_end_out(soap)
        || soap_envelope_end_out(soap))
        return soap->error;
    }
    if (soap_end_count(soap))
        return soap->error;
    if (soap_connect(soap, soap_extend_url(soap, soap_endpoint, NULL), soap_action)
        || soap_envelope_begin_out(soap)
        || soap_putheader(soap)
        || soap_body_begin_out(soap)
        || soap_put___wsdd__Bye(soap, &soap_tmp___wsdd__Bye, "-wsdd:Bye", NULL)
        || soap_body_end_out(soap)
        || soap_envelope_end_out(soap)
        || soap_end_send(soap))
        return soap_closesock(soap);
    return SOAP_OK;
}

//wsddapi.h
SOAP_FMAC5 int SOAP_FMAC6 soap_recv___wsdd__Bye(struct soap *soap, struct __wsdd__Bye *param)
{
    soap_default___wsdd__Bye(soap, param);
    soap_begin(soap);
    if (soap_begin_recv(soap)
        || soap_envelope_begin_in(soap)
        || soap_recv_header(soap)
        || soap_body_begin_in(soap))
        return soap_closesock(soap);
    soap_get___wsdd__Bye(soap, param, "-wsdd:Bye", NULL);
    if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
        soap->error = SOAP_OK;
    if (soap->error
        || soap_body_end_in(soap)
        || soap_envelope_end_in(soap)
        || soap_end_recv(soap))
        return soap_closesock(soap);
    return soap_closesock(soap);
}

//wsddapi.h
SOAP_FMAC5 int SOAP_FMAC6 soap_send___wsdd__Probe(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct wsdd__ProbeType *wsdd__Probe)
{
    struct __wsdd__Probe soap_tmp___wsdd__Probe;
    if (soap_action == NULL)
        soap_action = "http://docs.oasis-open.org/ws-dd/ns/discovery/2009/01/Probe";
    soap_begin(soap);
    soap->encodingStyle = NULL;
    soap_tmp___wsdd__Probe.wsdd__Probe = wsdd__Probe;
    soap_serializeheader(soap);
    soap_serialize___wsdd__Probe(soap, &soap_tmp___wsdd__Probe);
    if (soap_begin_count(soap))
        return soap->error;
    if (soap->mode & SOAP_IO_LENGTH)
    {
        if (soap_envelope_begin_out(soap)
            || soap_putheader(soap)
            || soap_body_begin_out(soap)
            || soap_put___wsdd__Probe(soap, &soap_tmp___wsdd__Probe, "-wsdd:Probe", NULL)
            || soap_body_end_out(soap)
            || soap_envelope_end_out(soap))
             return soap->error;
    }
    if (soap_end_count(soap))
        return soap->error;
    if (soap_connect(soap, soap_extend_url(soap, soap_endpoint, NULL), soap_action)
        || soap_envelope_begin_out(soap)
        || soap_putheader(soap)
        || soap_body_begin_out(soap)
        || soap_put___wsdd__Probe(soap, &soap_tmp___wsdd__Probe, "-wsdd:Probe", NULL)
        || soap_body_end_out(soap)
        || soap_envelope_end_out(soap)
        || soap_end_send(soap))
        return soap_closesock(soap);
    return SOAP_OK;
}

//wsddapi.h
SOAP_FMAC5 int SOAP_FMAC6 soap_recv___wsdd__Probe(struct soap *soap, struct __wsdd__Probe *param)
{
    soap_default___wsdd__Probe(soap, param);
    soap_begin(soap);
    if (soap_begin_recv(soap)
        || soap_envelope_begin_in(soap)
        || soap_recv_header(soap)
        || soap_body_begin_in(soap))
        return soap_closesock(soap);
    soap_get___wsdd__Probe(soap, param, "-wsdd:Probe", NULL);
    if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
        soap->error = SOAP_OK;
    if (soap->error
        || soap_body_end_in(soap)
        || soap_envelope_end_in(soap)
        || soap_end_recv(soap))
        return soap_closesock(soap);
    return soap_closesock(soap);
}

//wsddapi.h
SOAP_FMAC5 int SOAP_FMAC6 soap_send___wsdd__ProbeMatches(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct wsdd__ProbeMatchesType *wsdd__ProbeMatches)
{
    struct __wsdd__ProbeMatches soap_tmp___wsdd__ProbeMatches;
    if (soap_action == NULL)
        soap_action = "http://schemas.xmlsoap.org/ws/2005/04/discovery/ProbeMatches";
    soap_begin(soap);
    soap->encodingStyle = NULL;
    soap_tmp___wsdd__ProbeMatches.wsdd__ProbeMatches = wsdd__ProbeMatches;
    soap_serializeheader(soap);
    soap_serialize___wsdd__ProbeMatches(soap, &soap_tmp___wsdd__ProbeMatches);
    if (soap_begin_count(soap))
        return soap->error;
    if (soap->mode & SOAP_IO_LENGTH)
    {
        if (soap_envelope_begin_out(soap)
            || soap_putheader(soap)
            || soap_body_begin_out(soap)
            || soap_put___wsdd__ProbeMatches(soap, &soap_tmp___wsdd__ProbeMatches, "-wsdd:ProbeMatches", NULL)
            || soap_body_end_out(soap)
            || soap_envelope_end_out(soap))
            return soap->error;
    }
    if (soap_end_count(soap))
        return soap->error;
    if (soap_connect(soap, soap_extend_url(soap, soap_endpoint, NULL), soap_action)
        || soap_envelope_begin_out(soap)
        || soap_putheader(soap)
        || soap_body_begin_out(soap)
        || soap_put___wsdd__ProbeMatches(soap, &soap_tmp___wsdd__ProbeMatches, "-wsdd:ProbeMatches", NULL)
        || soap_body_end_out(soap)
        || soap_envelope_end_out(soap)
        || soap_end_send(soap))
        return soap_closesock(soap);
    return SOAP_OK;
}

//wsddapi.h
SOAP_FMAC5 int SOAP_FMAC6 soap_recv___wsdd__ProbeMatches(struct soap *soap, struct __wsdd__ProbeMatches *param)
{
    soap_default___wsdd__ProbeMatches(soap, param);
    soap_begin(soap);
    if (soap_begin_recv(soap)
        || soap_envelope_begin_in(soap)
        || soap_recv_header(soap)
        || soap_body_begin_in(soap))
        return soap_closesock(soap);
    soap_get___wsdd__ProbeMatches(soap, param, "-wsdd:ProbeMatches", NULL);
    if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
        soap->error = SOAP_OK;
    if (soap->error
        || soap_body_end_in(soap)
        || soap_envelope_end_in(soap)
        || soap_end_recv(soap))
        return soap_closesock(soap);
    return soap_closesock(soap);
}

//wsddapi.h
SOAP_FMAC5 int SOAP_FMAC6 soap_send___wsdd__Resolve(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct wsdd__ResolveType *wsdd__Resolve)
{
    struct __wsdd__Resolve soap_tmp___wsdd__Resolve;
    if (soap_action == NULL)
        soap_action = "http://schemas.xmlsoap.org/ws/2005/04/discovery/Resolve";
    soap_begin(soap);
    soap->encodingStyle = NULL;
    soap_tmp___wsdd__Resolve.wsdd__Resolve = wsdd__Resolve;
    soap_serializeheader(soap);
    soap_serialize___wsdd__Resolve(soap, &soap_tmp___wsdd__Resolve);
    if (soap_begin_count(soap))
        return soap->error;
    if (soap->mode & SOAP_IO_LENGTH)
    {
        if (soap_envelope_begin_out(soap)
        || soap_putheader(soap)
        || soap_body_begin_out(soap)
        || soap_put___wsdd__Resolve(soap, &soap_tmp___wsdd__Resolve, "-wsdd:Resolve", NULL)
        || soap_body_end_out(soap)
        || soap_envelope_end_out(soap))
         return soap->error;
    }
    if (soap_end_count(soap))
        return soap->error;
    if (soap_connect(soap, soap_extend_url(soap, soap_endpoint, NULL), soap_action)
        || soap_envelope_begin_out(soap)
        || soap_putheader(soap)
        || soap_body_begin_out(soap)
        || soap_put___wsdd__Resolve(soap, &soap_tmp___wsdd__Resolve, "-wsdd:Resolve", NULL)
        || soap_body_end_out(soap)
        || soap_envelope_end_out(soap)
        || soap_end_send(soap))
        return soap_closesock(soap);
    return SOAP_OK;
}

//wsddapi.h
SOAP_FMAC5 int SOAP_FMAC6 soap_recv___wsdd__Resolve(struct soap *soap, struct __wsdd__Resolve *param)
{
    soap_default___wsdd__Resolve(soap, param);
    soap_begin(soap);
    if (soap_begin_recv(soap)
        || soap_envelope_begin_in(soap)
        || soap_recv_header(soap)
        || soap_body_begin_in(soap))
        return soap_closesock(soap);
    soap_get___wsdd__Resolve(soap, param, "-wsdd:Resolve", NULL);
    if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
        soap->error = SOAP_OK;
    if (soap->error
        || soap_body_end_in(soap)
        || soap_envelope_end_in(soap)
        || soap_end_recv(soap))
        return soap_closesock(soap);
    return soap_closesock(soap);
}

//wsddapi.h
SOAP_FMAC5 int SOAP_FMAC6 soap_send___wsdd__ResolveMatches(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct wsdd__ResolveMatchesType *wsdd__ResolveMatches)
{
    struct __wsdd__ResolveMatches soap_tmp___wsdd__ResolveMatches;
    if (soap_action == NULL)
        soap_action = "http://schemas.xmlsoap.org/ws/2005/04/discovery/ResolveMatches";
    soap_begin(soap);
    soap->encodingStyle = NULL;
    soap_tmp___wsdd__ResolveMatches.wsdd__ResolveMatches = wsdd__ResolveMatches;
    soap_serializeheader(soap);
    soap_serialize___wsdd__ResolveMatches(soap, &soap_tmp___wsdd__ResolveMatches);
    if (soap_begin_count(soap))
        return soap->error;
    if (soap->mode & SOAP_IO_LENGTH)
    {
        if (soap_envelope_begin_out(soap)
            || soap_putheader(soap)
            || soap_body_begin_out(soap)
            || soap_put___wsdd__ResolveMatches(soap, &soap_tmp___wsdd__ResolveMatches, "-wsdd:ResolveMatches", NULL)
            || soap_body_end_out(soap)
            || soap_envelope_end_out(soap))
            return soap->error;
    }
    if (soap_end_count(soap))
        return soap->error;
    if (soap_connect(soap, soap_extend_url(soap, soap_endpoint, NULL), soap_action)
        || soap_envelope_begin_out(soap)
        || soap_putheader(soap)
        || soap_body_begin_out(soap)
        || soap_put___wsdd__ResolveMatches(soap, &soap_tmp___wsdd__ResolveMatches, "-wsdd:ResolveMatches", NULL)
        || soap_body_end_out(soap)
        || soap_envelope_end_out(soap)
        || soap_end_send(soap))
        return soap_closesock(soap);
    return SOAP_OK;
}

//wsddapi.h
SOAP_FMAC5 int SOAP_FMAC6 soap_recv___wsdd__ResolveMatches(struct soap *soap, struct __wsdd__ResolveMatches *param)
{
    soap_default___wsdd__ResolveMatches(soap, param);
    soap_begin(soap);
    if (soap_begin_recv(soap)
        || soap_envelope_begin_in(soap)
        || soap_recv_header(soap)
        || soap_body_begin_in(soap))
        return soap_closesock(soap);
    soap_get___wsdd__ResolveMatches(soap, param, "-wsdd:ResolveMatches", NULL);
    if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
        soap->error = SOAP_OK;
    if (soap->error
        || soap_body_end_in(soap)
        || soap_envelope_end_in(soap)
        || soap_end_recv(soap))
        return soap_closesock(soap);
    return soap_closesock(soap);
}
