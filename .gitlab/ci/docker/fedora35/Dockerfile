FROM fedora:35
MAINTAINER Ben Boeckel <ben.boeckel@kitware.com>

COPY install_deps.sh /root/install_deps.sh
RUN sh /root/install_deps.sh

COPY install_cuda.sh /root/install_cuda.sh
RUN sh /root/install_cuda.sh

COPY install_adios.sh /root/install_adios.sh
RUN sh /root/install_adios.sh

COPY install_openvr.sh /root/install_openvr.sh
RUN sh /root/install_openvr.sh

# XXX(fedora35): ispc crashes building openvkl
# COPY install_ospray.sh /root/install_ospray.sh
# RUN sh /root/install_ospray.sh

# XXX(vpn): no stable URL for use in OptiX and MDL SDKs
# COPY install_visrtx.sh /root/install_visrtx.sh
# RUN sh /root/install_visrtx.sh
