#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <xmpp_helper.h>

#include "xmpp_chat.h"
#include "xmpp_utils.h"
#include "xmpp_ibb.h"

char g_tojid[256] = "";
xmpp_ibb_session_t *g_session;
bool g_looping = true;
FILE *g_send_fs = NULL;
FILE *g_recv_fs = NULL;
int   g_send_off = 0;

static void _send(int off)
{
    fprintf(stderr, "%s(%d)\n", __FUNCTION__, off);
    if (feof(g_send_fs)) {
        xmpp_ibb_close(g_session);
        return;
    }
    char msg[xmpp_ibb_get_blocksize(g_session) * 3 / 4];
    xmppdata_t xdata;
    fseek(g_send_fs, off, SEEK_SET);
    xdata.size = fread(msg, 1, sizeof(msg), g_send_fs);
    fprintf(stderr, "xdata.size(%d) = fread(msg, sizeof(msg)(%ld), 1, g_send_fs\n", xdata.size, sizeof(msg));
    xdata.data = msg;
    xmpp_ibb_send_data(g_session, &xdata);
    g_send_off += xdata.size;
}

static void _recv(xmppdata_t *xdata)
{
    fwrite(xdata->data, 1, xdata->size, g_recv_fs);
}


static int conn_handler(xmpp_t *xmpp, xmppconn_info_t *conninfo, void *udata)
{
    if (conninfo->connevent != 0) {
        fprintf(stderr, "  status(%d) error(%d) errorType(%d) errorText '%s'\n",
                conninfo->connevent, conninfo->error, conninfo->errortype,
                conninfo->errortext);
        g_looping = false;
        return -1;
    }
    printf( "\n\n       login full JID: %s\n\n\n", xmpphelper_get_bound_jid(xmpp));
    return 0;
}

static int open_cb(xmpp_ibb_session_t *sess, char *type)
{
    printf("\n  %s() type '%s'\n", __FUNCTION__, type);
    strncpy(g_tojid, xmpp_ibb_get_remote_jid(sess), sizeof(g_tojid));
    g_session = sess;
    if (g_recv_fs == NULL) {
        char fname[64];
        xmpp_conn_t *conn = xmpp_ibb_get_conn(sess);
        const char *bjid = xmpp_conn_get_bound_jid(conn);
        char *sid = xmpp_ibb_get_sid(sess);
        snprintf(fname, sizeof(fname), "%s-%s.dat", bjid, sid);
        char *pos;
        while ((pos=strchr(fname, '/')) != NULL || (pos=strchr(fname, '@')) != NULL) {
            *pos = '-';
        }
        fprintf(stderr, "fopen(%s, 'a')\n", fname);
        g_recv_fs = fopen(fname, "a");
        if (g_recv_fs == NULL) {
            perror("fopen() recv failed.\n");
        }
    }
    if (strncmp("result", type, 6) == 0)
        printf("%s() result\n", __FUNCTION__);
    return 0;
}

static int close_cb(xmpp_ibb_session_t *sess, char *type)
{
    printf("\n  %s() type '%s'\n", __FUNCTION__, type);
//    xmpp_ibb_release(sess);
    g_session = NULL;
    g_tojid[0] = '\0';
    if (g_recv_fs != NULL) {
        fclose(g_recv_fs);
        g_recv_fs = NULL;
    }
    if (g_send_fs != NULL) {
        fclose(g_send_fs);
        g_send_fs = NULL;
    }
    if (strncmp("result", type, 6) == 0)
        printf("%s() result\n", __FUNCTION__);
    return 0;
}

static int recv_cb(xmpp_ibb_session_t *sess, xmppdata_t *xdata)
{
    printf("\n  %s()\n", __FUNCTION__);
    if (xdata != NULL) {
        printf("    data'%p' size(%d)\n", (char *) xdata->data, xdata->size);
        _recv(xdata);
    } else {
        printf("  %s() result\n", __FUNCTION__);
        _send(g_send_off);
    }
    return 0;
}

static int error_cb(xmpp_ibb_session_t *sess, xmpperror_t *xerr)
{
    printf("\n  %s() code(%d) type '%s' msg '%s'\n", __FUNCTION__, xerr->code, xerr->type, xerr->mesg);
//    xmpp_ibb_release(sess);
    g_session = NULL;
    g_tojid[0] = '\0';
    return 0;
}

