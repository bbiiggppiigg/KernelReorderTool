
vectoradd_hip.exe.2:	file format elf64-x86-64


Disassembly of section .text:

0000000000206780 <_start>:
  206780: f3 0f 1e fa                  	endbr64
  206784: 31 ed                        	xorl	%ebp, %ebp
  206786: 49 89 d1                     	movq	%rdx, %r9
  206789: 5e                           	popq	%rsi
  20678a: 48 89 e2                     	movq	%rsp, %rdx
  20678d: 48 83 e4 f0                  	andq	$-16, %rsp
  206791: 50                           	pushq	%rax
  206792: 54                           	pushq	%rsp
  206793: 4c 8d 05 f6 08 00 00         	leaq	2294(%rip), %r8  # 207090 <__libc_csu_fini>
  20679a: 48 8d 0d 7f 08 00 00         	leaq	2175(%rip), %rcx  # 207020 <__libc_csu_init>
  2067a1: 48 8d 3d f8 00 00 00         	leaq	248(%rip), %rdi  # 2068a0 <main>
  2067a8: ff 15 f2 1c 00 00            	callq	*7410(%rip)  # 2084a0 <vectoradd_hip.cpp+0x2084a0>
  2067ae: f4                           	hlt

00000000002067af <.annobin_init.c_end>:
  2067af: 90                           	nop

00000000002067b0 <_dl_relocate_static_pie>:
  2067b0: f3 0f 1e fa                  	endbr64
  2067b4: c3                           	retq

00000000002067b5 <.annobin_static_reloc.c_end.unlikely>:
  2067b5: cc                           	int3
  2067b6: cc                           	int3
  2067b7: cc                           	int3
  2067b8: cc                           	int3
  2067b9: cc                           	int3
  2067ba: cc                           	int3
  2067bb: cc                           	int3
  2067bc: cc                           	int3
  2067bd: cc                           	int3
  2067be: cc                           	int3
  2067bf: cc                           	int3

00000000002067c0 <deregister_tm_clones>:
  2067c0: 48 8d 3d 01 2d 00 00         	leaq	11521(%rip), %rdi  # 2094c8 <__TMC_LIST__>
  2067c7: 48 8d 05 fa 2c 00 00         	leaq	11514(%rip), %rax  # 2094c8 <__TMC_LIST__>
  2067ce: 48 39 f8                     	cmpq	%rdi, %rax
  2067d1: 74 15                        	je	0x2067e8 <deregister_tm_clones+0x28>
  2067d3: 48 8b 05 d6 1c 00 00         	movq	7382(%rip), %rax  # 2084b0 <vectoradd_hip.cpp+0x2084b0>
  2067da: 48 85 c0                     	testq	%rax, %rax
  2067dd: 74 09                        	je	0x2067e8 <deregister_tm_clones+0x28>
  2067df: ff e0                        	jmpq	*%rax
  2067e1: 0f 1f 80 00 00 00 00         	nopl	(%rax)
  2067e8: c3                           	retq
  2067e9: 0f 1f 80 00 00 00 00         	nopl	(%rax)

00000000002067f0 <register_tm_clones>:
  2067f0: 48 8d 3d d1 2c 00 00         	leaq	11473(%rip), %rdi  # 2094c8 <__TMC_LIST__>
  2067f7: 48 8d 35 ca 2c 00 00         	leaq	11466(%rip), %rsi  # 2094c8 <__TMC_LIST__>
  2067fe: 48 29 fe                     	subq	%rdi, %rsi
  206801: 48 89 f0                     	movq	%rsi, %rax
  206804: 48 c1 ee 3f                  	shrq	$63, %rsi
  206808: 48 c1 f8 03                  	sarq	$3, %rax
  20680c: 48 01 c6                     	addq	%rax, %rsi
  20680f: 48 d1 fe                     	sarq	%rsi
  206812: 74 14                        	je	0x206828 <register_tm_clones+0x38>
  206814: 48 8b 05 9d 1c 00 00         	movq	7325(%rip), %rax  # 2084b8 <vectoradd_hip.cpp+0x2084b8>
  20681b: 48 85 c0                     	testq	%rax, %rax
  20681e: 74 08                        	je	0x206828 <register_tm_clones+0x38>
  206820: ff e0                        	jmpq	*%rax
  206822: 66 0f 1f 44 00 00            	nopw	(%rax,%rax)
  206828: c3                           	retq
  206829: 0f 1f 80 00 00 00 00         	nopl	(%rax)

0000000000206830 <__do_global_dtors_aux>:
  206830: f3 0f 1e fa                  	endbr64
  206834: 80 3d 85 2d 00 00 00         	cmpb	$0, 11653(%rip)  # 2095c0 <completed.0>
  20683b: 75 13                        	jne	0x206850 <__do_global_dtors_aux+0x20>
  20683d: 55                           	pushq	%rbp
  20683e: 48 89 e5                     	movq	%rsp, %rbp
  206841: e8 7a ff ff ff               	callq	0x2067c0 <deregister_tm_clones>
  206846: c6 05 73 2d 00 00 01         	movb	$1, 11635(%rip)  # 2095c0 <completed.0>
  20684d: 5d                           	popq	%rbp
  20684e: c3                           	retq
  20684f: 90                           	nop
  206850: c3                           	retq
  206851: 66 66 2e 0f 1f 84 00 00 00 00 00     	nopw	%cs:(%rax,%rax)
  20685c: 0f 1f 40 00                  	nopl	(%rax)

0000000000206860 <frame_dummy>:
  206860: f3 0f 1e fa                  	endbr64
  206864: eb 8a                        	jmp	0x2067f0 <register_tm_clones>
  206866: cc                           	int3
  206867: cc                           	int3
  206868: cc                           	int3
  206869: cc                           	int3
  20686a: cc                           	int3
  20686b: cc                           	int3
  20686c: cc                           	int3
  20686d: cc                           	int3
  20686e: cc                           	int3
  20686f: cc                           	int3

0000000000206870 <_Z8get_timev>:
  206870: 48 83 ec 18                  	subq	$24, %rsp
  206874: 48 8d 7c 24 08               	leaq	8(%rsp), %rdi
  206879: 31 f6                        	xorl	%esi, %esi
  20687b: e8 70 08 00 00               	callq	0x2070f0 <gettimeofday@plt>
  206880: 48 69 44 24 08 40 42 0f 00   	imulq	$1000000, 8(%rsp), %rax
  206889: 48 03 44 24 10               	addq	16(%rsp), %rax
  20688e: 48 83 c4 18                  	addq	$24, %rsp
  206892: c3                           	retq
  206893: 66 2e 0f 1f 84 00 00 00 00 00	nopw	%cs:(%rax,%rax)
  20689d: 0f 1f 00                     	nopl	(%rax)

