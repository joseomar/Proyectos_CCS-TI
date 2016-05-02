FAT16 Freescale library for SD/SDHC cards (http://code.google.com/p/sdfatlite)
------------------------
Copyright (c) 2012 by EQUISER 
This is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation.

Foreword: 
This project improves the "FAT Lite" library developed by Freescale and published in the DEMOFLEXISJMSD_DataLogger_V2 example. SDFatLite is an Freescale library that supports FAT16 file systems on standard and high capacity SD cards.

Characteristics:
- Only supports short 8.3 file names, but doesn't crash if there are long file names the card.
- Supports file creation, deletion, read, write and overwrite.
- Doesn't supports access to subdirectories, creation, and deletion of subdirectories (can only deal with files in the root/home directory).
- It is designed for 8/32 bits (ColdFire v1) Freescale MCUs.

Instructions:
Use of SDFatLite is illustrated by the example provided in the SDFatLite/examples directory, made for the Freescale MCF51JM128 MCU.

Bugs and feedback: 
Please emaill guillerosemberg@gmail.com or evolentini@gmail.com. Have fun!
