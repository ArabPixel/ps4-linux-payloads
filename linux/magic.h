#ifdef PS4_5_05
#define kern_off_printf 0x436040
#define kern_off_snprintf 0x436350
#define kern_off_copyin 0x1EA710
#define kern_off_copyout 0x1EA630
#define kern_off_copyinstr 0x1EAB40
#define kern_off_kmem_alloc_contig 0xF1C90
#define kern_off_kmem_free 0xFCE50
#define kern_off_pmap_extract 0x2E0570
#define kern_off_pmap_protect 0x2E3090
#define kern_off_sched_pin 0x31FF40
#define kern_off_sched_unpin 0x31FF50
#define kern_off_smp_rendezvous 0x1B85B0
#define kern_off_smp_no_rendevous_barrier 0x1B8370
#define kern_off_icc_query_nowait 0x44020
#define kern_off_kernel_map 0x1AC60E0
#define kern_off_sysent 0x107C610
#define kern_off_kernel_pmap_store 0x22CB570
#define kern_off_Starsha_UcodeInfo 0
#define kern_off_gpu_devid_is_9924 0x4DE010
#define kern_off_gc_get_fw_info 0x4D37A0
#define kern_off_pml4pml4i 0x22CB560
#define kern_off_dmpml4i 0x22CB564
#define kern_off_dmpdpi 0x22CB568
#define kern_off_eap_hdd_key 0x2790C90
#define kern_off_edid 0x281A3F4
#define kern_off_wlanbt 0x2BFBA0
#define kern_off_kern_reboot 0x10D390
#define kern_off_set_gpu_freq 0x4D2530
#define kern_off_set_pstate 0x4CC8A0
#define kern_off_update_vddnp 0x4D2AA0
#define kern_off_set_cu_power_gate 0x4D2C40
#define kern_off_pstate_before_shutdown 0x10D97E
#define kern_off_set_nclk_mem_spd 0x30C270

#elif defined PS4_6_72 //ethylamine Joonie86 mircoho
#define kern_off_printf 0x123280
#define kern_off_snprintf 0x123590
#define kern_off_copyin 0x3C17A0
#define kern_off_copyout 0x3C16B0
#define kern_off_copyinstr 0x3C1C50
#define kern_off_kmem_alloc_contig 0x0B71C0
#define kern_off_kmem_free 0x250900
#define kern_off_pmap_extract 0x04E420
#define kern_off_pmap_protect 0x050F50
#define kern_off_sched_pin 0x446A30
#define kern_off_sched_unpin 0x446A40
#define kern_off_smp_rendezvous 0x2C93C0
#define kern_off_smp_no_rendevous_barrier 0x2C9180
#define kern_off_icc_query_nowait 0x3F290
#define kern_off_kernel_map 0x220DFC0
#define kern_off_sysent 0x111E000
#define kern_off_kernel_pmap_store 0x1BB7880
#define kern_off_Starsha_UcodeInfo 0
#define kern_off_gpu_devid_is_9924 0x4E82D0
#define kern_off_gc_get_fw_info 0x4E1A50
#define kern_off_pml4pml4i 0x1BB7870
#define kern_off_dmpml4i 0x1BB7874
#define kern_off_dmpdpi 0x1BB7878
#define kern_off_eap_hdd_key 0x26DCCD0
#define kern_off_edid 0x2764100
#define kern_off_wlanbt 0x1CDD80
#define kern_off_kern_reboot 0x206D50
#define kern_off_set_gpu_freq 0x4E0530
#define kern_off_set_pstate 0x4FC2E0
#define kern_off_update_vddnp 0x4E0B10
#define kern_off_set_cu_power_gate 0x4E0F20
#define kern_off_pstate_before_shutdown 0x20734E
#define kern_off_set_nclk_mem_spd 0x19A390

