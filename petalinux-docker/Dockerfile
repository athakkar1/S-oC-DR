FROM ubuntu:16.04

ENV DEBIAN_FRONTEND=noninteractive
ENV LANG=en_US.UTF-8
ENV LC_ALL=en_US.UTF-8
ENV LANGUAGE=en_US.UTF-8
ENV TERM=xterm
ENV SHELL=/bin/bash

# Fix apt sources for EOL Ubuntu 16.04
RUN sed -i 's|^deb |deb [trusted=yes] |g' /etc/apt/sources.list

# Install host dependencies required by Petalinux 2017.4
RUN dpkg --add-architecture i386 && apt-get update && apt-get install -y --allow-unauthenticated \
    tofrodos gawk xvfb git libncurses5-dev tftpd-hpa zlib1g-dev zlib1g-dev:i386 \
    libssl-dev flex bison chrpath socat autoconf libtool texinfo gcc-multilib \
    libsdl1.2-dev libglib2.0-dev screen pax cpio sudo wget locales \
    libc6-i386 lib32z1 lib32stdc++6 iproute2 net-tools vim diffstat xterm \
    && locale-gen en_US.UTF-8 \
    && rm -rf /var/lib/apt/lists/*

# Petalinux directory (MUST be at /opt/pkg/petalinux)
RUN mkdir -p /opt/pkg/petalinux

# Create non-root user (Yocto SDK installer refuses root)
RUN useradd -m -s /bin/bash builder && chown -R builder:builder /opt/pkg

# Copy and patch Petalinux installer
COPY petalinux-v2017.4-final-installer.run /tmp/
RUN chmod +x /tmp/petalinux-v2017.4-final-installer.run && \
    sed -i 's/^function accept_license {/function accept_license { return 0; /' /tmp/petalinux-v2017.4-final-installer.run && \
    sed -i 's/^if \[ "\$UID" == 0 \]; then/if false; then # root check disabled/' /tmp/petalinux-v2017.4-final-installer.run && \
    sed -i 's|if ! "${plnxinstallerdir}"/${envchecker} >(tee -a "${PLNXINSTALL_LOG}"); then|if false; then # env-check skipped|' /tmp/petalinux-v2017.4-final-installer.run

# Install Petalinux as non-root
USER builder
WORKDIR /home/builder
RUN /tmp/petalinux-v2017.4-final-installer.run /opt/pkg/petalinux
USER root
RUN rm /tmp/petalinux-v2017.4-final-installer.run

# Disable env-check in settings.sh (bash bugs cause failures in Docker)
RUN sed -i '/petalinux-env-check$/s/^/# DISABLED: /' /opt/pkg/petalinux/settings.sh

# Clone meta-qt4 layer dependency for meta-sdr
RUN cd /opt/pkg/petalinux/components/yocto/source/arm/layers \
    && (git clone https://git.yoctoproject.org/meta-qt4 \
        || git clone git://git.yoctoproject.org/meta-qt4) \
    && cd meta-qt4 && git checkout morty

# ============================================================
# Clone and configure the SOCDR Petalinux project
# ============================================================

# Clone Petalinux-Zybo from Digilent (with submodules)
RUN cd /opt && git clone --recursive https://github.com/Digilent/Petalinux-Zybo.git

WORKDIR /opt/Petalinux-Zybo/Zybo

# Clone u-boot-digilent for ext-local-src
RUN mkdir -p components/ext_sources && \
    cd components/ext_sources && \
    git clone https://github.com/Digilent/u-boot-digilent.git

# Fix digilent-apps submodule to commit that avoids private repo fetch errors
RUN cd project-spec/meta-user/recipes-apps/digilent-apps && \
    git checkout 06ec7256376eba3010fd8dbf18a3e8da1327e3b5

# Clone meta-sdr (rocko branch — matches Petalinux 2017.4's Yocto 2.2)
RUN cd project-spec && \
    git clone https://github.com/balister/meta-sdr.git && \
    cd meta-sdr && git checkout rocko

# Get latest libhackrf and rtlsdr recipes from meta-sdr kirkstone branch
RUN cd project-spec/meta-sdr && \
    git fetch origin kirkstone && \
    cd recipes-support && \
    rm -rf libhackrf rtlsdr && \
    git checkout origin/kirkstone -- libhackrf rtlsdr

# Fix rtl-sdr recipe syntax for Yocto 2.2 (Morty doesn't support do_install:append)
RUN sed -i 's/do_install:append/do_install_append/' \
    project-spec/meta-sdr/recipes-support/rtlsdr/rtl-sdr_git.bb

# Apply SOCDR project config customizations
COPY project-files/config project-spec/configs/config
COPY project-files/petalinux-image.bbappend project-spec/meta-user/recipes-core/images/petalinux-image.bbappend
COPY project-files/system-user.dtsi project-spec/meta-user/recipes-bsp/device-tree/files/system-user.dtsi

# Remove the stock rootfs_config so petalinux-config generates a fresh one
# (the Digilent default conflicts when libhackrf/rtl-sdr are added as NEW packages)
RUN rm -f project-spec/configs/rootfs_config

# ============================================================
# Entrypoint
# ============================================================

COPY entrypoint.sh /usr/local/bin/
RUN chmod +x /usr/local/bin/entrypoint.sh

WORKDIR /opt/Petalinux-Zybo/Zybo

ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
CMD ["bash"]