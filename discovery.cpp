#include "discovery.h"
#include "gsoap/onvif/core/onvifdiscovery/plugin/wsddapi.h"
#include "gsoap/onvif/core/onvifdiscovery/onvifdiscoverywsddService.h"
#include "gsoap/onvif/core/onvifdiscovery/wsdd.nsmap"
Discovery::Discovery()
{

}

Discovery::~Discovery()
{

}

void Discovery::Demo()
{

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
