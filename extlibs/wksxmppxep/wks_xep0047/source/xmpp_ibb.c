#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "strophe.h"
#include "xmpp_ibb.h"
#include "xmpp_utils.h"
#include "xmpp_common.h"

#define IBB_DEFAULT_BLOCK_SIZE 4096

extern time_t glast_ping_time;
xmpp_ibb_session_t *gXMPP_IBB_handle_head = NULL, *gXMPP_IBB_handle_tail = NULL;
ilist_t *g_list = NULL;

typedef struct _xmpp_ibb_userdata_t
{
    xmpp_ibb_open_cb open_cb;
    xmpp_ibb_close_cb close_cb;
    xmpp_ibb_data_cb recv_cb;
    xmpp_ibb_error_cb error_cb;
    ilist_t *ilist;
} xmpp_ibb_userdata_t;

static bool _find_id(void *item, void *key)
{
    xmpp_ibb_session_t *sess;
    sess = (xmpp_ibb_session_t *) item;
    if (strncmp(sess->id, (char *) key, strlen(key)) == 0)
        return true;
    return false;
}

static bool _find_sid(void *item, void *key)
{
    xmpp_ibb_session_t *sess;
    sess = (xmpp_ibb_session_t *) item;
    if (strncmp(sess->sid, (char *) key, strlen(key)) == 0)
        return true;
    return false;
}

static bool _find_peer(void *item, void *key)
{
    xmpp_ibb_session_t *sess;
    sess = (xmpp_ibb_session_t *) item;
    if (strncmp(sess->peer, (char *) key, strlen(key)) == 0)
        return true;
    return false;
}

static void _ibb_session_release(xmpp_ibb_session_t *sess)
{
    if (sess == NULL) { return; }

    ilist_remove(g_list, sess);
    if (sess->userdata != NULL) {
        free(sess->userdata);
    }
    free(sess);
    //printf("%s(): list size %d.", __FUNCTION__, ilist_size(g_list));
}

static xmpp_ibb_session_t * _ibb_session_init(xmpp_conn_t * const conn, char * const peer, char * const sid)
{
    xmpp_ibb_session_t *sess = NULL;
    if (conn == NULL || peer == NULL) {
        return NULL;
    }
    sess = malloc(sizeof(struct _xmpp_ibb_session_t));
    if (sess == NULL) {
        return NULL;
    }
    sess->conn = conn;
    nmtoken_generate(sess->id, 8);
    if (sid != NULL && strlen(sid) > 0) {
        strncpy(sess->sid, sid, sizeof(sess->sid));
    } else {
        nmtoken_generate(sess->sid, 8);
    }
    sess->block_size = IBB_DEFAULT_BLOCK_SIZE;
    sess->conn = conn;
    strncpy(sess->peer, peer, sizeof(sess->peer));
    sess->state = STATE_OPENING;
    sess->send_seq = -1;
    sess->recv_seq = -1;
    sess->data_ack_count = -1;
    sess->recv_data = NULL;
    sess->next = NULL;
    sess->internal_next = NULL;
    sess->userdata = NULL;
    return sess;
}