#elif defined PS4_7_00 //tihmstar EchoStretch
#define kern_off_printf 0x0BC730
#define kern_off_snprintf 0x0BCA30
#define kern_off_copyin 0x02F230
#define kern_off_copyout 0x02F140
#define kern_off_copyinstr 0x02F6E0
#define kern_off_kmem_alloc_contig 0x430780
#define kern_off_kmem_free 0x1172C0
#define kern_off_pmap_extract 0x3DED30
#define kern_off_pmap_protect 0x3DFF70
#define kern_off_sched_pin 0x329870
#define kern_off_sched_unpin 0x329890
#define kern_off_smp_rendezvous 0x4447A0
#define kern_off_smp_no_rendevous_barrier 0x444560
#define kern_off_icc_query_nowait 0x3B2F80
#define kern_off_kernel_map 0x21C8EE0
#define kern_off_sysent 0x1125660
#define kern_off_kernel_pmap_store 0x22c5268
#define kern_off_Starsha_UcodeInfo 0
#define kern_off_gpu_devid_is_9924 0x4EE3C0
#define kern_off_gc_get_fw_info 0x4F6830
#define kern_off_pml4pml4i 0x22C5258
#define kern_off_dmpml4i 0x22C525C
#define kern_off_dmpdpi 0x22C5260
#define kern_off_eap_hdd_key 0x26E0CD0
#define kern_off_edid 0x27645E0
#define kern_off_wlanbt 0x0DECB0
#define kern_off_kern_reboot 0x2CD780
#define kern_off_set_gpu_freq 0x4F5350
#define kern_off_set_pstate 0x4EF6C0
#define kern_off_update_vddnp 0x4F5910
#define kern_off_set_cu_power_gate 0x4F5D20
#define kern_off_pstate_before_shutdown 0x2CDD6E

#elif defined PS4_7_02 //tihmstar EchoStretch
#define kern_off_printf 0x0BC730
#define kern_off_snprintf 0x0BCA30
#define kern_off_copyin 0x02F230
#define kern_off_copyout 0x02F140
#define kern_off_copyinstr 0x02F6E0
#define kern_off_kmem_alloc_contig 0x430780
#define kern_off_kmem_free 0x1172C0
#define kern_off_pmap_extract 0x3DED30
#define kern_off_pmap_protect 0x3E18A0
#define kern_off_sched_pin 0x329870
#define kern_off_sched_unpin 0x329890
#define kern_off_smp_rendezvous 0x4447A0
#define kern_off_smp_no_rendevous_barrier 0x444560
#define kern_off_icc_query_nowait 0x3B2F80
#define kern_off_kernel_map 0x21C8EE0
#define kern_off_sysent 0x1125660
#define kern_off_kernel_pmap_store 0x22c5268
#define kern_off_Starsha_UcodeInfo 0
#define kern_off_gpu_devid_is_9924 0x4EE3C0
#define kern_off_gc_get_fw_info 0x4F6830
#define kern_off_pml4pml4i 0x22C5258
#define kern_off_dmpml4i 0x22C525C
#define kern_off_dmpdpi 0x22C5260
#define kern_off_eap_hdd_key 0x26E0CD0
#define kern_off_edid 0x27645E0
#define kern_off_wlanbt 0x0DECB0
#define kern_off_kern_reboot 0x2CD780
#define kern_off_set_gpu_freq 0x4F5350
#define kern_off_set_pstate 0x4EF6C0
#define kern_off_update_vddnp 0x4F5910
#define kern_off_set_cu_power_gate 0x4F5D20
#define kern_off_pstate_before_shutdown 0x2CDD6E

#elif defined PS4_9_00 //codedwrench
#define kern_off_printf	0x0B7A30
#define kern_off_snprintf 0x0B7D30
#define kern_off_copyin	0x2716A0
#define kern_off_copyout 0x2715B0
#define kern_off_copyinstr 0x271B50
#define kern_off_kmem_alloc_contig 0x270880
#define kern_off_kmem_free 0x37C040
#define kern_off_pmap_extract 0x12D050
#define kern_off_pmap_protect 0x12E1F0
#define kern_off_sched_pin 0x1CD0D0
#define kern_off_sched_unpin 0x1CD0F0
#define kern_off_smp_rendezvous	0x432BF0
#define kern_off_smp_no_rendevous_barrier 0x432A00
#define kern_off_icc_query_nowait 0x2E1760
#define kern_off_kernel_map 0x2268D48
#define kern_off_sysent	0x1100310
#define kern_off_kernel_pmap_store 0x1B904B0
#define kern_off_Starsha_UcodeInfo 0
#define kern_off_gpu_devid_is_9924 0x4AC260
#define kern_off_gc_get_fw_info	0x4DF280
#define kern_off_pml4pml4i 0x1B904A0
#define kern_off_dmpml4i 0x1B904A4
#define kern_off_dmpdpi	0x1B904A8
#define kern_off_eap_hdd_key 0x26C4C90
#define kern_off_edid 0x274C058
#define kern_off_wlanbt	0x180860
#define kern_off_kern_reboot 0x29A380
#define kern_off_set_gpu_freq 0x4DDDC0
#define kern_off_set_pstate 0x4D6FC0
#define kern_off_update_vddnp 0x4DE360
#define kern_off_set_cu_power_gate 0x4DE770
#define kern_off_pstate_before_shutdown 0x29A970
#define kern_off_set_nclk_mem_spd 0