00000000002068a0 <main>:
  2068a0: 41 57                        	pushq	%r15
  2068a2: 41 56                        	pushq	%r14
  2068a4: 41 55                        	pushq	%r13
  2068a6: 41 54                        	pushq	%r12
  2068a8: 53                           	pushq	%rbx
  2068a9: 48 81 ec 40 03 00 00         	subq	$832, %rsp
  2068b0: 48 8d 7c 24 28               	leaq	40(%rsp), %rdi
  2068b5: 31 f6                        	xorl	%esi, %esi
  2068b7: e8 44 08 00 00               	callq	0x207100 <hipGetDeviceProperties@plt>
  2068bc: bf e0 95 20 00               	movl	$2135520, %edi
  2068c1: be bb 0f 20 00               	movl	$2101179, %esi
  2068c6: ba 0e 00 00 00               	movl	$14, %edx
  2068cb: e8 40 08 00 00               	callq	0x207110 <_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@plt>
  2068d0: 8b b4 24 74 01 00 00         	movl	372(%rsp), %esi
  2068d7: bf e0 95 20 00               	movl	$2135520, %edi
  2068dc: e8 3f 08 00 00               	callq	0x207120 <_ZNSolsEi@plt>
  2068e1: 49 89 c6                     	movq	%rax, %r14
  2068e4: 48 8b 00                     	movq	(%rax), %rax
  2068e7: 48 8b 40 e8                  	movq	-24(%rax), %rax
  2068eb: 49 8b 9c 06 f0 00 00 00      	movq	240(%r14,%rax), %rbx
  2068f3: 48 85 db                     	testq	%rbx, %rbx
  2068f6: 0f 84 f9 04 00 00            	je	0x206df5 <main+0x555>
  2068fc: 80 7b 38 00                  	cmpb	$0, 56(%rbx)
  206900: 74 05                        	je	0x206907 <main+0x67>
  206902: 8a 43 43                     	movb	67(%rbx), %al
  206905: eb 16                        	jmp	0x20691d <main+0x7d>
  206907: 48 89 df                     	movq	%rbx, %rdi
  20690a: e8 21 08 00 00               	callq	0x207130 <_ZNKSt5ctypeIcE13_M_widen_initEv@plt>
  20690f: 48 8b 03                     	movq	(%rbx), %rax
  206912: 48 89 df                     	movq	%rbx, %rdi
  206915: be 0a 00 00 00               	movl	$10, %esi
  20691a: ff 50 30                     	callq	*48(%rax)
  20691d: 0f be f0                     	movsbl	%al, %esi
  206920: 4c 89 f7                     	movq	%r14, %rdi
  206923: e8 18 08 00 00               	callq	0x207140 <_ZNSo3putEc@plt>
  206928: 48 89 c7                     	movq	%rax, %rdi
  20692b: e8 20 08 00 00               	callq	0x207150 <_ZNSo5flushEv@plt>
  206930: bf e0 95 20 00               	movl	$2135520, %edi
  206935: be 00 0e 20 00               	movl	$2100736, %esi
  20693a: ba 0e 00 00 00               	movl	$14, %edx
  20693f: e8 cc 07 00 00               	callq	0x207110 <_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@plt>
  206944: 8b b4 24 70 01 00 00         	movl	368(%rsp), %esi
  20694b: bf e0 95 20 00               	movl	$2135520, %edi
  206950: e8 cb 07 00 00               	callq	0x207120 <_ZNSolsEi@plt>
  206955: 49 89 c6                     	movq	%rax, %r14
  206958: 48 8b 00                     	movq	(%rax), %rax
  20695b: 48 8b 40 e8                  	movq	-24(%rax), %rax
  20695f: 49 8b 9c 06 f0 00 00 00      	movq	240(%r14,%rax), %rbx
  206967: 48 85 db                     	testq	%rbx, %rbx
  20696a: 0f 84 85 04 00 00            	je	0x206df5 <main+0x555>
  206970: 80 7b 38 00                  	cmpb	$0, 56(%rbx)
  206974: 74 05                        	je	0x20697b <main+0xdb>
  206976: 8a 43 43                     	movb	67(%rbx), %al
  206979: eb 16                        	jmp	0x206991 <main+0xf1>
  20697b: 48 89 df                     	movq	%rbx, %rdi
  20697e: e8 ad 07 00 00               	callq	0x207130 <_ZNKSt5ctypeIcE13_M_widen_initEv@plt>
  206983: 48 8b 03                     	movq	(%rbx), %rax
  206986: 48 89 df                     	movq	%rbx, %rdi
  206989: be 0a 00 00 00               	movl	$10, %esi
  20698e: ff 50 30                     	callq	*48(%rax)
  206991: 0f be f0                     	movsbl	%al, %esi
  206994: 4c 89 f7                     	movq	%r14, %rdi
  206997: e8 a4 07 00 00               	callq	0x207140 <_ZNSo3putEc@plt>
  20699c: 48 89 c7                     	movq	%rax, %rdi
  20699f: e8 ac 07 00 00               	callq	0x207150 <_ZNSo5flushEv@plt>
  2069a4: bf e0 95 20 00               	movl	$2135520, %edi
  2069a9: be 4b 0f 20 00               	movl	$2101067, %esi
  2069ae: ba 11 00 00 00               	movl	$17, %edx
  2069b3: e8 58 07 00 00               	callq	0x207110 <_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@plt>
  2069b8: 48 8d 5c 24 28               	leaq	40(%rsp), %rbx
  2069bd: 48 89 df                     	movq	%rbx, %rdi
  2069c0: e8 9b 07 00 00               	callq	0x207160 <strlen@plt>
  2069c5: bf e0 95 20 00               	movl	$2135520, %edi
  2069ca: 48 89 de                     	movq	%rbx, %rsi
  2069cd: 48 89 c2                     	movq	%rax, %rdx
  2069d0: e8 3b 07 00 00               	callq	0x207110 <_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@plt>
  2069d5: 48 8b 05 04 2c 00 00         	movq	11268(%rip), %rax  # 2095e0 <_ZSt4cout>
  2069dc: 48 8b 40 e8                  	movq	-24(%rax), %rax
  2069e0: 48 8b 98 d0 96 20 00         	movq	2135760(%rax), %rbx
  2069e7: 48 85 db                     	testq	%rbx, %rbx
  2069ea: 0f 84 05 04 00 00            	je	0x206df5 <main+0x555>
  2069f0: 80 7b 38 00                  	cmpb	$0, 56(%rbx)
  2069f4: 74 05                        	je	0x2069fb <main+0x15b>
  2069f6: 8a 43 43                     	movb	67(%rbx), %al
  2069f9: eb 16                        	jmp	0x206a11 <main+0x171>
  2069fb: 48 89 df                     	movq	%rbx, %rdi
  2069fe: e8 2d 07 00 00               	callq	0x207130 <_ZNKSt5ctypeIcE13_M_widen_initEv@plt>
  206a03: 48 8b 03                     	movq	(%rbx), %rax
  206a06: 48 89 df                     	movq	%rbx, %rdi
  206a09: be 0a 00 00 00               	movl	$10, %esi
  206a0e: ff 50 30                     	callq	*48(%rax)
  206a11: 0f be f0                     	movsbl	%al, %esi
  206a14: bf e0 95 20 00               	movl	$2135520, %edi
  206a19: e8 22 07 00 00               	callq	0x207140 <_ZNSo3putEc@plt>
  206a1e: 48 89 c7                     	movq	%rax, %rdi
  206a21: e8 2a 07 00 00               	callq	0x207150 <_ZNSo5flushEv@plt>
  206a26: bf e0 95 20 00               	movl	$2135520, %edi
  206a2b: be 0d 10 20 00               	movl	$2101261, %esi
  206a30: ba 1a 00 00 00               	movl	$26, %edx
  206a35: e8 d6 06 00 00               	callq	0x207110 <_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@plt>
  206a3a: 48 8b 05 9f 2b 00 00         	movq	11167(%rip), %rax  # 2095e0 <_ZSt4cout>
  206a41: 48 8b 40 e8                  	movq	-24(%rax), %rax
  206a45: 48 8b 98 d0 96 20 00         	movq	2135760(%rax), %rbx
  206a4c: 48 85 db                     	testq	%rbx, %rbx
  206a4f: 0f 84 a0 03 00 00            	je	0x206df5 <main+0x555>
  206a55: 80 7b 38 00                  	cmpb	$0, 56(%rbx)
  206a59: 74 05                        	je	0x206a60 <main+0x1c0>
  206a5b: 8a 43 43                     	movb	67(%rbx), %al
  206a5e: eb 16                        	jmp	0x206a76 <main+0x1d6>
  206a60: 48 89 df                     	movq	%rbx, %rdi
  206a63: e8 c8 06 00 00               	callq	0x207130 <_ZNKSt5ctypeIcE13_M_widen_initEv@plt>
  206a68: 48 8b 03                     	movq	(%rbx), %rax
  206a6b: 48 89 df                     	movq	%rbx, %rdi
  206a6e: be 0a 00 00 00               	movl	$10, %esi
  206a73: ff 50 30                     	callq	*48(%rax)
  206a76: 0f be f0                     	movsbl	%al, %esi
  206a79: bf e0 95 20 00               	movl	$2135520, %edi
  206a7e: e8 bd 06 00 00               	callq	0x207140 <_ZNSo3putEc@plt>
  206a83: 48 89 c7                     	movq	%rax, %rdi
  206a86: e8 c5 06 00 00               	callq	0x207150 <_ZNSo5flushEv@plt>
  206a8b: bf 00 04 00 00               	movl	$1024, %edi
  206a90: e8 db 06 00 00               	callq	0x207170 <malloc@plt>
  206a95: 49 89 c6                     	movq	%rax, %r14
  206a98: bf 00 04 00 00               	movl	$1024, %edi
  206a9d: e8 ce 06 00 00               	callq	0x207170 <malloc@plt>
  206aa2: 49 89 c7                     	movq	%rax, %r15
  206aa5: bf 00 04 00 00               	movl	$1024, %edi
  206aaa: e8 c1 06 00 00               	callq	0x207170 <malloc@plt>
  206aaf: 48 89 c3                     	movq	%rax, %rbx
  206ab2: 66 0f 6f 05 16 a3 ff ff      	movdqa	-23786(%rip), %xmm0  # 200dd0 <__dso_handle+0x38>
  206aba: b8 0c 00 00 00               	movl	$12, %eax
  206abf: 66 0f 6f 0d 19 a3 ff ff      	movdqa	-23783(%rip), %xmm1  # 200de0 <__dso_handle+0x48>
  206ac7: 0f 28 15 e2 a2 ff ff         	movaps	-23838(%rip), %xmm2  # 200db0 <__dso_handle+0x18>
  206ace: 66 0f 6f 1d ea a2 ff ff      	movdqa	-23830(%rip), %xmm3  # 200dc0 <__dso_handle+0x28>
  206ad6: 66 0f 6f 25 12 a3 ff ff      	movdqa	-23790(%rip), %xmm4  # 200df0 <__dso_handle+0x58>
  206ade: 66 0f 6f 2d ba a2 ff ff      	movdqa	-23878(%rip), %xmm5  # 200da0 <__dso_handle+0x8>
  206ae6: 66 2e 0f 1f 84 00 00 00 00 00	nopw	%cs:(%rax,%rax)
  206af0: 66 0f 6f f0                  	movdqa	%xmm0, %xmm6
  206af4: 66 0f fe f1                  	paddd	%xmm1, %xmm6
  206af8: 0f 5b f8                     	cvtdq2ps	%xmm0, %xmm7
  206afb: 0f 5b f6                     	cvtdq2ps	%xmm6, %xmm6
  206afe: 41 0f 29 7c 87 d0            	movaps	%xmm7, -48(%r15,%rax,4)
  206b04: 41 0f 29 74 87 e0            	movaps	%xmm6, -32(%r15,%rax,4)
  206b0a: 0f 59 fa                     	mulps	%xmm2, %xmm7
  206b0d: 0f 59 f2                     	mulps	%xmm2, %xmm6
  206b10: 0f 29 7c 83 d0               	movaps	%xmm7, -48(%rbx,%rax,4)
  206b15: 0f 29 74 83 e0               	movaps	%xmm6, -32(%rbx,%rax,4)
  206b1a: 66 0f 6f f0                  	movdqa	%xmm0, %xmm6
  206b1e: 66 0f fe f3                  	paddd	%xmm3, %xmm6
  206b22: 66 0f 6f f8                  	movdqa	%xmm0, %xmm7
  206b26: 66 0f fe fc                  	paddd	%xmm4, %xmm7
  206b2a: 0f 5b f6                     	cvtdq2ps	%xmm6, %xmm6
  206b2d: 0f 5b ff                     	cvtdq2ps	%xmm7, %xmm7
  206b30: 41 0f 29 74 87 f0            	movaps	%xmm6, -16(%r15,%rax,4)
  206b36: 41 0f 29 3c 87               	movaps	%xmm7, (%r15,%rax,4)
  206b3b: 0f 59 f2                     	mulps	%xmm2, %xmm6
  206b3e: 0f 59 fa                     	mulps	%xmm2, %xmm7
  206b41: 0f 29 74 83 f0               	movaps	%xmm6, -16(%rbx,%rax,4)
  206b46: 0f 29 3c 83                  	movaps	%xmm7, (%rbx,%rax,4)
  206b4a: 66 0f fe c5                  	paddd	%xmm5, %xmm0
  206b4e: 48 83 c0 10                  	addq	$16, %rax
  206b52: 48 3d 0c 01 00 00            	cmpq	$268, %rax
  206b58: 75 96                        	jne	0x206af0 <main+0x250>
  206b5a: 48 8d 7c 24 10               	leaq	16(%rsp), %rdi
  206b5f: be 00 04 00 00               	movl	$1024, %esi
  206b64: e8 17 06 00 00               	callq	0x207180 <hipMalloc@plt>
  206b69: 85 c0                        	testl	%eax, %eax
  206b6b: 0f 85 89 02 00 00            	jne	0x206dfa <main+0x55a>
  206b71: 48 8d 7c 24 08               	leaq	8(%rsp), %rdi
  206b76: be 00 04 00 00               	movl	$1024, %esi
  206b7b: e8 00 06 00 00               	callq	0x207180 <hipMalloc@plt>
  206b80: 85 c0                        	testl	%eax, %eax
  206b82: 0f 85 8b 02 00 00            	jne	0x206e13 <main+0x573>
  206b88: 48 89 e7                     	movq	%rsp, %rdi
  206b8b: be 00 04 00 00               	movl	$1024, %esi
  206b90: e8 eb 05 00 00               	callq	0x207180 <hipMalloc@plt>
  206b95: 85 c0                        	testl	%eax, %eax
  206b97: 0f 85 8f 02 00 00            	jne	0x206e2c <main+0x58c>
  206b9d: 48 8b 7c 24 08               	movq	8(%rsp), %rdi
  206ba2: ba 00 04 00 00               	movl	$1024, %edx
  206ba7: 4c 89 fe                     	movq	%r15, %rsi
  206baa: b9 01 00 00 00               	movl	$1, %ecx
  206baf: e8 dc 05 00 00               	callq	0x207190 <hipMemcpy@plt>
  206bb4: 85 c0                        	testl	%eax, %eax
  206bb6: 0f 85 89 02 00 00            	jne	0x206e45 <main+0x5a5>
  206bbc: 48 8b 3c 24                  	movq	(%rsp), %rdi
  206bc0: ba 00 04 00 00               	movl	$1024, %edx
  206bc5: 48 89 de                     	movq	%rbx, %rsi
  206bc8: b9 01 00 00 00               	movl	$1, %ecx
  206bcd: e8 be 05 00 00               	callq	0x207190 <hipMemcpy@plt>
  206bd2: 85 c0                        	testl	%eax, %eax
  206bd4: 0f 85 84 02 00 00            	jne	0x206e5e <main+0x5be>
  206bda: 48 8d 7c 24 18               	leaq	24(%rsp), %rdi
  206bdf: 31 f6                        	xorl	%esi, %esi
  206be1: e8 0a 05 00 00               	callq	0x2070f0 <gettimeofday@plt>
  206be6: 4c 8b 6c 24 18               	movq	24(%rsp), %r13
  206beb: 4c 8b 64 24 20               	movq	32(%rsp), %r12
  206bf0: 48 8b 74 24 10               	movq	16(%rsp), %rsi
  206bf5: 48 8b 54 24 08               	movq	8(%rsp), %rdx
  206bfa: 48 8b 0c 24                  	movq	(%rsp), %rcx
  206bfe: bf 68 0f 20 00               	movl	$2101096, %edi
  206c03: 31 c0                        	xorl	%eax, %eax
  206c05: e8 96 05 00 00               	callq	0x2071a0 <printf@plt>
  206c0a: 48 bf 02 00 00 00 02 00 00 00	movabsq	$8589934594, %rdi
  206c14: 48 ba 08 00 00 00 08 00 00 00	movabsq	$34359738376, %rdx
  206c1e: be 01 00 00 00               	movl	$1, %esi
  206c23: b9 01 00 00 00               	movl	$1, %ecx
  206c28: 45 31 c0                     	xorl	%r8d, %r8d
  206c2b: 45 31 c9                     	xorl	%r9d, %r9d
  206c2e: e8 7d 05 00 00               	callq	0x2071b0 <__hipPushCallConfiguration@plt>
  206c33: 85 c0                        	testl	%eax, %eax
  206c35: 75 1e                        	jne	0x206c55 <main+0x3b5>
  206c37: 48 8b 7c 24 10               	movq	16(%rsp), %rdi
  206c3c: 48 8b 74 24 08               	movq	8(%rsp), %rsi
  206c41: 48 8b 14 24                  	movq	(%rsp), %rdx
  206c45: b9 10 00 00 00               	movl	$16, %ecx
  206c4a: 41 b8 10 00 00 00            	movl	$16, %r8d
  206c50: e8 ab 02 00 00               	callq	0x206f00 <_Z27__device_stub__vectoradd_k1PfPKfS1_ii>
  206c55: 48 8d 7c 24 18               	leaq	24(%rsp), %rdi
  206c5a: 31 f6                        	xorl	%esi, %esi
  206c5c: e8 8f 04 00 00               	callq	0x2070f0 <gettimeofday@plt>
  206c61: 48 8b 44 24 18               	movq	24(%rsp), %rax
  206c66: 4c 29 e8                     	subq	%r13, %rax
  206c69: 48 8b 4c 24 20               	movq	32(%rsp), %rcx
  206c6e: 4c 29 e1                     	subq	%r12, %rcx
  206c71: 48 69 c0 40 42 0f 00         	imulq	$1000000, %rax, %rax
  206c78: 48 01 c8                     	addq	%rcx, %rax
  206c7b: 0f 57 c0                     	xorps	%xmm0, %xmm0
  206c7e: f3 48 0f 2a c0               	cvtsi2ss	%rax, %xmm0
  206c83: f3 0f 5e 05 09 a1 ff ff      	divss	-24311(%rip), %xmm0  # 200d94 <_IO_stdin_used+0x4>
  206c8b: f3 0f 5a c0                  	cvtss2sd	%xmm0, %xmm0
  206c8f: bf 2e 0e 20 00               	movl	$2100782, %edi
  206c94: b0 01                        	movb	$1, %al
  206c96: e8 05 05 00 00               	callq	0x2071a0 <printf@plt>
  206c9b: 48 8b 74 24 10               	movq	16(%rsp), %rsi
  206ca0: ba 00 04 00 00               	movl	$1024, %edx
  206ca5: 4c 89 f7                     	movq	%r14, %rdi
  206ca8: b9 02 00 00 00               	movl	$2, %ecx
  206cad: e8 de 04 00 00               	callq	0x207190 <hipMemcpy@plt>
  206cb2: 85 c0                        	testl	%eax, %eax
  206cb4: 0f 85 bd 01 00 00            	jne	0x206e77 <main+0x5d7>
  206cba: f3 41 0f 10 06               	movss	(%r14), %xmm0
  206cbf: f3 0f 5a c0                  	cvtss2sd	%xmm0, %xmm0
  206cc3: bf 37 0e 20 00               	movl	$2100791, %edi
  206cc8: b0 01                        	movb	$1, %al
  206cca: e8 d1 04 00 00               	callq	0x2071a0 <printf@plt>
  206ccf: f3 41 0f 10 46 04            	movss	4(%r14), %xmm0
  206cd5: f3 0f 5a c0                  	cvtss2sd	%xmm0, %xmm0
  206cd9: bf 37 0e 20 00               	movl	$2100791, %edi
  206cde: b0 01                        	movb	$1, %al
  206ce0: e8 bb 04 00 00               	callq	0x2071a0 <printf@plt>
  206ce5: f3 41 0f 10 46 08            	movss	8(%r14), %xmm0
  206ceb: f3 0f 5a c0                  	cvtss2sd	%xmm0, %xmm0
  206cef: bf 37 0e 20 00               	movl	$2100791, %edi
  206cf4: b0 01                        	movb	$1, %al
  206cf6: e8 a5 04 00 00               	callq	0x2071a0 <printf@plt>
  206cfb: f3 41 0f 10 46 0c            	movss	12(%r14), %xmm0
  206d01: f3 0f 5a c0                  	cvtss2sd	%xmm0, %xmm0
  206d05: bf 37 0e 20 00               	movl	$2100791, %edi
  206d0a: b0 01                        	movb	$1, %al
  206d0c: e8 8f 04 00 00               	callq	0x2071a0 <printf@plt>
  206d11: f3 41 0f 10 46 10            	movss	16(%r14), %xmm0
  206d17: f3 0f 5a c0                  	cvtss2sd	%xmm0, %xmm0
  206d1b: bf 37 0e 20 00               	movl	$2100791, %edi
  206d20: b0 01                        	movb	$1, %al
  206d22: e8 79 04 00 00               	callq	0x2071a0 <printf@plt>
  206d27: f3 41 0f 10 46 14            	movss	20(%r14), %xmm0
  206d2d: f3 0f 5a c0                  	cvtss2sd	%xmm0, %xmm0
  206d31: bf 37 0e 20 00               	movl	$2100791, %edi
  206d36: b0 01                        	movb	$1, %al
  206d38: e8 63 04 00 00               	callq	0x2071a0 <printf@plt>
  206d3d: f3 41 0f 10 46 18            	movss	24(%r14), %xmm0
  206d43: f3 0f 5a c0                  	cvtss2sd	%xmm0, %xmm0
  206d47: bf 37 0e 20 00               	movl	$2100791, %edi
  206d4c: b0 01                        	movb	$1, %al
  206d4e: e8 4d 04 00 00               	callq	0x2071a0 <printf@plt>
  206d53: f3 41 0f 10 46 1c            	movss	28(%r14), %xmm0
  206d59: f3 0f 5a c0                  	cvtss2sd	%xmm0, %xmm0
  206d5d: bf 37 0e 20 00               	movl	$2100791, %edi
  206d62: b0 01                        	movb	$1, %al
  206d64: e8 37 04 00 00               	callq	0x2071a0 <printf@plt>
  206d69: f3 41 0f 10 46 20            	movss	32(%r14), %xmm0
  206d6f: f3 0f 5a c0                  	cvtss2sd	%xmm0, %xmm0
  206d73: bf 37 0e 20 00               	movl	$2100791, %edi
  206d78: b0 01                        	movb	$1, %al
  206d7a: e8 21 04 00 00               	callq	0x2071a0 <printf@plt>
  206d7f: f3 41 0f 10 46 24            	movss	36(%r14), %xmm0
  206d85: f3 0f 5a c0                  	cvtss2sd	%xmm0, %xmm0
  206d89: bf 37 0e 20 00               	movl	$2100791, %edi
  206d8e: b0 01                        	movb	$1, %al
  206d90: e8 0b 04 00 00               	callq	0x2071a0 <printf@plt>
  206d95: 48 8b 7c 24 10               	movq	16(%rsp), %rdi
  206d9a: e8 21 04 00 00               	callq	0x2071c0 <hipFree@plt>
  206d9f: 85 c0                        	testl	%eax, %eax
  206da1: 0f 85 e9 00 00 00            	jne	0x206e90 <main+0x5f0>
  206da7: 48 8b 7c 24 08               	movq	8(%rsp), %rdi
  206dac: e8 0f 04 00 00               	callq	0x2071c0 <hipFree@plt>
  206db1: 85 c0                        	testl	%eax, %eax
  206db3: 0f 85 f0 00 00 00            	jne	0x206ea9 <main+0x609>
  206db9: 48 8b 3c 24                  	movq	(%rsp), %rdi
  206dbd: e8 fe 03 00 00               	callq	0x2071c0 <hipFree@plt>
  206dc2: 85 c0                        	testl	%eax, %eax
  206dc4: 0f 85 f8 00 00 00            	jne	0x206ec2 <main+0x622>
  206dca: 4c 89 f7                     	movq	%r14, %rdi
  206dcd: e8 fe 03 00 00               	callq	0x2071d0 <free@plt>
  206dd2: 4c 89 ff                     	movq	%r15, %rdi
  206dd5: e8 f6 03 00 00               	callq	0x2071d0 <free@plt>
  206dda: 48 89 df                     	movq	%rbx, %rdi
  206ddd: e8 ee 03 00 00               	callq	0x2071d0 <free@plt>
  206de2: 31 c0                        	xorl	%eax, %eax
  206de4: 48 81 c4 40 03 00 00         	addq	$832, %rsp
  206deb: 5b                           	popq	%rbx
  206dec: 41 5c                        	popq	%r12
  206dee: 41 5d                        	popq	%r13
  206df0: 41 5e                        	popq	%r14
  206df2: 41 5f                        	popq	%r15
  206df4: c3                           	retq
  206df5: e8 e6 03 00 00               	callq	0x2071e0 <_ZSt16__throw_bad_castv@plt>
  206dfa: bf b1 0e 20 00               	movl	$2100913, %edi
  206dff: be 7e 10 20 00               	movl	$2101374, %esi
  206e04: b9 5d 0f 20 00               	movl	$2101085, %ecx
  206e09: ba 93 00 00 00               	movl	$147, %edx
  206e0e: e8 dd 03 00 00               	callq	0x2071f0 <__assert_fail@plt>
  206e13: bf 90 10 20 00               	movl	$2101392, %edi
  206e18: be 7e 10 20 00               	movl	$2101374, %esi
  206e1d: b9 5d 0f 20 00               	movl	$2101085, %ecx
  206e22: ba 94 00 00 00               	movl	$148, %edx
  206e27: e8 c4 03 00 00               	callq	0x2071f0 <__assert_fail@plt>
  206e2c: bf ca 0f 20 00               	movl	$2101194, %edi
  206e31: be 7e 10 20 00               	movl	$2101374, %esi
  206e36: b9 5d 0f 20 00               	movl	$2101085, %ecx
  206e3b: ba 95 00 00 00               	movl	$149, %edx
  206e40: e8 ab 03 00 00               	callq	0x2071f0 <__assert_fail@plt>
  206e45: bf 28 10 20 00               	movl	$2101288, %edi
  206e4a: be 7e 10 20 00               	movl	$2101374, %esi
  206e4f: b9 5d 0f 20 00               	movl	$2101085, %ecx
  206e54: ba 97 00 00 00               	movl	$151, %edx
  206e59: e8 92 03 00 00               	callq	0x2071f0 <__assert_fail@plt>
  206e5e: bf 3c 0e 20 00               	movl	$2100796, %edi
  206e63: be 7e 10 20 00               	movl	$2101374, %esi
  206e68: b9 5d 0f 20 00               	movl	$2101085, %ecx
  206e6d: ba 98 00 00 00               	movl	$152, %edx
  206e72: e8 79 03 00 00               	callq	0x2071f0 <__assert_fail@plt>
  206e77: bf f4 0e 20 00               	movl	$2100980, %edi
  206e7c: be 7e 10 20 00               	movl	$2101374, %esi
  206e81: b9 5d 0f 20 00               	movl	$2101085, %ecx
  206e86: ba b2 00 00 00               	movl	$178, %edx
  206e8b: e8 60 03 00 00               	callq	0x2071f0 <__assert_fail@plt>
  206e90: bf 92 0e 20 00               	movl	$2100882, %edi
  206e95: be 7e 10 20 00               	movl	$2101374, %esi
  206e9a: b9 5d 0f 20 00               	movl	$2101085, %ecx
  206e9f: ba bb 00 00 00               	movl	$187, %edx
  206ea4: e8 47 03 00 00               	callq	0x2071f0 <__assert_fail@plt>
  206ea9: bf 81 0f 20 00               	movl	$2101121, %edi
  206eae: be 7e 10 20 00               	movl	$2101374, %esi
  206eb3: b9 5d 0f 20 00               	movl	$2101085, %ecx
  206eb8: ba bc 00 00 00               	movl	$188, %edx
  206ebd: e8 2e 03 00 00               	callq	0x2071f0 <__assert_fail@plt>
  206ec2: bf 0f 0e 20 00               	movl	$2100751, %edi
  206ec7: be 7e 10 20 00               	movl	$2101374, %esi
  206ecc: b9 5d 0f 20 00               	movl	$2101085, %ecx
  206ed1: ba bd 00 00 00               	movl	$189, %edx
  206ed6: e8 15 03 00 00               	callq	0x2071f0 <__assert_fail@plt>
  206edb: cc                           	int3
  206edc: cc                           	int3
  206edd: cc                           	int3
  206ede: cc                           	int3
  206edf: cc                           	int3