static int _ibb_set_handler(xmpp_conn_t * const conn, xmpp_stanza_t * const stanza, void * const userdata)
{
    xmpp_stanza_t *child;
    xmpp_ibb_userdata_t * udata = (xmpp_ibb_userdata_t *) userdata;
    xmpp_ibb_session_t * sess;
    char *from, *id, *type;

    id = xmpp_stanza_get_id(stanza);
    from = xmpp_stanza_get_attribute(stanza, "from");
    type = xmpp_stanza_get_type(stanza);
    if ((child = xmpp_stanza_get_child_by_name(stanza, "open")) != NULL) {
        char *sid = xmpp_stanza_get_attribute(child, "sid");
        char *bsize = xmpp_stanza_get_attribute(child, "block-size");
        if (sid == NULL || bsize == NULL) {
            xmpp_iq_ack_error(conn, id, from, "cancel", "not-acceptable");
            return 1;
        } else {
            xmpp_iq_ack_result(conn, id, from);
        }
        sess = _ibb_session_init(conn, from, sid);
        strncpy(sess->id, id, sizeof(sess->id));
        strncpy(sess->peer, from, sizeof(sess->peer));
        sess->state = STATE_READY;
        sess->block_size = atoi(bsize);
        if (udata != NULL && udata->open_cb != NULL)
            udata->open_cb(sess, type);
        ilist_add(g_list, sess);
    } else if ((child = xmpp_stanza_get_child_by_name(stanza, "data")) != NULL) {
        char *sid = xmpp_stanza_get_attribute(child, "sid");
        sess = ilist_finditem_func(g_list, _find_sid, sid);
        if (sess != NULL) {
            xmppdata_t xdata;
            int seq = 0;
            char *intext = xmpp_stanza_get_text(child);
            xmpp_iq_ack_result(conn, id, from);
            xdata.from = from;
            strncpy(sess->id, id, sizeof(sess->id));
            seq = atoi(xmpp_stanza_get_attribute(child, "seq"));
            if (seq != (sess->recv_seq + 1)) {
                //printf("sequence number is not continue. new seq %d last seq %d\n", seq, sess->recv_seq);
            }
            sess->recv_seq = seq;
            xmpp_b64decode(intext, (char **) &xdata.data, (size_t *) &xdata.size);
            if (udata != NULL && udata->recv_cb != NULL)
                udata->recv_cb(sess, &xdata);
            xmpp_b64free(sess->recv_data);
            sess->recv_data = NULL;
        } else {
            //printf("unknown session is not in handle.\n");
            xmpp_iq_ack_error(conn, id, from, "cancel", "item-not-found");
        }
    } else if ((child = xmpp_stanza_get_child_by_name(stanza, "close")) != NULL) {
        char *sid = xmpp_stanza_get_attribute(child, "sid");
        sess = ilist_finditem_func(g_list, _find_sid, sid);
        if (sess != NULL) {
            xmpp_iq_ack_result(conn, id, from);
            strncpy(sess->id, id, sizeof(sess->id));
            sess->state = STATE_NONE;
            if (udata != NULL && udata->close_cb != NULL)
                udata->close_cb(sess, type);
            _ibb_session_release(sess);
        }
    }

    time(&glast_ping_time);

    return 1;
}

static int _ibb_result_handler(xmpp_conn_t * const conn, xmpp_stanza_t * const stanza, void * const userdata)
{
    xmpp_ibb_session_t * sess;
    xmpp_ibb_userdata_t * udata = (xmpp_ibb_userdata_t *) userdata;
    char *id, *type;

    id = xmpp_stanza_get_id(stanza);
    type = xmpp_stanza_get_type(stanza);
    sess = ilist_finditem_func(g_list, _find_id, id);
    if (sess != NULL) {
        if (sess->state == STATE_OPENING) {
            //session created ack
            sess->state = STATE_READY;
            if (udata != NULL && udata->open_cb != NULL)
                udata->open_cb(sess, type);
        } else if (sess->state == STATE_SENDING) {
            sess->state = STATE_READY;
            if (udata != NULL && udata->recv_cb != NULL)
                udata->recv_cb(sess, NULL);
            //data sent ack
        } else if (sess->state == STATE_CLOSING) {
            sess->state = STATE_NONE;
            if (udata != NULL && udata->close_cb != NULL)
                udata->close_cb(sess, type);
            _ibb_session_release(sess);
        }
    }

    time(&glast_ping_time);
    return 1;
}

static int _ibb_pres_handler(xmpp_conn_t * const conn, xmpp_stanza_t * const stanza, void * const userdata)
{
    char *type;

    type = xmpp_stanza_get_type(stanza);
    if (strncmp(type, "unavailable", 11) == 0) {
        char *from;
        xmpp_ibb_session_t * sess;
        xmpp_ibb_userdata_t * udata = (xmpp_ibb_userdata_t *) userdata;
        from = xmpp_stanza_get_attribute(stanza, "from");
        sess = ilist_finditem_func(g_list, _find_peer, from);
        if (sess != NULL) {
            //printf("target '%s' unavailable\n", from);
            if (udata != NULL && udata->close_cb != NULL)
                udata->close_cb(sess, "result");
            _ibb_session_release(sess);
        }
    }
    time(&glast_ping_time);
    return 1;
}

static int _ibb_error_handler(xmpp_conn_t * const conn, xmpp_stanza_t * const stanza, void * const userdata)
{
    xmpp_ibb_session_t * sess;
    char *id;

    id = xmpp_stanza_get_id(stanza);
    sess = ilist_finditem_func(g_list, _find_id, id);
    if (sess != NULL) {
        xmpp_stanza_t *error;
        xmpperror_t xerr;

        error = xmpp_stanza_get_child_by_name(stanza, "error");
        xmpp_error_stanza(error, &xerr);
        xmpp_ibb_userdata_t * udata = (xmpp_ibb_userdata_t *) userdata;
        sess->state = STATE_FAILED;
        if (udata != NULL && udata->error_cb != NULL)
            udata->error_cb(sess, &xerr);
        _ibb_session_release(sess);
    }

    time(&glast_ping_time);
    return 1;
}

