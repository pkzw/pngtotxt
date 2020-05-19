# Build instructions for Windows


Building pngtotxt on Windows is possible using MSYS2 building platform.  

## Prerequisites

Install and update MSYS2 from [https://www.msys2.org](https://www.msys2.org). 

From an MSYS2 command prompt install the following packages

```sh
$ pacman -S --needed base-devel git mingw-w64-i686-gcc mingw-w64-i686-cmake 
```

for a 32-bit target architecture and

```sh
$ pacman -S --needed base-devel git mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake
```
 
for a 64-bit target architecture.

## Build instructions

1. **Download, build, and install libpng**

   You may download libpng sources from Sourceforge:	

    ```sh
    mkdir -p ~/build
    cd ~/build
    wget http://prdownloads.sourceforge.net/libpng/libpng-1.6.37.tar.gz
    tar -xvf ~/build/libpng-1.6.37.tar.gz
    cd ~/build/libpng-1.6.37
    make -f scripts/makefile.msys
    make -f scripts/makefile.msys install
    ```

2. **Clone, configure, build, and install current project**

   The following example will install the program to a staging directory in `~/destdir`. From there you may copy the installed artifacts to the final target directory (e.g. `C:\Program Files (x86)`).

    ```sh
    # Clone project from GitHub
    mkdir -p ~/git
    cd ~/git
    git clone https://github.com/pkzw/pngtotxt
    # Configure build
    mkdir -p ~/build/pngtotxt
    cd ~/build/pngtotxt
    cmake ~/git/pngtotxt/
    # Build project
    make
    # Install
    mkdir -p ~/destdir
    make DESTDIR=~/destdir install
    ```