#elif defined PS4_9_03 //mircoho
#define kern_off_printf 0x0B79E0
#define kern_off_snprintf 0x0B7CE0
#define kern_off_copyin 0x271320
#define kern_off_copyout 0x271230
#define kern_off_copyinstr 0x2717D0
#define kern_off_kmem_alloc_contig 0x270500
#define kern_off_kmem_free 0x37A240
#define kern_off_pmap_extract 0x12D000
#define kern_off_pmap_protect 0x12E1A0
#define kern_off_sched_pin 0x1CD080
#define kern_off_sched_unpin 0x1CD0A0
#define kern_off_smp_rendezvous 0x430B60
#define kern_off_smp_no_rendevous_barrier 0x430970
#define kern_off_icc_query_nowait 0x2E13E0
#define kern_off_kernel_map 0x2264d48
#define kern_off_sysent 0x10fc310
#define kern_off_kernel_pmap_store 0x1b8c4b0
#define kern_off_Starsha_UcodeInfo 0
#define kern_off_gpu_devid_is_9924 0x4AA160
#define kern_off_gc_get_fw_info 0x4DD180
#define kern_off_pml4pml4i 0x1b8c4a0 //
#define kern_off_dmpml4i 0x1b8c4a4 //
#define kern_off_dmpdpi 0x1b8c4a8
#define kern_off_eap_hdd_key 0x26c0c90
#define kern_off_edid 0x2748058
#define kern_off_wlanbt 0x180810
#define kern_off_kern_reboot 0x29A000
#define kern_off_set_gpu_freq 0x4DBCC0
#define kern_off_set_pstate 0x4D4EC0
#define kern_off_update_vddnp 0x4DC260
#define kern_off_set_cu_power_gate 0x4DC670
#define kern_off_pstate_before_shutdown 0x29A5F0
#define kern_off_set_nclk_mem_spd 0

#elif defined PS4_9_60 //Eintim23
#define kern_off_printf	0x205470
#define kern_off_snprintf 0x205770
#define kern_off_copyin	0x201EB0
#define kern_off_copyout 0x201DC0
#define kern_off_copyinstr 0x202360
#define kern_off_kmem_alloc_contig 0x3CAF80
#define kern_off_kmem_free 0x188BA0
#define kern_off_pmap_extract 0x42F4B0
#define kern_off_pmap_protect 0x430650
#define kern_off_sched_pin 0x457C50
#define kern_off_sched_unpin 0x457C70
#define kern_off_smp_rendezvous	0x52530
#define kern_off_smp_no_rendevous_barrier 0x52340
#define kern_off_icc_query_nowait 0x72780
#define kern_off_kernel_map 0x2147830
#define kern_off_sysent 0x10F92F0
#define kern_off_kernel_pmap_store 0x2228E88
#define kern_off_Starsha_UcodeInfo 0
#define kern_off_gpu_devid_is_9924 0x4D6AA0
#define kern_off_gc_get_fw_info	0x4C3C00
#define kern_off_pml4pml4i 0x2228E78
#define kern_off_dmpml4i 0x2228E7C
#define kern_off_dmpdpi	0x2228E80
#define kern_off_eap_hdd_key 0x26B4C60
#define kern_off_edid 0x2738FE8
#define kern_off_wlanbt	0x26CA70
#define kern_off_kern_reboot 0x331DF0
#define kern_off_set_gpu_freq 0x4C2740
#define kern_off_set_pstate 0x4C4B10
#define kern_off_update_vddnp 0x4C2CE0
#define kern_off_set_cu_power_gate 0x4C30F0
#define kern_off_pstate_before_shutdown 0x3323E0
#define kern_off_set_nclk_mem_spd 0

