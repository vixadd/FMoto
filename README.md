# FMoto - The First Motorola FM Transmitter Modification
This is the CPEN Capstone for the Moto Mod Project being worked on. Fall/Spring 2017
The main theme of this project is based on the topic of Consumer Electronics. Being able to sell new innovative advancements in the world of hardware modifications for the Motorola Moto Z.

### Project - [![Build Status](https://travis-ci.org/vixadd/FMoto.svg?branch=master)](https://travis-ci.org/vixadd/FMoto)

## Setup
For setting up the firmware. Follow the guide provided by Element14's step by step proceedure.
This is time consuming but lays the groundwork for future development should you be developing at a specific electrical station.

https://www.element14.com/community/groups/moto-mods/blog/2017/04/22/moto-mods-developer-part-1-getting-started-virtual-machine-setup-and-linux-install

## Developer Instructions
To start off with, all developers need to initialize and set up the firmware repositories required for successful development.
Run the following from the root of the repository.
```bash
$ export BUILD_TOP=`pwd`
$ git submodule init
$ git submodule update --recursive

# ---------- Install Requirements --------------

$ sudo apt-add-repository ppa:webupd8team/java
$ sudo apt-get update
$ sudo apt-get install oracle-java8-installer

# ---- Confirm Java Version "1.8.0_121-b13" ---- 
# -- Install latest Version of Android Studio --
```
The latest version of android studio allows us to interact with the firmware, in order to control the Hardware Modification.
Be sure to import the required mod-lib packages from Motorola Mobility's Website.


```bash
$ sudo apt-get install -y git gperf flex bison libncurses5-dev gcc-arm-none-eabi python-pip
$ sudo pip install pyelftools
$ sudo apt-get install -y libusb-1.0-0-dev libftdi-dev libtool autoconf texinfo

# --- Setup OpenOCD for Hardware Development ---
$ cd openocd
$ ./bootstrap
$ ./configure --prefix=/usr/local
$ make
$ sudo make install
$ cd ..
```
Since OpenOCD uses the USB, we need to grant permissions to your linux username so it can use the FTDI. Enter the commands below in terminal:

```bash
$ id -u -n # Can also use 'echo $User'
$ sudo -s
# echo 'SUBSYSTEMS=="usb", ATTRS{idVendor}=="0403", MODE="0666", OWNER="<user name>"'  >> /etc/udev/rules.d/20-ftdi.rules
# udevadm control --reload
# exit
```
The MDK provides utilities for using GDB, we will install with the commands below:
```bash
$ sudo apt-get install -y libexpat1-dev zlib1g-dev guile-2.0-dev python2.7-dev
$ cd ~
$ wget http://ftp.gnu.org/gnu/gdb/gdb-7.11.tar.gz
$ tar -zxf gdb-7.11.tar.gz
$ cd gdb-7.11
$ ./configure --prefix=/usr/local --program-prefix=arm-none-eabi- --target=arm-none-eabi --with-python --with-guile
$ make
$ sudo make install
$ cd $BUILD_TOP
```
Build kconfig-mconf, Nuttx uses the same configuration editor that is used to build the Linux Kernel.
```bash
$ cd $BUILD_TOP/nuttx/misc/tools/kconfig-frontends
$ ./configure --enable-mconf --disable-nconf --disable-gconf --disable-qconf
$ make
$ sudo make install
$ sudo ldconfig
```
Configure NuttX for new build
```bash
$ export PATH=$PATH:$BUILD_TOP/manifesto:$BUILD_TOP/bootrom-tools
$ cd $BUILD_TOP/nuttx/nuttx
$ make distclean
$ cd ./tools
$ ./configure.sh hdk/muc/base_unpowered # use for devices with no external battery.
```
At this point the configs/hdk/muc/base_unpowered/defconfig will be copied to ./nuttx/nuttx/.config and the setenv.sh and Make.defs file from that same directory will be copied up to ./nuttx/nuttx.

```bash
$ cd $BUILD_TOP/nuttx/nuttx
$ make
```
The firmware output is located in the <b>nuttx.bin</b>, <b>nuttx.hex</b> and <b>nuttx.tftf</b> files in the ./nuttx/nuttx
These are to be directly loaded into the 

For this project, I decided to flash the firmware using Motorola's MDK Utility Application on Android, instead of the OpenOCD route, flashing bootloader and firmware.
I configured NuttX by doing the following:
```bash
$ cd $BUILD_TOP/nuttx/nuttx
$ make menuconfig

```
Once executed, a text based menu system in the terminal will pop up showing you the config menu. 
Navigate to the “Device Drivers” menu and make sure the parameters below are checked. Leave all other settings how they are, when finished, exit.

```

```

These settings are crucial. They spawn the Hardware Manifest File that tells Android which signals it's calling on.
In this case the Board being used draws mainly I2C and I2S signals from the MuC and the MHB respectively.

The Si4713 acts as an FM Transmitter, being a slave to the MuC's master SDA and SCL I2C signals on the board. The UDA1334 takes I2S signals from the Moto Highspeed Bridge (MHB) and pipes left and right audio through it's OUTL and OUTR pins. This is signaled to the FM Transmitter who then takes the audio and sends it out over FM bands.

![si4713](https://user-images.githubusercontent.com/9141655/38448629-96c1d5b2-39d3-11e8-9318-8f572005288a.png)
![uda1334](https://user-images.githubusercontent.com/9141655/38448631-9b011098-39d3-11e8-9ce0-36575dc5adf0.png)

