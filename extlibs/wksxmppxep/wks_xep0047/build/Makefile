.PHONY: all clean

XMPPLIB  ?= /root/libstrophe-2

SRCPATH = ../source
HDRPATH = ../include

BUILDPATH = ../build
OBJPATH = $(BUILDPATH)/obj
RLSPATH = $(BUILDPATH)/release
LIBPATH = $(BUILDPATH)/lib

XMPPLIB_INC = $(XMPPLIB)
INC_DIRS += -I$(XMPPLIB_INC)
INC_DIRS += -I$(HDRPATH)


CC = gcc
AR = ar
CFLAGS = -O -Wall  -Wl,--gc-sections -Wl,-s -std=gnu99 -fPIC
DEFS = #-DTB_LOG

VPATH = $(SRCPATH)
SRCS=$(wildcard $(SRCPATH)/*.c) 
HEADERS=$(wildcard $(HDRPATH)/*.h)
NOTDIRSRCS=$(notdir $(SRCS))
OBJS = $(patsubst %.c,$(OBJPATH)/%.o,$(NOTDIRSRCS))

all: libraries

libraries: libwksxep0047.a libwksxep0047.so

libwksxep0047.a: ${LIBPATH}/libwksxep0047.a

${LIBPATH}/libwksxep0047.a: $(OBJS)
	@mkdir -p $(LIBPATH)
	$(AR) rcs $@ $^
	@echo Compile $@ successful....

libwksxep0047.so: ${LIBPATH}/libwksxep0047.so

${LIBPATH}/libwksxep0047.so: $(OBJS)
	@mkdir -p $(LIBPATH)
	$(CC) -o $@ -shared $^
	@echo Compile $@ successful....

$(OBJS): $(OBJPATH)/%.o: %.c
	@mkdir -p $(OBJPATH)
	$(CC) $(CFLAGS) -c -o $@  $< $(INC_DIRS) $(DEFS)

clean:
	rm -rf $(LIBPATH)/wks_xep0047 $(OBJPATH) $(LIBPATH)
