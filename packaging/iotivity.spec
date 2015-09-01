Name: iotivity
Version: 0.9.2
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
Requires(postun): /sbin/ldconfig
Requires(post): /sbin/ldconfig

%define release_mode true

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

%ifarch armv7l armv7l armv7hl armv7nhl armv7tnhl armv7thl
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


scons -j 4 TARGET_OS=tizen TARGET_ARCH=%{RPM_ARCH} TARGET_TRANSPORT=IP RELEASE=%{release_mode}

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}%{_includedir}
mkdir -p %{buildroot}%{_libdir}
mkdir -p %{buildroot}%{_bindir}

%if %{release_mode} == "true"

cp out/tizen/*/release/resource/examples/devicediscoveryclient %{buildroot}%{_bindir}
cp out/tizen/*/release/resource/examples/devicediscoveryserver %{buildroot}%{_bindir}
cp out/tizen/*/release/resource/examples/fridgeclient %{buildroot}%{_bindir}
cp out/tizen/*/release/resource/examples/fridgeserver %{buildroot}%{_bindir}
cp out/tizen/*/release/resource/examples/garageclient %{buildroot}%{_bindir}
cp out/tizen/*/release/resource/examples/garageserver %{buildroot}%{_bindir}
cp out/tizen/*/release/resource/examples/groupclient %{buildroot}%{_bindir}
cp out/tizen/*/release/resource/examples/groupserver %{buildroot}%{_bindir}
cp out/tizen/*/release/resource/examples/lightserver %{buildroot}%{_bindir}
cp out/tizen/*/release/resource/examples/presenceclient %{buildroot}%{_bindir}
cp out/tizen/*/release/resource/examples/presenceserver %{buildroot}%{_bindir}
cp out/tizen/*/release/resource/examples/roomclient %{buildroot}%{_bindir}
cp out/tizen/*/release/resource/examples/roomserver %{buildroot}%{_bindir}
cp out/tizen/*/release/resource/examples/simpleclient %{buildroot}%{_bindir}
cp out/tizen/*/release/resource/examples/simpleclientHQ %{buildroot}%{_bindir}
cp out/tizen/*/release/resource/examples/simpleclientserver %{buildroot}%{_bindir}
cp out/tizen/*/release/resource/examples/simpleserver %{buildroot}%{_bindir}
cp out/tizen/*/release/resource/examples/simpleserverHQ %{buildroot}%{_bindir}
cp out/tizen/*/release/resource/examples/threadingsample %{buildroot}%{_bindir}
cp out/tizen/*/release/lib*.so %{buildroot}%{_libdir}
cp out/tizen/*/release/libSSMSDK.a %{buildroot}%{_libdir}
cp out/tizen/*/release/libppm.a %{buildroot}%{_libdir}
cp out/tizen/*/release/service/protocol-plugin/plugins/mqtt-fan/*.so %{buildroot}%{_libdir}
cp out/tizen/*/release/service/protocol-plugin/plugins/mqtt-light/*.so %{buildroot}%{_libdir}

%else

cp out/tizen/*/debug/resource/examples/devicediscoveryclient %{buildroot}%{_bindir}
cp out/tizen/*/debug/resource/examples/devicediscoveryserver %{buildroot}%{_bindir}
cp out/tizen/*/debug/resource/examples/fridgeclient %{buildroot}%{_bindir}
cp out/tizen/*/debug/resource/examples/fridgeserver %{buildroot}%{_bindir}
cp out/tizen/*/debug/resource/examples/garageclient %{buildroot}%{_bindir}
cp out/tizen/*/debug/resource/examples/garageserver %{buildroot}%{_bindir}
cp out/tizen/*/debug/resource/examples/groupclient %{buildroot}%{_bindir}
cp out/tizen/*/debug/resource/examples/groupserver %{buildroot}%{_bindir}
cp out/tizen/*/debug/resource/examples/lightserver %{buildroot}%{_bindir}
cp out/tizen/*/debug/resource/examples/presenceclient %{buildroot}%{_bindir}
cp out/tizen/*/debug/resource/examples/presenceserver %{buildroot}%{_bindir}
cp out/tizen/*/debug/resource/examples/roomclient %{buildroot}%{_bindir}
cp out/tizen/*/debug/resource/examples/roomserver %{buildroot}%{_bindir}
cp out/tizen/*/debug/resource/examples/simpleclient %{buildroot}%{_bindir}
cp out/tizen/*/debug/resource/examples/simpleclientHQ %{buildroot}%{_bindir}
cp out/tizen/*/debug/resource/examples/simpleclientserver %{buildroot}%{_bindir}
cp out/tizen/*/debug/resource/examples/simpleserver %{buildroot}%{_bindir}
cp out/tizen/*/debug/resource/examples/simpleserverHQ %{buildroot}%{_bindir}
cp out/tizen/*/debug/resource/examples/threadingsample %{buildroot}%{_bindir}
cp out/tizen/*/debug/lib*.so %{buildroot}%{_libdir}
cp out/tizen/*/debug/libSSMSDK.a %{buildroot}%{_libdir}
cp out/tizen/*/debug/libppm.a %{buildroot}%{_libdir}
cp out/tizen/*/debug/service/protocol-plugin/plugins/mqtt-fan/*.so %{buildroot}%{_libdir}
cp out/tizen/*/debug/service/protocol-plugin/plugins/mqtt-light/*.so %{buildroot}%{_libdir}

%endif

cp resource/csdk/stack/include/*.h %{buildroot}%{_includedir}
cp resource/csdk/logger/include/*.h %{buildroot}%{_includedir}
cp resource/csdk/ocrandom/include/*.h %{buildroot}%{_includedir}
cp -r resource/oc_logger/include/* %{buildroot}%{_includedir}
cp resource/include/*.h %{buildroot}%{_includedir}

cp service/things-manager/sdk/inc/*.h %{buildroot}%{_includedir}
cp service/soft-sensor-manager/SDK/cpp/include/*.h %{buildroot}%{_includedir}
cp service/protocol-plugin/plugin-manager/src/*.h %{buildroot}%{_includedir}

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
%{_libdir}/libBMISensor.so
%{_libdir}/libDiscomfortIndexSensor.so
%{_libdir}/libmosquittopp.so
%{_libdir}/libpmimpl.so
%{_libdir}/libSSMCore.so
%{_libdir}/libTGMSDKLibrary.so
%{_libdir}/fanserver*.so
%{_libdir}/lightserver*.so
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
%if 0%{?tizen_version_major} < 3
%{_datadir}/license/%{name}-test
%else
%license LICENSE.APLv2
%endif

%files devel
%defattr(-,root,root,-)
%{_libdir}/lib*.a
%{_includedir}/*
