scos
===
This is a simple operating system, which I wrote from scratch in my high school in order to learn how an OS runs on a machine. It is partly a mimic of Linux 0.11. It contains a bootloader, a kernel, a shell and some tools that can run on it for demonstration.

* It runs in x86 protected mode(32-bit), the kernal runs in ring 0, and other programs runs in ring 3.
* It partially supports memory paging, with page mapping from vitual address space to physical address space and allocating physical pages on demand.
* It (only) supports floppy disk controller and FAT-12 system. (the whole OS is installed in a 1.44 MB floppy disk)
* It supports DMA to read/write the disk.
* It supports preemptive multi-tasking, and each process(no thread) has 64MB memory (all process sharing the same system page table) and equal CPU time slice (a simple implementation).
* It cantains a system API on which the shell and other tools base. It is called through software interruption.

build
---
The `Makefile` was originally wrote for `djgpp` on Windows, and I adapt it to OS X. There are still some problems now:
* `ld` from `djgpp`(GNU) and from `xcode` are quite different. Our binaries need the `text section` being put at the begining of the file and specified address origins, but Apple's `ld` does not support `link script` and raw binary format, it always puts a head before anything else in the file. For now, we use `-preload -segaddr __TEXT xxxxx` flags to output a simplest file structure, and just use `dd` to cut off the head.
* After El Capitan, `Disk Utility` seems not to support loading empty image file any more (file created by `dd bs=1024 count=1440 if=/dev/zero of=disk.img`) and images created by `hdiutil` always contains an extra sector at the begining of the disk (the boot sector is moved to the second sector). So I cannot find a way to create a standard FAT-12 disk image right now. For now, you could use any 1.44MB FAT-12 floppy disk image you have or found on the Internet, backup and format them.

To build is quite simple:

make sure that you have `nasm` installed
```
# use GNU-toolset
make
# use OSX toolset
make OSX_TOOLSET=1
```
There are 5 binary files built, `boot.bin`, `kernel.bin`, `shell.bin`, `edit.bin`, `demo.bin`. 
1. Mount the disk image on your system
2. Use the `boot.bin` to replace the boot sector of the disk. 
```
# replace '/dev/fda1' with the filename of your mounted image
dd bs=512 count=1 if=boot.bin of=/dev/fda1
```
3. Put other binaries files into the disk directly through `File Explorer` or `Finder`.

Caution: if you use the OSX toolset, please remember to cut off the head part from the files:
```
# our code and other sections start after 8192 bytes from the begining of the file
dd bs=8192 skip=1 if=old.bin of=new.bin
```
