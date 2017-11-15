#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>
#include <signal.h>
#include <unistd.h>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include <strophe.h>

#include "xmppclient.h"
#include "xmpp_ibb.h"

#define CTX_VAL 0x99
char xmpp_un[MAX_XMPPUN_LEN] = "";
char xmpp_pw[MAX_XMPPPW_LEN] = "";
char xmpp_server[MAX_XMPPSV_LEN] = "cloud01.workssys.com";
int bQuit = 1;

extern time_t glast_ping_time;
int gQuitFlag = 0;

typedef enum
{
    CMD_ESTABLISH,
    CMD_SEND,
    CMD_CLOSE
} CMD_TYPE;

typedef struct _itf_cmd
{
    CMD_TYPE type;
    char peer[MAX_XMPPUN_LEN];
    char message[256];
    xmpp_ibb_session_t *handle;
    struct _itf_cmd *next;
} itf_cmd;

itf_cmd *cmd_head = NULL, *cmd_tail = NULL;
xmpp_ibb_session_t *handle_head = NULL, *handle_tail = NULL;

void add_handle(xmpp_ibb_session_t *handle)
{
    xmpp_ibb_session_t *handle_temp = handle_head;
    while (handle_temp != NULL) {
        if (handle_temp == handle)
            return;
        else
            handle_temp = handle_temp->next;
    }
    if (handle_head == NULL) {
        handle_head = handle;
    } else {
        handle_tail->next = handle;
    }
    handle_tail = handle;
}
void delete_handle(xmpp_ibb_session_t *handle)
{
    xmpp_ibb_session_t *handle_temp = handle_head, *handle_last = handle_temp;
    while (handle_temp != NULL) {
        if (handle_temp == handle) {
            if (handle_temp == handle_head) {
                handle_head = handle_head->next;
            } else if (handle_temp == handle_tail) {
                handle_tail = handle_last;
                handle_last->next = NULL;
            } else {
                handle_last->next = handle_temp->next;
            }
            free(handle);
            return;
        } else {
            handle_last = handle_temp;
            handle_temp = handle_temp->next;
        }
    }
}
void add_cmd(itf_cmd *cmd)
{
    if (cmd_head == NULL) {
        cmd_head = cmd;
    } else {
        cmd_tail->next = cmd;
    }
    cmd_tail = cmd;
}

