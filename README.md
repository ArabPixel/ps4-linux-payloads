# ![PS4](https://img.shields.io/badge/-PS4-003791?style=flat&logo=PlayStation) Linux Payloads for FW 5.05 - 12.02
(Southbridge: Aeolia & Belize(2) & Baikal)

**Linux-Payloads** kexec for PlayStation 4.

The host with precompiled Linux payloads only works with GoldHEN v2.4b18.5/v2.4b18.6 BinLoader. Just open the web browser and cache the host—it will also work offline too.
https://ps4boot.github.io [▶️ click](https://www.youtube.com/watch?v=T3rXMWw6nIM)

you’ll find Linux payloads for your firmware, along with some extra payloads. The rest are already included in GoldHEN.

 
# Supported Firmwares

*   FW 5.05 ✅
*   FW 6.72 ✅
*   FW 7.00 / 7.02(?) ✅
*   FW 9.00 ✅ 
*   FW 9.03 / 9.04 ✅ 
*   FW 9.50 / 9.51 / 9.60 ✅ 
*   FW 10.00 / 10.01 ✅ 
*   FW 10.50 / 10.70 / 10.71 ✅ 
*   FW 11.00 ✅ 
*   FW 11.02 ✅ 
*   FW 11.50 / 11.52 ✅
*   FW 12.00 / 12.02 ✅ 


## New
- Automatic boot files placement – The kernel (bzImage) and initramfs.cpio.gz are now automatically copied to /data/linux/boot on the internal drive from the external fat32 partition. Why? No external drive is needed to boot into the rescue shell, only first time.

- RTC time passed to initramfs – The current time from OrbisOS is added to the kernel command line (time=CURRENTTIME), ensuring the correct time is set at boot instead of defaulting to 1970, even if the RTC hardware cannot be read directly. Why? why not. but you need a prepared initramfs that reads the Time from the cmdline and set the time. 


## Info 
the internal path by default is at: ``/data/linux/boot``  
the rest is coming from the initramfs.cpio.gz init configuration
so you can go into the rescue shell without a usb stick just upload the ``bzImage`` and ``initramfs.cpio.gz`` over ftp to your PS4 Drive
``/data/linux/boot/[bzimage,initramfs.cpio.gz]`` 
and of course it will work too with a USB / HDD Drive.  USB have highest prio so if a USB is connected he will  use this bzImage and initramfs.cpio.gz from there 

bootargs.txt you can also add a textfile for changing the cmdline.
vram.txt with this you can change the vram over a textfile.

## Note 
* With new GoldHEN Version v2.4b18.5/v2.4b18.6 use .elf`s Files instead of .bin it works better 100% Success. 

* Don`t use PRO Payloads for Phat or Slim. 

* if you need UART just add this to the cmdline i have disabled .... just for now on newer Kernel it doesnt work.

Aeolia/Belize: ``console=uart8250,mmio32,0xd0340000``

Baikal: ``console=uart8250,mmio32,0xC890E000``


## Compile
    git clone https://github.com/ps4boot/ps4-linux-payloads
    cd ps4-linux-payloads/linux
    make


## Credits
* marcan, shuffle2, eeply, rancido, valeryy, ethylamine, Joonie86 (Linux)
* sleirsgoevy (for the script and better exploit FW 672) 
* AlAzif / KiwiDog / Specter / Celesteblue / ChendoChap / zecoxao / SocraticBliss / ctn123 (Exploit and Fun Stuff for the Console)
* bestpig / EchoStretch / EinTim23 / tihmstar (Offsets)
* others ... 