0000000000206ee0 <_GLOBAL__sub_I_vectoradd_hip.cpp>:
  206ee0: 50                           	pushq	%rax
  206ee1: bf c1 95 20 00               	movl	$2135489, %edi
  206ee6: e8 15 03 00 00               	callq	0x207200 <_ZNSt8ios_base4InitC1Ev@plt>
  206eeb: bf 10 72 20 00               	movl	$2126352, %edi
  206ef0: be c1 95 20 00               	movl	$2135489, %esi
  206ef5: ba 98 0d 20 00               	movl	$2100632, %edx
  206efa: 58                           	popq	%rax
  206efb: e9 20 03 00 00               	jmp	0x207220 <__cxa_atexit@plt>

0000000000206f00 <_Z27__device_stub__vectoradd_k1PfPKfS1_ii>:
  206f00: e9 fb 90 0f 00               	jmp	0x300000 <vectoradd_hip.cpp+0x300000>
  206f05: 89 7c 24 48                  	movl	%edi, 72(%rsp)
  206f09: 48 89 74 24 40               	movq	%rsi, 64(%rsp)
  206f0e: 48 89 54 24 38               	movq	%rdx, 56(%rsp)
  206f13: 89 4c 24 04                  	movl	%ecx, 4(%rsp)
  206f17: 44 89 04 24                  	movl	%r8d, (%rsp)
  206f1b: 48 8d 44 24 48               	leaq	72(%rsp), %rax
  206f20: 48 89 44 24 50               	movq	%rax, 80(%rsp)
  206f25: 48 8d 44 24 40               	leaq	64(%rsp), %rax
  206f2a: 48 89 44 24 58               	movq	%rax, 88(%rsp)
  206f2f: 48 8d 44 24 38               	leaq	56(%rsp), %rax
  206f34: 48 89 44 24 60               	movq	%rax, 96(%rsp)
  206f39: 48 8d 44 24 04               	leaq	4(%rsp), %rax
  206f3e: 48 89 44 24 68               	movq	%rax, 104(%rsp)
  206f43: 48 89 e0                     	movq	%rsp, %rax
  206f46: 48 89 44 24 70               	movq	%rax, 112(%rsp)
  206f4b: 48 8d 7c 24 28               	leaq	40(%rsp), %rdi
  206f50: 48 8d 74 24 18               	leaq	24(%rsp), %rsi
  206f55: 48 8d 54 24 10               	leaq	16(%rsp), %rdx
  206f5a: 48 8d 4c 24 08               	leaq	8(%rsp), %rcx
  206f5f: e8 cc 02 00 00               	callq	0x207230 <__hipPopCallConfiguration@plt>
  206f64: e9 ff 90 0f 00               	jmp	0x300068 <vectoradd_hip.cpp+0x300068>
  206f69: 8b 54 24 30                  	movl	48(%rsp), %edx
  206f6d: 48 8b 4c 24 18               	movq	24(%rsp), %rcx
  206f72: 44 8b 44 24 20               	movl	32(%rsp), %r8d
  206f77: 4c 8d 4c 24 50               	leaq	80(%rsp), %r9
  206f7c: bf d8 10 20 00               	movl	$2101464, %edi
  206f81: ff 74 24 08                  	pushq	8(%rsp)
  206f85: ff 74 24 18                  	pushq	24(%rsp)
  206f89: e8 b2 02 00 00               	callq	0x207240 <hipLaunchKernel@plt>
  206f8e: e9 6e 91 0f 00               	jmp	0x300101 <vectoradd_hip.cpp+0x300101>
  206f93: 00 00                        	addb	%al, (%rax)
  206f95: c3                           	retq
  206f96: 66 2e 0f 1f 84 00 00 00 00 00	nopw	%cs:(%rax,%rax)

