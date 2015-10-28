Name: iotivity
Version: 1.0.0
Release: 0
Summary: IoTivity Base Stack & IoTivity Services
Group: Network & Connectivity/Other
License: Apache-2.0
URL: https://www.iotivity.org/
Source0: %{name}-%{version}.tar.bz2
Source1001: %{name}.manifest
Source1002: %{name}-test.manifest
BuildRequires:  gettext, expat-devel
BuildRequires:  python, libcurl-devel
BuildRequires:  scons
BuildRequires:  openssl-devel
BuildRequires:  boost-devel
BuildRequires:  boost-thread
BuildRequires:  boost-system
BuildRequires:  boost-filesystem
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(uuid)
BuildRequires:  pkgconfig(capi-network-wifi)
BuildRequires:  pkgconfig(capi-network-bluetooth)
BuildRequires:  pkgconfig(capi-appfw-app-common)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(sqlite3)
Requires(postun): /sbin/ldconfig
Requires(post): /sbin/ldconfig

%define RELEASE True

## If tizen 2.x, RELEASE follows tizen_build_binary_release_type_eng. ##
## and if tizen 3.0, RELEASE follows tizen_build_devel_mode. ##
%if 0%{?tizen_build_devel_mode} == 1 || 0%{?tizen_build_binary_release_type_eng} == 1
%define RELEASE False
%endif

%{!?TARGET_TRANSPORT: %define TARGET_TRANSPORT IP,BT}
%{!?SECURED: %define SECURED 0}
%{!?LOGGING: %define LOGGING True}
%{!?ROUTING: %define ROUTING GW}

%description
IoTivity Base (RICH & LITE) Stack & IoTivity Services

%package service
Summary: Development files for %{name}
Group: Network & Connectivity/Service
Requires: %{name} = %{version}-%{release}

%description service
The %{name}-service package contains service libraries files for
developing applications that use %{name}-service.

%package test
Summary: Development files for %{name}
Group: Network & Connectivity/Testing
Requires: %{name} = %{version}-%{release}

%description test
The %{name}-test package contains example files to show
how the iotivity works using %{name}-test

%package devel
Summary: Development files for %{name}
Group: Network & Connectivity/Development
Requires: %{name} = %{version}-%{release}
Requires: pkgconfig

%description devel
The %{name}-devel package contains libraries and header files for
developing applications that use %{name}.

%prep
%setup -q -n %{name}-%{version}
cp LICENSE.md LICENSE.APLv2
cp %{SOURCE1001} .
%if 0%{?tizen_version_major} < 3
cp %{SOURCE1002} .
%else
cp %{SOURCE1001} ./%{name}-test.manifest
%endif

%build
%define RPM_ARCH %{_arch}

%ifarch armv7l armv7hl armv7nhl armv7tnhl armv7thl
%define RPM_ARCH "armeabi-v7a"
%endif

%ifarch aarch64
%define RPM_ARCH "arm64"
%endif

%ifarch x86_64
%define RPM_ARCH "x86_64"
%endif

%ifarch %{ix86}
%define RPM_ARCH "x86"
%endif


scons -j 4 TARGET_OS=tizen TARGET_ARCH=%{RPM_ARCH} TARGET_TRANSPORT=%{TARGET_TRANSPORT} \
		RELEASE=%{RELEASE} SECURED=%{SECURED} LOGGING=%{LOGGING} ROUTING=%{ROUTING}

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}%{_includedir}
mkdir -p %{buildroot}%{_libdir}
mkdir -p %{buildroot}%{_libdir}/pkgconfig
mkdir -p %{buildroot}%{_bindir}


%if %{RELEASE} == "True"
%define build_mode release
%else
%define build_mode debug
%endif

