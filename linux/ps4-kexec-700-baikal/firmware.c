/*
 * ps4-kexec - a kexec() implementation for Orbis OS / FreeBSD
 *
 * Copyright (C) 2015-2016 shuffle2 <godisgovernment@gmail.com>
 * Copyright (C) 2015-2016 Hector Martin "marcan" <marcan@marcan.st>
 *
 * This code is licensed to you under the 2-clause BSD license. See the LICENSE
 * file for more information.
 */

#include "firmware.h"
#include "types.h"
#include "kernel.h"
#include "string.h"
#include "types.h"
#include "crc32.h"

#define DIR  0040755
#define FILE 0100644

struct firmware_header {
    u32 size_bytes;
    u32 header_size_bytes;
    u16 header_version_major;
    u16 header_version_minor;
    u16 ip_version_major;
    u16 ip_version_minor;
    u32 ucode_version;
    u32 ucode_size_bytes;
    u32 ucode_array_offset_bytes;
    u32 crc32;
    union {
        struct {
            u32 ucode_feature_version;
            u32 jt_offset;
            u32 jt_size;
            u8 end[];
        } gfx1;
        struct {
            u32 ucode_feature_version;
            u32 save_and_restore_offset;
            u32 clear_state_descriptor_offset;
            u32 avail_scratch_ram_locations;
            u32 master_pkt_description_offset;
            u8 end[];
        } rlc1;
        struct {
            u32 ucode_feature_version;
            u32 ucode_change_version;
            u32 jt_offset;
            u32 jt_size;
            u8 end[];
        } sdma1;
        u8 raw[0xe0];
    };
};

static inline char hex(u8 c)
{
    if (c <= 9)
        return '0' + c;
    return 'a' + c - 10;
}

static void hex8(u8 **p, u32 val)
{
    *(*p)++ = hex(val >> 28);
    *(*p)++ = hex((val >> 24) & 0xf);
    *(*p)++ = hex((val >> 20) & 0xf);
    *(*p)++ = hex((val >> 16) & 0xf);
    *(*p)++ = hex((val >> 12) & 0xf);
    *(*p)++ = hex((val >> 8) & 0xf);
    *(*p)++ = hex((val >> 4) & 0xf);
    *(*p)++ = hex(val & 0xf);
}

void cpio_hdr(u8 **p, const char *name, u32 mode, size_t size)
{
    size_t name_len = strlen(name);

    // Pad to 4 byte multiple
    while (((uintptr_t)*p) & 0x3)
        *(*p)++ = 0;

    memcpy(*p, "070701", 6);
    *p += 6;
    hex8(p, 0); // c_ino
    hex8(p, mode); // c_mode
    hex8(p, 0); // c_uid
    hex8(p, 0); // c_gid
    hex8(p, 1); // c_nlink
    hex8(p, 0); // c_mtime
    hex8(p, size); // c_filesize
    hex8(p, 0); // c_maj
    hex8(p, 0); // c_min
    hex8(p, 0); // c_rmaj
    hex8(p, 0); // c_rmin
    hex8(p, name_len + 1); // c_namesize
    hex8(p, 0); // c_chksum
    memcpy(*p, name, name_len);
    *p += name_len;
    *(*p)++ = 0;

    while (((uintptr_t)*p) & 0x3)
        *(*p)++ = 0;
}

struct fw_header_t {
    u64 size_words;
    char *unk_ident;
    u64 unk;
    void *blob;
    u64 unk2;
};

struct fw_info_t {
    struct fw_header_t *rlc;
    struct fw_header_t *sdma0;
    struct fw_header_t *sdma1;
    struct fw_header_t *ce;
    struct fw_header_t *pfp;
    struct fw_header_t *me;
    struct fw_header_t *mec1;
    struct fw_header_t *mec2;
};

