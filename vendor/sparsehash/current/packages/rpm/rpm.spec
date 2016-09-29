%define	RELEASE	1
%define rel     %{?CUSTOM_RELEASE} %{!?CUSTOM_RELEASE:%RELEASE}
%define	prefix	/usr

Name: %NAME
Summary: hash_map and hash_set classes with minimal space overhead
Version: %VERSION
Release: %rel
Group: Development/Libraries
URL: http://code.google.com/p/sparsehash
License: BSD
Vendor: Google Inc. and others
Packager: Google Inc. and others <google-sparsehash@googlegroups.com>
Source: http://%{NAME}.googlecode.com/files/%{NAME}-%{VERSION}.tar.gz
Distribution: Redhat 7 and above.
Buildroot: %{_tmppath}/%{name}-root
Prefix: %prefix
Buildarch: noarch

%description
The %name package contains several hash-map implementations, similar
in API to the SGI hash_map class, but with different performance
characteristics.  sparse_hash_map uses very little space overhead: 1-2
bits per entry.  dense_hash_map is typically faster than the default
SGI STL implementation.  This package also includes hash-set analogues
of these classes.

%changelog
	* Wed Apr 22 2009  <opensource@google.com>
	- Change build rule to use %configure instead of ./configure
	- Change install to use DESTDIR instead of prefix for make install
	- Use wildcards for doc/ and lib/ directories
        - Use {_includedir} instead of {prefix}/include

	* Fri Jan 14 2005 <opensource@google.com>
	- First draft

%prep
%setup

%build
# I can't use '% configure', because it defines -m32 which breaks on
# my development environment for some reason.  But I do take
# as much from % configure (in /usr/lib/rpm/macros) as I can.
./configure --prefix=%{_prefix} --exec-prefix=%{_exec_prefix} --bindir=%{_bindir} --sbindir=%{_sbindir} --sysconfdir=%{_sysconfdir} --datadir=%{_datadir} --includedir=%{_includedir} --libdir=%{_libdir} --libexecdir=%{_libexecdir} --localstatedir=%{_localstatedir} --sharedstatedir=%{_sharedstatedir} --mandir=%{_mandir} --infodir=%{_infodir}
make

%install
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)

%docdir %{prefix}/share/doc/%{NAME}-%{VERSION}
%{prefix}/share/doc/%{NAME}-%{VERSION}/*

%{_includedir}/google
%{_includedir}/sparsehash
%{_libdir}/pkgconfig/*.pc
