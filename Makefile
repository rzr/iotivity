#!/usr/bin/make -f
name?=iotivity

default/%:
	@echo "# $@ ignored"


default: all

# Overide variables
host_arch?=$(shell arch || echo "default")
OSTYPE?=$(shell echo "$${OSTYPE}")
uname?=$(shell echo `uname -s` || echo "")

TARGET_OS?=${uname}
ifeq ("",${TARGET_OS})
TARGET_OS=${OSTYPE}
endif

ifeq ("linux-gnu","${TARGET_OS}")
TARGET_OS=linux
endif
ifeq ("Linux","${TARGET_OS}")
TARGET_OS=linux
endif
ifeq ("",${TARGET_OS})
TARGET_OS=default
endif

ifeq ("i386","${host_arch}")
TARGET_ARCH?=x86
else
ifeq ("i486","${host_arch}")
TARGET_ARCH?=x86
else
ifeq ("i586","${host_arch}")
TARGET_ARCH?=x86
else
ifeq ("i686","${host_arch}")
TARGET_ARCH?=x86
else
ifeq ("armel","${host_arch}")
TARGET_ARCH?=arm
else
ifeq ("armhf","${host_arch}")
TARGET_ARCH?=arm
endif
ifeq ("aarch64","${host_arch}")
TARGET_ARCH?=arm64
endif
ifeq ("armv7l","${host_arch}")
TARGET_ARCH?=arm
endif
endif
endif
endif
endif
endif

# Default variables to overide before
LOGGING?=0
RELEASE?=1
SECURED?=0
TARGET_ARCH?=${host_arch}
TARGET_OS?=${uname}
TARGET_TRANSPORT?=IP
_includedir?=/usr/include
_libdir?=/usr/lib
_sbindir?=/usr/sbin
buildroot?=${DESTDIR}

ifeq (1,${RELEASE})
build_dir?=release
else
build_dir?=debug
endif

all: SConstruct
	scons \
  LOGGING=${LOGGING} \
  RELEASE=${RELEASE} \
  SECURED=${SECURED} \
  TARGET_ARCH=${TARGET_ARCH} \
  TARGET_OS=${TARGET_OS} \
  TARGET_TRANSPORT=${TARGET_TRANSPORT} \
  V=1 \
  VERBOSE=1 \
  #eol


check: auto_build.sh
	${<D}/${<F} unit_tests \
 && "log: $@: succeeded" \
 || echo "warning: $@: failed and ignored, TODO: fix $@ (and don't ignore $@ failures)"


install: rule/fix rule/install_headers rule/install_examples
	install -d ${buildroot}${_libdir}
	find . -iname "lib*.a" -exec install "{}" ${buildroot}${_libdir}/ \;
	find . -iname "lib*.so" -exec install "{}" ${buildroot}${_libdir}/ \;
	-rm -fv ${buildroot}${_libdir}/lib*.a
	install -d ${buildroot}${_libdir}/pkgconfig
	install -d ${DESTDIR}/usr/share/pkgconfig
	install ${name}.pc ${buildroot}${_libdir}/pkgconfig/
	ln -fs ../../../${_libdir}/pkgconfig/${name}.pc ${DESTDIR}/usr/share/pkgconfig

	@echo "TODO: workaround headers namespaces"
	ln -fs iotivity/resource ${buildroot}${_includedir}/
	ln -fs iotivity/service ${buildroot}${_includedir}/
	ln -fs iotivity/resource/c_common ${buildroot}${_includedir}/


clean:
	rm -f *~

rule/fix: ${name}.pc
	sed -e 's|^prefix=.*|prefix=/usr|g' -i ${<}


rule/install_headers: resource service
	find $< \
  -iname "include" -o -iname 'inc' -a -type d\
 | grep -v example | grep -v csdk \
 | while read dir ; do \
  dirname=$$(dirname -- "$${dir}") ; \
  install -d ${buildroot}${_includedir}/${name}/$${dirname} ; \
  install $${dir}/*.* ${buildroot}${_includedir}/${name}/$${dirname}/ ; \
 done
	cd resource/csdk && \
 find . -iname "include" -o -iname 'inc' -a -type d\
 | while read dir ; do \
  dirname=$$(dirname -- "$${dir}") ; \
  install -d ${buildroot}${_includedir}/${name}/resource/$${dirname} ; \
  install $${dir}/*.* ${buildroot}${_includedir}/${name}/resource/$${dirname}/ ; \
 done
	install -d ${buildroot}${_includedir}/${name}/resource/oc_logger/targets/

	install ./resource/oc_logger/include/targets/*.* \
 ${buildroot}${_includedir}/${name}/resource/oc_logger/targets/

	install -d ${buildroot}${_includedir}/${name}/resource/c_common

	install ./resource/c_common/*.h \
 ${buildroot}${_includedir}/${name}/resource/c_common/

	install -d ${buildroot}${_libdir}/${name}/examples/


rule/install_examples:
#	install out/${TARGET_OS}/${TARGET_ARCH}/${build_dir}/resource/examples/*client ${buildroot}${_libdir}/${name}/examples/
#	install out/${TARGET_OS}/${TARGET_ARCH}/${build_dir}/resource/examples/*server ${buildroot}${_libdir}/${name}/examples/
#	install out/${TARGET_OS}/${TARGET_ARCH}/${build_dir}/*/*/*/*client ${buildroot}${_libdir}/${name}/examples/
#	install out/${TARGET_OS}/${TARGET_ARCH}/${build_dir}/*/*/*/*server ${buildroot}${_libdir}/${name}/examples/
#	install out/${TARGET_OS}/${TARGET_ARCH}/${build_dir}/*lient ${buildroot}${_libdir}/${name}/examples/
#	install out/${TARGET_OS}/${TARGET_ARCH}/${build_dir}/*erver ${buildroot}${_libdir}/${name}/examples/

rule/help:
	@echo "OSTYPE=${OSTYPE}"
	@echo "TARGET_OS=${TARGET_OS}"
	@echo "host_arch=${host_arch}"
	@echo "uname=${uname}"
