## Toolchain
This directory tree contains the utilities needed for compiling and downloading code (firmware) to the lego NXT. The toolchain is self-contained and you should not need any files outside this directory, except for the generic usb library.

## Requirments
The firmware toolchain requires the 32 bit version of the usb library:
```
sudo apt-get install libc6-i386
sudo apt-get install libusb-0.1-4:i386
```
If instead you have the 64-bit version, libusb i686, installed, make sure to install libusb i386 as well.
