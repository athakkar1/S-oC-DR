# SOCDR Docker Build Environment

Self-contained, reproducible Petalinux 2017.4 build environment for the SOCDR project.

## You only need two things:

1. **Docker**  
2. **Petalinux 2017.4 installer** (`petalinux-v2017.4-final-installer.run`) from the [Xilinx Download Archive](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/embedded-design-tools/archive.html) (requires Xilinx account)

Place the installer in this directory, then:

```bash
docker build -t socdr-build .
docker run -it --rm socdr-build
```

## Inside the Container

The Petalinux project is at `/opt/Petalinux-Zybo/Zybo/`. You land there automatically.

```bash
# 1. Generate build configuration (interactive, one-time)
petalinux-config
# Navigate and exit to save. This also processes the hardware description (HDF).

# 2. Build the project
petalinux-build

# 3. Package boot image
petalinux-package --boot --force \
    --fsbl images/linux/zynq_fsbl.elf \
    --fpga images/linux/system_wrapper.bit \
    --u-boot
```

Verify user layers in petalinux-config:
- `Yocto Settings → User Layers → 0` = `${PROOT}/project-spec/meta-sdr`
- `Yocto Settings → User Layers → 1` = `/opt/pkg/petalinux/components/yocto/source/arm/layers/meta-qt4`

## External Volume (optional)

To persist build artifacts outside the container:

```bash
mkdir -p build-output
docker run -it --rm -v "$(pwd)/build-output:/opt/Petalinux-Zybo/Zybo/build" socdr-build
```

Or mount your own variant of the project:

```bash
docker run -it --rm -v /path/to/your/project:/project socdr-build
```

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