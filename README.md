Zynq-SDR

- Project Description
  - HackRF is an SDR platform that enables duplex RF capabilities from 1 MHz to 6 GHz. It is a cheap, open source solution that allows hobbyists and professionals to prototype their RF algorithms and create cool projects that utilize a previously gatekept and expensive technology. The internals of the HackRF uses an ARM Cortex M4 based digital signal controller and CPLD (a much more primitive FPGA) to do its logic, making advanced signal processing hard to do on the edge. This means that the user must use a desktop to run things like GNU Radio or deep learning algorithms to offload complex signal processing to a much more powerful machine. This makes it hard to use the HackRF for embedded prototyping applications, since it is virtually unusable without a laptop or desktop. To address this issue, an SoC can be used as a controller of the HackRF and hardware accelerator for signal processing. The Zybo is a cheap Zynq SoC based board made by Digilent that features FPGA fabric and dual-core ARM A9 processors, as well as a breakout of many different peripherals that can be used for embedded applications. These Zynq SoC’s can run embedded linux with the help of Xilinx’s PetaLinux toolkit, which makes it easy to run all the programs that the SDR community has already developed (GNU Radio, HackRF libraries, PySDR) on an edge embedded device rather than a desktop. On top of this, a user could leverage the PL side of the SoC to create fast parallelized signal processing capabilities that not even a desktop can achieve. In order for this to work, I propose Bag End, a Zynq based framework that abstracts the interfacing needed to communicate with and control the HackRF. It will come with an embedded linux image that sets up the PL for seamless data communication of I/Q data from the HackRF, includes the community SDR software and libraries, and a web server control interface to remote into the Zynq and control the HackRF.
