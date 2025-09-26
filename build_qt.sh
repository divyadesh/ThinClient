#!/bin/bash


source /opt/poky/4.0.27/environment-setup-armv8a-poky-linux

/opt/poky/4.0.27/sysroots/x86_64-pokysdk-linux/usr/bin/qmake ThinClient.pro

make
