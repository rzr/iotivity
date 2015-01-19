Name: iotivity
Version: 0.9.0
Release: 0
Summary: IoTivity Base Stack & IoTivity Services
Group: System Environment/Libraries
License: Apache-2.0
URL: https://www.iotivity.org/
Source0: %{name}-%{version}.tar.bz2
BuildRequires: expat-devel
BuildRequires: gettext-tools
BuildRequires: python, libcurl-devel
BuildRequires: scons
BuildRequires: openssl-devel
BuildRequires: boost-devel
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(capi-network-bluetooth)
BuildRequires: pkgconfig(capi-system-device)
Requires(postun): /sbin/ldconfig
Requires(post): /sbin/ldconfig

%define release_mode true

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

%prep
%setup -q -n %{name}-%{version}

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


%build

scons -j 4 TARGET_OS=tizen TARGET_ARCH=%{RPM_ARCH} RELEASE=%{release_mode}

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}%{_includedir}
mkdir -p %{buildroot}%{_includedir}/targets
mkdir -p %{buildroot}%{_libdir}
mkdir -p %{buildroot}%{_sbindir}

%if %{release_mode} == "true"
cp out/tizen/*/release/lib*.so %{buildroot}%{_libdir}
cp out/tizen/*/release/lib*.a %{buildroot}%{_libdir}
%else
cp out/tizen/*/debug/lib*.so %{buildroot}%{_libdir}
cp out/tizen/*/debug/lib*.a %{buildroot}%{_libdir}
%endif

cp resource/csdk/stack/include/ocstack.h %{buildroot}%{_includedir}
cp resource/csdk/stack/include/ocstackconfig.h %{buildroot}%{_includedir}
cp resource/csdk/ocsocket/include/ocsocket.h %{buildroot}%{_includedir}
cp resource/oc_logger/include/oc_logger.hpp %{buildroot}%{_includedir}
cp resource/oc_logger/include/oc_log_stream.hpp %{buildroot}%{_includedir}
cp resource/oc_logger/include/oc_logger.h %{buildroot}%{_includedir}
cp resource/oc_logger/include/oc_logger_types.h %{buildroot}%{_includedir}
cp resource/oc_logger/include/targets/oc_console_logger.h %{buildroot}%{_includedir}/targets
cp resource/oc_logger/include/targets/oc_ostream_logger.h %{buildroot}%{_includedir}/targets
cp resource/include/*.h %{buildroot}%{_includedir}

cp service/things-manager/sdk/inc/*.h %{buildroot}%{_includedir}
cp service/soft-sensor-manager/SDK/cpp/include/*.h %{buildroot}%{_includedir}


%clean
rm -rf %{buildroot}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_libdir}/lib*.so
%{_libdir}/lib*.a

%files devel
%defattr(-,root,root,-)
%{_includedir}/*
