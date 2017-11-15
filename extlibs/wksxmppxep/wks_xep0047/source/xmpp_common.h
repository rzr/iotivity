#ifndef __XMPP_COMMON__
#define __XMPP_COMMON__

#include <pthread.h>

#include "../include/xmpp_helper.h"
#include "strophe.h"

#define XMLNS_PING "urn:xmpp:ping"

#ifdef __cplusplus
extern "C"
{
#endif

struct _xmpp_t
{
    xmpp_ctx_t *ctx;
    xmpp_conn_t *conn;
    xmpp_mem_t *mem;
    xmpp_log_t *log;
    pthread_t pth;
    xmppconn_handler callback;
    int force_tls;
    void *userdata;
};

/**
 *
 * @param conn conn for libstrophe
 * @param to to target, if null or "", will not specific target.
 */
void xmpp_presence(xmpp_conn_t * const conn, char * const to);

void xmpp_ping(xmpp_conn_t * const conn, char* const id, char * const to,
        char * const type);

void xmpp_iq_ack_result(xmpp_conn_t * const conn, char * const id,
        char * const to);

void xmpp_iq_ack_error(xmpp_conn_t * const conn, char * const id,
        char * const to, char * const etype, char * const emesg);

void xmpp_error_stanza(xmpp_stanza_t * const error, xmpperror_t *xerr);

#ifdef __cplusplus
}
#endif

#endif//__XMPP_COMMON__
