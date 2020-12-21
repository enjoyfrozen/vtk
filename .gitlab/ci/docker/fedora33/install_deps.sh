#!/bin/sh

# Install extra dependencies for VTK
dnf install -y --setopt=install_weak_deps=False \
   bzip2 patch doxygen git-core git-lfs

# Development tools
dnf install -y --setopt=install_weak_deps=False \
    libasan libubsan clang-tools-extra

# MPI dependencies
dnf install -y --setopt=install_weak_deps=False \
   openmpi-devel mpich-devel

# Qt dependencies
dnf install -y --setopt=install_weak_deps=False \
   qt5-qtbase-devel qt5-qttools-devel

# Mesa dependencies
dnf install -y --setopt=install_weak_deps=False \
   mesa-libOSMesa-devel mesa-libOSMesa mesa-dri-drivers mesa-libGL* glx-utils

# External dependencies
dnf install -y --setopt=install_weak_deps=False \
    libXcursor-devel libharu-devel utf8cpp-devel pugixml-devel libtiff-devel \
    eigen3-devel double-conversion-devel lz4-devel expat-devel glew-devel \
    hdf5-devel hdf5-mpich-devel hdf5-openmpi-devel hdf5-devel netcdf-devel \
    netcdf-mpich-devel netcdf-openmpi-devel libogg-devel libtheora-devel \
    jsoncpp-devel gl2ps-devel protobuf-devel libxkbcommon libxcrypt-compat \
    boost-devel tbb-devel postgresql-server-devel libpq-devel mariadb-devel \
    libiodbc-devel

# Python dependencies
dnf install -y --setopt=install_weak_deps=False \
   python3-twisted python3-autobahn python3 python3-devel python3-numpy \
   python3-pip python3-mpi4py-mpich python3-mpi4py-openmpi

python3 -m pip install wslink

dnf clean all
