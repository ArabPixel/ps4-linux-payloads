/*
 * ps4-kexec - a kexec() implementation for Orbis OS / FreeBSD
 *
 * Copyright (C) 2015-2016 shuffle2 <godisgovernment@gmail.com>
 * Copyright (C) 2015-2016 Hector Martin "marcan" <marcan@marcan.st>
 *
 * This code is licensed to you under the 2-clause BSD license. See the LICENSE
 * file for more information.
 */

#ifndef FIRMWARE_H
#define FIRMWARE_H

#include "types.h"

//sizes eap_hdd_key
#define EAP_HDD_KEY_SIZE 0x20
#define EDID_SIZE	256

// sizes for liverpool
#define LVP_FW_CE_SIZE 8576
#define LVP_FW_ME_SIZE 16768
#define LVP_FW_MEC_SIZE 16768
#define LVP_FW_MEC2_SIZE 16768
#define LVP_FW_PFP_SIZE 16768
#define LVP_FW_RLC_SIZE 6144
#define LVP_FW_SDMA_SIZE 4200
#define LVP_FW_SDMA1_SIZE 4200
// sizes for gladius
#define GL_FW_CE_SIZE 8576
#define GL_FW_ME_SIZE 16768
#define GL_FW_MEC_SIZE 16768
#define GL_FW_MEC2_SIZE 16768
#define GL_FW_PFP_SIZE 16768
#define GL_FW_RLC_SIZE 8192
#define GL_FW_SDMA_SIZE 4200
#define GL_FW_SDMA1_SIZE 4200

#define MAX(x ,y) (((x) > (y)) ? (x) : (y))
#define MAX_FW_SIZE(engine) MAX(LVP_FW_ ## engine ## _SIZE, GL_FW_ ## engine ## _SIZE)

#define FW_CE_SIZE MAX_FW_SIZE(CE)
#define FW_ME_SIZE MAX_FW_SIZE(ME)
#define FW_MEC_SIZE MAX_FW_SIZE(MEC)
#define FW_MEC2_SIZE MAX_FW_SIZE(MEC2)
#define FW_PFP_SIZE MAX_FW_SIZE(PFP)
#define FW_RLC_SIZE MAX_FW_SIZE(RLC)
#define FW_SDMA_SIZE MAX_FW_SIZE(SDMA)
#define FW_SDMA1_SIZE MAX_FW_SIZE(SDMA1)

// Conservative value (max 113 bytes plus name size plus alignment)
#define CPIO_HEADER_SIZE 256

#define FW_HEADER_SIZE 256

// Leave space for 16 files (currently 12)
#define FW_CPIO_SIZE (EAP_HDD_KEY_SIZE /* + EDID_SIZE */ + (CPIO_HEADER_SIZE * 16) + FW_CE_SIZE + FW_ME_SIZE + \
                       FW_MEC_SIZE + FW_MEC2_SIZE + FW_PFP_SIZE + \
                       FW_RLC_SIZE + FW_SDMA_SIZE + FW_SDMA1_SIZE + \
                       FW_HEADER_SIZE * 8)

ssize_t firmware_extract(void *dest);

#endif
