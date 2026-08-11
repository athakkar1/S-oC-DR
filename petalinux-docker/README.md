# SOCDR Docker Build Environment

Self-contained, reproducible Petalinux 2017.4 build environment for the SOCDR project.

## TL;DR

**Prerequisites:** Docker + `petalinux-v2017.4-final-installer.run` placed in this directory.

```bash
# 1. Build the Docker image (~30 min, ~30 GB)
docker build -t socdr-build .

# 2. Run interactively (persists build artifacts to host)
mkdir -p ~/socdr-build-output
docker run -it --rm \
    -v ~/socdr-build-output/build:/opt/Petalinux-Zybo/Zybo/build \
    -v ~/socdr-build-output/images:/opt/Petalinux-Zybo/Zybo/images \
    socdr-build

# 3. Inside the container: one-time interactive config (~5 min)
petalinux-config
#    Verify: Yocto Settings → User Layers → 0 = ${PROOT}/project-spec/meta-sdr
#                         User Layers → 1 = /opt/pkg/.../meta-qt4
#    Exit to save

# 4. Inside the container: build (~2-4 hours first time)
petalinux-build

# 5. Inside the container: package boot image
petalinux-package --boot --force \
    --fsbl images/linux/zynq_fsbl.elf \
    --fpga images/linux/system_wrapper.bit \
    --u-boot
```

**Build artifacts** land in `~/socdr-build-output/` on your host:
- `build/` — Yocto build tree (cached for faster rebuilds)
- `images/linux/` — `BOOT.BIN`, `image.ub`, `rootfs.ext4`, etc. (flash these to SD card)

**Subsequent builds** (with cached `build/`): just run step 2, then step 4.

---

## What the Dockerfile Sets Up

| Step | Details |
|------|---------|
| Ubuntu 16.04 + build deps | tofrodos, gawk, xvfb, git, zlib1g-dev:i386, etc. |
| Petalinux 2017.4 | Patched: auto-accept license, skip root check, skip env-check |
| meta-qt4 (morty) | SDR layer dependency |
| Clone Petalinux-Zybo | The Digilent Zybo BSP project |
| Clone u-boot-digilent | Digilent U-Boot fork for ext-local-src |
| Fix digilent-apps | Checkout commit 06ec725 (avoids private repo fetch errors) |
| Clone meta-sdr (rocko) | SDR layer: GNU Radio, HackRF, RTL-SDR, etc. |
| Update libhackrf/rtlsdr recipes | Latest from meta-sdr kirkstone branch |
| Fix rtl-sdr recipe | `do_install:append` → `do_install_append` for Yocto 2.2 |
| Apply SOCDR configs | SD rootfs, libhackrf/rtl-sdr in image, meta-sdr/meta-qt4 user layers |

## Tips

- **First build is slow** (2-4 hours). Rebuilds with sstate-cache are much faster.
- **Image size**: ~30 GB. Ensure ~60 GB free for building.
- **`petalinux-config` must be interactive** (curses menu). Use `docker run -it`.
- Don't commit `build/` or `images/` — keep in .gitignore.