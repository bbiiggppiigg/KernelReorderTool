
tmp_00.hsaco:	file format elf64-amdgpu


Disassembly of section .text:

0000000000001000 <_Z9mmmKernelP15HIP_vector_typeIfLj4EES1_S1_jj$local>:
	s_branch 327                                               // 000000001000: BF820147 <BB0_6+0xb4>
	v_cndmask_b32_e32 v0, s16, v0, vcc                         // 000000001004: 00000010
	s_load_dwordx2 s[10:11], s[6:7], 0x18                      // 000000001008: C0060283 00000018
	s_waitcnt lgkmcnt(0)                                       // 000000001010: BF8C007F
	s_load_dword s2, s[4:5], 0x4                               // 000000001014: C0020082 00000004
	s_mov_b32 s4, 0                                            // 00000000101C: BE840080
	s_waitcnt lgkmcnt(0)                                       // 000000001020: BF8C007F
	s_and_b32 s3, s2, 0xffff                                   // 000000001024: 8603FF02 0000FFFF
	s_lshr_b32 s2, s2, 16                                      // 00000000102C: 8F029002
	s_mul_i32 s9, s9, s2                                       // 000000001030: 92090209
	s_mul_i32 s8, s8, s3                                       // 000000001034: 92080308
	v_add_u32_e32 v1, vcc, s9, v1                              // 000000001038: 32020209
	s_lshr_b32 s2, s11, 2                                      // 00000000103C: 8F02820B
	v_add_u32_e32 v0, vcc, s8, v0                              // 000000001040: 32000008
	s_cmp_lg_u32 s10, 0                                        // 000000001044: BF07800A
	v_lshlrev_b32_e32 v2, 2, v1                                // 000000001048: 24040282
	s_cbranch_scc0 BB0_4                                       // 00000000104C: BF8400EF
	s_lshr_b32 s5, s10, 2                                      // 000000001050: 8F05820A
	v_add_u32_e32 v4, vcc, 1, v2                               // 000000001054: 32080481
	v_mul_lo_u32 v23, s5, v4                                   // 000000001058: D2850017 00020805
	v_add_u32_e32 v4, vcc, 2, v2                               // 000000001060: 32080482
	s_load_dwordx4 s[12:15], s[6:7], 0x0                       // 000000001064: C00A0303 00000000
	v_mul_lo_u32 v25, s5, v4                                   // 00000000106C: D2850019 00020805
	v_add_u32_e32 v4, vcc, 3, v2                               // 000000001074: 32080483
	v_mul_lo_u32 v21, v2, s5                                   // 000000001078: D2850015 00000B02
	v_mul_lo_u32 v27, s5, v4                                   // 000000001080: D285001B 00020805
	s_mov_b32 s5, s4                                           // 000000001088: BE850004
	s_mov_b32 s6, s4                                           // 00000000108C: BE860004
	s_mov_b32 s7, s4                                           // 000000001090: BE870004
	v_mov_b32_e32 v4, s4                                       // 000000001094: 7E080204
	v_mov_b32_e32 v7, s7                                       // 000000001098: 7E0E0207
	v_mov_b32_e32 v5, s5                                       // 00000000109C: 7E0A0205
	v_mov_b32_e32 v6, s6                                       // 0000000010A0: 7E0C0206
	v_mov_b32_e32 v12, v7                                      // 0000000010A4: 7E180307
	v_mov_b32_e32 v16, v7                                      // 0000000010A8: 7E200307
	v_mov_b32_e32 v20, v7                                      // 0000000010AC: 7E280307
	v_or_b32_e32 v8, 1, v2                                     // 0000000010B0: 28100481
	v_or_b32_e32 v3, 2, v2                                     // 0000000010B4: 28060482
	v_or_b32_e32 v1, 3, v2                                     // 0000000010B8: 28020483
	s_lshl_b32 s3, s2, 2                                       // 0000000010BC: 8E038202
	s_lshl_b32 s8, s2, 1                                       // 0000000010C0: 8E088102
	s_mul_i32 s9, s2, 3                                        // 0000000010C4: 92098302
	v_mov_b32_e32 v22, 0                                       // 0000000010C8: 7E2C0280
	v_mov_b32_e32 v29, v0                                      // 0000000010CC: 7E3A0300
	v_mov_b32_e32 v11, v6                                      // 0000000010D0: 7E160306
	v_mov_b32_e32 v10, v5                                      // 0000000010D4: 7E140305
	v_mov_b32_e32 v9, v4                                       // 0000000010D8: 7E120304
	v_mov_b32_e32 v15, v6                                      // 0000000010DC: 7E1E0306
	v_mov_b32_e32 v14, v5                                      // 0000000010E0: 7E1C0305
	v_mov_b32_e32 v13, v4                                      // 0000000010E4: 7E1A0304
	v_mov_b32_e32 v19, v6                                      // 0000000010E8: 7E260306
	v_mov_b32_e32 v18, v5                                      // 0000000010EC: 7E240305
	v_mov_b32_e32 v17, v4                                      // 0000000010F0: 7E220304

