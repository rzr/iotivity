#ifndef _STROPHE_HELPER_
#define _STROPHE_HELPER_

#ifdef __cplusplus
extern "C"
{
#endif

#include "strophe.h"
#include "xmpp_types.h"

typedef int (*xmppconn_handler)(xmpp_t *xmpp, xmppconn_info_t *conninfo, void *userdata);

xmpp_t *xmpphelper_new(xmppconn_handler callback, xmpp_mem_t *mem, xmpp_log_t *log, void *userdata);

int xmpphelper_connect(xmpp_t *xmpp, char *host, int port, char *jid, char *pass);

int xmpphelper_run(xmpp_t *xmpp);

int xmpphelper_stop(xmpp_t *xmpp);

int xmpphelper_join(xmpp_t *xmpp);

int xmpphelper_release(xmpp_t *xmpp);

xmpp_conn_t *xmpphelper_get_conn(xmpp_t *xmpp);

const char * xmpphelper_get_bound_jid(xmpp_t *xmpp);

int xmpphelper_force_tls(xmpp_t *xmpp);

#ifdef __cplusplus
}
#endif

#endif//_STROPHE_HELPER_