0000000000206fa0 <__hip_module_ctor>:
  206fa0: 48 83 ec 28                  	subq	$40, %rsp
  206fa4: 48 8b 3d 1d 26 00 00         	movq	9757(%rip), %rdi  # 2095c8 <__hip_gpubin_handle>
  206fab: 48 85 ff                     	testq	%rdi, %rdi
  206fae: 75 14                        	jne	0x206fc4 <__hip_module_ctor+0x24>
  206fb0: bf 60 55 20 00               	movl	$2119008, %edi
  206fb5: e8 96 02 00 00               	callq	0x207250 <__hipRegisterFatBinary@plt>
  206fba: 48 89 c7                     	movq	%rax, %rdi
  206fbd: 48 89 05 04 26 00 00         	movq	%rax, 9732(%rip)  # 2095c8 <__hip_gpubin_handle>
  206fc4: 0f 57 c0                     	xorps	%xmm0, %xmm0
  206fc7: 0f 11 44 24 10               	movups	%xmm0, 16(%rsp)
  206fcc: 0f 11 04 24                  	movups	%xmm0, (%rsp)
  206fd0: be d8 10 20 00               	movl	$2101464, %esi
  206fd5: ba a0 0f 20 00               	movl	$2101152, %edx
  206fda: b9 a0 0f 20 00               	movl	$2101152, %ecx
  206fdf: 41 b8 ff ff ff ff            	movl	$4294967295, %r8d
  206fe5: 45 31 c9                     	xorl	%r9d, %r9d
  206fe8: e8 73 02 00 00               	callq	0x207260 <__hipRegisterFunction@plt>
  206fed: bf 00 70 20 00               	movl	$2125824, %edi
  206ff2: 48 83 c4 28                  	addq	$40, %rsp
  206ff6: e9 a5 00 00 00               	jmp	0x2070a0 <atexit>
  206ffb: 0f 1f 44 00 00               	nopl	(%rax,%rax)