00000000000010f4 <BB0_2>:
	v_lshlrev_b64 v[30:31], 4, v[21:22]                        // 0000000010F4: D28F001E 00022A84
	s_waitcnt lgkmcnt(0)                                       // 0000000010FC: BF8C007F
	v_mov_b32_e32 v24, s13                                     // 000000001100: 7E30020D
	v_add_u32_e32 v32, vcc, s12, v30                           // 000000001104: 32403C0C
	v_addc_u32_e32 v33, vcc, v24, v31, vcc                     // 000000001108: 38423F18
	v_mov_b32_e32 v24, v22                                     // 00000000110C: 7E300316
	v_lshlrev_b64 v[30:31], 4, v[23:24]                        // 000000001110: D28F001E 00022E84
	v_mov_b32_e32 v26, s13                                     // 000000001118: 7E34020D
	v_add_u32_e32 v34, vcc, s12, v30                           // 00000000111C: 32443C0C
	v_addc_u32_e32 v35, vcc, v26, v31, vcc                     // 000000001120: 38463F1A
	v_mov_b32_e32 v26, v22                                     // 000000001124: 7E340316
	v_lshlrev_b64 v[30:31], 4, v[25:26]                        // 000000001128: D28F001E 00023284
	v_mov_b32_e32 v24, s13                                     // 000000001130: 7E30020D
	v_add_u32_e32 v36, vcc, s12, v30                           // 000000001134: 32483C0C
	v_mov_b32_e32 v28, v22                                     // 000000001138: 7E380316
	v_addc_u32_e32 v37, vcc, v24, v31, vcc                     // 00000000113C: 384A3F18
	v_lshlrev_b64 v[30:31], 4, v[27:28]                        // 000000001140: D28F001E 00023684
	v_mov_b32_e32 v41, v22                                     // 000000001148: 7E520316
	v_add_u32_e32 v38, vcc, s12, v30                           // 00000000114C: 324C3C0C
	v_mov_b32_e32 v30, v22                                     // 000000001150: 7E3C0316
	v_addc_u32_e32 v39, vcc, v24, v31, vcc                     // 000000001154: 384E3F18
	v_lshlrev_b64 v[30:31], 4, v[29:30]                        // 000000001158: D28F001E 00023A84
	v_mov_b32_e32 v24, s15                                     // 000000001160: 7E30020F
	v_add_u32_e32 v30, vcc, s14, v30                           // 000000001164: 323C3C0E
	v_addc_u32_e32 v31, vcc, v24, v31, vcc                     // 000000001168: 383E3F18
	v_add_u32_e32 v40, vcc, s2, v29                            // 00000000116C: 32503A02
	v_lshlrev_b64 v[40:41], 4, v[40:41]                        // 000000001170: D28F0028 00025084
	v_mov_b32_e32 v43, v22                                     // 000000001178: 7E560316
	v_add_u32_e32 v40, vcc, s14, v40                           // 00000000117C: 3250500E
	v_addc_u32_e32 v41, vcc, v24, v41, vcc                     // 000000001180: 38525318
	v_add_u32_e32 v42, vcc, s8, v29                            // 000000001184: 32543A08
	v_lshlrev_b64 v[42:43], 4, v[42:43]                        // 000000001188: D28F002A 00025484
	s_add_i32 s4, s4, 4                                        // 000000001190: 81048404
	v_add_u32_e32 v42, vcc, s14, v42                           // 000000001194: 3254540E
	v_addc_u32_e32 v43, vcc, v24, v43, vcc                     // 000000001198: 38565718
	v_add_u32_e32 v44, vcc, 8, v32                             // 00000000119C: 32584088
	v_addc_u32_e32 v45, vcc, 0, v33, vcc                       // 0000000011A0: 385A4280
	v_add_u32_e32 v46, vcc, 8, v30                             // 0000000011A4: 325C3C88
	v_addc_u32_e32 v47, vcc, 0, v31, vcc                       // 0000000011A8: 385E3E80
	v_add_u32_e32 v48, vcc, 8, v40                             // 0000000011AC: 32605088
	v_addc_u32_e32 v49, vcc, 0, v41, vcc                       // 0000000011B0: 38625280
	v_add_u32_e32 v50, vcc, 8, v42                             // 0000000011B4: 32645488
	flat_load_dwordx2 v[32:33], v[32:33]                       // 0000000011B8: DC540000 20000020
	flat_load_dwordx2 v[44:45], v[44:45]                       // 0000000011C0: DC540000 2C00002C
	v_addc_u32_e32 v51, vcc, 0, v43, vcc                       // 0000000011C8: 38665680
	flat_load_dwordx2 v[30:31], v[30:31]                       // 0000000011CC: DC540000 1E00001E
	flat_load_dwordx2 v[46:47], v[46:47]                       // 0000000011D4: DC540000 2E00002E
	flat_load_dwordx2 v[40:41], v[40:41]                       // 0000000011DC: DC540000 28000028
	flat_load_dwordx2 v[48:49], v[48:49]                       // 0000000011E4: DC540000 30000030
	flat_load_dwordx2 v[42:43], v[42:43]                       // 0000000011EC: DC540000 2A00002A
	flat_load_dwordx2 v[50:51], v[50:51]                       // 0000000011F4: DC540000 32000032
	flat_load_dwordx2 v[52:53], v[34:35]                       // 0000000011FC: DC540000 34000022
	s_cmp_ge_u32 s4, s10                                       // 000000001204: BF090A04
	s_waitcnt vmcnt(4) lgkmcnt(4)                              // 000000001208: BF8C0474
	v_mul_f32_e32 v24, v33, v40                                // 00000000120C: 0A305121
	v_mul_f32_e32 v26, v33, v41                                // 000000001210: 0A345321
	s_waitcnt vmcnt(3) lgkmcnt(3)                              // 000000001214: BF8C0373
	v_mul_f32_e32 v28, v33, v48                                // 000000001218: 0A386121
	v_mul_f32_e32 v54, v33, v49                                // 00000000121C: 0A6C6321
	v_mac_f32_e32 v24, v32, v30                                // 000000001220: 2C303D20
	v_mac_f32_e32 v26, v32, v31                                // 000000001224: 2C343F20
	v_mac_f32_e32 v28, v32, v46                                // 000000001228: 2C385D20
	v_mac_f32_e32 v54, v32, v47                                // 00000000122C: 2C6C5F20
	v_add_u32_e32 v32, vcc, 8, v36                             // 000000001230: 32404888
	v_addc_u32_e32 v33, vcc, 0, v37, vcc                       // 000000001234: 38424A80
	flat_load_dwordx2 v[36:37], v[36:37]                       // 000000001238: DC540000 24000024
	flat_load_dwordx2 v[32:33], v[32:33]                       // 000000001240: DC540000 20000020
	s_waitcnt vmcnt(2) lgkmcnt(2)                              // 000000001248: BF8C0272
	v_mul_f32_e32 v55, v53, v40                                // 00000000124C: 0A6E5135
	v_mul_f32_e32 v56, v53, v41                                // 000000001250: 0A705335
	v_mul_f32_e32 v57, v53, v48                                // 000000001254: 0A726135
	v_mul_f32_e32 v53, v53, v49                                // 000000001258: 0A6A6335
	v_mac_f32_e32 v55, v52, v30                                // 00000000125C: 2C6E3D34
	v_mac_f32_e32 v56, v52, v31                                // 000000001260: 2C703F34
	v_mac_f32_e32 v57, v52, v46                                // 000000001264: 2C725D34
	v_mac_f32_e32 v53, v52, v47                                // 000000001268: 2C6A5F34
	v_mac_f32_e32 v26, v44, v43                                // 00000000126C: 2C34572C
	v_mac_f32_e32 v24, v44, v42                                // 000000001270: 2C30552C
	v_mac_f32_e32 v28, v44, v50                                // 000000001274: 2C38652C
	v_mac_f32_e32 v54, v44, v51                                // 000000001278: 2C6C672C
	s_waitcnt vmcnt(1) lgkmcnt(1)                              // 00000000127C: BF8C0171
	v_mul_f32_e32 v52, v37, v40                                // 000000001280: 0A685125
	v_mul_f32_e32 v58, v37, v41                                // 000000001284: 0A745325
	v_mul_f32_e32 v59, v37, v48                                // 000000001288: 0A766125
	v_mul_f32_e32 v60, v37, v49                                // 00000000128C: 0A786325
	v_mac_f32_e32 v52, v36, v30                                // 000000001290: 2C683D24
	v_mac_f32_e32 v58, v36, v31                                // 000000001294: 2C743F24
	v_mac_f32_e32 v59, v36, v46                                // 000000001298: 2C765D24
	v_mac_f32_e32 v60, v36, v47                                // 00000000129C: 2C785F24
	v_add_u32_e32 v36, vcc, 8, v38                             // 0000000012A0: 32484C88
	v_addc_u32_e32 v37, vcc, 0, v39, vcc                       // 0000000012A4: 384A4E80
	flat_load_dwordx2 v[38:39], v[38:39]                       // 0000000012A8: DC540000 26000026
	flat_load_dwordx2 v[36:37], v[36:37]                       // 0000000012B0: DC540000 24000024
	s_waitcnt vmcnt(2) lgkmcnt(2)                              // 0000000012B8: BF8C0272
	v_mac_f32_e32 v52, v32, v42                                // 0000000012BC: 2C685520
	v_mac_f32_e32 v58, v32, v43                                // 0000000012C0: 2C745720
	v_mac_f32_e32 v59, v32, v50                                // 0000000012C4: 2C766520
	v_mac_f32_e32 v60, v32, v51                                // 0000000012C8: 2C786720
	s_waitcnt vmcnt(1) lgkmcnt(1)                              // 0000000012CC: BF8C0171
	v_mul_f32_e32 v40, v39, v40                                // 0000000012D0: 0A505127
	v_mul_f32_e32 v61, v38, v31                                // 0000000012D4: 0A7A3F26
	v_mac_f32_e32 v40, v38, v30                                // 0000000012D8: 2C503D26
	v_add_u32_e32 v30, vcc, s9, v29                            // 0000000012DC: 323C3A09
	v_mov_b32_e32 v31, v22                                     // 0000000012E0: 7E3E0316
	v_lshlrev_b64 v[30:31], 4, v[30:31]                        // 0000000012E4: D28F001E 00023C84
	v_mac_f32_e32 v61, v39, v41                                // 0000000012EC: 2C7A5327
	v_mul_f32_e32 v41, v39, v48                                // 0000000012F0: 0A526127
	v_mul_f32_e32 v48, v39, v49                                // 0000000012F4: 0A606327
	v_mac_f32_e32 v41, v38, v46                                // 0000000012F8: 2C525D26
	v_mac_f32_e32 v48, v38, v47                                // 0000000012FC: 2C605F26
	v_mov_b32_e32 v38, s15                                     // 000000001300: 7E4C020F
	v_add_u32_e32 v30, vcc, s14, v30                           // 000000001304: 323C3C0E
	v_addc_u32_e32 v31, vcc, v38, v31, vcc                     // 000000001308: 383E3F26
	v_add_u32_e32 v34, vcc, 8, v34                             // 00000000130C: 32444488
	v_addc_u32_e32 v35, vcc, 0, v35, vcc                       // 000000001310: 38464680
	v_add_u32_e32 v38, vcc, 8, v30                             // 000000001314: 324C3C88
	v_addc_u32_e32 v39, vcc, 0, v31, vcc                       // 000000001318: 384E3E80
	flat_load_dwordx2 v[34:35], v[34:35]                       // 00000000131C: DC540000 22000022
	flat_load_dwordx2 v[30:31], v[30:31]                       // 000000001324: DC540000 1E00001E
	flat_load_dwordx2 v[38:39], v[38:39]                       // 00000000132C: DC540000 26000026
	s_waitcnt vmcnt(3) lgkmcnt(3)                              // 000000001334: BF8C0373
	v_mac_f32_e32 v61, v36, v43                                // 000000001338: 2C7A5724
	v_mac_f32_e32 v40, v36, v42                                // 00000000133C: 2C505524
	v_mac_f32_e32 v41, v36, v50                                // 000000001340: 2C526524
	v_mac_f32_e32 v48, v36, v51                                // 000000001344: 2C606724
	v_add_u32_e32 v21, vcc, 1, v21                             // 000000001348: 322A2A81
	v_add_u32_e32 v29, vcc, s3, v29                            // 00000000134C: 323A3A03
	v_add_u32_e32 v23, vcc, 1, v23                             // 000000001350: 322E2E81
	v_add_u32_e32 v25, vcc, 1, v25                             // 000000001354: 32323281
	v_add_u32_e32 v27, vcc, 1, v27                             // 000000001358: 32363681
	s_waitcnt vmcnt(2) lgkmcnt(2)                              // 00000000135C: BF8C0272
	v_mac_f32_e32 v55, v34, v42                                // 000000001360: 2C6E5522
	v_mac_f32_e32 v56, v34, v43                                // 000000001364: 2C705722
	v_mac_f32_e32 v57, v34, v50                                // 000000001368: 2C726522
	v_mac_f32_e32 v53, v34, v51                                // 00000000136C: 2C6A6722
	s_waitcnt vmcnt(1) lgkmcnt(1)                              // 000000001370: BF8C0171
	v_mac_f32_e32 v24, v45, v30                                // 000000001374: 2C303D2D
	v_mac_f32_e32 v26, v45, v31                                // 000000001378: 2C343F2D
	s_waitcnt vmcnt(0) lgkmcnt(0)                              // 00000000137C: BF8C0070
	v_mac_f32_e32 v28, v45, v38                                // 000000001380: 2C384D2D
	v_mac_f32_e32 v54, v45, v39                                // 000000001384: 2C6C4F2D
	v_mac_f32_e32 v55, v35, v30                                // 000000001388: 2C6E3D23
	v_mac_f32_e32 v56, v35, v31                                // 00000000138C: 2C703F23
	v_mac_f32_e32 v57, v35, v38                                // 000000001390: 2C724D23
	v_mac_f32_e32 v53, v35, v39                                // 000000001394: 2C6A4F23
	v_mac_f32_e32 v52, v33, v30                                // 000000001398: 2C683D21
	v_mac_f32_e32 v58, v33, v31                                // 00000000139C: 2C743F21
	v_mac_f32_e32 v59, v33, v38                                // 0000000013A0: 2C764D21
	v_mac_f32_e32 v60, v33, v39                                // 0000000013A4: 2C784F21
	v_mac_f32_e32 v61, v37, v31                                // 0000000013A8: 2C7A3F25
	v_mac_f32_e32 v40, v37, v30                                // 0000000013AC: 2C503D25
	v_mac_f32_e32 v41, v37, v38                                // 0000000013B0: 2C524D25
	v_mac_f32_e32 v48, v37, v39                                // 0000000013B4: 2C604F25
	v_add_f32_e32 v5, v5, v61                                  // 0000000013B8: 020A7B05
	v_add_f32_e32 v18, v18, v26                                // 0000000013BC: 02243512
	v_add_f32_e32 v17, v17, v24                                // 0000000013C0: 02223111
	v_add_f32_e32 v19, v19, v28                                // 0000000013C4: 02263913
	v_add_f32_e32 v20, v20, v54                                // 0000000013C8: 02286D14
	v_add_f32_e32 v13, v13, v55                                // 0000000013CC: 021A6F0D
	v_add_f32_e32 v14, v14, v56                                // 0000000013D0: 021C710E
	v_add_f32_e32 v15, v15, v57                                // 0000000013D4: 021E730F
	v_add_f32_e32 v16, v16, v53                                // 0000000013D8: 02206B10
	v_add_f32_e32 v9, v9, v52                                  // 0000000013DC: 02126909
	v_add_f32_e32 v10, v10, v58                                // 0000000013E0: 0214750A
	v_add_f32_e32 v11, v11, v59                                // 0000000013E4: 0216770B
	v_add_f32_e32 v12, v12, v60                                // 0000000013E8: 0218790C
	v_add_f32_e32 v4, v4, v40                                  // 0000000013EC: 02085104
	v_add_f32_e32 v6, v6, v41                                  // 0000000013F0: 020C5306
	v_add_f32_e32 v7, v7, v48                                  // 0000000013F4: 020E6107
	s_cbranch_scc0 BB0_2                                       // 0000000013F8: BF84FF3E
	s_mov_b64 s[4:5], 0                                        // 0000000013FC: BE840180
	s_and_b64 vcc, exec, s[4:5]                                // 000000001400: 86EA047E
	s_cbranch_vccnz BB0_5                                      // 000000001404: BF870002
	s_branch BB0_6                                             // 000000001408: BF820018