void itf()
{
    char cmd[4] = "";
    while (printf("input e for establish, s for send, c for close and q for quit: ") && fgets(cmd, sizeof(cmd), stdin)) {
        itf_cmd *cmd_temp = NULL;
        xmpp_ibb_session_t *handle_temp;
        int count = 0;
        char countc[8] = "";

        switch (cmd[0])
        {
            case 'e':
                cmd_temp = (itf_cmd*) malloc(sizeof(itf_cmd));
                cmd_temp->type = CMD_ESTABLISH;
                printf("input receiver's xmpp account: ");
                fgets(cmd_temp->peer, sizeof(cmd_temp->peer), stdin);
                cmd_temp->peer[strlen(cmd_temp->peer) - 1] = '\0';
                //scanf( "%s", cmd_temp->peer );
                cmd_temp->next = NULL;
                add_cmd(cmd_temp);
                break;
            case 's':
                cmd_temp = (itf_cmd*) malloc(sizeof(itf_cmd));
                cmd_temp->type = CMD_SEND;
                printf("handle:\n");
                handle_temp = handle_head;
                while (handle_temp != NULL) {
                    printf("%d, account: %s, id, %s, sid: %s\n", count, handle_temp->peer, handle_temp->id, handle_temp->sid);
                    handle_temp = handle_temp->next;
                    count++;
                }
                printf("input which session you're sending to: ");
                //scanf("%d", &count);
                fgets(countc, sizeof(countc), stdin);
                count = atoi(countc);
                handle_temp = handle_head;
                while (count > 0) {
                    handle_temp = handle_temp->next;
                    count--;
                    if (handle_temp == NULL)
                        break;
                }
                cmd_temp->handle = handle_temp;
                if (cmd_temp->handle == NULL) {
                    free(cmd_temp);
                    break;
                }
                printf("input the message: ");
                fgets(cmd_temp->message, sizeof(cmd_temp->message), stdin);
                cmd_temp->message[strlen(cmd_temp->message) - 1] = '\0';
                //scanf( cmd_temp->message );
                cmd_temp->next = NULL;
                add_cmd(cmd_temp);
                break;
            case 'c':
                cmd_temp = (itf_cmd*) malloc(sizeof(itf_cmd));
                cmd_temp->type = CMD_CLOSE;
                printf("handle:\n");
                handle_temp = handle_head;
                while (handle_temp != NULL) {
                    printf("%d, account: %s, id: %s, sid: %s\n", count, handle_temp->peer, handle_temp->id, handle_temp->sid);
                    handle_temp = handle_temp->next;
                    count++;
                }
                printf("input which session you're closing: ");
                //scanf("%d", &count);
                fgets(countc, sizeof(countc), stdin);
                count = atoi(countc);
                handle_temp = handle_head;
                while (count > 0) {
                    handle_temp = handle_temp->next;
                    count--;
                    if (handle_temp == NULL)
                        break;
                }
                cmd_temp->handle = handle_temp;
                if (cmd_temp->handle == NULL) {
                    free(cmd_temp);
                    break;
                }
                cmd_temp->next = NULL;
                add_cmd(cmd_temp);
                break;
            case 'h':
                printf("handle:\n");
                handle_temp = handle_head;
                while (handle_temp != NULL) {
                    printf("%d, account: %s, id: %s, sid: %s\n", count, handle_temp->peer, handle_temp->id, handle_temp->sid);
                    handle_temp = handle_temp->next;
                    count++;
                }
                break;
            case 'q':
                break;
            default:
                printf("unrecognized command\n");
                break;
        }
    }
}

/* SIGINT handler: set gQuitFlag to 1 for graceful termination */
void handleSigInt(int signum)
{

    if (signum == SIGINT) {
        gQuitFlag = 1;
    }
}

static int _ibb_open_cb(xmpp_ibb_session_t *session_handle)
{
    printf("====Open CB====\n");
    printf("id: %s\n", session_handle->id);
    printf("sid: %s\n", session_handle->sid);
    printf("peer: %s\n", session_handle->peer);
    printf("state: %d\n", session_handle->state);
    printf("===============\n");

    if (session_handle->state == STATE_READY)
        add_handle(session_handle);
    return 0;
}

static void _ibb_send_cb(xmpp_ibb_session_t *session_handle)
{
    printf("====Send CB====\n");
    printf("id: %s\n", session_handle->id);
    printf("sid: %s\n", session_handle->sid);
    printf("peer: %s\n", session_handle->peer);
    printf("recv_seq: %d\n", session_handle->recv_seq);
    printf("send_seq: %d\n", session_handle->send_seq);
    printf("data_ack_count: %d\n", session_handle->data_ack_count);
    printf("state: %d\n", session_handle->state);
    printf("===============\n");
}

static void _ibb_recv_cb(xmpp_ibb_session_t *session_handle)
{
    printf("====Recv CB====\n");
    printf("id: %s\n", session_handle->id);
    printf("sid: %s\n", session_handle->sid);
    printf("peer: %s\n", session_handle->peer);
    printf("state: %d\n", session_handle->state);
    printf("seq: %d\n", session_handle->recv_seq);
    printf("msg: %s\n", session_handle->recv_data);
    printf("===============\n");
}

static void _ibb_close_cb(xmpp_ibb_session_t *session_handle)
{
    printf("====Close CB====\n");
    printf("id: %s\n", session_handle->id);
    printf("sid: %s\n", session_handle->sid);
    printf("peer: %s\n", session_handle->peer);
    printf("state: %d\n", session_handle->state);
    printf("===============\n");

    delete_handle(session_handle);
}

