#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

#include "xmpp_utils.h"

void nmtoken_generate(char *dst, int length)
{
    int i, random;

    for (i = 0; i < length; i++) {
        random = rand() % (26 + 26 + 10);
        if (random < 26)
            dst[i] = 'a' + random;
        else if (random < 26 + 26)
            dst[i] = 'A' + random - 26;
        else
            dst[i] = '0' + random - 26 - 26;
    }
    dst[length] = '\0';

}

char *xmpp_b64encode(const char *data, size_t dlen, char **encdata)
{
    BIO *bio, *b64;
    FILE *stream;
    int encSize = 4 * ceil((double) dlen / 3);

    if (encdata == NULL) {
        return NULL;
    }
    if (data == NULL || dlen <= 0) {
        *encdata = NULL;
        return NULL;
    }

    *encdata = (char *) malloc(encSize + 1);
    stream = fmemopen((void *) *encdata, encSize + 1, "w");

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_fp(stream, BIO_NOCLOSE);
    bio = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, data, dlen);
    BIO_flush(bio);

    BIO_free_all(bio);
    fclose(stream);

    return *encdata;
}

static size_t decDataLength(const char* encdata)
{

    int len = strlen(encdata);
    int padding = 0;

    if (encdata[len - 1] == '=' && encdata[len - 2] == '=') //last two chars are =
        padding = 2;
    else if (encdata[len - 1] == '=') //last char is =
        padding = 1;

    return (size_t) ((len * 3) / 4) - padding;
}

char *xmpp_b64decode(const char *encdata, char **decdata, size_t *dlen)
{
    BIO *bio, *b64;
    FILE *stream;
    int len;

    if (decdata == NULL || dlen == NULL) {
        return NULL;
    }
    if (encdata == NULL) {
        *decdata = NULL;
        *dlen = 0;
        return NULL;
    }

    *dlen = decDataLength(encdata);
    *decdata = (char *) malloc((*dlen) + 1);
    stream = fmemopen((void *) encdata, strlen(encdata), "r");

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_fp(stream, BIO_NOCLOSE);
    bio = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    len = BIO_read(bio, *decdata, strlen(encdata));
    (*decdata)[len] = '\0';
    BIO_free_all(bio);
    fclose(stream);

    return *decdata;
}

void xmpp_b64free(void *ptr)
{
    if (ptr == NULL) {
        return;
    }
    free(ptr);
}

typedef struct _node_t
{
    void *item;
    struct _node_t *next;
} node_t;

struct _ilist_t
{
    int size;
    node_t *head;
//    node_t *tail;
};

ilist_t * ilist_new()
{
    ilist_t *il;
    il = (ilist_t *) malloc(sizeof(struct _ilist_t));
    if (il == NULL) {
        return NULL;
    }
    il->size = 0;
    il->head = NULL;
//    il->tail = NULL;
    return il;
}

static node_t * ilist_find_node(ilist_t *il, void *item)
{
    node_t *nd;
    if (il == NULL || item == NULL) {
        return NULL;
    }
    for (nd = il->head; nd != NULL; nd = nd->next) {
        if (nd->item == item)
            return nd;
    }
    return NULL;
}

static void ilist_remove_node(ilist_t *il, node_t *nd)
{
    node_t *tmp;
    if (il == NULL) {
        return;
    }
    if (nd == NULL)
        return;
    if (il->head == nd) {
        il->head = nd->next;
    } else {
        for (tmp = il->head; tmp != NULL; tmp = tmp->next) {
            if (tmp->next == nd) {
                tmp->next = nd->next;
                break;
            }
        }
    }
    il->size--;
    free(nd);
}

void ilist_destroy(ilist_t *il)
{
    node_t *nd;
    if (il == NULL) {
        return;
    }
    while ((nd = il->head) != NULL) {
        ilist_remove_node(il, nd);
    }
    free(il);
}

void ilist_add(ilist_t *il, void *item)
{
    node_t *nd;
    if (il == NULL || item == NULL) {
        return;
    }
    nd = (node_t *) malloc(sizeof(struct _node_t));
    nd->item = item;
    nd->next = il->head;
    il->head = nd;
    il->size++;
}

void ilist_remove(ilist_t *il, void *item)
{
    node_t *nd;
    if (il == NULL || item == NULL) {
        return;
    }
    nd = ilist_find_node(il, item);
    if (nd != NULL) {
        ilist_remove_node(il, nd);
    }
}

void * ilist_finditem_func(ilist_t *il, find_fp ff, void *key)
{
    node_t *nd;
    if (il == NULL || ff == NULL || key == NULL)
        return NULL;
    for (nd = il->head; nd != NULL; nd = nd->next) {
        if (ff(nd->item, key)) {
//            printf("\n  %s-%d: %s(%s) found!!", __FILE__, __LINE__, __FUNCTION__, (char *) key);
            return nd->item;
        }
    }
    return NULL;
}

bool ilist_foundinlist(ilist_t *il, void * item)
{
    node_t *nd;
    if (il == NULL) {
        return false;
    }
    for (nd = il->head; nd != NULL; nd = nd->next) {
        if (nd->item == item) {
            return true;
        }
    }
    return false;
}

int ilist_size(ilist_t *il)
{
    if (il == NULL) {
        return -1;
    }
    return il->size;
}