struct fw_expected_sizes_t {
    u64 rlc;
    u64 sdma0;
    u64 sdma1;
    u64 ce;
    u64 pfp;
    u64 me;
    u64 mec1;
    u64 mec2;
};
static const struct fw_expected_sizes_t liverpool_fw_sizes = {
    LVP_FW_RLC_SIZE,
    LVP_FW_SDMA_SIZE,
    LVP_FW_SDMA1_SIZE,
    LVP_FW_CE_SIZE,
    LVP_FW_PFP_SIZE,
    LVP_FW_ME_SIZE,
    LVP_FW_MEC_SIZE,
    LVP_FW_MEC2_SIZE
};
static const struct fw_expected_sizes_t gladius_fw_sizes = {
    GL_FW_RLC_SIZE,
    GL_FW_SDMA_SIZE,
    GL_FW_SDMA1_SIZE,
    GL_FW_CE_SIZE,
    GL_FW_PFP_SIZE,
    GL_FW_ME_SIZE,
    GL_FW_MEC_SIZE,
    GL_FW_MEC2_SIZE
};

void copy_edid(u8 **p, int sz)
{
	int i;
	u8 *edid = *p;
	u8 *off_edid = kern.edid;
	
	memset(edid, 0, sz);
    *p += sz;
	
	for(i = 0; i < sz; i++)
		*(edid + i) = *(off_edid + i);
	
	*p += sz;
}

void copy_eap_hdd_key(u8 **p)
{
	int i;
	u8 *eap_key = *p;
	u8 *off_eap_key = kern.eap_hdd_key;
	
	memset(eap_key, 0, 0x20);
    *p += 0x20;
	
	for(i = 0; i < 0x20; i++)
	{
		if(i < 0x10)
			*(eap_key + i) = *(off_eap_key + 0xF - i);
		else
			*(eap_key + i) = *(off_eap_key + 0x2F - i);
	}
	*p += 0x20;
}

int copy_firmware(u8 **p, const char *name, struct fw_header_t *hdr, size_t expected_size)
{
    kern.printf("Copying %s firmware\n", name);
    if (expected_size != (hdr->size_words * 4)) {
        kern.printf("copy_firmware: %s: expected size %d, got %d\n",
                    name, expected_size, hdr->size_words * 4);
        return 0;
    }

    struct firmware_header *fhdr = (struct firmware_header*)*p;
    memset(fhdr, 0, sizeof(*fhdr));
    *p += sizeof(*fhdr);

    memcpy(*p, hdr->blob, expected_size);

    fhdr->size_bytes = expected_size + sizeof(*fhdr);
    fhdr->header_size_bytes = offsetof(struct firmware_header, raw);
    fhdr->header_version_major = 1;
    fhdr->header_version_minor = 0;
    fhdr->ucode_version = 0x10;
    fhdr->ucode_size_bytes = expected_size;
    fhdr->ucode_array_offset_bytes = sizeof(*fhdr);

    *p += expected_size;

    return 1;
}

int copy_gfx_firmware(u8 **p, const char *name, struct fw_header_t *hdr, size_t expected_size)
{
    struct firmware_header *fhdr = (struct firmware_header*)*p;
    if (!copy_firmware(p, name, hdr, expected_size))
        return 0;

    fhdr->ip_version_major = 7;
    fhdr->ip_version_minor = 2;
    fhdr->header_size_bytes = offsetof(struct firmware_header, gfx1.end);
    fhdr->gfx1.ucode_feature_version = 21;
    fhdr->gfx1.jt_offset = (expected_size & ~0xfff) >> 2;
    fhdr->gfx1.jt_size = (expected_size & 0xfff) >> 2;

    fhdr->crc32 = crc32(0, fhdr->raw, sizeof(fhdr->raw) + expected_size);
    return 1;
}

