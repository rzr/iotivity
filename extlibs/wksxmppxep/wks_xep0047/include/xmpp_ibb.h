#ifndef __XMPP_IBB_H__
#define __XMPP_IBB_H__

#include "strophe.h"
#include "xmpp_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_XMPPUN_LEN  128
#define MAX_XMPPPW_LEN  128
#define MAX_XMPPSV_LEN  128
#define MAX_ID_LEN      128
#define MAX_JID_LEN     255
#define MAX_SID_LEN     128

#define XMLNS_IBB "http://jabber.org/protocol/ibb"
typedef enum
{
    STATE_NONE,
    STATE_OPENING,
    STATE_READY,
    STATE_SENDING,
    STATE_CLOSING,
    STATE_FAILED
} xmpp_ibb_state_t;

/* xmpp_ibb_data_t and xmpp_ibb_session_t is not used in version 1.0 now.
 It is used for multi IBB session handle
 */
typedef struct _xmpp_ibb_session_t
{
    //for session
    xmpp_conn_t* conn;
    char id[MAX_ID_LEN];
    char sid[MAX_SID_LEN];
    int block_size;
    char peer[MAX_JID_LEN];
    xmpp_ibb_state_t state;

    //for data
    int send_seq;
    int recv_seq;
    int data_ack_count; //how many data ack has been received
    unsigned char *recv_data;
    void *userdata;

    struct _xmpp_ibb_session_t *next;
    struct _xmpp_ibb_session_t *internal_next;

} xmpp_ibb_session_t;

/**
 * xmpp_ibb_open_cb
 *
 * callback function, when open required or ack result of open.
 *
 * @param sess session of IBB
 * @param type type of stanza, set or result
 * @return
 */
typedef int (*xmpp_ibb_open_cb)(xmpp_ibb_session_t *sess, char *type);

/**
 * xmpp_ibb_close_cb
 *
 * callback function, when close required or ack result of close,
 *   IBB session freed after called.
 *
 * @param sess session of IBB
 * @param type type of stanza set or result
 * @return 0 is OK, others error.
 */
typedef int (*xmpp_ibb_close_cb)(xmpp_ibb_session_t *sess, char *type);

/**
 * xmpp_ibb_data_cb
 *
 * callback function, when data received or ack result of send data.
 *
 * @param sess session of IBB
 * @param xdata xdata if NULL, means stanza type is 'result',
 *          otherwise stanza type is set, and message data in xdata
 * @return 0 is OK, others error.
 */
typedef int (*xmpp_ibb_data_cb)(xmpp_ibb_session_t *sess, xmppdata_t *xdata);

/**
 * xmpp_ibb_error_cb
 *
 * callback function, when error occurred,
 *   IBB session freed after called.
 *
 * @param sess session of IBB
 * @param xerror error information
 * @return 0 is OK, others error.
 */
typedef int (*xmpp_ibb_error_cb)(xmpp_ibb_session_t *sess, xmpperror_t *xerror);

typedef struct _xmpp_ibb_reg_funcs_t {
    xmpp_ibb_open_cb open_cb;
    xmpp_ibb_close_cb close_cb;
    xmpp_ibb_data_cb recv_cb;
    xmpp_ibb_error_cb error_cb;
} xmpp_ibb_reg_funcs_t;

/**
 * xmpp_ibb_register
 *
 * register callback functions for IBB.
 *
 * @param conn conn of libstrophe.
 * @param reg_funcs register functions for IBB
 */
void xmpp_ibb_register(xmpp_conn_t * const conn, xmpp_ibb_reg_funcs_t *reg_funcs);

/**
 * xmpp_ibb_unregister
 *
 * unregister callback for IBB.
 *
 * @param conn conn of libstrophe.
 */
void xmpp_ibb_unregister(xmpp_conn_t * const conn);

/**
 * xmpp_ibb_open
 *
 * establish an IBB session.
 *
 * @param conn conn of libstrophe
 * @param jid target jid to establish
 * @param sid if set, specific session id, otherwise use random generate.
 * @return session of IBB
 */
xmpp_ibb_session_t *xmpp_ibb_open(xmpp_conn_t * const conn, char * const jid, char * const sid);

/**
 * xmpp_ibb_close
 *
 * close an IBB session.
 *
 * @param sess session of IBB
 * @return 0 is OK, others error.
 */
int xmpp_ibb_close(xmpp_ibb_session_t *sess);

/**
 * xmpp_ibb_send_data
 *
 * send data.
 *
 * @param sess session of IBB
 * @param xdata message data to send
 * @return 0 is OK, others error.
 */
int xmpp_ibb_send_data(xmpp_ibb_session_t *sess, xmppdata_t *xdata);

/**
 * xmpp_ibb_get_conn
 *
 * get conn of libstrophe.
 *
 * @param sess session of IBB
 * @return conn of libstrophe
 */
xmpp_conn_t * xmpp_ibb_get_conn(xmpp_ibb_session_t *sess);

/**
 * xmpp_ibb_get_sid
 *
 * get session id.
 *
 * @param sess session of IBB
 * @return session id
 */
char * xmpp_ibb_get_sid(xmpp_ibb_session_t *sess);

/**
 * xmpp_ibb_get_remote_jid
 *
 * get remote JID.
 *
 * @param sess session of IBB
 * @return remote jid
 */
char * xmpp_ibb_get_remote_jid(xmpp_ibb_session_t *sess);

/**
 * xmpp_ibb_get_session_by_sid
 *
 * get IBB session by session id.
 *
 * @param sid session id
 * @return session of IBB, if not found, return NULL
 */
xmpp_ibb_session_t *xmpp_ibb_get_session_by_sid(char *sid);

/**
 * xmpp_ibb_userdata_alloc
 *
 * allocate memory for user data, memory will be released in xmpp_ibb_release()
 *
 * @param sess session of IBB
 * @param udata [OUT] memory pointer for user data
 * @param size memory size required
 * @return 0 is OK, others error.
 */
int xmpp_ibb_userdata_alloc(xmpp_ibb_session_t *sess, void **udata, int size);

/**
 * get block size of IBB session
 *
 * @param sess session of IBB
 * @return block size
 */
int xmpp_ibb_get_blocksize(xmpp_ibb_session_t *sess);

/**
 * set block size for IBB session
 * @param sess session of IBB
 * @param bsize block size
 */
void xmpp_ibb_set_blocksize(xmpp_ibb_session_t *sess, int bsize);

#ifdef __cplusplus
}
#endif

#endif//__XMPP_IBB_H__
