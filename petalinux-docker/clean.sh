#!/bin/bash
# Clean all Docker build artifacts for the SOCDR petalinux-docker project.
# Run from S-oC-DR/petalinux-docker/

set -e

DIR="$(cd "$(dirname "$0")" && pwd)"
echo "Cleaning build artifacts in $DIR"

rm -rf "$DIR/build"
rm -rf "$DIR/images"

# Optionally remove Docker image (uncomment if desired)
# docker rmi socdr-build 2>/dev/null || true

echo "Done. build/ and images/ removed."