void print_usage()
{
    printf("Usage: command [-s host -p port -j jid -w password]\n");
    printf("      -s host, xmpp server hostname or ip address\n");
    printf("      -p port, xmpp service port\n");
    printf("      -j jid, login bare JID\n");
    printf("      -w password, login passowrd\n");
    printf("      -f ,force TLS\n");
}

static bool doCmd(xmpp_t *xmpp, char cmd)
{
    int looping = true;
    xmpp_conn_t *conn = xmpphelper_get_conn(xmpp);
    switch (cmd)
    {
        case 'r':
//            xmpp_ibb_release(g_session);
            break;
        case 'q':
            xmpphelper_stop(xmpp);
            looping = false;
            break;
        case 'e':
            printf("input target jid to establish session: ");
            fgets(g_tojid, sizeof(g_tojid), stdin);
            fprintf(stderr, "tojid '%s' size(%ld)", g_tojid, strlen(g_tojid));
            g_tojid[strlen(g_tojid) - 1] = '\0';
            g_session = xmpp_ibb_open(conn, g_tojid, NULL);
            break;
        case 'c':
            if (g_session == NULL || strlen(g_tojid) == 0) {
                printf("session is not setup. session<%p> target'%s'.", g_session, g_tojid);
                return looping;
            }
            xmpp_ibb_close(g_session);
            if (g_send_fs != NULL) {
                fclose(g_send_fs);
                g_send_fs = NULL;
            }
            if (g_recv_fs != NULL) {
                fclose(g_recv_fs);
                g_recv_fs = NULL;
            }
            g_session = NULL;
            g_tojid[0] = '\0';
            break;
        case 's':
        {
            if (g_session == NULL || strlen(g_tojid) == 0) {
                printf("session is not setup. session<%p> target'%s'.", g_session, g_tojid);
                return looping;
            }
            printf("select file send to target jid '%s': ", g_tojid);
            char msg[1024] = "";
            fgets(msg, sizeof(msg), stdin);
            char *pos;
            if ((pos=strchr(msg, '\n')) != NULL)
                    *pos = '\0';
            g_send_fs = fopen(msg, "r");
            if (g_send_fs == NULL) {
                perror("fopen() send failed.\n");
                return -1;
            }
            _send(g_send_off);
            break;
        }
        default:
            printf("\n 'q' to quit, 'e' establish ibb session, 's' send data to '%s', 'c' close ibb session: ", g_tojid);
            break;
    }
    return looping;
}

int main(int argc, char *argv[])
{
    bool looping = true;
    int opt;
    int force_tls = 0;
    xmpp_t *xmpp;
    char msg[1024] = "";
    char *host = "localhost", *jid = "user1@localhost", *pass = "1234";
    int port = 5222;

    while ((opt = getopt(argc, argv, "s:p:w:j:t:hf")) != -1) {
        switch (opt)
        {
            case 's':
                host = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 'w':
                pass = optarg;
                break;
            case 'f':
                force_tls = 1;
                break;
            case 'j':
                jid = optarg;
                break;
            case 'h':
            default:
                print_usage();
                return -1;
        }
    }
    xmpp_log_t *log = xmpp_get_default_logger(XMPP_LEVEL_DEBUG);
    xmpp = xmpphelper_new(conn_handler, NULL, log, NULL);
    if (force_tls == 1) {
        xmpphelper_force_tls(xmpp);
    }
    xmpphelper_connect(xmpp, host, port, jid, pass);
    xmpp_ibb_reg_funcs_t regfuncs;
    regfuncs.open_cb = open_cb;
    regfuncs.close_cb = close_cb;
    regfuncs.recv_cb = recv_cb;
    regfuncs.error_cb = error_cb;
    xmpp_ibb_register(xmpphelper_get_conn(xmpp), &regfuncs);
    xmpphelper_run(xmpp);

    while (looping && g_looping) {
        looping = doCmd(xmpp, msg[0]);
        fgets(msg, sizeof(msg), stdin);
    }
    xmpphelper_join(xmpp);

    xmpp_ibb_unregister(xmpphelper_get_conn(xmpp));

    xmpphelper_release(xmpp);
    return 0;
}