#elif defined PS4_10_00 //EinTim23
#define kern_off_printf	0xC50F0
#define kern_off_snprintf 0xC53F0
#define kern_off_copyin	0x472F10
#define kern_off_copyout 0x472E20
#define kern_off_copyinstr 0x4733C0
#define kern_off_kmem_alloc_contig 0x1D8B30
#define kern_off_kmem_free 0x33B210
#define kern_off_pmap_extract 0xDFA00
#define kern_off_pmap_protect 0xE0BA0
#define kern_off_sched_pin 0x1D7290
#define kern_off_sched_unpin 0x1D72B0
#define kern_off_smp_rendezvous	0x16BC40
#define kern_off_smp_no_rendevous_barrier 0x16BA50
#define kern_off_icc_query_nowait 0x24E230
#define kern_off_kernel_map 0x227BEF8
#define kern_off_sysent 0x1102D90
#define kern_off_kernel_pmap_store 0x2182D60
#define kern_off_Starsha_UcodeInfo 0
#define kern_off_gpu_devid_is_9924 0x4B3300
#define kern_off_gc_get_fw_info	0x4CF590
#define kern_off_pml4pml4i 0x2182D50
#define kern_off_dmpml4i 0x2182D54
#define kern_off_dmpdpi	0x2182D58
#define kern_off_eap_hdd_key 0x26C4D00
#define kern_off_edid 0x274F370
#define kern_off_wlanbt	0x2C7E20
#define kern_off_kern_reboot 0x480CE0
#define kern_off_set_gpu_freq 0x4CE0D0
#define kern_off_set_pstate 0x4B1500
#define kern_off_update_vddnp 0x4CE670
#define kern_off_set_cu_power_gate 0x4CEA80
#define kern_off_pstate_before_shutdown 0x4812d0
#define kern_off_set_nclk_mem_spd 0

#elif defined PS4_10_50 //EchoStretch
#define kern_off_printf 0x00450e80
#define kern_off_snprintf 0x00451180
#define kern_off_copyin 0x000d7560
#define kern_off_copyout 0x000d7470
#define kern_off_copyinstr 0x000d7a10
#define kern_off_kmem_alloc_contig 0x0042bb80
#define kern_off_kmem_free 0x00428b30
#define kern_off_pmap_extract 0x000444d0
#define kern_off_pmap_protect 0x00045670
#define kern_off_sched_pin 0x004447e0
#define kern_off_sched_unpin 0x00444800
#define kern_off_smp_rendezvous 0x00091190
#define kern_off_smp_no_rendevous_barrier 0x001e27c0
#define kern_off_icc_query_nowait 0x000695f0
#define kern_off_kernel_map 0x022a9250
#define kern_off_sysent 0x011029c0
#define kern_off_kernel_pmap_store 0x01b2cee0
#define kern_off_Starsha_UcodeInfo 0
#define kern_off_gpu_devid_is_9924 0x004b7de0
#define kern_off_gc_get_fw_info 0x004d9960
#define kern_off_pml4pml4i 0x01b2ced0
#define kern_off_dmpml4i 0x01b2ced4
#define kern_off_dmpdpi 0x01b2ced8
#define kern_off_eap_hdd_key 0x026c4d00
#define kern_off_edid 0x02748f90
#define kern_off_wlanbt 0x0034d640
#define kern_off_kern_reboot 0x0045d6d0
#define kern_off_set_gpu_freq 0x004d84a0
#define kern_off_set_pstate 0x004dc040
#define kern_off_update_vddnp 0x004d8a40
#define kern_off_set_cu_power_gate 0x004d8e50
#define kern_off_pstate_before_shutdown 0x0045dcc0
#define kern_off_set_nclk_mem_spd 0

