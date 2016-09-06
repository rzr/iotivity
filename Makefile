#!/usr/bin/make -f
# -*- makefile -*-
# ex: set tabstop=4 noexpandtab:
# Copyright 2017 Samsung Electronics France SAS
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

name?=iotivity

default/%:
	@echo "# $@ ignored"


default: all

# Override variables
host_arch?=$(shell uname -m || echo "default")
OSTYPE?=$(shell echo "$${OSTYPE}")
uname?=$(shell echo `uname -s` || echo "")
DESTDIR?=/
prefix?=/usr

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
ifeq ("armv7l","${host_arch}")
TARGET_ARCH?=arm
endif
ifeq ("aarch64","${host_arch}")
TARGET_ARCH?=arm64
endif
endif
endif
endif
endif
endif

# Default variables to override:
#release_mode?=0
#secure_mode?=0
#logging_mode?=True

TARGET_ARCH?=${host_arch}
TARGET_OS?=${uname}
TARGET_TRANSPORT?=IP
bindir?=/usr/bin/
includedir?=/usr/include
libdir?=/usr/lib
sbindir?=/usr/sbin

ifeq (0,${release_mode})
build_dir?=debug
debug_mode?=1
else
build_dir?=release
release_mode?=1
debug_mode?=0
endif

config_build+=--prefix=${prefix}
config_build+=VERBOSE=1 V=1
config_build+=DEBUG=${debug_mode} RELEASE=${release_mode}
config_build+=TARGET_ARCH=${TARGET_ARCH}
config_build+=TARGET_OS=${TARGET_OS}

config_install+=--install-sandbox=${DESTDIR}

# Overide default config if defined
ifneq ("","${logging_mode}")
config_build+=LOGGING=${logging_mode}
endif

ifneq ("","${secure_mode}")
config_build+=SECURED=${secure_mode}
endif

# Default variables to override:
#config_build+=TARGET_TRANSPORT=IP
#config_build+=WITH_TCP=1

outdir?=out/${TARGET_OS}/${TARGET_ARCH}/${build_dir}
version?=$(shell git describe || echo 0.0.0)
package?=${name}-${version}
distdir?=${CURDIR}/..
tarball?=${distdir}/${package}.tar.gz
doc_file?=${name}-doc.zip


all: build

build: SConstruct ${prep_dir}
	scons ${config_build}

prep_dir?=extlibs/tinycbor/tinycbor

${prep_dir}: prep.sh
	EXEC_MODE=true ${<D}/${<F}

check/%: auto_build.sh
	scons ${config_build} TEST=1 \
 | tee -a ${@D}.log.tmp

check/ignore:
	${MAKE} check \
 && echo "log: $@: succeeded" \
 || echo "warning: $@: failed and ignored, TODO: fix $@ (and don't ignore $@ failures)"

check/default: auto_build.sh
	${<D}/${<F} unit_tests

check/unknown:
	@echo "${@D}: TODO support arch"

check/skip:
	@echo "${@D}: TODO support arch"

check: check/skip
	@echo "TODO: verify $<"

install/scons:
	scons ${config_build} install ${config_install} 

install/post/binary: ${DESTDIR}
	find "$<" -type f -perm u+x -exec chrpath -d "{}" \;
	find "$<" -type f -iname "lib*.so" -exec chrpath -d "{}" \;

install: install/scons install/post/binary install/extra
	@echo "$@: $^"

install/extra: LICENSE.in ${DESTDIR}${includedir}/iotivity/resource
	@echo "TODO: package: $^"
	cat $<
	@echo "TODO: deprecate legacy path"
	ln -fs iotivity/c_common iotivity/resource iotivity/service ${DESTDIR}${includedir}/

clean: SConstruct
	-ls out && scons clean || echo "ignored"

cleanall: clean
	rm -rf out *~

distclean: cleanall
	rm -rf .scons*

dist: ${tarball}
	ls -l $<

rule/help:
	@echo "OSTYPE=${OSTYPE}"
	@echo "TARGET_OS=${TARGET_OS}"
	@echo "host_arch=${host_arch}"
	@echo "uname=${uname}"

${tarball}: ${CURDIR} Makefile distclean ${prep_dir}
	cd ${<} && tar cvfz \
 ${@} \
 --transform "s|^./|${name}-${version}/|" \
 --exclude 'debian' --exclude-vcs \
 ./

LICENSE.in: Makefile ${prep_dir}
	find . \
 -iname "LICEN*E*"  \
 -o -name "*BSD*" \
 -o -name "*COPYING*" \
 -o -name "*GPL*" \
 -o -name "*MIT*" \
 | sort | uniq \
 | grep -v '.git/' \
 | grep -v '$@' \
 | grep -v 'libcoap-4.1.1/LICENSE.GPL'  \
 | while read file ; do \
   dir=$$(dirname -- "$${file}") ;\
   echo "Files: $${dir}/*" ;\
   echo "License: $${file}" ;\
   sed 's/^/ /' "$${file}" ;\
   echo "" ;\
   echo "" ;\
 done > $@

rule/doc: out/doc

${doc_file}: resource/docs/javadocGen.sh
	rm -rf ${doc_file}
	mkdir -p ${<D}/html
	touch ${<D}/html/footer.html
	cd ${<D} && ./${<F}
	cd ${CURDIR} && zip -r9 ${doc_file} ./${<D}
	find . \
 -iname 'DoxyFile' -printf "%h\n" \
 -o -iname 'extlibs' -prune -a -iname 'out' -prune \
 | while read file ; do \
  cd "${CURDIR}/$${file}" && doxygen ; \
  cd ${CURDIR} ; \
  zip -r9 ${doc_file} $${file} ; \
done
	find . -iname "*.zip"

out/doc: ${doc_file}
	mkdir -p $@ && cd $@ && unzip ${CURDIR}/$<
