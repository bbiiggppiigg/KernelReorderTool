
tmp_00.hsaco:	file format elf64-amdgpu


Disassembly of section .text:

0000000000001000 <_Z15vectoradd_floatPfPKfS1_ii>:
	s_load_dword s0, s[6:7], 0x18                              // 000000001000: C0020003 00000018
	s_load_dword s10, s[4:5], 0x4                              // 000000001008: C0020282 00000004
	s_waitcnt lgkmcnt(0)                                       // 000000001010: BF8CC07F
	s_lshr_b32 s1, s10, 16                                     // 000000001014: 8F01900A
	s_mul_i32 s9, s9, s1                                       // 000000001018: 92090109
	v_add_u32_e32 v1, s9, v1                                   // 00000000101C: 68020209
	v_mul_lo_u32 v1, v1, s0                                    // 000000001020: D2850001 00000101
	s_and_b32 s4, s10, 0xffff                                  // 000000001028: 8604FF0A 0000FFFF
	s_load_dwordx2 s[0:1], s[6:7], 0x0                         // 000000001030: C0060003 00000000
	s_load_dwordx2 s[2:3], s[6:7], 0x8                         // 000000001038: C0060083 00000008
	s_mul_i32 s8, s8, s4                                       // 000000001040: 92080408
	v_add3_u32 v0, s8, v0, v1                                  // 000000001044: D1FF0000 04060008
	v_ashrrev_i32_e32 v1, 31, v0                               // 00000000104C: 2202009F
	v_lshlrev_b64 v[2:3], 2, v[0:1]                            // 000000001050: D28F0002 00020082
	s_waitcnt lgkmcnt(0)                                       // 000000001058: BF8CC07F
	v_mov_b32_e32 v4, s3                                       // 00000000105C: 7E080203
	v_add_co_u32_e32 v2, vcc, s2, v2                           // 000000001060: 32040402
	v_addc_co_u32_e32 v3, vcc, v4, v3, vcc                     // 000000001064: 38060704
	global_load_dword v2, v[2:3], off                          // 000000001068: DC508000 027F0002
	s_mov_b32 s2, 0x41a00000                                   // 000000001070: BE8200FF 41A00000
	s_waitcnt vmcnt(0)                                         // 000000001078: BF8C0F70
	v_cmp_ngt_f32_e32 vcc, s2, v2                              // 00000000107C: 7C960402
	s_and_saveexec_b64 s[2:3], vcc                             // 000000001080: BE82206A
	s_cbranch_execz BB0_2                                      // 000000001084: BF88000D
	s_load_dwordx2 s[4:5], s[6:7], 0x10                        // 000000001088: C0060103 00000010
	v_lshlrev_b64 v[3:4], 2, v[0:1]                            // 000000001090: D28F0003 00020082
	s_waitcnt lgkmcnt(0)                                       // 000000001098: BF8CC07F
	v_mov_b32_e32 v5, s5                                       // 00000000109C: 7E0A0205
	v_add_co_u32_e32 v3, vcc, s4, v3                           // 0000000010A0: 32060604
	v_addc_co_u32_e32 v4, vcc, v5, v4, vcc                     // 0000000010A4: 38080905
	global_load_dword v3, v[3:4], off                          // 0000000010A8: DC508000 037F0003
	s_waitcnt vmcnt(0)                                         // 0000000010B0: BF8C0F70
	v_fma_f32 v2, v3, v3, v2                                   // 0000000010B4: D1CB0002 040A0703

00000000000010bc <BB0_2>:
	s_or_b64 exec, exec, s[2:3]                                // 0000000010BC: 87FE027E
	v_lshlrev_b64 v[0:1], 2, v[0:1]                            // 0000000010C0: D28F0000 00020082
	v_mov_b32_e32 v3, s1                                       // 0000000010C8: 7E060201
	v_add_co_u32_e32 v0, vcc, s0, v0                           // 0000000010CC: 32000000
	v_addc_co_u32_e32 v1, vcc, v3, v1, vcc                     // 0000000010D0: 38020303
	global_store_dword v[0:1], v2, off                         // 0000000010D4: DC708000 007F0200
	s_endpgm                                                   // 0000000010DC: BF810000