- Top Level Block Diagram
  - To-Do/Notes
    - Meta-SDR
      - Meta-SDR github found [here](https://github.com/balister/meta-sdr)
      - Says it depends on "openembedded-core", "meta-openembedded", and meta-qt4
      - you can find layers that come with the petalinux install at /opt/pkg/petalinux/components/yocto/source/arm/layers/
      - It includes "core" and all of the layers in meta-openembedded. Missing meta-qt4 layer completely.
      - Now, I am unsure of if these layers are already being used in yocto build, and if not how to add them.
      - One thing is this "petalinux-config -c rootfs" command which allows you to select what to add to rootfs. probably need to do something here.
      - Ok I got it to build, but I don't think it shows up in the rootfs. I need to add the package to it somehow.
      - Build specific recipes using "petalinux-build -c <name-of-recipe>
      - Sifted through so many compile errors with meta-sdr, a lot seem to be because of version mismatches. Petalinux 2017.4 runs on Yocto 2.2, and meta-sdr only has supported branches for 2.1 and 2.3 and others. 
      - Fixed a bunch of errors that honestly I cannot name all of, but notably including meta-qt4 in installed Yocto layers, adding meta-qt4 and meta-sdr to user layers in petalinux-config, fixing broken fetch links, hardcoding python-io instead of python-io-native in a recipe, etc.
      - Building rtl-sdr and hackrf libs seem to work, I need to decide whether to continue trying to build gnuradio, or to just get these libs on the board and write an interface to the SDR myself.
    - Hand-Written Interface
      - GNURadio build is a bust. Too many version/dependency conflicts. Even if I fixed the compile errors I am getting right now who knows how many more there will be. The other option would be to upgrade the Digilent project to 2023/2024 standards, but I cannot be fucked learning how Yocto works and write all these bitbake recipes myself. Maybe a project for myself, but IDC for now.
      - On the other hand, I got the libhackrf and rtl-sdr support software to build. HackRF comes with some helpful binaries for debug, version info, firmware flashing, etc., as well as the libhackrf pre-compiled libraries that I can leverage in writing my own interface. Hopefully this means I don't need to include any more dependencies and any code I write can just compile out of the box.
      - Now, I need to figure out how to add built packages/libraries into rootfs and just get all the libraries/binaries onto the rootfs and see if they work.
      - I also really need to research HackRF docs to see what building an interface will look like.
    - C Interface
      - [https://github.com/GvozdevLeonid/python_hackrf/blob/main/python_hackrf/pylibhackrf/pyhackrf.pyx](https://github.com/GvozdevLeonid/python_hackrf/blob/main/python_hackrf/pylibhackrf/pyhackrf.pyx)
      - [https://pysdr.org/content/hackrf.html](https://pysdr.org/content/hackrf.html)
      - Use existing HackRF tool source code, and python example/library to put together the API calls and data processing necessary to get a stream of I/Q data
    - Edit Linux base hardware image
    - Source control the project
    - Write report
    - Meeting Notes
      - Links to hardware used should be listed
  - 
- Resources
  - makeZynq software developers guide: [https://docs.amd.com/r/en-US/ug821-zynq-7000-swdev](https://docs.amd.com/r/en-US/ug821-zynq-7000-swdev)
  - BSP and libraries overview (xilinx HAL documentation): [https://docs.amd.com/r/en-US/oslib_rm](https://docs.amd.com/r/en-US/oslib_rm)
  - zybo petalinux bsp release: [https://digilent.com/reference/software/petalinux/start?srsltid=AfmBOoq6opcVTQKC6MQ4xMvDz8r5vLUt36NBnjheI-KECojten7uKawR](https://nam02.safelinks.protection.outlook.com/?url=https%3A%2F%2Fdigilent.com%2Freference%2Fsoftware%2Fpetalinux%2Fstart%3Fsrsltid%3DAfmBOoq6opcVTQKC6MQ4xMvDz8r5vLUt36NBnjheI-KECojten7uKawR&data=05%7C02%7Cathakka5%40stevens.edu%7C06812fe230264ed3962408dd31b7deb4%7C8d1a69ec03b54345ae21dad112f5fb4f%7C0%7C0%7C638721387968883682%7CUnknown%7CTWFpbGZsb3d8eyJFbXB0eU1hcGkiOnRydWUsIlYiOiIwLjAuMDAwMCIsIlAiOiJXaW4zMiIsIkFOIjoiTWFpbCIsIldUIjoyfQ%3D%3D%7C0%7C%7C%7C&sdata=f0NLkQ0nupIHUEPcWbykVx9bAL3VkWkl%2BqKFFRWWvgs%3D&reserved=0)
  - zybo vivado block diagram for linux: [https://github.com/Digilent/Zybo-base-linux](https://github.com/Digilent/Zybo-base-linux)
  - zybo petalinux readme: [https://github.com/Digilent/Petalinux-Zybo/blob/master/README.md](https://nam02.safelinks.protection.outlook.com/?url=https%3A%2F%2Fgithub.com%2FDigilent%2FPetalinux-Zybo%2Fblob%2Fmaster%2FREADME.md&data=05%7C02%7Cathakka5%40stevens.edu%7C06812fe230264ed3962408dd31b7deb4%7C8d1a69ec03b54345ae21dad112f5fb4f%7C0%7C0%7C638721387968905869%7CUnknown%7CTWFpbGZsb3d8eyJFbXB0eU1hcGkiOnRydWUsIlYiOiIwLjAuMDAwMCIsIlAiOiJXaW4zMiIsIkFOIjoiTWFpbCIsIldUIjoyfQ%3D%3D%7C0%7C%7C%7C&sdata=Q8y5hG9brlBGljoHQ18exGq8TOpfti3NOkVIO5L7qmE%3D&reserved=0)
  - meta-sdr is an openembed layer that can be used in yocto: [https://github.com/balister/meta-sdr](https://nam02.safelinks.protection.outlook.com/?url=https%3A%2F%2Fgithub.com%2Fbalister%2Fmeta-sdr&data=05%7C02%7Cathakka5%40stevens.edu%7C06812fe230264ed3962408dd31b7deb4%7C8d1a69ec03b54345ae21dad112f5fb4f%7C0%7C0%7C638721387968922324%7CUnknown%7CTWFpbGZsb3d8eyJFbXB0eU1hcGkiOnRydWUsIlYiOiIwLjAuMDAwMCIsIlAiOiJXaW4zMiIsIkFOIjoiTWFpbCIsIldUIjoyfQ%3D%3D%7C0%7C%7C%7C&sdata=NyxOfPHhtRJvP6XO%2BjfLj9UrA6ejQ1CjZbCYglgAaaU%3D&reserved=0)
  - adding layer to rootfs: [https://adaptivesupport.amd.com/s/question/0D52E00006iHveUSAS/add-metalayer-to-petalinux-project?language=en_US](https://nam02.safelinks.protection.outlook.com/?url=https%3A%2F%2Fadaptivesupport.amd.com%2Fs%2Fquestion%2F0D52E00006iHveUSAS%2Fadd-metalayer-to-petalinux-project%3Flanguage%3Den_US&data=05%7C02%7Cathakka5%40stevens.edu%7C06812fe230264ed3962408dd31b7deb4%7C8d1a69ec03b54345ae21dad112f5fb4f%7C0%7C0%7C638721387968938854%7CUnknown%7CTWFpbGZsb3d8eyJFbXB0eU1hcGkiOnRydWUsIlYiOiIwLjAuMDAwMCIsIlAiOiJXaW4zMiIsIkFOIjoiTWFpbCIsIldUIjoyfQ%3D%3D%7C0%7C%7C%7C&sdata=nazfIPzQcm4oh4HWOTEdLrKM%2BGN0GBJ6Eky2T%2FrE1LY%3D&reserved=0)
  - Very similar project to mine, includes device drivers for handling PS-PL communication and custom GNU Radio blocks for Zynq-based architectures: [https://wiki.gnuradio.org/index.php/Zynq](https://wiki.gnuradio.org/index.php/Zynq)
- Milestones
  - Milestone 1 (Linux):
    - Build a linux image and get it to run on the Zybo. Familiarize yourself with the Petalinux toolkit and try compiling and running a simple hello world on the Zybo’s operating system.
      - Completed, refer to documentation.
    - Learn how the internals (Yocto) works, and include the meta-sdr layer to the linux image. Try running gnu-radio, and confirm the included libraries appear when this linux image is booted.
  - Milestone 2 (HackRF Comm.):
    - Initialize the USB OTG peripheral, and try and receive I/Q data on linux.
    - Write a C interface that leverages the HackRF API to get a stream of I/Q Data
  - Milestone 3 (Programmable Logic):
    - Set up an AXI connection between PL and PS, try to loopback some data to confirm its working.
    - Try passing the I/Q data through AXI to PL, and then process it with some simple FIR kernel, and pass it back.
  - Milestone 4 (PS-PL-SDR Framework):
    - Develop some kind of framework that can be used in the future on other SoC architectures, other SDRs, etc. This 
- Questions
  - What should the framing be for the project? Open source contribution, low cost solution for SDR prototyping, educational purpose?
    - it can both a lost cost solution for SDR protoyping, and **for teaching how to run linux on embedded targets/use programmable logic for RF tasks**
  - Professor Lu said to reference papers in our proposal and future papers, how does this work? Am I supposed to be building off of previous research or is it ok to do something completely from scratch like this?
    - Reference things related to parts of the project like PetaLinux or the boards or anything adjacent to the problem being solved. Or reference importance of parallel programming platform in signal processing
    - Google scholar, get recent stuff, use overleaf.
  - Any design considerations/changes I should make at first glance? Nice to have features? What should end demo look like?
    - 
  - A lot of papers referenced for the class have some kind of metric, what metrics can I measure on a project like this?
    - cost
    - embedded system performance metrics (power usage, system clocks used)
- Concerns
  - Old Petalinux and Yocto versions cause a ton of version conflicts/dependency issues. Could result in trouble when trying to config anything else.
- Documentation
  - Getting Linux on the Board
    - Tool Versions, Operating Systems, Etc.
      - Ubuntu 22.04
      - ZYBO (legacy) with Zynq XC7Z010-1CLG400C
      - Petalinux 2017.4 (Yocto Project 2.2 Morty)
    - Using [https://github.com/Digilent/Petalinux-Zybo/blob/master/README.md](https://github.com/Digilent/Petalinux-Zybo/blob/master/README.md) as reference during initial compilation of linux image
    - First step is to get necessary libraries as directed by guide
      - ```
sudo -s

apt-get install tofrodos gawk xvfb git libncurses5-dev tftpd zlib1g-dev zlib1g-dev:i386 libssl-dev flex bison chrpath socat autoconf libtool texinfo gcc-multilib libsdl1.2-dev libglib2.0-dev screen pax 

reboot
```
    - Then create install directory for petalinux and adjust permissions
      - ```
sudo -s
mkdir -p /opt/pkg/petalinux
chown <your_user_name> /opt/pkg/
chgrp <your_user_name> /opt/pkg/
chgrp <your_user_name> /opt/pkg/petalinux/
chown <your_user_name> /opt/pkg/petalinux/
exit
```
    - Next download petalinux 2017.4 installer from [https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/embedded-design-tools/archive.html](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/embedded-design-tools/archive.html)
    - Then run the installer
      - ```
cd ~/Downloads
chmod +x ./petalinux-v2017.4-final-installer.run
./petalinux-v2017.4-final-installer.run /opt/pkg/petalinux
```
    - I ran into issues here since the installer was made with python2 and wants Ubuntu 16. I am running Ubuntu 22, which calls python2 "python2" instead of "python". Fix was found at [https://stackoverflow.com/questions/62980416/python-error-when-installing-xilinx-petalinux-on-ubuntu-20-04](https://stackoverflow.com/questions/62980416/python-error-when-installing-xilinx-petalinux-on-ubuntu-20-04)
      - First install python2
        - ```
sudo apt-get install python2
```
      - Then add a fake entry for "python". Create a backup of the status file in case something happens. The fake entry to add and path can be found below.
        - ```
cd /var/lib/dpkg
cp ./status ./status.backup
vi ./status


Add this Entry to "status":

Package: python
Status: install ok installed
Maintainer: Fake Entry <fake@example.com>
Architecture: all
Version: 2.7.17
Description: fake package for petalin

```
    - Another issue with python is that the binary is called python2 but petalinux calls "python". To fix this add a quick symlink.
      - ```
sudo ln -sf /usr/bin/python2 /usr/bin/python

```
    - Now launch a new terminal and source the Petalinux environment
      - ```
source /opt/pkg/petalinux/settings.sh

```
    - Now we need to get "Petalinux-Zybo" repository for Digilent
      - ```
git clone [https://github.com/Digilent/Petalinux-Zybo/tree/master](https://github.com/Digilent/Petalinux-Zybo/tree/master)
```
    - Navigate to Petalinux-Zybo/Zybo (root of petalinux project) and run the build command
      - ```
petalinux-build
```
    - The first error I got from building was a utf-8 locale error in python. To solve this a certain function from the yocto install must be changed. Path may vary but use the path in the code block below as guidance when navigating through petalinux install directory. The actual code change can be found in this Xilinx forum: [https://adaptivesupport.amd.com/s/question/0D52E00006hpjH7SAI/petalinux-build-fails-with-locale-errors-how-to-disable-locale-checks?language=en_US](https://adaptivesupport.amd.com/s/question/0D52E00006hpjH7SAI/petalinux-build-fails-with-locale-errors-how-to-disable-locale-checks?language=en_US)
      - ```
vi /opt/pkg/petalinux/components/yocto/source/arm/buildtools/sysroots/x86_64-petalinux-linux/usr/lib/python3.5/
/locale.py
```
    - The next error I ran into was a GCC version issue. I thought it was worth trying an older version of GCC, which worked.
      - ```
sudo apt install gcc-9
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 9
gcc --version
```
    - The next issues were "do_fetch" errors that occurred when trying to access "private" Digilent git repositories. This is a known issue, and a patch was launched within the "Digilent-Apps" submodule (commit 06ec725) within the petalinux-zybo repository. The initial forum where I saw this was found at [https://forum.digilent.com/topic/25425-old-zybo-project-with-20174-petalinux-tools-github-do_fetch-fails/](https://forum.digilent.com/topic/25425-old-zybo-project-with-20174-petalinux-tools-github-do_fetch-fails/).
      - ```
cd ./Petalinux-Zybo/Zybo/project-spec/meta-user/recipes-apps/digilent-apps/
git checkout 06ec7256376eba3010fd8dbf18a3e8da1327e3b5
```
    - The next issue is an issue in some Yocto recipes checking out the Digilent U-Boot repository. I found a similar issue where the fix was downloading this repo manually, and the fix can be found at [https://forum.digilent.com/topic/22104-u-boot-digilent-fetching-error-building-petalinux-zybo-z7-20-bsp-project/](https://forum.digilent.com/topic/22104-u-boot-digilent-fetching-error-building-petalinux-zybo-z7-20-bsp-project/).
      - ```
cd ./Petalinux-Zybo/Zybo/components/
mkdir ext_sources
cd ext_sources
git clone https://github.com/Digilent/u-boot-digilent.git
cd u-boot-digilent
sudo apt-get install libncursesw5
petalinux-config
Linux Components Selection->u-boot->Select ext-local-src
Linux Components Selection->External u-boot local source settings->Enter the path "${TOPDIR}/../components/ext_sources/u-boot-digilent"
```
    - Now, I was able to run petalinux-build and have a successful build. Images can be found at Petalinux-Zybo/Zybo/images/linux/
    - Finally you must package the build artifacts. Go to the root of your petalinux project directory for this.
      - ```
petalinux-package --boot --force --fsbl images/linux/zynq_fsbl.elf --fpga images/linux/system_wrapper.bit --u-boot

```
    - To allow the root filesystem to live in our SD card instead of memory, we need to do some config as per the Digilent guide.
      - ```
petalinux-config
Image Packaging Configuration -> Root filesystem type -> Select SD

```
    - Then we must change the "bootargs" line in a yocto config file.
      - ```
vi Petalinux-Zybo/Zybo/project-spec/meta-user/recipes-bsp/device-tree/files/system-user.dtsi 

Replace bootargs line with 
bootargs = "console=ttyPS0,115200 earlyprintk uio_pdrv_genirq.of_id=generic-uio root=/dev/mmcblk0p2 rw rootwait";

```
    - Now **build and package** with petalinux-build and petalinux-package again
    - Then plug your microSD card into a USB adapter and plug this into your machine. Run "sudo gparted" if gparted is installed, otherwise install it. Once in gparted, follow below steps for formatting drive correctly.
      - First in the top right make sure you are working with the correct drive, do NOT do anything to your main hard drive.
      - Then, unmount all partitions that are mounted in your SD card.
      - Then delete all partitions, this should make gparted show your drive as completely unallocated.
      - Now, click on "Partition" -> "New"
      - Enter 512 MiB as the space of the partition, and select the formatting as fat32.
      - Then make another new partition, and enter 3000 MiB as the space of the partition and format it as ext4.
      - Exit gparted, and run "lsblk". Find your drive in the list of devices, and confirm that it has two partitions sdX1 and sdX2 that match your size and formatting (X can be any letter usually a). Keep track of these sdX names and their corresponding partitions.
      - Now mount the fat partition with temp folders.
        - ```
mkdir fatFormat
sudo mount /dev/sdX1 ./fatFormat
cp Petalinux-Zybo/Zybo/images/linux/BOOT.bin ./fatFormat
cp Petalinux-Zybo/Zybo/images/linux/image.ub./fatFormat
```
      - now we must copy the root file system to the second partition. again please make sure you know the /dev/sdX location and partition number.
        - ```
sudo unmount /dev/sdX2
sudo dd if=images/linux/rootfs.ext4 of=/dev/sdX2
sync
sudo resize2fs /dev/sdX2
sync
```
      - now eject the sd card from the reader, plug it into the Zybo, and move the boot jumper from JTAG to SD.
      - open a terminal, install picocom, and run following command to get uart shell
        - ```
picocom -b 115200 /dev/ttyUSB1
```
      - now plug your zybo into your laptop and you should see bootloader start info and eventually be prompted by a root shell for the linux OS running on your Zybo!
        - 
  - Include libhackrf and rtl-sdr packages to rootfs
    - To compile libhackrf, first we need meta-sdr on the "rocko" or Yocto Project 2.4 branch. 
      - ```
cd ./Petalinux-Zybo/Zybo/project-spec
git clone https://github.com/balister/meta-sdr.git
cd meta-sdr
git checkout rocko
```
    - Then, we want to grab the latest recipes for libhackrf and rtl-sdr and put them in our existing Rocko version of meta-sdr.
      - ```
git checkout kirkstone
git checkout rocko
cd recipes-support
rm -r libhackrf
rm -r rtlsdr
git checkout kirkstone -- libhackrf
git checkout kirkstone -- rtlsdr
```
    - Now, we must amend the rtl-sdr bitbake file for syntax purposes. Change "do_install:append" to "do_install_append".
    - Now we need meta-qt4 which is a dependency of meta-sdr
      - ```
cd /opt/pkg/petalinux/components/yocto/source/arm/layers/
git clone git://git.yoctoproject.org/meta-qt4
cd meta-qt4
git checkout morty
```
    - Then, we need to add the meta-sdr and meta-qt4 layers to the Yocto layers.
      - ```
petalinux-config
Yocto Settings -> User Layers -> user layer 0
Enter the Path: ${PROOT}/project-spec/meta-sdr
Yocto Settings -> User Layers -> user layer 1
Enter the Path: /opt/pkg/petalinux/components/yocto/source/arm/layers/meta-qt4
```
    - Then, you should be able to try building rtl-sdr and libhackrf
      - ```
petalinux-build -c rtl-sdr
petalinux-build -c libhackrf
```
    - If this succeeds, now we can add it to rootfs.
      - ```
vi ./Zybo/project-spec/configs/rootfs_config
Add the following:
CONFIG_libhackrf=y
CONFIG_rtl-sdr=y
```
      - ```
vi ./Zybo/project-spec/meta-user/recipes-core/images/petalinux-image.bbappend
Add the following:
IMAGE_INSTALL_append = " libhackrf"
IMAGE_INSTALL_append = " rtl-sdr"

```
    - Confirm that libhackrtf and rtl-sdr are added to the user packages by running "petalinux-config -c rootfs" and navigating to "user packages". You should see libhackrf and rtl-sdr listed and selected as user packages.
    - Now we can build and package the whole image. Flash the new image onto the board.
    - It is recommended to buy an external power supply to the board so that it can provide enough amperage to the HackRF when plugged into the USB peripheral. Speaking of USB peripheral, make sure JP1 is shorted on the board to enable master mode on the USB. Also, make sure the power supply jumper is set to "WALL". Then plug in your HackRF and power supply and power that sucker on. You should see LEDs lighting up.
    - Now try running "hackrf_info" to get terminal output like below
      - 
  - Write C API to communicate with the board
    - Libhackrf is the API written for the host to communicate with the HackRF. it can be found in the HackRF official repo at /host/libhackrf/src/. use this [link](https://github.com/greatscottgadgets/hackrf/tree/43e6f99fe8543094d18ff3a6550ed2066c398862) to get to the specific commit being used in our project
    - To use the library, first, we need to create a file called "hackrf.h" in /usr/include. You can copy the hackrf.h text from the [official repo](https://github.com/greatscottgadgets/hackrf/blob/43e6f99fe8543094d18ff3a6550ed2066c398862/host/libhackrf/src/hackrf.h) into this file.
    - Next we need to make a symlink between [libhackrf.so](http://libhackrf.so/) and libhackrf.so.0.5.0. Use the following commands to do this:
      - ```

ln -s /usr/lib/libhackrf.so.0.5.0 /usr/lib/libhackrf.so
ldconfig

```
    - Next, we can make a test file that includes the simple hackrf_info source code and try and compile to make sure we can use the hackrf libs. This code can be found [here](https://github.com/greatscottgadgets/hackrf/blob/43e6f99fe8543094d18ff3a6550ed2066c398862/host/hackrf-tools/src/hackrf_info.c). Make sure you replace the reference to "TOOL_VERSION" in the code as this is a variable generated by cmake, which we are not using.
    - Once you have your test file with the hackrf_info source code, try compiling with the following command:
      - ```
gcc hackRFtest.c -o hack -I/usr/include -L/usr/lib -lhackrf
```
    - This should create an executable, and running it should yield terminal output similar to below
      - 
    - After taking the API calls needed from hackrf_transfer.c, writing I?Q data to a file, and then using an SCP call to transfer the raw I/Q file, a rudimentary interface can be written that will store a fixed amount of samples and send it over using the SSH tunnel. Find the command and a spectrogram from MATLAB of captured data in the FM radio band.
      - ```
scp -oHostKeyAlgorithms=+ssh-rsa -oPubkeyAcceptedAlgorithms=+ssh-rsa root@192.168.1.161:/home/root/out.txt /home/somalianpirate/Documents/petalinux/hackRFTest/hackrf.txt

```
      - 

FROM ubuntu:16.04

# Install host deps + Petalinux 2017.4 at /opt/pkg/petalinux

# Then mount your project at runtime:

# docker run -v ~/Documents/petalinux/Petalinux-Zybo:/project -it <image>

