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


## If tizen 2.x, RELEASE follows tizen_build_binary_release_type_eng. ##
## and if tizen 3.0, RELEASE follows tizen_build_devel_mode. ##
%if 0%{?tizen_build_devel_mode} == 1 || 0%{?tizen_build_binary_release_type_eng} == 1
%define RELEASE False
%else
%define RELEASE True
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

scons --prefix=%{_prefix} \
		TARGET_OS=tizen TARGET_ARCH=%{RPM_ARCH} TARGET_TRANSPORT=%{TARGET_TRANSPORT} \
		RELEASE=%{RELEASE} SECURED=%{SECURED} LOGGING=%{LOGGING} ROUTING=%{ROUTING} \
		INSTALL_ROOT=%{buildroot} LIB_INSTALL_DIR=%{_libdir}


%install
rm -rf %{buildroot}
scons install --prefix=%{_prefix} \
		TARGET_OS=tizen TARGET_ARCH=%{RPM_ARCH} TARGET_TRANSPORT=%{TARGET_TRANSPORT} \
		RELEASE=%{RELEASE} SECURED=%{SECURED} LOGGING=%{LOGGING} ROUTING=%{ROUTING} \
		INSTALL_ROOT=%{buildroot} LIB_INSTALL_DIR=%{_libdir}

%if %{RELEASE} == "True"
%define build_mode release
%else
%define build_mode debug
%endif

# For Example
mkdir -p %{buildroot}%{_bindir}
cp out/tizen/*/%{build_mode}/examples/OICMiddle/OICMiddle %{buildroot}%{_bindir}
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

# For iotcon
cp resource/csdk/stack/include/ocpayload.h %{buildroot}%{_includedir}/resource
cp resource/csdk/ocrandom/include/ocrandom.h %{buildroot}%{_includedir}/resource

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
%{_bindir}/OICMiddle
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
