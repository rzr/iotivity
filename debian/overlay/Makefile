#!/usr/bin/make -f

sudo?=sudo

distribution?=testing
#distribution?=precise
distribution?=sid

architecture?=i386
default/%:
	echo "# $@ ignored"

default: all 


all: build

# ['x86', 'x86_64', 'arm', 'arm64']

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

release_mode?=false
build_mode?=debug
secure_mode?=0
target_os?=linux
logging_mode?=true


build: SConstruct deps
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

deps/ubuntu: extlibs/boost/boost_1_55_0 

deps: extlibs/tinycbor/tinycbor

setup:
	pwd

debian/overlay/extlibs/hippomocks-master.zip:
	wget -O$@ https://github.com/TizenTeam/hippomocks/archive/master.zip

setup/ubuntu: extlibs/boost/boost_1_55_0 extlibs/tinycbor/tinycbor 
	cd $< &&  ./bootstrap.sh --with-libraries=system,filesystem,date_time,thread,regex,log,iostreams,program_options --prefix=/usr/local
	cd $< && ${sudo} ./b2 install
	${sudo} sh -c 'echo '/usr/local/lib' >> /etc/ld.so.conf.d/local.conf'
	ldconfig

boost_file?=boost_1_55_0.tar.bz2
boost_url?=http://freefr.dl.sourceforge.net/project/boost/boost/1.55.0/${boost_file}
gtest_url?=https://googletest.googlecode.com/files/gtest-1.7.0.zip

files/ubuntu: files debian/${boost_file} 

files: extlibs/tinycbor/tinycbor.tar.bz2 \
 extlibs/gtest/gtest-1.7.0.zip \
 extlibs/hippomocks-2f40aa11e31499432283b67f9d3449a3cd7b9c4d.zip
	git add -f $^

extlibs/gtest/gtest-1.7.0.zip:
	mkdir -p ${@D}
	wget -O $@ ${gtest_url}

debian/${boost_file}:
	wget -O $@ ${boost_url}

# https://launchpad.net/~mapnik/+archive/ubuntu/boost-backports-1-54
# http://packages.ubuntu.com/trusty/libboost-thread1.55-dev
extlibs/boost/boost_1_55_0: debian/${boost_file}
	mkdir -p ${@D}
	cd ${@D} && tar xvfj ${CURDIR}/$<

extlibs/tinycbor/tinycbor: extlibs/tinycbor/tinycbor.tar.bz2
	mkdir -p ${@D}
	cd ${@D} && tar xvfj ${CURDIR}/$<

extlibs/tinycbor/tinycbor.tar.bz2:
	mkdir -p tmp
	git clone https://github.com/01org/tinycbor.git tmp/tinycbor
	cd tmp && tar cvfj ${CURDIR}/$@ --exclude-vcs tinycbor
	echo debian/tinycbor.tar.bz2 >> debian/source/include-binaries

extlibs/hippomocks-2f40aa11e31499432283b67f9d3449a3cd7b9c4d.zip:
	mkdir -p ${@D}
	wget -O ${@} https://github.com/dascandy/hippomocks/archive/2f40aa11e31499432283b67f9d3449a3cd7b9c4d.zip

apt:
	${sudo} apt-get install libglib2.0-dev \
 scons \
 unzip \
 uuid-dev \
 python-dev \
 autotools-dev \
 libicu-dev \
 libbz2-dev \
 sudo \
 file \
 apt-build

apt/debian:
	${sudo} apt-get install \
 autotools-dev \
 libc6-dev \
 libbz2-dev \
 libglib2.0-dev \
 libicu-dev \
 python-dev \
 scons \
 sudo \
 unzip \
 uuid-dev \
 libboost-date-time-dev \
 libboost-iostreams-dev \
 libboost-log-dev \
 libboost-program-options-dev \
 libboost-regex-dev \
 libboost-system-dev \
 libboost-thread-dev \
 libboost-all-dev


ubuntu:
	~/bin/git.sh op ci
	make reset
	~/bin/arc.sh -e debian_
	debuild -S -uc
#	${sudo} pbuilder --login --distribution ${distribution} --architecture ${architecture} 
#       sudo mount --bind $(pwd) //var/cache/pbuilder/build//31705/${CURDIR}
	${sudo} pbuilder --update --distribution ${distribution} --architecture ${architecture} 
	${sudo} pbuilder --build --distribution ${distribution} --architecture ${architecture} ../*.dsc

#distclean:
#	rm -rf * .sconf_temp .scon* 


pbuilder: debian/control
	~/bin/git.sh op ci
	make reset
	~/bin/arc.sh -e debian_
	debuild -S -uc
	${sudo} pbuilder --login --distribution ${distribution} --architecture ${architecture} 
	${sudo} pbuilder --update --distribution ${distribution} --architecture ${architecture} 
	${sudo} pbuilder --build --distribution ${distribution} --architecture ${architecture} ../*.dsc

#distclean:
#	rm -rf * .sconf_temp .scon* 

reset:
	rm -rf * .sconf_temp .scon* && git checkout .

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

	find out
	find out -executable -type f