int copy_rlc_firmware(u8 **p, const char *name, struct fw_header_t *hdr, size_t expected_size)
{
    struct firmware_header *fhdr = (struct firmware_header*)*p;
    if (!copy_firmware(p, name, hdr, expected_size))
        return 0;

    fhdr->ip_version_major = 7;
    fhdr->ip_version_minor = 2;
    fhdr->header_size_bytes = offsetof(struct firmware_header, rlc1.end);
    fhdr->rlc1.ucode_feature_version = 1;
    fhdr->rlc1.save_and_restore_offset = 0x90;
    fhdr->rlc1.clear_state_descriptor_offset = 0x3d;
    fhdr->rlc1.avail_scratch_ram_locations = 0x270; // 0x170 for bonaire, 0x270 for kabini??
    fhdr->rlc1.master_pkt_description_offset = 0;

    fhdr->crc32 = crc32(0, fhdr->raw, sizeof(fhdr->raw) + expected_size);
    return 1;
}

int copy_sdma_firmware(u8 **p, const char *name, struct fw_header_t *hdr, size_t expected_size, int idx)
{
    struct firmware_header *fhdr = (struct firmware_header*)*p;
    if (!copy_firmware(p, name, hdr, expected_size))
        return 0;

    fhdr->ip_version_major = 2;
    fhdr->ip_version_minor = 1;
    fhdr->header_size_bytes = offsetof(struct firmware_header, sdma1.end);
    fhdr->sdma1.ucode_feature_version = idx == 0 ? 9 : 0;
    fhdr->sdma1.ucode_change_version = 0;
    fhdr->sdma1.jt_offset = (expected_size & ~0xfff) >> 2;
    fhdr->sdma1.jt_size = (expected_size & 0xfff) >> 2;

    fhdr->crc32 = crc32(0, fhdr->raw, sizeof(fhdr->raw) + expected_size);
    return 1;
}

static const u32 pfp_nop_handler[] = {
    0xdc120000, //     mov r4, ctr
    0x31144000, //     seteq r5, r4, #0x4000
    0x95400009, //     cbz r5, l0
    0xc4200016, //     ldw r8, [r0, #0x16]
    0xdc030000, //     mov ctr, r0
    0xcc000049, //     stw r0, [r0, #0x49]
    0xcc200013, //     stw r0, [r8, #0x13]
    0xc424007e, //     ldw r9, [r0, #0x7e]
    0x96400000, // l1: cbz r9, l1
    0x7c408001, //     mov r2, r1
    0x88000000, //     btab
    0xd440007f, // l0: stm r1, [r0, #0x7f]
    0x7c408001, //     mov r2, r1
    0x88000000, //     btab
};

static const u32 ce_nop_handler[] = {
    0xdc120000, //     mov r4, ctr
    0x31144000, //     seteq r5, r4, #0x4000
    0x95400009, //     cbz r5, l0
    0xc420000c, //     ldw r8, [r0, #0xc]
    0xdc030000, //     mov ctr, r0
    0xcc00002f, //     stw r0, [r0, #0x2f]
    0xcc200012, //     stw r0, [r8, #0x12]
    0xc424007e, //     ldw r9, [r0, #0x7e]
    0x96400000, // l1: cbz r9, l1
    0x7c408001, //     mov r2, r1
    0x88000000, //     btab
    0xd440007f, // l0: stm r1, [r0, #0x7f]
    0x7c408001, //     mov r2, r1
    0x88000000, //     btab
};

static const u32 mec_nop_handler[] = {
    0xdc120000, //     mov r4, ctr
    0x31144000, //     seteq r5, r4, #0x4000
    0x95400009, //     cbz r5, l0
    0xc43c000c, //     ldw r15, [r0, #0x9]
    0xdc030000, //     mov ctr, r0
    0xcc00002b, //     stw r0, [r0, #0x2b]
    0xcc3c000d, //     stw r0, [r15, #0xd]
    0xc424007e, //     ldw r9, [r0, #0x7e]
    0x96400000, // l1: cbz r9, l1
    0x7c408001, //     mov r2, r1
    0x88000000, //     btab
    0xd440007f, // l0: stm r1, [r0, #0x7f]
    0x7c408001, //     mov r2, r1
    0x88000000, //     btab
};

