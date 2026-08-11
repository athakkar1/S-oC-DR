#!/bin/bash
set -e

SETTINGS="/opt/pkg/petalinux/settings.sh"
[ -f "$SETTINGS" ] || { echo "ERROR: Petalinux not found."; exit 1; }

# Determine project root: prefer mounted project, fall back to baked-in
if [ -f /project/config.project ]; then
    PROJ="/project"
elif [ -f /project/Zybo/config.project ]; then
    PROJ="/project/Zybo"
elif [ -f /opt/Petalinux-Zybo/Zybo/config.project ]; then
    PROJ="/opt/Petalinux-Zybo/Zybo"
else
    echo "No Petalinux project found."
    echo "Mount your project at /project or build the image with the baked-in project."
    cd /
    exec "$@"
    exit 0
fi

echo "Petalinux project: $PROJ"

# Clean stale host-pathed build configs
if [ -f "$PROJ/build/conf/bblayers.conf" ] && grep -q "/home/somalianpirate" "$PROJ/build/conf/bblayers.conf" 2>/dev/null; then
    echo "Removing stale host-pathed build configs..."
    rm -f "$PROJ/build/conf/bblayers.conf" "$PROJ/build/conf/local.conf" "$PROJ/build/conf/plnxtool.conf"
    echo "You must re-run 'petalinux-config' before building."
fi

if [ -f "$PROJ/build/sanity_info" ] && grep -q "/home/somalianpirate" "$PROJ/build/sanity_info" 2>/dev/null; then
    echo "WARNING: Stale build artifacts detected."
    echo "Run: rm -rf $PROJ/build/"
fi

if [ ! -f "$PROJ/build/conf/bblayers.conf" ]; then
    echo "Run 'petalinux-config' to generate the build configuration."
fi

cd "$PROJ"

# Source Petalinux
source "$SETTINGS"

echo ""
echo "Petalinux 2017.4 ready."
echo "Next: petalinux-config   (interactive, one-time setup)"
echo "      petalinux-build"
echo ""

exec "$@"