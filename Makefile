#!/usr/bin/make -f

architecture?=i386
default/%:
	echo "# $@ ignored"

default: all 


all: build

TARGET_OS?=linux

host_arch?=$(shell arch)

ifeq (i386,${host_arch})
TARGET_ARCH?=x86
else
ifeq (i486,${host_arch})
TARGET_ARCH?=x86
else
ifeq (i586,${host_arch})
TARGET_ARCH?=x86
else
ifeq (i686,${host_arch})
TARGET_ARCH?=x86
else
ifeq (armel,${host_arch})
TARGET_ARCH?=arm
else
ifeq (armhf,${host_arch})
TARGET_ARCH?=arm
endif
ifeq (aarch64,${host_arch})
TARGET_ARCH?=arm64
endif
ifeq (armv7l,${host_arch})
TARGET_ARCH?=arm
endif
endif
endif
endif
endif
endif


TARGET_ARCH?=${host_arch}

release_mode?=0
build_mode?=debug
secure_mode?=0
target_os?=linux
logging_mode?=1


build: SConstruct
	scons \
    RELEASE=${release_mode} \
    SECURED=${secure_mode} \
    TARGET_ARCH=${TARGET_ARCH} \
    TARGET_OS=${TARGET_OS} \
    TARGET_TRANSPORT=IP \
    LOGGING=${logging_mode} \
    V=1 \
    VERBOSE=1 \
    #eol

check: auto_build.sh
	${<D}/${<F} unit_tests

buildroot?=${DESTDIR}
_sbindir?=/usr/sbin
_libdir?=/usr/lib
_includedir?=/usr/include
name?=iotivity

install:
	install -d ${buildroot}${_sbindir}
	install -d ${buildroot}${_libdir}

	find . -iname "lib*.a" -exec install "{}" ${buildroot}${_libdir}/ \;
	find . -iname "lib*.so" -exec install "{}" ${buildroot}${_libdir}/ \;

	find resource service -iname "include" -o -iname 'inc' -a -type d\
  | grep -v example | grep -v csdk | while read include ; do \
 dirname=$$(dirname -- "$${include}") ; \
 install -d ${buildroot}${_includedir}/${name}/$${dirname} ; \
 install $${include}/*.* ${buildroot}${_includedir}/${name}/$${dirname}/ ; \
 done

	cd resource/csdk && \
	find . -iname "include" -o -iname 'inc' -a -type d\
 | while read include ; do \
 dirname=$$(dirname -- "$${include}") ; \
 install -d ${buildroot}${_includedir}/${name}/resource/$${dirname} ; \
 install $${include}/*.* ${buildroot}${_includedir}/${name}/resource/$${dirname}/ ; \
 done

	install -d ${buildroot}${_includedir}/${name}/resource/oc_logger/targets/
	install ./resource/oc_logger/include/targets/*.* ${buildroot}${_includedir}/${name}/resource/oc_logger/targets/
	install -d ${buildroot}${_includedir}/${name}/resource/c_common
	install ./resource/c_common/*.h ${buildroot}${_includedir}/${name}/resource/c_common/
	install -d ${buildroot}${_libdir}/${name}/examples/
	install -d ${buildroot}${_libdir}/pkgconfig
	install -d ${DESTDIR}/usr/share/pkgconfig
	install iotivity.pc ${buildroot}${_libdir}/pkgconfig/
	install iotivity.pc ${DESTDIR}/usr/share/pkgconfig
#	install out/${TARGET_OS}/${TARGET_ARCH}/${build_mode}/resource/examples/*client ${buildroot}${_libdir}/${name}/examples/
#	install out/${TARGET_OS}/${TARGET_ARCH}/${build_mode}/resource/examples/*server ${buildroot}${_libdir}/${name}/examples/
#	install out/${TARGET_OS}/${TARGET_ARCH}/${build_mode}/*/*/*/*client ${buildroot}${_libdir}/${name}/examples/
#	install out/${TARGET_OS}/${TARGET_ARCH}/${build_mode}/*/*/*/*server ${buildroot}${_libdir}/${name}/examples/
#	install out/${TARGET_OS}/${TARGET_ARCH}/${build_mode}/*lient ${buildroot}${_libdir}/${name}/examples/
#	install out/${TARGET_OS}/${TARGET_ARCH}/${build_mode}/*erver ${buildroot}${_libdir}/${name}/examples/
	rm -fv ${buildroot}${_libdir}/libcoap.a
	rm -fv ${buildroot}${_libdir}/liboc.a
	rm -fv ${buildroot}${_libdir}/liboc_logger.a
	rm -fv ${buildroot}${_libdir}/libmosquitto.a
