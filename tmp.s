
tmp_00.hsaco:	file format elf64-amdgpu


Disassembly of section .text:

0000000000001000 <_Z15vectoradd_floatPfPKfS1_ii>:
	s_mov_b32 m0, -1                                           // 000000001000: BEFC00C1
	s_lshl_b32 s10, s9, 4                                      // 000000001004: 8E0A8409
	s_add_u32 s10, s8, s10                                     // 000000001008: 800A0A08
	s_movk_i32 s11, 0x0                                        // 00000000100C: B00B0000
	v_mov_b32_e32 v2, 0                                        // 000000001010: 7E040280
	v_mov_b32_e32 v3, 0                                        // 000000001014: 7E060280
	v_mov_b32_e32 v4, 0                                        // 000000001018: 7E080280
	s_load_dwordx2 s[0:1], s[6:7], 0x18                        // 00000000101C: C0060003 00000018
	s_load_dword s2, s[4:5], 0x4                               // 000000001024: C0020082 00000004
	s_waitcnt lgkmcnt(0)                                       // 00000000102C: BF8CC07F
	s_lshr_b32 s3, s2, 16                                      // 000000001030: 8F039002
	s_mul_i32 s9, s9, s3                                       // 000000001034: 92090309
	v_add_u32_e32 v1, s9, v1                                   // 000000001038: 68020209
	v_mul_lo_u32 v1, v1, s0                                    // 00000000103C: D2850001 00000101
	s_and_b32 s2, s2, 0xffff                                   // 000000001044: 8602FF02 0000FFFF
	s_mul_i32 s8, s8, s2                                       // 00000000104C: 92080208
	s_mul_i32 s0, s1, s0                                       // 000000001050: 92000001
	v_add3_u32 v0, s8, v0, v1                                  // 000000001054: D1FF0000 04060008
	v_cmp_gt_i32_e32 vcc, s0, v0                               // 00000000105C: 7D880000
	s_and_saveexec_b64 s[0:1], vcc                             // 000000001060: BE80206A
	s_mov_b64 s[14:15], exec                                   // 000000001064: BE8E017E
	s_mov_b64 exec, 1                                          // 000000001068: BEFE0181
	s_mul_i32 s12, 1, s10                                      // 00000000106C: 920C0A81
	s_lshl_b32 s12, s12, 3                                     // 000000001070: 8E0C830C
	v_mov_b32_e32 v4, s12                                      // 000000001074: 7E08020C
	s_cmp_eq_u64 s[0:1], s[14:15]                              // 000000001078: BF120E00
	v_mov_b32_e32 v3, src_scc                                  // 00000000107C: 7E0602FD
	ds_add_u32 v4, v3                                          // 000000001080: DA000000 00000304
	s_cmp_eq_u64 0, s[14:15]                                   // 000000001088: BF120E80
	v_mov_b32_e32 v3, src_scc                                  // 00000000108C: 7E0602FD
	ds_write_b64 v4, v[2:3]                                    // 000000001090: DA9A0000 00000204
	s_waitcnt vmcnt(0) expcnt(0) lgkmcnt(0)                    // 000000001098: BF8C0000
	s_mov_b64 exec, s[16:17]                                   // 00000000109C: BEFE0110
	s_cbranch_execz BB0_2                                      // 0000000010A0: BF880019
	s_load_dwordx4 s[0:3], s[6:7], 0x0                         // 0000000010A4: C00A0003 00000000
	s_load_dwordx4 s[8:11], s[6:7], 0x10                       // 0000000010AC: C00A0203 00000010
	v_ashrrev_i32_e32 v1, 31, v0                               // 0000000010B4: 2202009F
	v_lshlrev_b64 v[0:1], 2, v[0:1]                            // 0000000010B8: D28F0000 00020082
	s_waitcnt lgkmcnt(0)                                       // 0000000010C0: BF8CC07F
	v_mov_b32_e32 v3, s1                                       // 0000000010C4: 7E060201
	v_add_co_u32_e32 v2, vcc, s0, v0                           // 0000000010C8: 32040000
	v_addc_co_u32_e32 v3, vcc, v3, v1, vcc                     // 0000000010CC: 38060303
	v_mov_b32_e32 v5, s9                                       // 0000000010D0: 7E0A0209
	v_add_co_u32_e32 v4, vcc, s8, v0                           // 0000000010D4: 32080008
	v_addc_co_u32_e32 v5, vcc, v5, v1, vcc                     // 0000000010D8: 380A0305
	v_mov_b32_e32 v6, s3                                       // 0000000010DC: 7E0C0203
	v_add_co_u32_e32 v0, vcc, s2, v0                           // 0000000010E0: 32000002
	v_addc_co_u32_e32 v1, vcc, v6, v1, vcc                     // 0000000010E4: 38020306
	global_load_dword v6, v[0:1], off                          // 0000000010E8: DC508000 067F0000
	global_load_dword v7, v[4:5], off                          // 0000000010F0: DC508000 077F0004
	s_waitcnt vmcnt(0)                                         // 0000000010F8: BF8C0F70
	v_add_f32_e32 v0, v6, v7                                   // 0000000010FC: 02000F06
	global_store_dword v[2:3], v0, off                         // 000000001100: DC708000 007F0002

0000000000001108 <BB0_2>:
	s_mov_b64 exec, 1                                          // 000000001108: BEFE0181
	v_mov_b32_e32 v0, s0                                       // 00000000110C: 7E000200
	v_mov_b32_e32 v1, s1                                       // 000000001110: 7E020201
	v_mov_b32_e32 v4, 0                                        // 000000001114: 7E080280
	ds_read_b64 v[2:3], v4                                     // 000000001118: DAEC0000 02000004
	s_waitcnt vmcnt(0) expcnt(0) lgkmcnt(0)                    // 000000001120: BF8C0000
	global_store_dwordx2 v[0:1], v[2:3], off                   // 000000001124: DC748000 007F0200
	s_endpgm                                                   // 00000000112C: BF810000