#define PACKET_TYPE_NOP 0x10

static void patch_fw(void *p, const u32 *handler, int handler_size) {
    int size = ((struct firmware_header*)p)->ucode_size_bytes;
    int code_size = (size & ~0xfff) / 4;
    int nop_start = code_size - 0x10;

    u32 *fw = p + sizeof(struct firmware_header);
    kern.printf("NOP handler at 0x%x\n", nop_start);
    memcpy(&fw[nop_start], handler, handler_size);

    // patch the branch table entry
    for (int off = code_size; off < size/4; off++) {
        if ((fw[off] >> 16) == PACKET_TYPE_NOP) {
            fw[off] = (PACKET_TYPE_NOP << 16) | nop_start;
        }
    }
}

struct fw_info_t *get_fw_info() {
    if (kern.gc_get_fw_info) {
        return kern.gc_get_fw_info();
    } else if (kern.Starsha_UcodeInfo) {
        return kern.Starsha_UcodeInfo;
    } else {
        return NULL;
    }
}

const struct fw_expected_sizes_t *get_fw_expected_sizes() {
    if (kern.gpu_devid_is_9924 && kern.gpu_devid_is_9924()) {
        return &gladius_fw_sizes;
    } else {
        return &liverpool_fw_sizes;
    }
}

const char * get_gpu_name() {
  if (kern.gpu_devid_is_9924 && kern.gpu_devid_is_9924()) {
      return "gladius";
  } else {
      return "liverpool";
  }
}

