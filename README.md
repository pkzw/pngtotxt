# pngtotxt
Console program to convert a PNG image (e.g. a QR-Code) to text using block graphic symbols.

The primary goal to develop this program has been to support [Aqbanking](https://www.aquamaniac.de/rdm/) console tools like `aqhbci-tool4` and `aqbanking-cli` in displaying QR-Code-based photoTANs without the need of invoking a graphical UI.

Table of Contents
-----------------

  - Overview
  - Dependencies
  - Invocation/running
  - Internationalization
  - Building & Installing
  - Supported Platforms
  - Getting the Source via Git


Overview
--------

pngtotxt is a tiny tool to convert a PNG image to text-based block graphic.

Home Page:
https://github.com/pkzw/pngtotxt


Dependencies
------------

pngtotxt depends on
- libiconv (see https://www.gnu.org/software/libiconv/) and
- libpng (see http://www.libpng.org/pub/png/libpng.html).

Running
-------

The following example will convert a QR-Code in file [qrcode.png](qrcode.png) to a text file [qrcode.txt](qrcode.txt):

```sh
$ pngtotxt qrcode.png > qrcode.txt
```
Performing a SEPA bank transfer with aqbanking-cli displaying photoTAN as console output:

```sh
$ aqbanking-cli --opticaltan=pngtotxt sepatransfer --iban=...
```

See program help for more information:

```sh
$ pngtotxt --help
Usage: pngtotxt [OPTIONS] [CTY] IFN
This program converts a PNG image to text using block graphic symbols.

Options:
  -a, --append      Append text to output file
  -h, --help        Display detailed help information
  -o, --output=OFN  Output file name (default is standard output)
  -v, --version     Display version information

CTY is the content-type of the input file (Default is 'image/png').
IFN is the name of the input file (required).
OFN is the name of the output file.
```


Internationalization
--------------------

The program is not yet internationalized and 
thus available in English language only. 


Building & Installing
---------------------

pngtotxt uses CMake to handle the build process.

The follwing example shows how to build the program on a Debian-based system:

```sh
# Install prerequisites
sudo apt update
sudo apt install build-essential cmake git libpng-dev
# Clone project from GitHub
mkdir -p ~/git
cd ~/git
git clone https://github.com/pkzw/pngtotxt
# Configure build
mkdir -p ~/build/pngtotxt
cd ~/build/pngtotxt
cmake ~/git/pngtotxt
# Build project
make
# Install
sudo make install
```
For building on Windows platform see [README.W32.md](README.W32.md)

Supported Platforms
-------------------

pngtotxt has been tested to work with the following operating systems:
- GNU/Linux  -- x86
- Windows    -- x86

pngtotxt can probably be made to work on any platform for which the dependencies
can be fulfilled and a C compiler is available, given sufficient expertise
and effort.


Getting Source with Git
-----------------------

You can browse the code at https://github.com/pkzw/pngtotxt. 
Clone URIs are on that page, or if you have a Github account
you can fork it there.

Thank you.
