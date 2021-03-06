Summary:	The CELT Low-Latency Audio Compression Codec.
Name:		libcelt
Version:	0.7.1
Release: 	0%{?dist}
Epoch:		0
Group:		System Environment/Libraries
License:	BSD
URL:		http://www.xiph.org/
Source:		http://downloads.xiph.org/releases/celt/celt-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)	
BuildRequires: 	libogg-devel >= 2:1.1
Requires:	pkg-config

%description
CELT is a fully open, non-proprietary, patent- and royalty-free,
low-latency compressed audio format for voice and music.

The libcelt package contains runtime libraries for use in programs
that support Ogg CELT.

%package devel
Summary: Files for CELT application development.
Group: Development/Libraries
Requires:	libogg-devel >= 2:1.1
Requires:	libcelt = %{epoch}:%{version}-%{release}

%description devel
The libcelt-devel package contains the header files and documentation
needed to develop applications with Ogg CELT.

%package utils
Summary: CELT codec utilities.
Group: Applications/Multimedia
Requires:	libcelt = %{epoch}:%{version}-%{release}

%description utils
Basic utilities for encoding, decoding and manipulating Ogg CELT streams.

%prep

%setup -q -n celt-%{version}

%build
%configure --with-ogg-libraries=%{_libdir}
make

%check
make check

%install
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install

# remove unpackaged files from the buildroot
rm -f $RPM_BUILD_ROOT%{_libdir}/*.la
rm -f $RPM_BUILD_ROOT%{_libdir}/*.a


%files
%defattr(-,root,root)
%doc AUTHORS COPYING README
%{_libdir}/libcelt.so.*

%files devel
%defattr(-,root,root)
%{_includedir}/celt
%{_libdir}/libcelt.so
%{_libdir}/pkgconfig/*.pc

%files utils
%defattr(-,root,root)
%{_bindir}/celtenc
%{_bindir}/celtdec

%clean 
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%changelog
* Mon Dec 22 2008 Monty Montgomery <monty@xiph.org> 0.5.1-0
- initial spec file created