#elif defined PS4_11_00 //EinTim23
#define kern_off_printf	0x2fcbd0
#define kern_off_snprintf 0x2fced0
#define kern_off_copyin	0x2ddfe0
#define kern_off_copyout 0x2ddef0
#define kern_off_copyinstr 0x2de490
#define kern_off_kmem_alloc_contig 0x22f8d0
#define kern_off_kmem_free 0x245fe0
#define kern_off_pmap_extract 0x114290
#define kern_off_pmap_protect 0x115430
#define kern_off_sched_pin 0x2bb9b0
#define kern_off_sched_unpin 0x2bb9d0
#define kern_off_smp_rendezvous	0x1342c0
#define kern_off_smp_no_rendevous_barrier 0x1340d0
#define kern_off_icc_query_nowait 0x39f3f0
#define kern_off_kernel_map 0x21ff130
#define kern_off_sysent 0x1101760
#define kern_off_kernel_pmap_store 0x2162a88
#define kern_off_Starsha_UcodeInfo 0
#define kern_off_gpu_devid_is_9924 0x4b4a70
#define kern_off_gc_get_fw_info	0x4b1fc0
#define kern_off_pml4pml4i 0x2162a78
#define kern_off_dmpml4i 0x2162a7c
#define kern_off_dmpdpi	0x2162a80
#define kern_off_eap_hdd_key 0x26c4cd0
#define kern_off_edid 0x2749e58
#define kern_off_wlanbt	0x1d1570
#define kern_off_kern_reboot 0x198060
#define kern_off_set_gpu_freq 0x4b0b00
#define kern_off_set_pstate 0x4c8ce0
#define kern_off_update_vddnp 0x4b10a0
#define kern_off_set_cu_power_gate 0x4b14b0
#define kern_off_pstate_before_shutdown 0x198650
#define kern_off_set_nclk_mem_spd 0x2F56A0

#elif defined PS4_11_02 //BestPig
#define kern_off_printf	0x2fcbf0
#define kern_off_snprintf 0x2fcef0
#define kern_off_copyin	0x2de000
#define kern_off_copyout 0x2ddf10
#define kern_off_copyinstr 0x2de4b0
#define kern_off_kmem_alloc_contig 0x22f8f0
#define kern_off_kmem_free 0x246000
#define kern_off_pmap_extract 0x1142b0
#define kern_off_pmap_protect 0x115450
#define kern_off_sched_pin 0x2bb9d0
#define kern_off_sched_unpin 0x2bb9f0
#define kern_off_smp_rendezvous	0x1342e0
#define kern_off_smp_no_rendevous_barrier 0x1340f0
#define kern_off_icc_query_nowait 0x39f410
#define kern_off_kernel_map 0x21ff130
#define kern_off_sysent 0x1101760
#define kern_off_kernel_pmap_store 0x2162a88
#define kern_off_Starsha_UcodeInfo 0
#define kern_off_gpu_devid_is_9924 0x4b4a00
#define kern_off_gc_get_fw_info	0x4b1f50
#define kern_off_pml4pml4i 0x2162a78
#define kern_off_dmpml4i 0x2162a7c
#define kern_off_dmpdpi	0x2162a80
#define kern_off_eap_hdd_key 0x26c4cd0
#define kern_off_edid 0x2749e58
#define kern_off_wlanbt	0x1d1590
#define kern_off_kern_reboot 0x198080
#define kern_off_set_gpu_freq 0x4b0a90
#define kern_off_set_pstate 0x4c8c70
#define kern_off_update_vddnp 0x4b1030
#define kern_off_set_cu_power_gate 0x4b1440
#define kern_off_pstate_before_shutdown 0x198670
#define kern_off_set_nclk_mem_spd 0x2f56c0

#elif defined PS4_11_50 //BestPig
#define kern_off_printf 0x2e01a0
#define kern_off_snprintf 0x2e04a0
#define kern_off_copyin 0x2bd590
#define kern_off_copyout 0x2bd4a0
#define kern_off_copyinstr 0x2bda40
#define kern_off_kmem_alloc_contig 0x24d2f0
#define kern_off_kmem_free 0x465970
#define kern_off_pmap_extract 0x573d0
#define kern_off_pmap_protect 0x58570
#define kern_off_sched_pin 0x231520
#define kern_off_sched_unpin 0x231540
#define kern_off_smp_rendezvous 0x1ad4e0
#define kern_off_smp_no_rendevous_barrier 0x1ad2f0
#define kern_off_icc_query_nowait 0x447890
#define kern_off_kernel_map 0x22d1d50
#define kern_off_sysent 0x1102b70
#define kern_off_kernel_pmap_store 0x1b2c3a0
#define kern_off_Starsha_UcodeInfo 0
#define kern_off_gpu_devid_is_9924 0x4ac300
#define kern_off_gc_get_fw_info 0x4bacb0
#define kern_off_pml4pml4i 0x1b2c390
#define kern_off_dmpml4i 0x1b2c394
#define kern_off_dmpdpi 0x1b2c398
#define kern_off_eap_hdd_key 0x26c4cf0
#define kern_off_edid 0x274a148
#define kern_off_wlanbt 0x4787b0
#define kern_off_kern_reboot 0x3a1b30
#define kern_off_set_gpu_freq 0x4b97f0
#define kern_off_set_pstate 0x4bbbc0
#define kern_off_update_vddnp 0x4b9d90
#define kern_off_set_cu_power_gate 0x4ba1a0
#define kern_off_pstate_before_shutdown 0x3a2120
#define kern_off_set_nclk_mem_spd 0

