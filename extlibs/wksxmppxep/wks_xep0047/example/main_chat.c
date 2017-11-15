#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <xmpp_helper.h>

#include "xmpp_chat.h"
#include "xmpp_utils.h"

char g_rejid[256];

static int chat_recv_handler(xmpp_conn_t *xmpp, xmppdata_t *xdata, void *udata)
{
    char *decdata;
    size_t decsize;
    fprintf(stderr, "\n  chat_recv_handler(conn<%p>, from'%s', msg'%s'\n\n", xmpp, xdata->from, (char *) xdata->data);
    xmpp_b64decode((char *) xdata->data, &decdata, &decsize);
    fprintf(stderr, "\n    try decode(decdata'%s', decsize(%ld))\n", decdata, decsize);
    strcpy(g_rejid, xdata->from);
    return 0;
}

static int conn_handler(xmpp_t *xmpp, xmppconn_info_t *conninfo, void *udata)
{
    if (conninfo->connevent != 0) {
        fprintf(stderr, "  status(%d) error(%d) errorType(%d) errorText '%s'\n",
                conninfo->connevent, conninfo->error, conninfo->errortype,
                conninfo->errortext);
        return -1;
    }
    printf( "\n\n       login full JID: %s\n\n\n", xmpphelper_get_bound_jid(xmpp));
    return 0;
}

void print_usage()
{
    printf("Usage: command [-s host -p port -j jid -w password -t tojid]\n");
}

int main(int argc, char *argv[])
{
    bool looping = true;
    int c, opt;
    xmpp_t *xmpp;
    xmppdata_t xdata;

    char *host = "localhost", *jid = "user1@localhost/res1", *pass = "1234", *tojid = "user1@localhost/res1";
    int port = 5222;

    while ((opt = getopt(argc, argv, "s:p:w:j:t:h")) != -1) {
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
            case 'j':
                jid = optarg;
                break;
            case 't':
                tojid = optarg;
                break;
            case 'h':
            default:
                print_usage();
                return -1;
        }
    }

    xmpp_log_t *log = xmpp_get_default_logger(XMPP_LEVEL_DEBUG);
    xmpp = xmpphelper_new(conn_handler, NULL, log, NULL);
    xmpphelper_connect(xmpp, host, port, jid, pass);
    xmppchat_handler_add(xmpphelper_get_conn(xmpp), chat_recv_handler, xmpp);
    xmpphelper_run(xmpp);

    while (looping) {
        c = getchar();
        switch (c)
        {
            case 'q':
                xmpphelper_stop(xmpp);
                looping = false;
                break;
            case 's':
                xdata.data = "hello world";
                xdata.tojid = tojid;
                xmppchat_send_message(xmpphelper_get_conn(xmpp), &xdata);
                break;
            case 'e':
            {
                char *data = "hello world base64!!";
                char *encdata;
                xmpp_b64encode(data, strlen(data), &encdata);
                xdata.data = encdata;
                xdata.tojid = tojid;
                xmppchat_send_message(xmpphelper_get_conn(xmpp), &xdata);
                xmpp_b64free(encdata);
                break;
            }
            case 'r':
                xdata.data = "reply message ";
                xdata.tojid = g_rejid;
                xmppchat_send_message(xmpphelper_get_conn(xmpp), &xdata);
                break;
            default:
                break;
        }
    }
    xmpphelper_join(xmpp);
    xmppchat_handler_del(xmpphelper_get_conn(xmpp), chat_recv_handler);

    xmpphelper_release(xmpp);
    return 0;
}
