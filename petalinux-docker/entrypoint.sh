#!/bin/bash
set -e

SETTINGS="/opt/pkg/petalinux/settings.sh"
[ -f "$SETTINGS" ] || { echo "ERROR: Petalinux not found."; exit 1; }

if [ -f /project/config.project ]; then
    PROJ="/project"
elif [ -f /project/Zybo/config.project ]; then
    PROJ="/project/Zybo"
elif [ -f /opt/Petalinux-Zybo/Zybo/config.project ]; then
    PROJ="/opt/Petalinux-Zybo/Zybo"
else
    echo "No Petalinux project found."
    cd /
    exec "$@"
    exit 0
fi

cd "$PROJ"
source "$SETTINGS"

echo ""
echo "=== SOCDR Petalinux 2017.4 ==="
echo ""
echo "1. petalinux-config"
echo "   Linux Components Selection -> u-boot -> ext-local-src"
echo "   External u-boot local source settings -> \${TOPDIR}/../components/ext_sources/u-boot-digilent"
echo "   Image Packaging Configuration -> Root filesystem type -> SD"
echo "   Yocto Settings -> User Layers -> 0 -> \${PROOT}/project-spec/meta-sdr"
echo "   Yocto Settings -> User Layers -> 1 -> /opt/pkg/petalinux/components/yocto/source/arm/layers/meta-qt4"
echo "   Exit to save"
echo ""
echo "2. petalinux-build"
echo ""
echo "3. After build works, add to project-spec/configs/rootfs_config:"
echo "   CONFIG_libhackrf=y"
echo "   CONFIG_rtl-sdr=y"
echo ""
echo "4. petalinux-build          (full build with SDR packages)"
echo ""
echo "5. petalinux-package --boot --force --fsbl images/linux/zynq_fsbl.elf --fpga images/linux/system_wrapper.bit --u-boot"
echo ""

exec "$@"