0000000000207000 <__hip_module_dtor>:
  207000: 50                           	pushq	%rax
  207001: 48 8b 3d c0 25 00 00         	movq	9664(%rip), %rdi  # 2095c8 <__hip_gpubin_handle>
  207008: 48 85 ff                     	testq	%rdi, %rdi
  20700b: 74 10                        	je	0x20701d <__hip_module_dtor+0x1d>
  20700d: e8 5e 02 00 00               	callq	0x207270 <__hipUnregisterFatBinary@plt>
  207012: 48 c7 05 ab 25 00 00 00 00 00 00     	movq	$0, 9643(%rip)  # 2095c8 <__hip_gpubin_handle>
  20701d: 58                           	popq	%rax
  20701e: c3                           	retq
  20701f: cc                           	int3

0000000000207020 <__libc_csu_init>:
  207020: f3 0f 1e fa                  	endbr64
  207024: 41 57                        	pushq	%r15
  207026: 49 89 d7                     	movq	%rdx, %r15
  207029: 41 56                        	pushq	%r14
  20702b: 49 89 f6                     	movq	%rsi, %r14
  20702e: 41 55                        	pushq	%r13
  207030: 41 89 fd                     	movl	%edi, %r13d
  207033: 41 54                        	pushq	%r12
  207035: 4c 8d 25 4c 12 00 00         	leaq	4684(%rip), %r12  # 208288 <__init_array_start>
  20703c: 55                           	pushq	%rbp
  20703d: 48 8d 2d 5c 12 00 00         	leaq	4700(%rip), %rbp  # 2082a0 <vectoradd_hip.cpp+0x2082a0>
  207044: 53                           	pushq	%rbx
  207045: 4c 29 e5                     	subq	%r12, %rbp
  207048: 48 83 ec 08                  	subq	$8, %rsp
  20704c: e8 63 00 00 00               	callq	0x2070b4 <_init>
  207051: 48 c1 fd 03                  	sarq	$3, %rbp
  207055: 74 1f                        	je	0x207076 <__libc_csu_init+0x56>
  207057: 31 db                        	xorl	%ebx, %ebx
  207059: 0f 1f 80 00 00 00 00         	nopl	(%rax)
  207060: 4c 89 fa                     	movq	%r15, %rdx
  207063: 4c 89 f6                     	movq	%r14, %rsi
  207066: 44 89 ef                     	movl	%r13d, %edi
  207069: 41 ff 14 dc                  	callq	*(%r12,%rbx,8)
  20706d: 48 83 c3 01                  	addq	$1, %rbx
  207071: 48 39 dd                     	cmpq	%rbx, %rbp
  207074: 75 ea                        	jne	0x207060 <__libc_csu_init+0x40>
  207076: 48 83 c4 08                  	addq	$8, %rsp
  20707a: 5b                           	popq	%rbx
  20707b: 5d                           	popq	%rbp
  20707c: 41 5c                        	popq	%r12
  20707e: 41 5d                        	popq	%r13
  207080: 41 5e                        	popq	%r14
  207082: 41 5f                        	popq	%r15
  207084: c3                           	retq