cp out/tizen/*/%{build_mode}/resource/examples/devicediscoveryclient %{buildroot}%{_bindir}
cp out/tizen/*/%{build_mode}/resource/examples/devicediscoveryserver %{buildroot}%{_bindir}
cp out/tizen/*/%{build_mode}/resource/examples/fridgeclient %{buildroot}%{_bindir}
cp out/tizen/*/%{build_mode}/resource/examples/fridgeserver %{buildroot}%{_bindir}
cp out/tizen/*/%{build_mode}/resource/examples/garageclient %{buildroot}%{_bindir}
cp out/tizen/*/%{build_mode}/resource/examples/garageserver %{buildroot}%{_bindir}
cp out/tizen/*/%{build_mode}/resource/examples/groupclient %{buildroot}%{_bindir}
cp out/tizen/*/%{build_mode}/resource/examples/groupserver %{buildroot}%{_bindir}
cp out/tizen/*/%{build_mode}/resource/examples/lightserver %{buildroot}%{_bindir}
cp out/tizen/*/%{build_mode}/resource/examples/presenceclient %{buildroot}%{_bindir}
cp out/tizen/*/%{build_mode}/resource/examples/presenceserver %{buildroot}%{_bindir}
cp out/tizen/*/%{build_mode}/resource/examples/roomclient %{buildroot}%{_bindir}
cp out/tizen/*/%{build_mode}/resource/examples/roomserver %{buildroot}%{_bindir}
cp out/tizen/*/%{build_mode}/resource/examples/simpleclient %{buildroot}%{_bindir}
cp out/tizen/*/%{build_mode}/resource/examples/simpleclientHQ %{buildroot}%{_bindir}
cp out/tizen/*/%{build_mode}/resource/examples/simpleclientserver %{buildroot}%{_bindir}
cp out/tizen/*/%{build_mode}/resource/examples/simpleserver %{buildroot}%{_bindir}
cp out/tizen/*/%{build_mode}/resource/examples/simpleserverHQ %{buildroot}%{_bindir}
cp out/tizen/*/%{build_mode}/resource/examples/threadingsample %{buildroot}%{_bindir}
cp out/tizen/*/%{build_mode}/resource/examples/oic_svr_db_server.json %{buildroot}%{_bindir}
cp out/tizen/*/%{build_mode}/resource/examples/oic_svr_db_client.json %{buildroot}%{_bindir}
if echo %{SECURED}|grep -qi '1'; then
	cp out/tizen/*/%{build_mode}/libocpmapi.a %{buildroot}%{_libdir}
fi
cp out/tizen/*/%{build_mode}/libcoap.a %{buildroot}%{_libdir}
cp out/tizen/*/%{build_mode}/lib*.so %{buildroot}%{_libdir}
cp out/tizen/*/%{build_mode}/%{name}.pc %{buildroot}%{_libdir}/pkgconfig

cp resource/csdk/stack/include/*.h %{buildroot}%{_includedir}
cp resource/csdk/logger/include/*.h %{buildroot}%{_includedir}
cp resource/csdk/ocrandom/include/*.h %{buildroot}%{_includedir}
cp resource/c_common/platform_features.h %{buildroot}%{_includedir}
cp -r resource/oc_logger/include/* %{buildroot}%{_includedir}
cp resource/include/*.h %{buildroot}%{_includedir}

cp service/things-manager/sdk/inc/*.h %{buildroot}%{_includedir}

%if 0%{?tizen_version_major} < 3
mkdir -p %{buildroot}/%{_datadir}/license
cp LICENSE.APLv2 %{buildroot}/%{_datadir}/license/%{name}
cp LICENSE.APLv2 %{buildroot}/%{_datadir}/license/%{name}-service
cp LICENSE.APLv2 %{buildroot}/%{_datadir}/license/%{name}-test
%endif

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_libdir}/liboc.so
%{_libdir}/liboc_logger.so
%{_libdir}/liboc_logger_core.so
%{_libdir}/liboctbstack.so
%{_libdir}/libconnectivity_abstraction.so
%if 0%{?tizen_version_major} < 3
%{_datadir}/license/%{name}
%else
%license LICENSE.APLv2
%endif

%files service
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_libdir}/libBMISensorBundle.so
%{_libdir}/libDISensorBundle.so
%{_libdir}/libresource_hosting.so
%{_libdir}/libTGMSDKLibrary.so
%{_libdir}/libHueBundle.so
%{_libdir}/librcs_client.so
%{_libdir}/librcs_common.so
%{_libdir}/librcs_container.so
%{_libdir}/librcs_server.so
%if 0%{?tizen_version_major} < 3
%{_datadir}/license/%{name}-service
%else
%license LICENSE.APLv2
%endif

%files test
%manifest %{name}-test.manifest
%defattr(-,root,root,-)
%{_bindir}/devicediscoveryclient
%{_bindir}/devicediscoveryserver
%{_bindir}/fridgeclient
%{_bindir}/fridgeserver
%{_bindir}/garageclient
%{_bindir}/garageserver
%{_bindir}/groupclient
%{_bindir}/groupserver
%{_bindir}/lightserver
%{_bindir}/presenceclient
%{_bindir}/presenceserver
%{_bindir}/roomclient
%{_bindir}/roomserver
%{_bindir}/simpleclient
%{_bindir}/simpleclientHQ
%{_bindir}/simpleclientserver
%{_bindir}/simpleserver
%{_bindir}/simpleserverHQ
%{_bindir}/threadingsample
%{_bindir}/oic_svr_db_server.json
%{_bindir}/oic_svr_db_client.json
%if 0%{?tizen_version_major} < 3
%{_datadir}/license/%{name}-test
%else
%license LICENSE.APLv2
%endif

%files devel
%defattr(-,root,root,-)
%{_libdir}/lib*.a
%{_libdir}/pkgconfig/%{name}.pc
%{_includedir}/*
