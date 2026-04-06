# ![PS4](https://img.shields.io/badge/-PS4-003791?style=flat&logo=PlayStation) Linux Payloads for FW 5.05 - 13.02
(Southbridge: Aeolia & Belize(2) & Baikal)

**Linux-Payloads** kexec for PlayStation 4.

The host with precompiled Linux payloads works well with GoldHEN's PayLoader. Just open the web browser and cache the host—it will also work offline too.
[PSFree-Enhanced](https://arabpixel.github.io/PSFree-Enhanced)

There's also a great [Linux setup guide by DionKill](https://dionkill.github.io/ps4-linux-tutorial/)

 
# Supported Firmwares

*   FW 5.05 ✅
*   FW 6.72 ✅
*   FW 7.00 / 7.01 / 7.02 ✅
*   FW 7.50 / 7.51 / 7.55 ✅ 
*   FW 8.00 / 8.01 / 8.03 ✅
*   FW 8.50 / 8.52 ✅
*   FW 9.00 ✅ 
*   FW 9.03 / 9.04 ✅ 
*   FW 9.50 / 9.51 / 9.60 ✅ 
*   FW 10.00 / 10.01 ✅ 
*   FW 10.50 / 10.70 / 10.71 ✅ 
*   FW 11.00 ✅ 
*   FW 11.02 ✅ 
*   FW 11.50 / 11.52 ✅
*   FW 12.00 / 12.02 ✅ 
*   FW 12.50 / 12.52 ✅
*   FW 13.00 ✅
*   FW 13.02(?) ✅

## Info 
The internal path by default is at: ``/data/linux/boot``, however it'll also check for ``/user/system/boot/`` in case it was not found.

the rest is coming from the initramfs.cpio.gz init configuration
so you can go into the rescue shell without a usb stick just upload the ``bzImage`` and ``initramfs.cpio.gz`` over ftp to your PS4 Drive:

``/data/linux/boot/[bzimage,initramfs.cpio.gz]`` 

or
``/user/system/boot/[bzimage,initramfs.cpio.gz]`` 

and of course it will work too with a USB / HDD Drive.  USB have highest priority. If a USB is connected, it will use the bzImage and initramfs.cpio.gz from there .

### vram.txt
Control VRAM size via a plain text file containing a number in **MB** (not GB).

| vram.txt value | VRAM allocated | Recommended Use |
|:-:|:-:|:---|
| `32`  | 32 MB | Headless Server (Maximum System RAM) |
| `64`  | 64 MB | Basic CLI / Light Background Tasks |
| `128` | 128 MB | Small Web Servers / Bot Hosting |
| `256` | 256 MB | Medium Servers / Minimal Desktop (No GUI apps) |
| `512` | 512 MB | Light Desktop / Multimedia (720p) |
| `1024` | 1 GB | Standard Desktop / Web Browsing (Default) |
| `2048` | 2 GB | Heavy Desktop / Light Gaming |
| `3072` | 3 GB | Gaming / Emulation (PS2, GameCube) |
| `4096` | 4 GB | High-End Gaming / Video Playback (1080p+) |

Default is 1024 MB (1 GB) if vram.txt is missing or invalid. Minimum is **32 MB**.

## Server Use (Low VRAM Payloads)

- **Why 32MB, 64MB, 128MB, 256MB, 512MB?** The idea behind these low VRAM payloads is for users who are repurposing the PS4 as a server for RAM and CPU intensive tasks. If you're just running server stuff headless, you don't need 1GB+ of memory wasted on the GPU. Using these payloads frees up that shared unified memory to be used as regular system RAM instead. 

- **How to use:** 
  - **Option 1:** Just run the payload with the desired VRAM size (e.g. `-32mb`, `-64mb`, etc.).
  - **Option 2:** Add the value (e.g. `32`, `64`, `128`, etc.) into your `vram.txt` file to set it manually. Don't use these if you plan on gaming or using heavy desktop graphics!
> Note: vram.txt takes priority. if `vram.txt` is present it'll ignore any other payload value you load and use the specified value from `vram.txt`

## Notes
* If you face a black screen, make sure to use a kernel from an up-to-date guide.
* Use .elf`s files instead of .bin whenever possible.
* if you need UART just add this to the cmdline i have disabled .... just for now on newer Kernel it doesnt work.

Aeolia/Belize: ``console=uart8250,mmio32,0xd0340000``

Baikal: ``console=uart8250,mmio32,0xC890E000``


## How to Compile
    git clone https://github.com/ArabPixel/ps4-linux-payloads
    cd ps4-linux-payloads/linux
    make


## Credits
* marcan, shuffle2, eeply, rancido, valeryy, ethylamine, Joonie86 (Linux)
* sleirsgoevy (for the script and better exploit FW 672) 
* AlAzif / KiwiDog / Specter / Celesteblue / ChendoChap / zecoxao / SocraticBliss / ctn123 (Exploit and Fun Stuff for the Console)
* bestpig / EchoStretch / EinTim23 / tihmstar / ArabPixel (Offsets)
* Contributors
* others ...


## Change log
- PS4 PRO, Southbridge and firmware version run-time detection [v24b](https://github.com/ArabPixel/ps4-linux-payloads/releases/tag/v24b)
  - No more separate Baikal or PRO payloads
  - firmware agnostic payload. AKA one payload works for all supported firmware versions

- Backup files path [v23](https://github.com/ArabPixel/ps4-linux-payloads/releases/tag/v23)
  - Looks for the files in ``/user/system/boot/``  if they weren't found in the default path ``/data/linux/boot/`` 

- Complete support for PS4 7.xx and 8.xx. [v23](https://github.com/ArabPixel/ps4-linux-payloads/releases/tag/v23)

- Sub-1GB VRAM payloads [v22](https://github.com/ArabPixel/ps4-linux-payloads/releases/tag/v22)
  - Added 128mb, 256mb etc.. payload sizes for PS4 used as a headless server where GPU memory is largely unused. Set `vram.txt` to `128`, `256` or `512` (MB) to use them.
  - **32 VRAM payloads are now supported.**

- PS4 12.5x, 13.0x Support. [v21.5](https://github.com/ArabPixel/ps4-linux-payloads/releases/tag/v21.5)

- Automatic boot files placement – The kernel (bzImage) and initramfs.cpio.gz are now automatically copied to /data/linux/boot on the internal drive from the external fat32 partition. Why? No external drive is needed to boot into the rescue shell, only first time.

- RTC time passed to initramfs – The current time from OrbisOS is added to the kernel command line (time=CURRENTTIME), ensuring the correct time is set at boot instead of defaulting to 1970, even if the RTC hardware cannot be read directly. Why? why not. but you need a prepared initramfs that reads the Time from the cmdline and set the time.

bootargs.txt you can also add a textfile for changing the cmdline.