0000000000207085 <.annobin___libc_csu_init.end>:
  207085: 66 66 2e 0f 1f 84 00 00 00 00 00     	nopw	%cs:(%rax,%rax)

0000000000207090 <__libc_csu_fini>:
  207090: f3 0f 1e fa                  	endbr64
  207094: c3                           	retq

0000000000207095 <.annobin_elf_init.c_end.unlikely>:
  207095: cc                           	int3
  207096: cc                           	int3
  207097: cc                           	int3
  207098: cc                           	int3
  207099: cc                           	int3
  20709a: cc                           	int3
  20709b: cc                           	int3
  20709c: cc                           	int3
  20709d: cc                           	int3
  20709e: cc                           	int3
  20709f: cc                           	int3

00000000002070a0 <atexit>:
  2070a0: f3 0f 1e fa                  	endbr64
  2070a4: 48 8b 15 ed 9c ff ff         	movq	-25363(%rip), %rdx  # 200d98 <__dso_handle>
  2070ab: 31 f6                        	xorl	%esi, %esi
  2070ad: e9 6e 01 00 00               	jmp	0x207220 <__cxa_atexit@plt>

Disassembly of section .init:

00000000002070b4 <_init>:
  2070b4: f3 0f 1e fa                  	endbr64
  2070b8: 48 83 ec 08                  	subq	$8, %rsp
  2070bc: 48 8b 05 e5 13 00 00         	movq	5093(%rip), %rax  # 2084a8 <vectoradd_hip.cpp+0x2084a8>
  2070c3: 48 85 c0                     	testq	%rax, %rax
  2070c6: 74 02                        	je	0x2070ca <_init+0x16>
  2070c8: ff d0                        	callq	*%rax
  2070ca: 48 83 c4 08                  	addq	$8, %rsp
  2070ce: c3                           	retq