000000000000140c <BB0_4>:
	s_cbranch_execz BB0_6                                      // 00000000140C: BF880017

0000000000001410 <BB0_5>:
	s_mov_b32 s4, 0                                            // 000000001410: BE840080
	s_mov_b32 s7, s4                                           // 000000001414: BE870004
	s_mov_b32 s5, s4                                           // 000000001418: BE850004
	s_mov_b32 s6, s4                                           // 00000000141C: BE860004
	v_mov_b32_e32 v20, s7                                      // 000000001420: 7E280207
	v_mov_b32_e32 v17, s4                                      // 000000001424: 7E220204
	v_mov_b32_e32 v19, s6                                      // 000000001428: 7E260206
	v_mov_b32_e32 v18, s5                                      // 00000000142C: 7E240205
	v_mov_b32_e32 v13, v17                                     // 000000001430: 7E1A0311
	v_mov_b32_e32 v9, v17                                      // 000000001434: 7E120311
	v_mov_b32_e32 v4, v17                                      // 000000001438: 7E080311
	v_or_b32_e32 v8, 1, v2                                     // 00000000143C: 28100481
	v_or_b32_e32 v3, 2, v2                                     // 000000001440: 28060482
	v_or_b32_e32 v1, 3, v2                                     // 000000001444: 28020483
	v_mov_b32_e32 v14, v18                                     // 000000001448: 7E1C0312
	v_mov_b32_e32 v15, v19                                     // 00000000144C: 7E1E0313
	v_mov_b32_e32 v16, v20                                     // 000000001450: 7E200314
	v_mov_b32_e32 v10, v18                                     // 000000001454: 7E140312
	v_mov_b32_e32 v11, v19                                     // 000000001458: 7E160313
	v_mov_b32_e32 v12, v20                                     // 00000000145C: 7E180314
	v_mov_b32_e32 v5, v18                                      // 000000001460: 7E0A0312
	v_mov_b32_e32 v6, v19                                      // 000000001464: 7E0C0313
	v_mov_b32_e32 v7, v20                                      // 000000001468: 7E0E0314

