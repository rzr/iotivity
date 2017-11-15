#include <stdlib.h>
#include <pthread.h>
#include <xmpp_helper.h>
#include "xmpp_common.h"

time_t glast_ping_time;

static int _ping_handler(xmpp_conn_t * const conn, xmpp_stanza_t * const stanza, void * const userdata)
{
    char *to;
    char *id;
    to = xmpp_stanza_get_attribute(stanza, "from");
    id = xmpp_stanza_get_attribute(stanza, "id");
    xmpp_ping(conn, id, to, "result");
    //xmpp_ping(conn, NULL, to, NULL);
    return 1;
}

static int _stanza_handler(xmpp_conn_t * const conn, xmpp_stanza_t * const stanza, void * const userdata)
{
    //fprintf(stderr, "    %s-%d: %s() name '%s' id '%s'\n", __FILE__, __LINE__, __FUNCTION__, xmpp_stanza_get_name(stanza), xmpp_stanza_get_id(stanza));
    return 1;
}

static void _conn_handler(xmpp_conn_t * const conn, const xmpp_conn_event_t status, const int error, xmpp_stream_error_t * const stream_error, void * const userdata)
{
    xmpp_t *xmpp;

    xmpp = (xmpp_t *) userdata;

    if (status == XMPP_CONN_CONNECT) {
        if (xmpp != NULL && xmpp->force_tls == 1 && xmpp_conn_is_secured(conn) == 0) {
            xmpphelper_stop(xmpp);
            if (xmpp->callback != NULL) {
                xmppconn_info_t conninfo;
                conninfo.connevent = (int) XMPP_CONN_DISCONNECT;
                conninfo.error = -1;
                conninfo.errortext = NULL;
                conninfo.errortype = 0;
                xmpp->callback(xmpp, &conninfo, xmpp->userdata);
            }
            return;
        } else {
            xmpp_presence(conn, "");
            xmpp_handler_add(conn, _ping_handler, XMLNS_PING, "iq", "get", xmpp);
            xmpp_handler_add(conn, _stanza_handler, NULL, NULL, NULL, xmpp);
        }
    } else if (status == XMPP_CONN_DISCONNECT) {
        xmpp_handler_delete(conn, _ping_handler);
        xmpp_handler_delete(conn, _stanza_handler);
        xmpp_stop(xmpp->ctx);
    } else {
        fprintf(stderr, "\n    unknown status(%d) \n\n", status);
    }

    if (xmpp != NULL && xmpp->callback != NULL) {
        xmppconn_info_t conninfo;
        conninfo.connevent = (int) status;
        conninfo.error = error;
        if (stream_error != NULL) {
            conninfo.errortype = (int) stream_error->type;
            conninfo.errortext = stream_error->text;
        } else {
            conninfo.errortext = NULL;
            conninfo.errortype = 0;
        }
        xmpp->callback(xmpp, &conninfo, xmpp->userdata);
    }
}

static void *pth_func(void *arg)
{
    xmpp_t *xmpp;
    xmpp = (xmpp_t *) arg;
    xmpp_run(xmpp->ctx);
    return NULL;
}

xmpp_t *xmpphelper_new(xmppconn_handler cb, xmpp_mem_t *mem, xmpp_log_t *log, void *userdata)
{
    xmpp_t *xmpp;
    xmpp = (xmpp_t *) malloc(sizeof(struct _xmpp_t));

    if (xmpp == NULL) {
        return NULL;
    }

    xmpp_initialize();
    xmpp->mem = mem;
    xmpp->log = log;
    xmpp->ctx = xmpp_ctx_new(NULL, xmpp->log);
    xmpp->conn = xmpp_conn_new(xmpp->ctx);
    xmpp->force_tls = 0;
    xmpp->callback = cb;
    xmpp->userdata = userdata;

    return xmpp;
}

int xmpphelper_connect(xmpp_t *xmpp, char *host, int port, char *jid, char *pass)
{
    if (xmpp == NULL) return -1;

    xmpp_conn_set_jid(xmpp->conn, jid);
    xmpp_conn_set_pass(xmpp->conn, pass);
    xmpp_connect_client(xmpp->conn, host, port, _conn_handler, xmpp);

    return 0;
}

int xmpphelper_run(xmpp_t *xmpp)
{
    if (xmpp == NULL) return -1;

    pthread_create(&xmpp->pth, NULL, pth_func, xmpp);

    return 0;
}

int xmpphelper_stop(xmpp_t *xmpp)
{
    if (xmpp == NULL) return -1;

    xmpp_disconnect(xmpp->conn);
    xmpp_stop(xmpp->ctx);

    return 0;
}

int xmpphelper_join(xmpp_t *xmpp)
{
    if (xmpp == NULL) return -1;

    pthread_join(xmpp->pth, NULL);

    return 0;
}

int xmpphelper_release(xmpp_t *xmpp)
{
    if (xmpp == NULL) return -1;

    xmpp_conn_release(xmpp->conn);
    xmpp->conn = NULL;
    xmpp_ctx_free(xmpp->ctx);
    xmpp->ctx = NULL;
    xmpp_shutdown();
    free(xmpp);
    return 0;
}

xmpp_conn_t *xmpphelper_get_conn(xmpp_t *xmpp)
{
    if (xmpp == NULL) return NULL;

    return xmpp->conn;
}

const char * xmpphelper_get_bound_jid(xmpp_t *xmpp)
{
    if (xmpp == NULL) return NULL;

    return xmpp_conn_get_bound_jid(xmpp->conn);
}

int xmpphelper_force_tls(xmpp_t *xmpp)
{
    if (xmpp == NULL) return -1;

    xmpp->force_tls = 1;

    return 0;
}