void xmpp_ibb_register(xmpp_conn_t * const conn, xmpp_ibb_reg_funcs_t *reg)
{
    static xmpp_ibb_userdata_t s_ibb_udata;

    if (conn == NULL || reg == NULL) {
        return;
    }

    srand(time(NULL)); //for generate random string
    s_ibb_udata.open_cb = reg->open_cb;
    s_ibb_udata.close_cb = reg->close_cb;
    s_ibb_udata.recv_cb = reg->recv_cb;
    s_ibb_udata.error_cb = reg->error_cb;
    if (g_list != NULL) {
        ilist_destroy(g_list);
    }
    g_list = ilist_new();
    xmpp_handler_add(conn, _ibb_set_handler, XMLNS_IBB, "iq", "set", &s_ibb_udata);
    xmpp_handler_add(conn, _ibb_result_handler, NULL, "iq", "result", &s_ibb_udata);
    xmpp_handler_add(conn, _ibb_error_handler, NULL, "iq", "error", &s_ibb_udata);
    xmpp_handler_add(conn, _ibb_pres_handler, NULL, "presence", "unavailable", &s_ibb_udata);
}

void xmpp_ibb_unregister(xmpp_conn_t * const conn)
{
    xmpp_handler_delete(conn, _ibb_set_handler);
    xmpp_handler_delete(conn, _ibb_result_handler);
    xmpp_handler_delete(conn, _ibb_error_handler);
    xmpp_handler_delete(conn, _ibb_pres_handler);
    ilist_destroy(g_list);
}

int xmpp_ibb_send_data(xmpp_ibb_session_t *sess, xmppdata_t *xdata)
{
    int i;
    xmpp_stanza_t *iq, *data, *text;
    char *encode, seqchar[16] = "";
    xmpp_ctx_t *ctx;
    if (sess == NULL || xdata == NULL) {
        return -1;
    }
    if (xdata->size > (sess->block_size / 4 * 3)) {
        return -1;
    }
    const char *jid = xmpp_conn_get_bound_jid(sess->conn);

    for (i = 0; i < 50; i++) {
        if (sess->state == STATE_READY) {
            break ;
        } else if (sess->state == STATE_SENDING) {
            fprintf(stderr, "skip invalid state(%d).\n", sess->state);
            break ;
        } else if (sess->state == STATE_OPENING) {
            usleep(100000);
        } else {
            fprintf(stderr, "invalid state(%d).\n", sess->state);
            break ;
        }
    }

    if (sess->state != STATE_SENDING && sess->state != STATE_READY) {
        fprintf(stderr, "xmpp_ibb_send_data() failed. state(%d) not ready.\n", sess->state);
        return -1;
    }

    if (!ilist_foundinlist(g_list, sess)) {
        fprintf(stderr, "session is not in handle, may be closed.\n");
        return -1;
    }

    ctx = xmpp_conn_get_context(sess->conn);

    iq = xmpp_stanza_new(ctx);
    data = xmpp_stanza_new(ctx);
    text = xmpp_stanza_new(ctx);
    nmtoken_generate(sess->id, 8);

    xmpp_stanza_set_name(iq, "iq");
    xmpp_stanza_set_type(iq, "set");
    xmpp_stanza_set_id(iq, sess->id);
    xmpp_stanza_set_attribute(iq, "to", sess->peer);
    xmpp_stanza_set_attribute(iq, "from", jid);

    xmpp_stanza_set_name(data, "data");
    xmpp_stanza_set_ns(data, XMLNS_IBB);
    xmpp_stanza_set_attribute(data, "sid", sess->sid);
    snprintf(seqchar, sizeof(seqchar), "%d", ++(sess->send_seq));
    xmpp_stanza_set_attribute(data, "seq", seqchar);

    xmpp_b64encode(xdata->data, xdata->size, &encode);
    if (encode == NULL) {
        xmpp_stanza_set_text_with_size(text, "", 0);
    } else {
        xmpp_stanza_set_text_with_size(text, encode, strlen(encode));
    }

    xmpp_stanza_add_child(data, text);
    xmpp_stanza_add_child(iq, data);
    sess->state = STATE_SENDING;
    xmpp_send(sess->conn, iq);

    xmpp_stanza_release(text);
    xmpp_stanza_release(data);
    xmpp_stanza_release(iq);
    xmpp_b64free(encode);

    return sess->send_seq;
}

xmpp_conn_t * xmpp_ibb_get_conn(xmpp_ibb_session_t *sess)
{
    if (sess == NULL)
        return NULL;

    return sess->conn;
}

char * xmpp_ibb_get_sid(xmpp_ibb_session_t *sess)
{
    if (sess == NULL)
        return NULL;

    return sess->sid;
}

