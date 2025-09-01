# FIRST-TIME SETUP
This Project uses the 'nob.h' build tool and thus is required to be compiled by hand once!
For this, i've provided a small setup script.<br>
Simply run:
```bash
$ ./setup.sh
```
# BUILDING THE PROJECT
To build the project, execute the following command inside the project root
```bash
$ ./nob
```
# RUNNING THE PROJECT
To run the project, execute the following command inside the project root
```bash
$ ./nob run
```
# CLEANING
To clean the project, execute the following command inside the project root
```bash
$ ./nob clean
```
# DEPENDENCIES
before compiling, make sure you have the following dependencies installed:
- gcc for building (clang support soon)
- Vulkan-SDK
- X11 SDK
<br>
'nob' is meant to be lightweight and thus does not support automatic dependency management.<br>
I'm not planing on adding similar features myself.<br>

<br>

Installation with pacman (Arch-based)
```bash
# pacman -S vulkan-headers vulkan-validation-layers gcc libx11
```
Installation with apt (Debian-based)
```bash
# apt install libvulkan libvulkan-dev vulkan-validationlayers-dev spriv-tools gcc libx11-dev
```
Installation with xbps (Void)
```bash
# xbps-install -S vulkan-headers vulkan-validation-layers gcc libX11-devel
```