Disassembly of section .fini:

00000000002070d0 <_fini>:
  2070d0: f3 0f 1e fa                  	endbr64
  2070d4: 48 83 ec 08                  	subq	$8, %rsp
  2070d8: 48 83 c4 08                  	addq	$8, %rsp
  2070dc: c3                           	retq

Disassembly of section .plt:

00000000002070e0 <.plt>:
  2070e0: ff 35 ea 23 00 00            	pushq	9194(%rip)  # 2094d0 <__TMC_LIST__+0x8>
  2070e6: ff 25 ec 23 00 00            	jmpq	*9196(%rip)  # 2094d8 <__TMC_LIST__+0x10>
  2070ec: 0f 1f 40 00                  	nopl	(%rax)

00000000002070f0 <gettimeofday@plt>:
  2070f0: ff 25 ea 23 00 00            	jmpq	*9194(%rip)  # 2094e0 <__TMC_LIST__+0x18>
  2070f6: 68 00 00 00 00               	pushq	$0
  2070fb: e9 e0 ff ff ff               	jmp	0x2070e0 <.plt>

0000000000207100 <hipGetDeviceProperties@plt>:
  207100: ff 25 e2 23 00 00            	jmpq	*9186(%rip)  # 2094e8 <__TMC_LIST__+0x20>
  207106: 68 01 00 00 00               	pushq	$1
  20710b: e9 d0 ff ff ff               	jmp	0x2070e0 <.plt>

0000000000207110 <_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@plt>:
  207110: ff 25 da 23 00 00            	jmpq	*9178(%rip)  # 2094f0 <__TMC_LIST__+0x28>
  207116: 68 02 00 00 00               	pushq	$2
  20711b: e9 c0 ff ff ff               	jmp	0x2070e0 <.plt>

0000000000207120 <_ZNSolsEi@plt>:
  207120: ff 25 d2 23 00 00            	jmpq	*9170(%rip)  # 2094f8 <__TMC_LIST__+0x30>
  207126: 68 03 00 00 00               	pushq	$3
  20712b: e9 b0 ff ff ff               	jmp	0x2070e0 <.plt>

0000000000207130 <_ZNKSt5ctypeIcE13_M_widen_initEv@plt>:
  207130: ff 25 ca 23 00 00            	jmpq	*9162(%rip)  # 209500 <__TMC_LIST__+0x38>
  207136: 68 04 00 00 00               	pushq	$4
  20713b: e9 a0 ff ff ff               	jmp	0x2070e0 <.plt>

0000000000207140 <_ZNSo3putEc@plt>:
  207140: ff 25 c2 23 00 00            	jmpq	*9154(%rip)  # 209508 <__TMC_LIST__+0x40>
  207146: 68 05 00 00 00               	pushq	$5
  20714b: e9 90 ff ff ff               	jmp	0x2070e0 <.plt>

0000000000207150 <_ZNSo5flushEv@plt>:
  207150: ff 25 ba 23 00 00            	jmpq	*9146(%rip)  # 209510 <__TMC_LIST__+0x48>
  207156: 68 06 00 00 00               	pushq	$6
  20715b: e9 80 ff ff ff               	jmp	0x2070e0 <.plt>

0000000000207160 <strlen@plt>:
  207160: ff 25 b2 23 00 00            	jmpq	*9138(%rip)  # 209518 <__TMC_LIST__+0x50>
  207166: 68 07 00 00 00               	pushq	$7
  20716b: e9 70 ff ff ff               	jmp	0x2070e0 <.plt>

0000000000207170 <malloc@plt>:
  207170: ff 25 aa 23 00 00            	jmpq	*9130(%rip)  # 209520 <__TMC_LIST__+0x58>
  207176: 68 08 00 00 00               	pushq	$8
  20717b: e9 60 ff ff ff               	jmp	0x2070e0 <.plt>

0000000000207180 <hipMalloc@plt>:
  207180: ff 25 a2 23 00 00            	jmpq	*9122(%rip)  # 209528 <__TMC_LIST__+0x60>
  207186: 68 09 00 00 00               	pushq	$9
  20718b: e9 50 ff ff ff               	jmp	0x2070e0 <.plt>

0000000000207190 <hipMemcpy@plt>:
  207190: ff 25 9a 23 00 00            	jmpq	*9114(%rip)  # 209530 <__TMC_LIST__+0x68>
  207196: 68 0a 00 00 00               	pushq	$10
  20719b: e9 40 ff ff ff               	jmp	0x2070e0 <.plt>

00000000002071a0 <printf@plt>:
  2071a0: ff 25 92 23 00 00            	jmpq	*9106(%rip)  # 209538 <__TMC_LIST__+0x70>
  2071a6: 68 0b 00 00 00               	pushq	$11
  2071ab: e9 30 ff ff ff               	jmp	0x2070e0 <.plt>

00000000002071b0 <__hipPushCallConfiguration@plt>:
  2071b0: ff 25 8a 23 00 00            	jmpq	*9098(%rip)  # 209540 <__TMC_LIST__+0x78>
  2071b6: 68 0c 00 00 00               	pushq	$12
  2071bb: e9 20 ff ff ff               	jmp	0x2070e0 <.plt>

00000000002071c0 <hipFree@plt>:
  2071c0: ff 25 82 23 00 00            	jmpq	*9090(%rip)  # 209548 <__TMC_LIST__+0x80>
  2071c6: 68 0d 00 00 00               	pushq	$13
  2071cb: e9 10 ff ff ff               	jmp	0x2070e0 <.plt>

00000000002071d0 <free@plt>:
  2071d0: ff 25 7a 23 00 00            	jmpq	*9082(%rip)  # 209550 <__TMC_LIST__+0x88>
  2071d6: 68 0e 00 00 00               	pushq	$14
  2071db: e9 00 ff ff ff               	jmp	0x2070e0 <.plt>

00000000002071e0 <_ZSt16__throw_bad_castv@plt>:
  2071e0: ff 25 72 23 00 00            	jmpq	*9074(%rip)  # 209558 <__TMC_LIST__+0x90>
  2071e6: 68 0f 00 00 00               	pushq	$15
  2071eb: e9 f0 fe ff ff               	jmp	0x2070e0 <.plt>

00000000002071f0 <__assert_fail@plt>:
  2071f0: ff 25 6a 23 00 00            	jmpq	*9066(%rip)  # 209560 <__TMC_LIST__+0x98>
  2071f6: 68 10 00 00 00               	pushq	$16
  2071fb: e9 e0 fe ff ff               	jmp	0x2070e0 <.plt>

