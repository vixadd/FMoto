# Motorola Moto Z Hardware Modifications Capstone
This is the CPEN Capstone for the Moto Mod Project being worked on. Fall/Spring 2017
The main theme of this project is based on the topic of Consumer Electronics. Being able to sell new innovative advancements in the world of hardware modifications for the Motorola Moto Z.

### Project - [![Build Status](https://travis-ci.org/vixadd/Moto.Mod.MDK.Capstone.svg)](https://travis-ci.org/vixadd/Moto.Mod.MDK.Capstone)

## Introduction
The world of smart phones has come a long way in recent years. Not only in the advancement of the software that allows us to use it, but in the hardware that makes it function. As far as hardware goes, there is a vast array of innovative, and research opportunities in the world of hardware development. One such development is the synthesis of hardware modifications, and extensions. In the current market there is a common dynamic that happens with how external devices are used in systems. Usually they connect through some form of wired or wireless connection. From a user experience standpoint, this is not easy to use. Consumers desire ease of use. Attachments for phones that add functionality to a phone through the hardware itself is the solution to this problem.

The project for this capstone involves an example of hardware extensions which can be used to transmit FM Radio frequencies out from the phone. FM Transmitters on the market currently have bluetooth connections with their phones. The purpose of this project is to cut out the bluetooth or wired portion, and show the potential of hardware extensions in smartphones. The potential of this project is to demonstrate the hardware extension’s capabilities, as well as its future potential. Appliances used in everyday households can be operated with central devices. Tracking, and operating Cars, Computers, and Dishwashers with a smartphone no longer becomes something wireless, but a simple attachment that connects to your phone through hardware. Hardware interfacing holds many applications. Applications that fall under the categories of entertainment, appliances, and home automation. In the Internet of Things age, it is crucial to come up with new ways to interface with hardware that connects us to the rest of the world. With a new perspective on hardware interfacing, comes new innovations in the market of consumer electronics. 


![alt text](https://static1.squarespace.com/static/5715eee9b09f95a12b44e0ed/t/5756e708a3360cdbab7d5bd8/1465313491952/hw-perfref-diagram-1-2.png?format=1000w "Moto Schematic Perforated Board")

As this potential market is only just starting to bud, it is vital that we as engineers look at it’s capabilities technologically, as well as economically. The usage of this technology, spanning through several applications allows for a broad market. The system of these extensions being predicated on ease of use, and their ability to be used in hardware that connects people in different Internet of Things applications. If this type of system is feasible in all respects, then promulgating the idea of hardware extensions for mobile devices of everyday use will be a profitable venture. Not just for engineers, but for consumers as well.


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
