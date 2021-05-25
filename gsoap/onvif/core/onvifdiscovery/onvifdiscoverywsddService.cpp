/* onvifdiscoverywsddService.cpp
   Generated by gSOAP 2.8.111 for ../../../gsoap-2.8.111/gsoap/import/wsdd10.h

gSOAP XML Web services tools
Copyright (C) 2000-2021, Robert van Engelen, Genivia Inc. All Rights Reserved.
The soapcpp2 tool and its generated software are released under the GPL.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
--------------------------------------------------------------------------------
A commercial use license is available from Genivia Inc., contact@genivia.com
--------------------------------------------------------------------------------
*/

#include "onvifdiscoverywsddService.h"

wsddService::wsddService()
{	this->soap = soap_new();
	this->soap_own = true;
	wsddService_init(SOAP_IO_DEFAULT, SOAP_IO_DEFAULT);
}

wsddService::wsddService(const wsddService& rhs)
{	this->soap = rhs.soap;
	this->soap_own = false;
}

wsddService::wsddService(struct soap *_soap)
{	this->soap = _soap;
	this->soap_own = false;
	wsddService_init(_soap->imode, _soap->omode);
}

wsddService::wsddService(soap_mode iomode)
{	this->soap = soap_new();
	this->soap_own = true;
	wsddService_init(iomode, iomode);
}

wsddService::wsddService(soap_mode imode, soap_mode omode)
{	this->soap = soap_new();
	this->soap_own = true;
	wsddService_init(imode, omode);
}

wsddService::~wsddService()
{	if (this->soap_own)
	{	this->destroy();
		soap_free(this->soap);
	}
}

void wsddService::wsddService_init(soap_mode imode, soap_mode omode)
{	soap_imode(this->soap, imode);
	soap_omode(this->soap, omode);
	static const struct Namespace namespaces[] = {
        { "SOAP-ENV", "http://www.w3.org/2003/05/soap-envelope", "http://schemas.xmlsoap.org/soap/envelope/", NULL },
        { "SOAP-ENC", "http://www.w3.org/2003/05/soap-encoding", "http://schemas.xmlsoap.org/soap/encoding/", NULL },
        { "xsi", "http://www.w3.org/2001/XMLSchema-instance", "http://www.w3.org/*/XMLSchema-instance", NULL },
        { "xsd", "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/*/XMLSchema", NULL },
        { "wsa", "http://schemas.xmlsoap.org/ws/2004/08/addressing", "http://www.w3.org/2005/08/addressing", NULL },
        { "wsdd", "http://schemas.xmlsoap.org/ws/2005/04/discovery", NULL, NULL },

        //Added by Vincent on 2021.05.24.
        { "tdn", "http://www.onvif.org/ver10/network/wsdl", NULL, NULL},

        { NULL, NULL, NULL, NULL}
    };
	soap_set_namespaces(this->soap, namespaces);
}

void wsddService::destroy()
{	soap_destroy(this->soap);
	soap_end(this->soap);
}

void wsddService::reset()
{	this->destroy();
	soap_done(this->soap);
	soap_initialize(this->soap);
	wsddService_init(SOAP_IO_DEFAULT, SOAP_IO_DEFAULT);
}

#ifndef WITH_PURE_VIRTUAL
wsddService *wsddService::copy()
{	wsddService *dup = SOAP_NEW_UNMANAGED(wsddService);
	if (dup)
	{	soap_done(dup->soap);
		soap_copy_context(dup->soap, this->soap);
	}
	return dup;
}
#endif

wsddService& wsddService::operator=(const wsddService& rhs)
{	if (this->soap != rhs.soap)
	{	if (this->soap_own)
			soap_free(this->soap);
		this->soap = rhs.soap;
		this->soap_own = false;
	}
	return *this;
}

int wsddService::soap_close_socket()
{	return soap_closesock(this->soap);
}

int wsddService::soap_force_close_socket()
{	return soap_force_closesock(this->soap);
}

int wsddService::soap_senderfault(const char *string, const char *detailXML)
{	return ::soap_sender_fault(this->soap, string, detailXML);
}

int wsddService::soap_senderfault(const char *subcodeQName, const char *string, const char *detailXML)
{	return ::soap_sender_fault_subcode(this->soap, subcodeQName, string, detailXML);
}

int wsddService::soap_receiverfault(const char *string, const char *detailXML)
{	return ::soap_receiver_fault(this->soap, string, detailXML);
}

int wsddService::soap_receiverfault(const char *subcodeQName, const char *string, const char *detailXML)
{	return ::soap_receiver_fault_subcode(this->soap, subcodeQName, string, detailXML);
}