char * xmpp_ibb_get_remote_jid(xmpp_ibb_session_t *sess)
{
    if (sess == NULL)
        return NULL;

    return sess->peer;
}

xmpp_ibb_session_t *xmpp_ibb_open(xmpp_conn_t * const conn, char * const peer, char * const sid)
{
    xmpp_ibb_session_t *sess;
    xmpp_stanza_t *iq, *open;
    xmpp_ctx_t *ctx;
    const char *jid = xmpp_conn_get_bound_jid(conn);
    char sizetemp[6] = "";
    int size;

    if (peer == NULL || strlen(peer) == 0) {
        return NULL;
    }
    sess = _ibb_session_init(conn, peer, sid);
    if (sess == NULL) {
        return NULL;
    }
    size = snprintf(sizetemp, sizeof(sizetemp), "%d", sess->block_size);
    if (size < sizeof(sizetemp)) {
        sizetemp[size] = '\0';
    }
    nmtoken_generate(sess->id, 8);

    ctx = xmpp_conn_get_context(conn);
    iq = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(iq, "iq");
    xmpp_stanza_set_type(iq, "set");
    xmpp_stanza_set_id(iq, sess->id);
    xmpp_stanza_set_attribute(iq, "from", jid);
    xmpp_stanza_set_attribute(iq, "to", sess->peer);

    open = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(open, "open");
    xmpp_stanza_set_ns(open, XMLNS_IBB);
    xmpp_stanza_set_attribute(open, "block-size", sizetemp);
    xmpp_stanza_set_attribute(open, "sid", sess->sid);
    xmpp_stanza_set_attribute(open, "stanza", "iq");

    xmpp_stanza_add_child(iq, open);
    xmpp_send(conn, iq);
    xmpp_stanza_release(open);
    xmpp_stanza_release(iq);

    ilist_add(g_list, sess);

    return sess;
}

int xmpp_ibb_close(xmpp_ibb_session_t *sess)
{
    xmpp_stanza_t *iq, *close;
    xmpp_ctx_t *ctx;
    const char *jid;

    if (sess == NULL) { return -1; }

    if (!ilist_foundinlist(g_list, sess)) {
        fprintf(stderr, "session is not in handle, may be closed.\n");
        return -1;
    }

    jid = xmpp_conn_get_bound_jid(sess->conn);
    ctx = xmpp_conn_get_context(sess->conn);

    iq = xmpp_stanza_new(ctx);
    close = xmpp_stanza_new(ctx);
    nmtoken_generate(sess->id, 8);

    xmpp_stanza_set_name(iq, "iq");
    xmpp_stanza_set_type(iq, "set");
    xmpp_stanza_set_id(iq, sess->id);
    xmpp_stanza_set_attribute(iq, "to", sess->peer);
    xmpp_stanza_set_attribute(iq, "from", jid);

    xmpp_stanza_set_name(close, "close");
    xmpp_stanza_set_ns(close, XMLNS_IBB);
    xmpp_stanza_set_attribute(close, "sid", sess->sid);

    xmpp_stanza_add_child(iq, close);
    xmpp_send(sess->conn, iq);
    xmpp_stanza_release(close);
    xmpp_stanza_release(iq);

    sess->state = STATE_CLOSING;

    return 0;
}

xmpp_ibb_session_t *xmpp_ibb_get_session_by_sid(char *sid)
{
    return ilist_finditem_func(g_list, _find_sid, sid);
}

int xmpp_ibb_userdata_alloc(xmpp_ibb_session_t *sess, void **udata, int size)
{
    if (udata == NULL) {
        return -1;
    }
    *udata = NULL;
    if (sess == NULL || size <= 0) {
        return -1;
    }
    if (!ilist_foundinlist(g_list, sess)) {
        fprintf(stderr, "session is not in handle, may be closed.\n");
        return -1;
    }

    if (sess->userdata != NULL) {
        //printf("%s() called again, free userdata.\n", __FUNCTION__);
        free(sess->userdata);
    }

    sess->userdata = malloc(size);
    if (sess->userdata == NULL) {
        fprintf(stderr, "can not allocate memory.\n");
        return -1;
    }
    *udata = sess->userdata;

    return 0;
}

int xmpp_ibb_get_blocksize(xmpp_ibb_session_t *sess)
{
    if (sess == NULL) {
        return 0;
    }
    return sess->block_size;
}

void xmpp_ibb_set_blocksize(xmpp_ibb_session_t *sess, int bsize)
{
    if (sess == NULL) {
        return;
    }
    sess->block_size = bsize;
}
