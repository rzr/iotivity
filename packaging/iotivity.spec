Name: iotivity
Version: 1.1.0
Release: 0
Summary: IoTivity Base Stack & IoTivity Services
Group: System Environment/Libraries
License: Apache-2.0
URL: https://www.iotivity.org/
Source0: %{name}-%{version}.tar.bz2
Source10: cereal.tar.bz2
Source100: tinycbor.tar.bz2
Source101: gtest-1.7.0.zip
Source102: https://github.com/dascandy/hippomocks/archive/2f40aa11e31499432283b67f9d3449a3cd7b9c4d.zip
Source103: http://www.sqlite.org/2015/sqlite-amalgamation-3081101.zip
BuildRequires: gettext-tools
BuildRequires: expat-devel
BuildRequires:	python, libcurl-devel
BuildRequires:	scons
BuildRequires:  unzip
BuildRequires:	openssl-devel
BuildRequires:  boost-devel
BuildRequires:  boost-thread
BuildRequires:  boost-system
BuildRequires:  boost-filesystem
BuildRequires:  boost-program-options
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(uuid)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(capi-network-wifi)
BuildRequires:  pkgconfig(capi-network-bluetooth)
BuildRequires:  pkgconfig(sqlite3)
Requires(postun): /sbin/ldconfig
Requires(post): /sbin/ldconfig

## If tizen 2.x, RELEASE follows tizen_build_binary_release_type_eng. ##
## and if tizen 3.0, RELEASE follows tizen_build_devel_mode. ##
%if 0%{?tizen_build_devel_mode} == 1 || 0%{?tizen_build_binary_release_type_eng} == 1
%define RELEASE False
%define BUILDTYPE debug
%else
%define RELEASE True
%define BUILDTYPE release
%endif
%define release_mode ${RELEASE}


%{!?TARGET_TRANSPORT: %define TARGET_TRANSPORT IP}
%{!?SECURED: %define SECURED 0}
%{!?LOGGING: %define LOGGING True}
%{!?ROUTING: %define ROUTING GW}

%description
IoTivity Base (RICH & LITE) Stack & IoTivity Services

%package devel
Summary: Development files for %{name}
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}
Requires: pkgconfig

%description devel
The %{name}-devel package contains libraries and header files for
developing applications that use %{name}.

%package examples
Summary: Examples files for %{name}
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}
Requires: pkgconfig

%description examples
Contains samples applications that use %{name}.

%prep
%setup -q -n %{name}-%{version} -a 10 -a 100 -a 101 -a 102 -a 103

%define secure_mode 0
%define RPM_ARCH %{_arch}
%define TARGET_OS tizen

# overide to prevent issues
%define _smp_mflags -j4

# overide arch if needed
%ifarch %arm
%if "%{tizen}" == "2.4"
%define RPM_ARCH arm-v7a
%endif
%else
%ifarch aarch64
%define RPM_ARCH arm64
%else
%ifarch i586 i686 %{ix86}
%define RPM_ARCH x86
%endif
%endif
%endif

%build

cp -rfv hippomocks-2f40aa11e31499432283b67f9d3449a3cd7b9c4d  extlibs/hippomocks-master
ln -fs ../../gtest-1.7.0  extlibs/gtest/gtest-1.7.0
ln -fs ../../sqlite-amalgamation-3081101 extlibs/sqlite3/

scons %{?_smp_mflags} \
    RELEASE=%{RELEASE} \
    SECURED=%{SECURED} \
    TARGET_ARCH=%{RPM_ARCH} \
    TARGET_OS=%{TARGET_OS} \
    TARGET_TRANSPORT=IP


%install
rm -rf %{buildroot}

CFLAGS="${CFLAGS:-%optflags}" ; export CFLAGS ;
echo scons install --install-sandbox=%{buildroot} --prefix=%{_prefix} \
    TARGET_OS=%{TARGET_OS} \
    TARGET_ARCH=%{RPM_ARCH} \
    TARGET_TRANSPORT=%{TARGET_TRANSPORT} \
	RELEASE=%{RELEASE} SECURED=%{SECURED} LOGGING=%{LOGGING} ROUTING=%{ROUTING} \
	LIB_INSTALL_DIR=%{_libdir}

install -d %{buildroot}%{_sbindir}

install -d %{buildroot}%{_libdir}

find . -iname "lib*.a" -exec install "{}" %{buildroot}%{_libdir}/ \;
find . -iname "lib*.so" -exec install "{}" %{buildroot}%{_libdir}/ \;

find resource service -iname "include" -o -iname 'inc' -a -type d\
    | grep -v example | grep -v csdk | while read include ; do \
    dirname=$(dirname -- "$include") ; \
    install -d %{buildroot}%{_includedir}/%{name}/${dirname} ; \
    install $include/*.* %{buildroot}%{_includedir}/%{name}/${dirname}/ ; \
done

cd resource/csdk
find . -iname "include" -o -iname 'inc' -a -type d\
    | while read include ; do \
    dirname=$(dirname -- "$include") ; \
    install -d %{buildroot}%{_includedir}/%{name}/resource/${dirname} ; \
    install $include/*.* %{buildroot}%{_includedir}/%{name}/resource/${dirname}/ ; \
done
cd -

install -d %{buildroot}%{_includedir}/%{name}/resource/oc_logger/targets/
install ./resource/oc_logger/include/targets/*.* %{buildroot}%{_includedir}/%{name}/resource/oc_logger/targets/

install -d %{buildroot}%{_includedir}/%{name}/resource/c_common
install ./resource/c_common/*.h %{buildroot}%{_includedir}/%{name}/resource/c_common/

install -d %{buildroot}%{_libdir}/%{name}/examples/
install out/%{TARGET_OS}/%{RPM_ARCH}/%{BUILDTYPE}/resource/examples/*client %{buildroot}%{_libdir}/%{name}/examples/
install out/%{TARGET_OS}/%{RPM_ARCH}/%{BUILDTYPE}/resource/examples/*server %{buildroot}%{_libdir}/%{name}/examples/

rm -fv %{buildroot}%{_libdir}/libcoap.a
rm -fv %{buildroot}%{_libdir}/liboc.a
rm -fv %{buildroot}%{_libdir}/liboc_logger.a
rm -fv %{buildroot}%{_libdir}/libmosquitto.a

rm -rf   %{buildroot}%{_includedir}/resource


%clean
rm -rf %{buildroot}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_libdir}/lib*.so

%files devel
%defattr(644,root,root,755)
%{_includedir}/%{name}/
%{_includedir}/%{name}/*
%{_libdir}/lib*.a
#%%{_libdir}/pkgconfig/%{name}.pc

%files examples
%defattr(-,root,root,-)
%{_libdir}/%{name}/examples/
%{_libdir}/%{name}/examples/*