void wsddService::soap_print_fault(FILE *fd)
{	::soap_print_fault(this->soap, fd);
}

#ifndef WITH_LEAN
#ifndef WITH_COMPAT
void wsddService::soap_stream_fault(std::ostream& os)
{	::soap_stream_fault(this->soap, os);
}
#endif

char *wsddService::soap_sprint_fault(char *buf, size_t len)
{	return ::soap_sprint_fault(this->soap, buf, len);
}
#endif

void wsddService::soap_noheader()
{	this->soap->header = NULL;
}

void wsddService::soap_header(char *wsa__MessageID, struct wsa__Relationship *wsa__RelatesTo, struct wsa__EndpointReferenceType *wsa__From, struct wsa__EndpointReferenceType *wsa__ReplyTo, struct wsa__EndpointReferenceType *wsa__FaultTo, char *wsa__To, char *wsa__Action, struct wsdd__AppSequenceType *wsdd__AppSequence)
{
	::soap_header(this->soap);
	this->soap->header->wsa__MessageID = wsa__MessageID;
	this->soap->header->wsa__RelatesTo = wsa__RelatesTo;
	this->soap->header->wsa__From = wsa__From;
	this->soap->header->wsa__ReplyTo = wsa__ReplyTo;
	this->soap->header->wsa__FaultTo = wsa__FaultTo;
	this->soap->header->wsa__To = wsa__To;
	this->soap->header->wsa__Action = wsa__Action;
	this->soap->header->wsdd__AppSequence = wsdd__AppSequence;
}

::SOAP_ENV__Header *wsddService::soap_header()
{	return this->soap->header;
}

#ifndef WITH_NOIO
int wsddService::run(int port, int backlog)
{	if (!soap_valid_socket(this->soap->master) && !soap_valid_socket(this->bind(NULL, port, backlog)))
		return this->soap->error;
	for (;;)
	{	if (!soap_valid_socket(this->accept()))
		{	if (this->soap->errnum == 0) // timeout?
				this->soap->error = SOAP_OK;
			break;
		}
		if (this->serve())
			break;
		this->destroy();
	}
	return this->soap->error;
}

#if defined(WITH_OPENSSL) || defined(WITH_GNUTLS)
int wsddService::ssl_run(int port, int backlog)
{	if (!soap_valid_socket(this->soap->master) && !soap_valid_socket(this->bind(NULL, port, backlog)))
		return this->soap->error;
	for (;;)
	{	if (!soap_valid_socket(this->accept()))
		{	if (this->soap->errnum == 0) // timeout?
				this->soap->error = SOAP_OK;
			break;
		}
		if (this->ssl_accept() || this->serve())
			break;
		this->destroy();
	}
	return this->soap->error;
}
#endif

SOAP_SOCKET wsddService::bind(const char *host, int port, int backlog)
{	return soap_bind(this->soap, host, port, backlog);
}

SOAP_SOCKET wsddService::accept()
{	return soap_accept(this->soap);
}

#if defined(WITH_OPENSSL) || defined(WITH_GNUTLS)
int wsddService::ssl_accept()
{	return soap_ssl_accept(this->soap);
}
#endif
#endif

int wsddService::serve()
{
#ifndef WITH_FASTCGI
	this->soap->keep_alive = this->soap->max_keep_alive + 1;
#endif
	do
	{
#ifndef WITH_FASTCGI
		if (this->soap->keep_alive > 0 && this->soap->max_keep_alive > 0)
			this->soap->keep_alive--;
#endif
		if (soap_begin_serve(this->soap))
		{	if (this->soap->error >= SOAP_STOP)
				continue;
			return this->soap->error;
		}
		if ((dispatch() || (this->soap->fserveloop && this->soap->fserveloop(this->soap))) && this->soap->error && this->soap->error < SOAP_STOP)
		{
#ifdef WITH_FASTCGI
			soap_send_fault(this->soap);
#else
			return soap_send_fault(this->soap);
#endif
		}
#ifdef WITH_FASTCGI
		soap_destroy(this->soap);
		soap_end(this->soap);
	} while (1);
#else
	} while (this->soap->keep_alive);
#endif
	return SOAP_OK;
}

static int serve___wsdd__Hello(struct soap*, wsddService*);
static int serve___wsdd__Bye(struct soap*, wsddService*);
static int serve___wsdd__Probe(struct soap*, wsddService*);
static int serve___wsdd__ProbeMatches(struct soap*, wsddService*);
static int serve___wsdd__Resolve(struct soap*, wsddService*);
static int serve___wsdd__ResolveMatches(struct soap*, wsddService*);