#elif defined PS4_12_00 //bestpig
#define kern_off_printf 0x2e03e0
#define kern_off_snprintf 0x2e06e0
#define kern_off_copyin 0x2bd670
#define kern_off_copyout 0x2bd580
#define kern_off_copyinstr 0x2bdb20
#define kern_off_kmem_alloc_contig 0x24D3D0
#define kern_off_kmem_free 0x465bb0
#define kern_off_pmap_extract 0x573d0
#define kern_off_pmap_protect 0x58570
#define kern_off_sched_pin 0x231600
#define kern_off_sched_unpin 0x231620
#define kern_off_smp_rendezvous 0x1ad4e0
#define kern_off_smp_no_rendevous_barrier 0x1ad2f0
#define kern_off_icc_query_nowait 0x447ad0
#define kern_off_kernel_map 0x22d1d50
#define kern_off_sysent 0x1102b70
#define kern_off_kernel_pmap_store 0x1b2c3a0
#define kern_off_Starsha_UcodeInfo 0x0
#define kern_off_gpu_devid_is_9924 0x4ac540
#define kern_off_gc_get_fw_info 0x4baef0
#define kern_off_pml4pml4i 0x1b2c390
#define kern_off_dmpml4i 0x1b2c394
#define kern_off_dmpdpi 0x1b2c398
#define kern_off_eap_hdd_key 0x26c4cf0
#define kern_off_edid 0x275e148
#define kern_off_wlanbt 0x4789f0
#define kern_off_kern_reboot 0x3a1d70
#define kern_off_set_gpu_freq 0x4b9a30
#define kern_off_set_pstate 0x4bbe00
#define kern_off_update_vddnp 0x4b9fd0
#define kern_off_set_cu_power_gate 0x4ba3e0
#define kern_off_pstate_before_shutdown 0x3a2360
#define kern_off_set_nclk_mem_spd 0

#elif defined PS4_12_50 //ArabPixel
#define kern_off_printf 0x2E0420
#define kern_off_snprintf 0x2E0720
#define kern_off_copyin 0x2BD6B0
#define kern_off_copyout 0x2BD5C0
#define kern_off_copyinstr 0x2BDB60
#define kern_off_kmem_alloc_contig 0x24D410
#define kern_off_kmem_free 0x465BF0
#define kern_off_pmap_extract 0x573D0
#define kern_off_pmap_protect 0x58570
#define kern_off_sched_pin 0x231640
#define kern_off_sched_unpin 0x231660
#define kern_off_smp_rendezvous 0x1AD520
#define kern_off_smp_no_rendevous_barrier 0x1AD330
#define kern_off_icc_query_nowait 0x447B10
#define kern_off_kernel_map 0x22D1D50
#define kern_off_sysent 0x1102B70
#define kern_off_kernel_pmap_store 0x1b2c3a0
#define kern_off_Starsha_UcodeInfo 0
#define kern_off_gpu_devid_is_9924 0x4AC580
#define kern_off_gc_get_fw_info 0x4BAF30
#define kern_off_pml4pml4i 0x1B2C390
#define kern_off_dmpml4i 0x1B2C394
#define kern_off_dmpdpi 0x1B2C398
#define kern_off_eap_hdd_key 0x26C4CF0
#define kern_off_edid 0x275E148
#define kern_off_wlanbt 0x478A30
#define kern_off_kern_reboot 0x3A1DB0
#define kern_off_set_gpu_freq 0x4B9A70
#define kern_off_set_pstate 0x4BBE40
#define kern_off_update_vddnp 0x4BA010
#define kern_off_set_cu_power_gate 0x4BA420
#define kern_off_pstate_before_shutdown 0x3A23A0
#define kern_off_set_nclk_mem_spd 0