void init_argument(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    xmpp_ctx_t *ctx = NULL;
    xmpp_conn_t* conn = NULL;
    XMPP_IBB_Ops_t xmpp_ibb_ops;

    pthread_t pid;

    init_argument(argc, argv);

    if (strlen(xmpp_un) < 1 || strlen(xmpp_pw) < 1) {
        printf("input xmpp acoount: ");
        scanf("%s", xmpp_un);
        printf("input xmpp password: ");
        scanf("%s", xmpp_pw);
    }

    conn = XMPP_Init(xmpp_un, xmpp_pw, xmpp_server, &ctx);
    XMPP_IBB_Init(conn, &xmpp_ibb_ops);
    //Hash_Init( ctx, HASH_TABLE_SIZE, xmpp_free );
    time(&glast_ping_time);

    xmpp_ibb_ops.ibb_open_fp = _ibb_open_cb;
    xmpp_ibb_ops.ibb_recv_fp = _ibb_recv_cb;
    xmpp_ibb_ops.ibb_send_fp = _ibb_send_cb;
    xmpp_ibb_ops.ibb_close_fp = _ibb_close_cb;

    pthread_create(&pid, NULL, (void*) itf, NULL);

    while ((!gQuitFlag) || (bQuit == 0)) {

        if (cmd_head != NULL) {
            xmpp_ibb_session_t *handle;
            switch (cmd_head->type)
            {
                case CMD_ESTABLISH:
                    handle = (xmpp_ibb_session_t*) malloc(sizeof(xmpp_ibb_session_t));
                    XMPP_IBB_Establish(conn, cmd_head->peer, handle);
                    add_handle(handle);
                    break;
                case CMD_SEND:
                    XMPP_IBB_Send(cmd_head->handle, cmd_head->message);
                    break;
                case CMD_CLOSE:
                    XMPP_IBB_Close(cmd_head->handle);
                    delete_handle(cmd_head->handle);
                    break;
            }
            itf_cmd *temp = cmd_head;
            cmd_head = cmd_head->next;
            free(temp);
        }
        xmpp_run_once(ctx, 500);
        usleep(5 * 1000);
    }

    printf("Quit\n");

    xmpp_stop(ctx);
    sleep(1);
    XMPP_IBB_Release(conn);
    XMPP_Close(conn, ctx);

    return 0;
}

void PrintUsage()
{

    printf("Usage: main \n\
        -q : Enable Quit by Ctrl+C or SIGINT\n\
        -h : This Help\n");

}

void init_argument(int argc, char* argv[])
{
    char c;

    while ((c = getopt(argc, argv, "u:t:q:h")) != -1) {
        switch (c)
        {
            case 'u':
                if (strcmp(optarg, "1") == 0) {
                    snprintf(xmpp_un, sizeof(xmpp_un), "andreac\\40workssys.com@cloud01.workssys.com/resource1");
                    snprintf(xmpp_pw, sizeof(xmpp_pw), "947c2c96o13cnjmujfh4l5i3is");
                } else if (strcmp(optarg, "2") == 0) {
                    snprintf(xmpp_un, sizeof(xmpp_un), "andreac\\40workssys.com@cloud01.workssys.com/resource2");
                    snprintf(xmpp_pw, sizeof(xmpp_pw), "947c2c96o13cnjmujfh4l5i3is");
                } else if (strcmp(optarg, "3") == 0) {
                    snprintf(xmpp_un, sizeof(xmpp_un), "andreac\\40workssys.com@cloud01.workssys.com/resource3");
                    snprintf(xmpp_pw, sizeof(xmpp_pw), "947c2c96o13cnjmujfh4l5i3is");
                }

            case 'q':

                bQuit = atoi(optarg);
                if (bQuit == 0)
                    printf("No Ctrl+C used\n");

                break;

            case 'h':
                PrintUsage();
                exit(-1);
                break;
            default:

                return;
        }

    }
}