000000000000146c <BB0_6>:
	v_mul_lo_u32 v2, v2, s2                                    // 00000000146C: D2850002 00000502
	v_mov_b32_e32 v22, 0                                       // 000000001474: 7E2C0280
	v_mov_b32_e32 v25, s1                                      // 000000001478: 7E320201
	v_add_u32_e32 v21, vcc, v2, v0                             // 00000000147C: 322A0102
	v_mul_lo_u32 v2, v8, s2                                    // 000000001480: D2850002 00000508
	v_lshlrev_b64 v[23:24], 4, v[21:22]                        // 000000001488: D28F0017 00022A84
	v_mov_b32_e32 v8, s1                                       // 000000001490: 7E100201
	v_add_u32_e32 v23, vcc, s0, v23                            // 000000001494: 322E2E00
	v_addc_u32_e32 v24, vcc, v25, v24, vcc                     // 000000001498: 38303119
	v_add_u32_e32 v21, vcc, v2, v0                             // 00000000149C: 322A0102
	flat_store_dwordx4 v[23:24], v[17:20]                      // 0000000014A0: DC7C0000 00001117
	s_nop 0                                                    // 0000000014A8: BF800000
	v_lshlrev_b64 v[17:18], 4, v[21:22]                        // 0000000014AC: D28F0011 00022A84
	v_mul_lo_u32 v19, v3, s2                                   // 0000000014B4: D2850013 00000503
	v_add_u32_e32 v2, vcc, s0, v17                             // 0000000014BC: 32042200
	v_addc_u32_e32 v3, vcc, v8, v18, vcc                       // 0000000014C0: 38062508
	flat_store_dwordx4 v[2:3], v[13:16]                        // 0000000014C4: DC7C0000 00000D02
	v_add_u32_e32 v21, vcc, v19, v0                            // 0000000014CC: 322A0113
	v_mul_lo_u32 v13, v1, s2                                   // 0000000014D0: D285000D 00000501
	v_lshlrev_b64 v[2:3], 4, v[21:22]                          // 0000000014D8: D28F0002 00022A84
	v_add_u32_e32 v1, vcc, s0, v2                              // 0000000014E0: 32020400
	v_addc_u32_e32 v2, vcc, v8, v3, vcc                        // 0000000014E4: 38040708
	v_add_u32_e32 v21, vcc, v13, v0                            // 0000000014E8: 322A010D
	flat_store_dwordx4 v[1:2], v[9:12]                         // 0000000014EC: DC7C0000 00000901
	v_lshlrev_b64 v[0:1], 4, v[21:22]                          // 0000000014F4: D28F0000 00022A84
	v_mov_b32_e32 v2, s1                                       // 0000000014FC: 7E040201
	v_add_u32_e32 v0, vcc, s0, v0                              // 000000001500: 32000000
	v_addc_u32_e32 v1, vcc, v2, v1, vcc                        // 000000001504: 38020302
	flat_store_dwordx4 v[0:1], v[4:7]                          // 000000001508: DC7C0000 00000400
	s_endpgm                                                   // 000000001510: BF810000
		...
	s_load_dwordx4 s[0:3], s[6:7], 0x10                        // 000000001520: C00A0003 00000010
	s_load_dwordx2 s[10:11], s[6:7], 0x18                      // 000000001528: C0060283 00000018
	s_branch 65207                                             // 000000001530: BF82FEB7 <_Z9mmmKernelP15HIP_vector_typeIfLj4EES1_S1_jj$local+0x10>
		...