#elif defined PS4_13_00 //ArabPixel
#define kern_off_printf 0x2E0440 // Done
#define kern_off_snprintf 0x2E0740 // Done
#define kern_off_copyin 0x2BD6D0 // Done
#define kern_off_copyout 0x2BD5E0 // Done
#define kern_off_copyinstr 0x2BDB80 // Done
#define kern_off_kmem_alloc_contig 0x24D430 // Done
#define kern_off_kmem_free 0x465C10 // Done
#define kern_off_pmap_extract 0x573D0 // Done
#define kern_off_pmap_protect 0x58570 // Done
#define kern_off_sched_pin 0x231660 // Done
#define kern_off_sched_unpin 0x231680 // Done
#define kern_off_smp_rendezvous 0x1AD520 // Done
#define kern_off_smp_no_rendevous_barrier 0x1AD330 // Done
#define kern_off_icc_query_nowait 0x447B30 // Done
#define kern_off_kernel_map 0x22D1D50 // Done
#define kern_off_sysent 0x1102B70 // Done
#define kern_off_kernel_pmap_store 0x1B2C3A0 // Done
#define kern_off_Starsha_UcodeInfo 0x0
#define kern_off_gpu_devid_is_9924 0x4AC5A0 // Done
#define kern_off_gc_get_fw_info 0x4BAF50 // Done 
#define kern_off_pml4pml4i 0x1B2C390 // Done
#define kern_off_dmpml4i 0x1B2C394 // Done
#define kern_off_dmpdpi 0x1B2C398 // Done
#define kern_off_eap_hdd_key 0x26C4CF0 // Done
#define kern_off_edid 0x275E148 // Done
#define kern_off_wlanbt 0x478A50 // Done
#define kern_off_kern_reboot 0x3A1DD0 // Done
#define kern_off_set_gpu_freq 0x4B9A90 // Done
#define kern_off_set_pstate 0x4BBE60 // Done
#define kern_off_update_vddnp 0x4BA030 // Done
#define kern_off_set_cu_power_gate 0x4BA440 // Done
#define kern_off_pstate_before_shutdown 0x3A23C0 // Done
#define kern_off_set_nclk_mem_spd 0

#elif defined PS4_13_02 //ArabPixel
#define kern_off_printf 0x2E0450 // Done
#define kern_off_snprintf 0x2E0750 // Done
#define kern_off_copyin 0x2BD6E0 // Done
#define kern_off_copyout 0x2BD5F0 // Done
#define kern_off_copyinstr 0x2BDB90 // Done 
#define kern_off_kmem_alloc_contig 0x24D440 // Done
#define kern_off_kmem_free 0x465C20 // Done
#define kern_off_pmap_extract 0x573D0 // Done
#define kern_off_pmap_protect 0x58570 // Done
#define kern_off_sched_pin 0x231670 // Done
#define kern_off_sched_unpin 0x231690 // Done
#define kern_off_smp_rendezvous 0x1AD530 // Done
#define kern_off_smp_no_rendevous_barrier 0x1AD340 // Done
#define kern_off_icc_query_nowait 0x447B40 // Done
#define kern_off_kernel_map 0x22D1D50 // Done
#define kern_off_sysent 0x1102B70 // Done
#define kern_off_kernel_pmap_store 0x1B2C3A0 // Done
#define kern_off_Starsha_UcodeInfo 0x0
#define kern_off_gpu_devid_is_9924 0x4AC5A0 // Done
#define kern_off_gc_get_fw_info 0x4BAF60 // Done
#define kern_off_pml4pml4i 0x1B2C390 // Done
#define kern_off_dmpml4i 0x1B2C394 // Done
#define kern_off_dmpdpi 0x1B2C398 // Done
#define kern_off_eap_hdd_key 0x26C4CF0 // Done
#define kern_off_edid 0x275E148 // Done
#define kern_off_wlanbt 0x478A60 // Done
#define kern_off_kern_reboot 0x3A1DE0 // Done
#define kern_off_set_gpu_freq 0x4B9AA0 // Done
#define kern_off_set_pstate 0x4BBE70 // Done
#define kern_off_update_vddnp 0x4BA040 // Done
#define kern_off_set_cu_power_gate 0x4BA450 // Done
#define kern_off_pstate_before_shutdown 0x3A23D0 // Done
#define kern_off_set_nclk_mem_spd 0

#endif