0000000000207200 <_ZNSt8ios_base4InitC1Ev@plt>:
  207200: ff 25 62 23 00 00            	jmpq	*9058(%rip)  # 209568 <__TMC_LIST__+0xa0>
  207206: 68 11 00 00 00               	pushq	$17
  20720b: e9 d0 fe ff ff               	jmp	0x2070e0 <.plt>

0000000000207210 <_ZNSt8ios_base4InitD1Ev@plt>:
  207210: ff 25 5a 23 00 00            	jmpq	*9050(%rip)  # 209570 <__TMC_LIST__+0xa8>
  207216: 68 12 00 00 00               	pushq	$18
  20721b: e9 c0 fe ff ff               	jmp	0x2070e0 <.plt>

0000000000207220 <__cxa_atexit@plt>:
  207220: ff 25 52 23 00 00            	jmpq	*9042(%rip)  # 209578 <__TMC_LIST__+0xb0>
  207226: 68 13 00 00 00               	pushq	$19
  20722b: e9 b0 fe ff ff               	jmp	0x2070e0 <.plt>

0000000000207230 <__hipPopCallConfiguration@plt>:
  207230: ff 25 4a 23 00 00            	jmpq	*9034(%rip)  # 209580 <__TMC_LIST__+0xb8>
  207236: 68 14 00 00 00               	pushq	$20
  20723b: e9 a0 fe ff ff               	jmp	0x2070e0 <.plt>

0000000000207240 <hipLaunchKernel@plt>:
  207240: ff 25 42 23 00 00            	jmpq	*9026(%rip)  # 209588 <__TMC_LIST__+0xc0>
  207246: 68 15 00 00 00               	pushq	$21
  20724b: e9 90 fe ff ff               	jmp	0x2070e0 <.plt>

0000000000207250 <__hipRegisterFatBinary@plt>:
  207250: ff 25 3a 23 00 00            	jmpq	*9018(%rip)  # 209590 <__TMC_LIST__+0xc8>
  207256: 68 16 00 00 00               	pushq	$22
  20725b: e9 80 fe ff ff               	jmp	0x2070e0 <.plt>

0000000000207260 <__hipRegisterFunction@plt>:
  207260: ff 25 32 23 00 00            	jmpq	*9010(%rip)  # 209598 <__TMC_LIST__+0xd0>
  207266: 68 17 00 00 00               	pushq	$23
  20726b: e9 70 fe ff ff               	jmp	0x2070e0 <.plt>

0000000000207270 <__hipUnregisterFatBinary@plt>:
  207270: ff 25 2a 23 00 00            	jmpq	*9002(%rip)  # 2095a0 <__TMC_LIST__+0xd8>
  207276: 68 18 00 00 00               	pushq	$24
  20727b: e9 60 fe ff ff               	jmp	0x2070e0 <.plt>

Disassembly of section .dyninstInst:

0000000000300000 <.dyninstInst>:
  300000: 48 83 ec 78                  	subq	$120, %rsp
  300004: 48 83 ec 20                  	subq	$32, %rsp
  300008: 48 89 7c 24 48               	movq	%rdi, 72(%rsp)
  30000d: 48 89 74 24 40               	movq	%rsi, 64(%rsp)
  300012: 48 89 54 24 38               	movq	%rdx, 56(%rsp)
  300017: 89 4c 24 04                  	movl	%ecx, 4(%rsp)
  30001b: 44 89 04 24                  	movl	%r8d, (%rsp)
  30001f: 48 8d 44 24 48               	leaq	72(%rsp), %rax
  300024: 48 89 44 24 50               	movq	%rax, 80(%rsp)
  300029: 48 8d 44 24 40               	leaq	64(%rsp), %rax
  30002e: 48 89 44 24 58               	movq	%rax, 88(%rsp)
  300033: 48 8d 44 24 38               	leaq	56(%rsp), %rax
  300038: 48 89 44 24 60               	movq	%rax, 96(%rsp)
  30003d: 48 8d 44 24 04               	leaq	4(%rsp), %rax
  300042: 48 89 44 24 68               	movq	%rax, 104(%rsp)
  300047: 48 89 e0                     	movq	%rsp, %rax
  30004a: 48 89 44 24 70               	movq	%rax, 112(%rsp)
  30004f: 48 8d 7c 24 28               	leaq	40(%rsp), %rdi
  300054: 48 8d 74 24 18               	leaq	24(%rsp), %rsi
  300059: 48 8d 54 24 10               	leaq	16(%rsp), %rdx
  30005e: 48 8d 4c 24 08               	leaq	8(%rsp), %rcx
  300063: e8 c8 71 f0 ff               	callq	0x207230 <__hipPopCallConfiguration@plt>
  300068: 48 8b 74 24 28               	movq	40(%rsp), %rsi
  30006d: 8b 54 24 30                  	movl	48(%rsp), %edx
  300071: 48 8b 4c 24 18               	movq	24(%rsp), %rcx
  300076: 44 8b 44 24 20               	movl	32(%rsp), %r8d
  30007b: 89 c8                        	movl	%ecx, %eax
  30007d: 48 c1 c9 20                  	rorq	$32, %rcx
  300081: 0f af c1                     	imull	%ecx, %eax
  300084: 48 c1 c9 20                  	rorq	$32, %rcx
  300088: 41 0f af c0                  	imull	%r8d, %eax
  30008c: c1 e8 06                     	shrl	$6, %eax
  30008f: 89 84 24 90 00 00 00         	movl	%eax, 144(%rsp)
  300096: 0f af c2                     	imull	%edx, %eax
  300099: 0f af c6                     	imull	%esi, %eax
  30009c: 48 c1 ce 20                  	rorq	$32, %rsi
  3000a0: 0f af c6                     	imull	%esi, %eax
  3000a3: 48 c1 ce 20                  	rorq	$32, %rsi
  3000a7: 6b c0 18                     	imull	$24, %eax, %eax
  3000aa: 48 89 c6                     	movq	%rax, %rsi
  3000ad: 48 8d bc 24 88 00 00 00      	leaq	136(%rsp), %rdi
  3000b5: e8 c6 70 f0 ff               	callq	0x207180 <hipMalloc@plt>
  3000ba: 48 8d 84 24 90 00 00 00      	leaq	144(%rsp), %rax
  3000c2: 48 89 84 24 80 00 00 00      	movq	%rax, 128(%rsp)
  3000ca: 48 8d 84 24 88 00 00 00      	leaq	136(%rsp), %rax
  3000d2: 48 89 44 24 78               	movq	%rax, 120(%rsp)
  3000d7: 4c 8d 4c 24 50               	leaq	80(%rsp), %r9
  3000dc: bf d8 10 20 00               	movl	$2101464, %edi
  3000e1: 48 8b 74 24 28               	movq	40(%rsp), %rsi
  3000e6: 8b 54 24 30                  	movl	48(%rsp), %edx
  3000ea: 48 8b 4c 24 18               	movq	24(%rsp), %rcx
  3000ef: 44 8b 44 24 20               	movl	32(%rsp), %r8d
  3000f4: ff 74 24 08                  	pushq	8(%rsp)
  3000f8: ff 74 24 18                  	pushq	24(%rsp)
  3000fc: e8 3f 71 f0 ff               	callq	0x207240 <hipLaunchKernel@plt>
  300101: 48 81 c4 88 00 00 00         	addq	$136, %rsp
  300108: 48 83 c4 20                  	addq	$32, %rsp
  30010c: c3                           	retq
		...
  3001a5: 00 00                        	addb	%al, (%rax)
  3001a7: 00                           	<unknown>
