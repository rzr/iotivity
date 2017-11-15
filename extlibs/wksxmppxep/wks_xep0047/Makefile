LIBSTROPHE_BASE ?= ../libstrophe

SRCPATH = ./source
HDRPATH = ./include
EXMPATH = ./example

XMPPLIB_LIB = ${LIBSTROPHE_BASE}/.libs
XMPPLIB_INC = ${LIBSTROPHE_BASE}
INC_DIRS += -I${XMPPLIB_INC}
INC_DIRS += -I${HDRPATH}


LIBNAME=wksxmppxep
SHAREDLIB=lib${LIBNAME}.so
STATICLIB=lib${LIBNAME}.a

CC = gcc
AR = ar
CFLAGS = -Wall -Werror -Wno-unused -fPIC
LDFLAGS =-Wl,-rpath=./ -Wl,-rpath=${XMPPLIB_LIB} -L./ -L${XMPPLIB_LIB} -l${LIBNAME} -lstrophe -lssl -lcrypto -lexpat -lm -lpthread

VPATH = ${SRCPATH}
SRCS=${wildcard ${SRCPATH}/*.c} 
HEADERS=${wildcard ${HDRPATH}/*.h}
OBJS = ${patsubst %.c,%.o,${SRCS}}

MSRCS=${wildcard ${EXMPATH}/*.c} 
MOBJS = ${patsubst %.c,%.o,${MSRCS}}

#TESTTARGET=main main_chat main_ibb
TESTTARGET=main_chat main_ibb main_ibb_data

all: libraries testprogram

testprogram: ${TESTTARGET}

main: example/main.o
	${CC} -o $@ $^ ${LDFLAGS}

main_chat: example/main_chat.o
	${CC} -o $@ $^ ${LDFLAGS}

main_ibb: example/main_ibb.o
	${CC} -o $@ $^ ${LDFLAGS}

main_ibb_data: example/main_ibb_data.o
	${CC} -o $@ $^ ${LDFLAGS}

libraries: ${SHAREDLIB} ${STATICLIB}

${STATICLIB}: ${OBJS}
	${AR} rcs $@ $^
	@echo Compile $@ successful....

${SHAREDLIB}: ${OBJS}
	${CC} -o $@ -shared $^
	@echo Compile $@ successful....

.c.o:
	${CC} ${CFLAGS} -o $@ -c $< ${INC_DIRS}

clean:
	rm -rf ${OBJS} ${SHAREDLIB} ${STATICLIB} ${TESTTARGET} ${MOBJS}
