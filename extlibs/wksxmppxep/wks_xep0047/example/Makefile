.HONY: all clean

XMPPLIB   = /root/libstrophe-2

HDRPATH = ../include
RLSPATH = ./release

OBJPATH = ./obj
BUILDPATH = ../build
LIBPATH = $(BUILDPATH)/lib
XMPPLIBPATH = $(XMPPLIB)/.libs

XMPPLIB_INC = $(XMPPLIB)
INC_DIRS += -I$(XMPPLIB_INC)
INC_DIRS += -I$(HDRPATH)


CC = gcc
AR = ar
CFLAGS = -O -Wall -Wl,--gc-sections -Wl,-s -std=gnu99
DEFS = #-DTB_LOG

VPATH = $(SRCPATH)
SRCS=$(wildcard $(SRCPATH)/*.c) 
HEADERS=$(wildcard $(HDRPATH)/*.h)
NOTDIRSRCS=$(notdir $(SRCS))
OBJS = $(patsubst %.c,$(OBJPATH)/%.o,$(NOTDIRSRCS))

LIBS = -L$(LIBPATH) -L$(XMPPLIBPATH) -lwksxep0047 -lstrophe -lcrypto -lssl -lexpat -lresolv -lpthread -lm


all: wks_xep0047

wks_xep0047:
	@mkdir -p $(OBJPATH) $(RLSPATH)
	$(CC) $(CFLAGS) -c main.c -o $(OBJPATH)/main.o $(INC_DIRS) $(DEFS)
	$(CC) -o $(RLSPATH)/$@ $(OBJPATH)/main.o $(LIBS) 
	@echo Compile WKS-OIC-RACLIENT successful....

clean:
	rm -rf $(OBJPATH) $(RLSPATH)
