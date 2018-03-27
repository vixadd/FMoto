
# FMoto - The First Motorola FM Transmitter Modification
This is the CPEN Capstone for the Moto Mod Project being worked on. Fall/Spring 2017
The main theme of this project is based on the topic of Consumer Electronics. Being able to sell new innovative advancements in the world of hardware modifications for the Motorola Moto Z.

### Project - [![Build Status](https://travis-ci.org/vixadd/Moto.Mod.MDK.Capstone.svg)](https://travis-ci.org/vixadd/Moto.Mod.MDK.Capstone)

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
