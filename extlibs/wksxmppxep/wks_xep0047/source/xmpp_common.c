/*
 * xmpp_common.c
 *
 *  Created on: Aug 7, 2015
 *      Author: user
 */
#include <string.h>
#include <stdlib.h>

#include "xmpp_common.h"

void xmpp_presence(xmpp_conn_t *conn, char *to)
{
    xmpp_ctx_t *ctx;
    xmpp_stanza_t *szpres;

    ctx = xmpp_conn_get_context(conn);
    szpres = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(szpres, "presence");
    if (to != NULL && strlen(to) > 0) {
        xmpp_stanza_set_attribute(szpres, "to", to);
    }
    xmpp_send(conn, szpres);
    xmpp_stanza_release(szpres);
}

void xmpp_ping(xmpp_conn_t * const conn, char* const id, char * const to,
        char * const type)
{
    if (type != NULL && strcmp(type, "result") == 0) {
        xmpp_iq_ack_result(conn, id, to);
    } else {
        xmpp_stanza_t *iq = NULL, *ping = NULL;
        xmpp_ctx_t *ctx;
        const char *jid = xmpp_conn_get_bound_jid(conn);

        ctx = xmpp_conn_get_context(conn);
        iq = xmpp_stanza_new(ctx);

        xmpp_stanza_set_name(iq, "iq");
        if (type != NULL && strlen(type) > 0) {
            xmpp_stanza_set_type(iq, type);
        } else {
            xmpp_stanza_set_type(iq, "get");
        }
        if (id != NULL && strlen(id) > 0) {
            xmpp_stanza_set_id(iq, id);
        } else {
            xmpp_stanza_set_id(iq, jid);
        }
        xmpp_stanza_set_attribute(iq, "from", jid);
        xmpp_stanza_set_attribute(iq, "to", to);

        ping = xmpp_stanza_new(ctx);
        xmpp_stanza_set_name(ping, "ping");
        xmpp_stanza_set_ns(ping, XMLNS_PING);
        xmpp_stanza_add_child(iq, ping);

        xmpp_send(conn, iq);
        if (ping != NULL)
            xmpp_stanza_release(ping);
        if (iq != NULL)
            xmpp_stanza_release(iq);
    }
}

void xmpp_error_stanza(xmpp_stanza_t * const error, xmpperror_t *xerr)
{
    char *mesg = NULL;
    char *code = NULL;
    xmpp_stanza_t *stanza = NULL;

    if (error == NULL || xerr == NULL) return;

    code = xmpp_stanza_get_attribute(error, "code");
    if (code == NULL)
        xerr->code = -1;
    else
        xerr->code = atoi(code);
    strncpy(xerr->type, xmpp_stanza_get_type(error), sizeof(xerr->type));
    stanza = xmpp_stanza_get_child_by_ns(error, XMPP_NS_STANZAS);
    if (stanza != NULL) mesg = xmpp_stanza_get_name(stanza);
    if (mesg != NULL) strncpy(xerr->mesg, mesg, sizeof(xerr->mesg));
}

void xmpp_iq_ack_result(xmpp_conn_t * const conn, char * const id,
        char * const to)
{
    xmpp_stanza_t *iq = NULL;
    xmpp_ctx_t *ctx;

    ctx = xmpp_conn_get_context(conn);
    iq = xmpp_stanza_new(ctx);

    if (iq == NULL || to == NULL) {
        fprintf(stderr, "%s() failed.\n", __FUNCTION__);
        return;
    }

    if (iq != NULL) {
        xmpp_stanza_set_name(iq, "iq");
        xmpp_stanza_set_type(iq, "result");
        xmpp_stanza_set_id(iq, id);
        xmpp_stanza_set_attribute(iq, "from", xmpp_conn_get_bound_jid(conn));
        xmpp_stanza_set_attribute(iq, "to", to);
        xmpp_send(conn, iq);
        xmpp_stanza_release(iq);
    }
}

void xmpp_iq_ack_error(xmpp_conn_t * const conn, char * const id,
        char * const to, char * const etype, char * const emesg)
{
    xmpp_stanza_t *iq = NULL;
    xmpp_stanza_t *stanza = NULL;
    xmpp_stanza_t *error = NULL;
    xmpp_ctx_t *ctx;

    ctx = xmpp_conn_get_context(conn);
    iq = xmpp_stanza_new(ctx);

    if (iq == NULL || to == NULL) {
        fprintf(stderr, "%s() failed.\n", __FUNCTION__);
        return;
    }

    if (iq != NULL) {
        xmpp_stanza_set_name(iq, "iq");
        xmpp_stanza_set_type(iq, "error");
        if (id != NULL)
            xmpp_stanza_set_id(iq, id);
        xmpp_stanza_set_attribute(iq, "from", xmpp_conn_get_bound_jid(conn));
        xmpp_stanza_set_attribute(iq, "to", to);

        error = xmpp_stanza_new(ctx);
        if (error != NULL) {
            xmpp_stanza_set_name(error, "error");
            xmpp_stanza_set_type(error, etype);
            xmpp_stanza_add_child(iq, error);
        }
    }

    if (error != NULL) {
        stanza = xmpp_stanza_new(ctx);
        if (stanza != NULL) {
            xmpp_stanza_set_name(stanza, emesg);
            xmpp_stanza_set_ns(stanza, XMPP_NS_STANZAS);
            xmpp_stanza_add_child(error, stanza);
        }
    }

    if (iq != NULL) {
        xmpp_send(conn, iq);
        xmpp_stanza_release(iq);
    }

    if (error != NULL) {
        xmpp_stanza_release(error);
    }

    if (stanza != NULL) {
        xmpp_stanza_release(stanza);
    }
}
