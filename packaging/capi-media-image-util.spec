Name:       capi-media-image-util
Summary:    A Image Utility library in Tizen Native API
Version:    0.1.42
Release:    2
Group:      Multimedia/API
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Source1001:     capi-media-image-util.manifest
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(mm-common)
BuildRequires:  pkgconfig(mmutil-common)
BuildRequires:  pkgconfig(mmutil-jpeg)
BuildRequires:  pkgconfig(mmutil-imgp)
BuildRequires:  pkgconfig(mmutil-png)
BuildRequires:  pkgconfig(mmutil-gif)
BuildRequires:  pkgconfig(mmutil-bmp)
BuildRequires:  pkgconfig(capi-base-common)
BuildRequires:  pkgconfig(capi-media-tool)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(gmodule-2.0)
BuildRequires:  pkgconfig(libtzplatform-config)
BuildRequires:  cmake

%description
A Image Utility library in Tizen Native API package

%package devel
Summary:    A Image Utility library in Tizen Native API (Development)
Group:      Multimedia/Development
Requires:   %{name} = %{version}-%{release}

%description devel
A Image Utility library in Tizen Native API (Development) package

%package tool
Summary:    Image Utility tools
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}

%description tool
Image Utility Library - Tools.

%prep
%setup -q
cp %{SOURCE1001} .

%build
export CFLAGS+=" -Wextra -Wno-array-bounds -D_FORTIFY_SOURCE=2"
export CFLAGS+=" -Wno-ignored-qualifiers -Wno-unused-parameter -Wshadow"
export CFLAGS+=" -Wwrite-strings -Wswitch-default"
export CFLAGS+=" -DGIF_ENCODER_V2"
export CFLAGS+=" -DPATH_LIBDIR=\\\"%{_libdir}\\\""
%if 0%{?gcov:1}
export CFLAGS+=" -fprofile-arcs -ftest-coverage"
export CXXFLAGS+=" -fprofile-arcs -ftest-coverage"
export FFLAGS+=" -fprofile-arcs -ftest-coverage"
export LDFLAGS+=" -lgcov"
%endif

MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
%cmake . -DFULLVER=%{version} -DMAJORVER=${MAJORVER} \
%if 0%{?model_build_feature_multimedia_image_hw_acceleration}
 -DFEATURE_ENABLE_HW_ACCELERATION:BOOL=ON
%else
 -DFEATURE_ENABLE_HW_ACCELERATION:BOOL=OFF
%endif
%__make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}

%make_install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%manifest %{name}.manifest
%license LICENSE.APLv2.0
%{_libdir}/lib*.so.*

%files devel
%manifest %{name}.manifest
%{_libdir}/lib*.so
%{_libdir}/pkgconfig/*.pc
%{_includedir}/media/*.h

%files tool
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_bindir}/*