ssize_t firmware_extract(void *dest)
{
    u8 *p = dest;

    // Yeah, this calls it Starsha... Liverpool, Starsha, ThebeJ, whatever.
    struct fw_info_t *info = get_fw_info();
    if (!info) {
        kern.printf("firmware_extract: Could not locate firmware table");
        return -1;
    }
    const struct fw_expected_sizes_t *fw_sizes = get_fw_expected_sizes();
	
	//Eap hdd key
	cpio_hdr(&p, "key", DIR, 0);
	cpio_hdr(&p, "key/eap_hdd_key.bin", FILE, 0x20);
	copy_eap_hdd_key(&p);
	
    cpio_hdr(&p, "lib", DIR, 0);
    cpio_hdr(&p, "lib/firmware", DIR, 0);
    
    /** We need detect the size of edid first, on some monitor it is 128 on other 256 bytes, so for now remove it **/
    int edid_sz = (((unsigned char*)kern.edid)[126]?256:128);
    cpio_hdr(&p, "lib/firmware/edid", DIR, 0);
    cpio_hdr(&p, "lib/firmware/edid/my_edid.bin", FILE, edid_sz);
	copy_edid(&p, edid_sz);
    
    char dir[7];
    if (kern.gpu_devid_is_9924 && kern.gpu_devid_is_9924())
      kern.snprintf(dir, sizeof(dir), "amdgpu");
    else
      kern.snprintf(dir, sizeof(dir), "amdgpu");

    char dir_path[64];
    kern.snprintf(dir_path, sizeof(dir_path), "lib/firmware/%s/", dir);
    cpio_hdr(&p, dir_path, DIR, 0);

    char pfp_path[64];
    kern.snprintf(pfp_path, sizeof(pfp_path), "%s%s_pfp.bin", dir_path, get_gpu_name());
    kern.printf("firmware_extract: Extract %s \n", pfp_path);
    cpio_hdr(&p, pfp_path, FILE, FW_HEADER_SIZE + fw_sizes->pfp);
    u8 *pfp = p;
    if (!copy_gfx_firmware(&p, "PFP", info->pfp, fw_sizes->pfp))
        return -1;
    patch_fw(pfp, pfp_nop_handler, sizeof(pfp_nop_handler));

    char me_path[64];
    kern.snprintf(me_path, sizeof(me_path), "%s%s_me.bin", dir_path, get_gpu_name());
    kern.printf("firmware_extract: Extract %s \n", me_path);
    cpio_hdr(&p, me_path, FILE, FW_HEADER_SIZE + fw_sizes->me);
    if (!copy_gfx_firmware(&p, "ME", info->me, fw_sizes->me))
        return -1;

    char ce_path[64];
    kern.snprintf(ce_path, sizeof(ce_path), "%s%s_ce.bin", dir_path, get_gpu_name());
    kern.printf("firmware_extract: Extract %s \n", ce_path);
    cpio_hdr(&p, ce_path, FILE, FW_HEADER_SIZE + fw_sizes->ce);
    u8 *ce = p;
    if (!copy_gfx_firmware(&p, "CE", info->ce, fw_sizes->ce))
        return -1;
    patch_fw(ce, ce_nop_handler, sizeof(ce_nop_handler));

    char mec_path[64];
    kern.snprintf(mec_path, sizeof(mec_path), "%s%s_mec.bin", dir_path, get_gpu_name());
    kern.printf("firmware_extract: Extract %s \n", mec_path);
    cpio_hdr(&p, mec_path, FILE, FW_HEADER_SIZE + fw_sizes->mec1);
    u8 *mec1 = p;
    if (!copy_gfx_firmware(&p, "MEC", info->mec1, fw_sizes->mec1))
        return -1;
    patch_fw(mec1, mec_nop_handler, sizeof(mec_nop_handler));

    char mec2_path[64];
    kern.snprintf(mec2_path, sizeof(mec2_path), "%s%s_mec2.bin", dir_path, get_gpu_name());
    kern.printf("firmware_extract: Extract %s \n", mec2_path);
    cpio_hdr(&p, mec2_path, FILE, FW_HEADER_SIZE + fw_sizes->mec2);
    u8 *mec2 = p;
    if (!copy_gfx_firmware(&p, "MEC2", info->mec2, fw_sizes->mec2))
        return -1;
    patch_fw(mec2, mec_nop_handler, sizeof(mec_nop_handler));

    char rlc_path[64];
    kern.snprintf(rlc_path, sizeof(rlc_path), "%s%s_rlc.bin", dir_path, get_gpu_name());
    kern.printf("firmware_extract: Extract %s \n", rlc_path);
    cpio_hdr(&p, rlc_path, FILE, FW_HEADER_SIZE + fw_sizes->rlc);
    if (!copy_rlc_firmware(&p, "RLC", info->rlc, fw_sizes->rlc))
        return -1;

    char sdma_path[64];
    kern.snprintf(sdma_path, sizeof(sdma_path), "%s%s_sdma.bin", dir_path,  get_gpu_name());
    kern.printf("firmware_extract: Extract %s \n", sdma_path);
    cpio_hdr(&p, sdma_path, FILE, FW_HEADER_SIZE + fw_sizes->sdma0);
    if (!copy_sdma_firmware(&p, "SDMA", info->sdma0, fw_sizes->sdma0, 0))
        return -1;
    cpio_hdr(&p, "TRAILER!!!", FILE, 0);

    char sdma1_path[64];
    kern.snprintf(sdma1_path, sizeof(sdma1_path), "%s%s_sdma1.bin", dir_path,  get_gpu_name());
    kern.printf("firmware_extract: Extract %s \n", sdma1_path);
    cpio_hdr(&p, sdma1_path, FILE, FW_HEADER_SIZE + fw_sizes->sdma1);
    if (!copy_sdma_firmware(&p, "SDMA1", info->sdma1, fw_sizes->sdma1, 1))
        return -1;
    cpio_hdr(&p, "TRAILER!!!", FILE, 0);

    size_t size = p - (u8*)dest;
    if (size > FW_CPIO_SIZE) {
        kern.printf("firmware_extract: overflow! %d > %d\n", size, FW_CPIO_SIZE);
        return -1;
    }

    return size;
}
