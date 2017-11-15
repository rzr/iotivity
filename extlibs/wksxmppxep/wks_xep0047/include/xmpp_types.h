/*
 * xmpp_types.h
 *
 *  Created on: Aug 5, 2015
 *      Author: user
 */

#ifndef INC_XMPP_TYPES_H_
#define INC_XMPP_TYPES_H_

#define XMPP_NS_STANZAS "urn:ietf:params:xml:ns:xmpp-stanzas"

typedef struct _xmpp_t xmpp_t;

typedef struct _xmppdata_t
{
    void *data;
    int size;
    char *tojid;
    char *from;
} xmppdata_t;

typedef struct _xmppconn_info_t
{
    int connevent;
    int error;
    int errortype;
    char *errortext;
} xmppconn_info_t;

typedef struct _xmpperror_t
{
    int  code;
    char type[8];
    char mesg[64];
    char *desc;
} xmpperror_t;

#endif /* INC_XMPP_TYPES_H_ */