int wsddService::dispatch()
{	return dispatch(this->soap);
}

int wsddService::dispatch(struct soap* soap)
{
	wsddService_init(soap->imode, soap->omode);
	(void)soap_peek_element(soap);
	if (!soap_match_tag(soap, soap->tag, "wsdd:Hello"))
		return serve___wsdd__Hello(soap, this);
	if (!soap_match_tag(soap, soap->tag, "wsdd:Bye"))
		return serve___wsdd__Bye(soap, this);
	if (!soap_match_tag(soap, soap->tag, "wsdd:Probe"))
		return serve___wsdd__Probe(soap, this);
	if (!soap_match_tag(soap, soap->tag, "wsdd:ProbeMatches"))
		return serve___wsdd__ProbeMatches(soap, this);
	if (!soap_match_tag(soap, soap->tag, "wsdd:Resolve"))
		return serve___wsdd__Resolve(soap, this);
	if (!soap_match_tag(soap, soap->tag, "wsdd:ResolveMatches"))
		return serve___wsdd__ResolveMatches(soap, this);
	return soap->error = SOAP_NO_METHOD;
}

static int serve___wsdd__Hello(struct soap *soap, wsddService *service)
{	struct __wsdd__Hello soap_tmp___wsdd__Hello;
	soap_default___wsdd__Hello(soap, &soap_tmp___wsdd__Hello);
	if (!soap_get___wsdd__Hello(soap, &soap_tmp___wsdd__Hello, "-wsdd:Hello", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = service->Hello(soap_tmp___wsdd__Hello.wsdd__Hello);
	if (soap->error)
		return soap->error;
	return soap_closesock(soap);
}

static int serve___wsdd__Bye(struct soap *soap, wsddService *service)
{	struct __wsdd__Bye soap_tmp___wsdd__Bye;
	soap_default___wsdd__Bye(soap, &soap_tmp___wsdd__Bye);
	if (!soap_get___wsdd__Bye(soap, &soap_tmp___wsdd__Bye, "-wsdd:Bye", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = service->Bye(soap_tmp___wsdd__Bye.wsdd__Bye);
	if (soap->error)
		return soap->error;
	return soap_closesock(soap);
}

static int serve___wsdd__Probe(struct soap *soap, wsddService *service)
{	struct __wsdd__Probe soap_tmp___wsdd__Probe;
	soap_default___wsdd__Probe(soap, &soap_tmp___wsdd__Probe);
	if (!soap_get___wsdd__Probe(soap, &soap_tmp___wsdd__Probe, "-wsdd:Probe", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = service->Probe(soap_tmp___wsdd__Probe.wsdd__Probe);
	if (soap->error)
		return soap->error;
	return soap_closesock(soap);
}

static int serve___wsdd__ProbeMatches(struct soap *soap, wsddService *service)
{	struct __wsdd__ProbeMatches soap_tmp___wsdd__ProbeMatches;
	soap_default___wsdd__ProbeMatches(soap, &soap_tmp___wsdd__ProbeMatches);
	if (!soap_get___wsdd__ProbeMatches(soap, &soap_tmp___wsdd__ProbeMatches, "-wsdd:ProbeMatches", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = service->ProbeMatches(soap_tmp___wsdd__ProbeMatches.wsdd__ProbeMatches);
	if (soap->error)
		return soap->error;
	return soap_closesock(soap);
}

static int serve___wsdd__Resolve(struct soap *soap, wsddService *service)
{	struct __wsdd__Resolve soap_tmp___wsdd__Resolve;
	soap_default___wsdd__Resolve(soap, &soap_tmp___wsdd__Resolve);
	if (!soap_get___wsdd__Resolve(soap, &soap_tmp___wsdd__Resolve, "-wsdd:Resolve", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = service->Resolve(soap_tmp___wsdd__Resolve.wsdd__Resolve);
	if (soap->error)
		return soap->error;
	return soap_closesock(soap);
}

static int serve___wsdd__ResolveMatches(struct soap *soap, wsddService *service)
{	struct __wsdd__ResolveMatches soap_tmp___wsdd__ResolveMatches;
	soap_default___wsdd__ResolveMatches(soap, &soap_tmp___wsdd__ResolveMatches);
	if (!soap_get___wsdd__ResolveMatches(soap, &soap_tmp___wsdd__ResolveMatches, "-wsdd:ResolveMatches", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = service->ResolveMatches(soap_tmp___wsdd__ResolveMatches.wsdd__ResolveMatches);
	if (soap->error)
		return soap->error;
	return soap_closesock(soap);
}
/* End of server object code */
