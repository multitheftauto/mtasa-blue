%define	RELEASE	1
%define rel     %{?CUSTOM_RELEASE} %{!?CUSTOM_RELEASE:%RELEASE}
%define	prefix	/usr

Name: %NAME
Summary: hash_map and hash_set classes with minimal space overhead
Version: %VERSION
Release: %rel
Group: Development/Libraries
URL: http://code.google.com/p/google-sparsehash
License: BSD
Vendor: Google
Packager: Google <opensource@google.com>
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
    * Fri Jan 14 2005 <opensource@google.com>
    - First draft

%prep
%setup

%build
./configure
make prefix=%prefix

%install
rm -rf $RPM_BUILD_ROOT
make prefix=$RPM_BUILD_ROOT%{prefix} install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)

%doc AUTHORS COPYING ChangeLog INSTALL NEWS README README.windows TODO
%doc doc/index.html doc/designstyle.css
%doc doc/dense_hash_map.html doc/dense_hash_set.html
%doc doc/sparse_hash_map.html doc/sparse_hash_set.html doc/sparsetable.html
%doc doc/implementation.html doc/performance.html

%{prefix}/include/google
