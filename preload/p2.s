
preload2.so:	file format elf64-x86-64


Disassembly of section .text:

0000000000002970 <deregister_tm_clones>:
    2970: 48 8d 3d a1 2e 00 00         	leaq	11937(%rip), %rdi  # 5818 <_GLOBAL_OFFSET_TABLE_>
    2977: 48 8d 05 9a 2e 00 00         	leaq	11930(%rip), %rax  # 5818 <_GLOBAL_OFFSET_TABLE_>
    297e: 48 39 f8                     	cmpq	%rdi, %rax
    2981: 74 15                        	je	0x2998 <deregister_tm_clones+0x28>
    2983: 48 8b 05 2e 1e 00 00         	movq	7726(%rip), %rax  # 47b8 <puts+0x47b8>
    298a: 48 85 c0                     	testq	%rax, %rax
    298d: 74 09                        	je	0x2998 <deregister_tm_clones+0x28>
    298f: ff e0                        	jmpq	*%rax
    2991: 0f 1f 80 00 00 00 00         	nopl	(%rax)
    2998: c3                           	retq
    2999: 0f 1f 80 00 00 00 00         	nopl	(%rax)

00000000000029a0 <register_tm_clones>:
    29a0: 48 8d 3d 71 2e 00 00         	leaq	11889(%rip), %rdi  # 5818 <_GLOBAL_OFFSET_TABLE_>
    29a7: 48 8d 35 6a 2e 00 00         	leaq	11882(%rip), %rsi  # 5818 <_GLOBAL_OFFSET_TABLE_>
    29ae: 48 29 fe                     	subq	%rdi, %rsi
    29b1: 48 89 f0                     	movq	%rsi, %rax
    29b4: 48 c1 ee 3f                  	shrq	$63, %rsi
    29b8: 48 c1 f8 03                  	sarq	$3, %rax
    29bc: 48 01 c6                     	addq	%rax, %rsi
    29bf: 48 d1 fe                     	sarq	%rsi
    29c2: 74 14                        	je	0x29d8 <register_tm_clones+0x38>
    29c4: 48 8b 05 f5 1d 00 00         	movq	7669(%rip), %rax  # 47c0 <puts+0x47c0>
    29cb: 48 85 c0                     	testq	%rax, %rax
    29ce: 74 08                        	je	0x29d8 <register_tm_clones+0x38>
    29d0: ff e0                        	jmpq	*%rax
    29d2: 66 0f 1f 44 00 00            	nopw	(%rax,%rax)
    29d8: c3                           	retq
    29d9: 0f 1f 80 00 00 00 00         	nopl	(%rax)

00000000000029e0 <__do_global_dtors_aux>:
    29e0: f3 0f 1e fa                  	endbr64
    29e4: 80 3d 15 2f 00 00 00         	cmpb	$0, 12053(%rip)  # 5900 <completed.0>
    29eb: 75 2b                        	jne	0x2a18 <__do_global_dtors_aux+0x38>
    29ed: 55                           	pushq	%rbp
    29ee: 48 83 3d d2 1d 00 00 00      	cmpq	$0, 7634(%rip)  # 47c8 <puts+0x47c8>
    29f6: 48 89 e5                     	movq	%rsp, %rbp
    29f9: 74 0c                        	je	0x2a07 <__do_global_dtors_aux+0x27>
    29fb: 48 8d 3d 7e 1b 00 00         	leaq	7038(%rip), %rdi  # 4580 <__dso_handle>
    2a02: e8 d9 09 00 00               	callq	0x33e0 <__cxa_finalize@plt>
    2a07: e8 64 ff ff ff               	callq	0x2970 <deregister_tm_clones>
    2a0c: c6 05 ed 2e 00 00 01         	movb	$1, 12013(%rip)  # 5900 <completed.0>
    2a13: 5d                           	popq	%rbp
    2a14: c3                           	retq
    2a15: 0f 1f 00                     	nopl	(%rax)
    2a18: c3                           	retq
    2a19: 0f 1f 80 00 00 00 00         	nopl	(%rax)

0000000000002a20 <frame_dummy>:
    2a20: f3 0f 1e fa                  	endbr64
    2a24: e9 77 ff ff ff               	jmp	0x29a0 <register_tm_clones>
    2a29: cc                           	int3
    2a2a: cc                           	int3
    2a2b: cc                           	int3
    2a2c: cc                           	int3
    2a2d: cc                           	int3
    2a2e: cc                           	int3
    2a2f: cc                           	int3

0000000000002a30 <main>:
    2a30: 31 c0                        	xorl	%eax, %eax
    2a32: c3                           	retq
    2a33: 66 2e 0f 1f 84 00 00 00 00 00	nopw	%cs:(%rax,%rax)
    2a3d: 0f 1f 00                     	nopl	(%rax)

0000000000002a40 <_Z41_Z27__device_stub__vectoradd_k1PfPKfS1_iiPfS_PKfii>:
    2a40: c3                           	retq
    2a41: 66 2e 0f 1f 84 00 00 00 00 00	nopw	%cs:(%rax,%rax)
    2a4b: 0f 1f 44 00 00               	nopl	(%rax,%rax)

0000000000002a50 <_Z27__device_stub__vectoradd_k1PfPKfS1_ii>:
    2a50: c3                           	retq
    2a51: 66 2e 0f 1f 84 00 00 00 00 00	nopw	%cs:(%rax,%rax)
    2a5b: 0f 1f 44 00 00               	nopl	(%rax,%rax)

0000000000002a60 <__hipRegisterFatBinary>:
    2a60: 48 83 ec 18                  	subq	$24, %rsp
    2a64: 48 89 fa                     	movq	%rdi, %rdx
    2a67: 48 8b 47 10                  	movq	16(%rdi), %rax
    2a6b: 48 89 44 24 10               	movq	%rax, 16(%rsp)
    2a70: 0f 10 07                     	movups	(%rdi), %xmm0
    2a73: 0f 29 04 24                  	movaps	%xmm0, (%rsp)
    2a77: 48 8b 4f 08                  	movq	8(%rdi), %rcx
    2a7b: 48 8d 3d 61 e8 ff ff         	leaq	-6047(%rip), %rdi  # 12e3 <puts+0x12e3>
    2a82: 48 89 d6                     	movq	%rdx, %rsi
    2a85: 31 c0                        	xorl	%eax, %eax
    2a87: e8 74 09 00 00               	callq	0x3400 <printf@plt>
    2a8c: 48 8b 05 45 1d 00 00         	movq	7493(%rip), %rax  # 47d8 <puts+0x47d8>
    2a93: 48 8b 00                     	movq	(%rax), %rax
    2a96: 48 89 44 24 08               	movq	%rax, 8(%rsp)
    2a9b: 48 8b 05 3e 1d 00 00         	movq	7486(%rip), %rax  # 47e0 <puts+0x47e0>
    2aa2: 48 89 e7                     	movq	%rsp, %rdi
    2aa5: ff 10                        	callq	*(%rax)
    2aa7: 48 83 c4 18                  	addq	$24, %rsp
    2aab: c3                           	retq
    2aac: 0f 1f 40 00                  	nopl	(%rax)

0000000000002ab0 <__hipRegisterFunction>:
    2ab0: 55                           	pushq	%rbp
    2ab1: 41 57                        	pushq	%r15
    2ab3: 41 56                        	pushq	%r14
    2ab5: 41 55                        	pushq	%r13
    2ab7: 41 54                        	pushq	%r12
    2ab9: 53                           	pushq	%rbx
    2aba: 50                           	pushq	%rax
    2abb: 4c 89 0c 24                  	movq	%r9, (%rsp)
    2abf: 45 89 c7                     	movl	%r8d, %r15d
    2ac2: 49 89 cc                     	movq	%rcx, %r12
    2ac5: 49 89 d5                     	movq	%rdx, %r13
    2ac8: 48 89 f3                     	movq	%rsi, %rbx
    2acb: 48 89 fd                     	movq	%rdi, %rbp
    2ace: 48 8d 3d 03 e9 ff ff         	leaq	-5885(%rip), %rdi  # 13d8 <puts+0x13d8>
    2ad5: 48 89 ee                     	movq	%rbp, %rsi
    2ad8: 48 89 da                     	movq	%rbx, %rdx
    2adb: 4c 89 e9                     	movq	%r13, %rcx
    2ade: 4d 89 e0                     	movq	%r12, %r8
    2ae1: 31 c0                        	xorl	%eax, %eax
    2ae3: e8 18 09 00 00               	callq	0x3400 <printf@plt>
    2ae8: 4c 8b 35 f9 1c 00 00         	movq	7417(%rip), %r14  # 47e8 <puts+0x47e8>
    2aef: 49 8b 06                     	movq	(%r14), %rax
    2af2: 48 85 c0                     	testq	%rax, %rax
    2af5: 75 16                        	jne	0x2b0d <__hipRegisterFunction+0x5d>
    2af7: 48 8d 35 b4 ea ff ff         	leaq	-5452(%rip), %rsi  # 15b2 <puts+0x15b2>
    2afe: 48 c7 c7 ff ff ff ff         	movq	$-1, %rdi
    2b05: e8 06 09 00 00               	callq	0x3410 <dlsym@plt>
    2b0a: 49 89 06                     	movq	%rax, (%r14)
    2b0d: 48 89 ef                     	movq	%rbp, %rdi
    2b10: 48 89 de                     	movq	%rbx, %rsi
    2b13: 4c 89 ea                     	movq	%r13, %rdx
    2b16: 4c 89 e1                     	movq	%r12, %rcx
    2b19: 45 89 f8                     	movl	%r15d, %r8d
    2b1c: 4c 8b 0c 24                  	movq	(%rsp), %r9
    2b20: 48 83 c4 08                  	addq	$8, %rsp
    2b24: 5b                           	popq	%rbx
    2b25: 41 5c                        	popq	%r12
    2b27: 41 5d                        	popq	%r13
    2b29: 41 5e                        	popq	%r14
    2b2b: 41 5f                        	popq	%r15
    2b2d: 5d                           	popq	%rbp
    2b2e: ff e0                        	jmpq	*%rax

0000000000002b30 <_Z12print_resultPjjjj>:
    2b30: 55                           	pushq	%rbp
    2b31: 41 57                        	pushq	%r15
    2b33: 41 56                        	pushq	%r14
    2b35: 41 55                        	pushq	%r13
    2b37: 41 54                        	pushq	%r12
    2b39: 53                           	pushq	%rbx
    2b3a: 48 83 ec 28                  	subq	$40, %rsp
    2b3e: 41 89 cf                     	movl	%ecx, %r15d
    2b41: 41 89 d5                     	movl	%edx, %r13d
    2b44: 89 f5                        	movl	%esi, %ebp
    2b46: 48 89 fb                     	movq	%rdi, %rbx
    2b49: 48 8d 3d ac e9 ff ff         	leaq	-5716(%rip), %rdi  # 14fc <puts+0x14fc>
    2b50: 89 d6                        	movl	%edx, %esi
    2b52: 31 c0                        	xorl	%eax, %eax
    2b54: e8 a7 08 00 00               	callq	0x3400 <printf@plt>
    2b59: 45 85 ed                     	testl	%r13d, %r13d
    2b5c: 0f 84 e0 00 00 00            	je	0x2c42 <_Z12print_resultPjjjj+0x112>
    2b62: 43 8d 04 3f                  	leal	(%r15,%r15), %eax
    2b66: 83 c0 04                     	addl	$4, %eax
    2b69: 89 44 24 0c                  	movl	%eax, 12(%rsp)
    2b6d: 45 85 ff                     	testl	%r15d, %r15d
    2b70: 0f 84 9a 00 00 00            	je	0x2c10 <_Z12print_resultPjjjj+0xe0>
    2b76: 44 89 f8                     	movl	%r15d, %eax
    2b79: 48 89 44 24 18               	movq	%rax, 24(%rsp)
    2b7e: 45 01 ff                     	addl	%r15d, %r15d
    2b81: b8 01 00 00 00               	movl	$1, %eax
    2b86: 4c 8d 25 a1 e7 ff ff         	leaq	-6239(%rip), %r12  # 132e <puts+0x132e>
    2b8d: 45 31 f6                     	xorl	%r14d, %r14d
    2b90: 44 89 6c 24 10               	movl	%r13d, 16(%rsp)
    2b95: 4c 89 7c 24 20               	movq	%r15, 32(%rsp)
    2b9a: 66 0f 1f 44 00 00            	nopw	(%rax,%rax)
    2ba0: 4c 8b 6c 24 18               	movq	24(%rsp), %r13
    2ba5: 89 44 24 14                  	movl	%eax, 20(%rsp)
    2ba9: 41 89 c7                     	movl	%eax, %r15d
    2bac: 0f 1f 40 00                  	nopl	(%rax)
    2bb0: 41 8d 47 ff                  	leal	-1(%r15), %eax
    2bb4: 8b 14 83                     	movl	(%rbx,%rax,4), %edx
    2bb7: 44 89 f8                     	movl	%r15d, %eax
    2bba: 8b 0c 83                     	movl	(%rbx,%rax,4), %ecx
    2bbd: 4c 89 e7                     	movq	%r12, %rdi
    2bc0: 89 ee                        	movl	%ebp, %esi
    2bc2: 31 c0                        	xorl	%eax, %eax
    2bc4: e8 37 08 00 00               	callq	0x3400 <printf@plt>
    2bc9: 41 83 c7 02                  	addl	$2, %r15d
    2bcd: 49 83 c5 ff                  	addq	$-1, %r13
    2bd1: 75 dd                        	jne	0x2bb0 <_Z12print_resultPjjjj+0x80>
    2bd3: 4c 8b 7c 24 20               	movq	32(%rsp), %r15
    2bd8: 43 8d 04 37                  	leal	(%r15,%r14), %eax
    2bdc: 48 8b 14 83                  	movq	(%rbx,%rax,4), %rdx
    2be0: 48 8b 4c 83 08               	movq	8(%rbx,%rax,4), %rcx
    2be5: 48 8d 3d 0a e7 ff ff         	leaq	-6390(%rip), %rdi  # 12f6 <puts+0x12f6>
    2bec: 89 ee                        	movl	%ebp, %esi
    2bee: 31 c0                        	xorl	%eax, %eax
    2bf0: e8 0b 08 00 00               	callq	0x3400 <printf@plt>
    2bf5: 8b 44 24 0c                  	movl	12(%rsp), %eax
    2bf9: 41 01 c6                     	addl	%eax, %r14d
    2bfc: 8b 4c 24 14                  	movl	20(%rsp), %ecx
    2c00: 01 c1                        	addl	%eax, %ecx
    2c02: 89 c8                        	movl	%ecx, %eax
    2c04: 44 8b 6c 24 10               	movl	16(%rsp), %r13d
    2c09: 45 39 ee                     	cmpl	%r13d, %r14d
    2c0c: 72 92                        	jb	0x2ba0 <_Z12print_resultPjjjj+0x70>
    2c0e: eb 32                        	jmp	0x2c42 <_Z12print_resultPjjjj+0x112>
    2c10: 4c 8d 3d df e6 ff ff         	leaq	-6433(%rip), %r15  # 12f6 <puts+0x12f6>
    2c17: 45 31 f6                     	xorl	%r14d, %r14d
    2c1a: 66 0f 1f 44 00 00            	nopw	(%rax,%rax)
    2c20: 44 89 f0                     	movl	%r14d, %eax
    2c23: 48 8b 14 83                  	movq	(%rbx,%rax,4), %rdx
    2c27: 48 8b 4c 83 08               	movq	8(%rbx,%rax,4), %rcx
    2c2c: 4c 89 ff                     	movq	%r15, %rdi
    2c2f: 89 ee                        	movl	%ebp, %esi
    2c31: 31 c0                        	xorl	%eax, %eax
    2c33: e8 c8 07 00 00               	callq	0x3400 <printf@plt>
    2c38: 44 03 74 24 0c               	addl	12(%rsp), %r14d
    2c3d: 45 39 ee                     	cmpl	%r13d, %r14d
    2c40: 72 de                        	jb	0x2c20 <_Z12print_resultPjjjj+0xf0>
    2c42: 48 83 c4 28                  	addq	$40, %rsp
    2c46: 5b                           	popq	%rbx
    2c47: 41 5c                        	popq	%r12
    2c49: 41 5d                        	popq	%r13
    2c4b: 41 5e                        	popq	%r14
    2c4d: 41 5f                        	popq	%r15
    2c4f: 5d                           	popq	%rbp
    2c50: c3                           	retq
    2c51: 66 2e 0f 1f 84 00 00 00 00 00	nopw	%cs:(%rax,%rax)
    2c5b: 0f 1f 44 00 00               	nopl	(%rax,%rax)

0000000000002c60 <_Z8callbackP12ihipStream_t10hipError_tPv>:
    2c60: 41 56                        	pushq	%r14
    2c62: 53                           	pushq	%rbx
    2c63: 50                           	pushq	%rax
    2c64: 48 89 d3                     	movq	%rdx, %rbx
    2c67: 4c 8d 35 64 e6 ff ff         	leaq	-6556(%rip), %r14  # 12d2 <puts+0x12d2>
    2c6e: 4c 89 f7                     	movq	%r14, %rdi
    2c71: e8 ba 07 00 00               	callq	0x3430 <puts@plt>
    2c76: 8b 73 14                     	movl	20(%rbx), %esi
    2c79: 48 8d 3d ce e7 ff ff         	leaq	-6194(%rip), %rdi  # 144e <puts+0x144e>
    2c80: 31 c0                        	xorl	%eax, %eax
    2c82: e8 79 07 00 00               	callq	0x3400 <printf@plt>
    2c87: e8 b4 07 00 00               	callq	0x3440 <hipDeviceSynchronize@plt>
    2c8c: 48 8b 33                     	movq	(%rbx), %rsi
    2c8f: 48 8b 7b 08                  	movq	8(%rbx), %rdi
    2c93: 8b 53 14                     	movl	20(%rbx), %edx
    2c96: b9 02 00 00 00               	movl	$2, %ecx
    2c9b: e8 b0 07 00 00               	callq	0x3450 <hipMemcpy@plt>
    2ca0: 4c 89 f7                     	movq	%r14, %rdi
    2ca3: 48 83 c4 08                  	addq	$8, %rsp
    2ca7: 5b                           	popq	%rbx
    2ca8: 41 5e                        	popq	%r14
    2caa: e9 81 07 00 00               	jmp	0x3430 <puts@plt>
    2caf: 90                           	nop

0000000000002cb0 <hipMemcpy>:
    2cb0: 55                           	pushq	%rbp
    2cb1: 41 57                        	pushq	%r15
    2cb3: 41 56                        	pushq	%r14
    2cb5: 41 54                        	pushq	%r12
    2cb7: 53                           	pushq	%rbx
    2cb8: 41 89 ce                     	movl	%ecx, %r14d
    2cbb: 49 89 d7                     	movq	%rdx, %r15
    2cbe: 48 89 f5                     	movq	%rsi, %rbp
    2cc1: 48 89 fb                     	movq	%rdi, %rbx
    2cc4: 4c 8b 25 25 1b 00 00         	movq	6949(%rip), %r12  # 47f0 <puts+0x47f0>
    2ccb: 49 83 3c 24 00               	cmpq	$0, (%r12)
    2cd0: 75 17                        	jne	0x2ce9 <hipMemcpy+0x39>
    2cd2: 48 8d 35 b8 e8 ff ff         	leaq	-5960(%rip), %rsi  # 1591 <puts+0x1591>
    2cd9: 48 c7 c7 ff ff ff ff         	movq	$-1, %rdi
    2ce0: e8 2b 07 00 00               	callq	0x3410 <dlsym@plt>
    2ce5: 49 89 04 24                  	movq	%rax, (%r12)
    2ce9: 48 8d 3d 15 e6 ff ff         	leaq	-6635(%rip), %rdi  # 1305 <puts+0x1305>
    2cf0: 48 89 de                     	movq	%rbx, %rsi
    2cf3: 48 89 ea                     	movq	%rbp, %rdx
    2cf6: 31 c0                        	xorl	%eax, %eax
    2cf8: e8 03 07 00 00               	callq	0x3400 <printf@plt>
    2cfd: 49 8b 04 24                  	movq	(%r12), %rax
    2d01: 48 89 df                     	movq	%rbx, %rdi
    2d04: 48 89 ee                     	movq	%rbp, %rsi
    2d07: 4c 89 fa                     	movq	%r15, %rdx
    2d0a: 44 89 f1                     	movl	%r14d, %ecx
    2d0d: 5b                           	popq	%rbx
    2d0e: 41 5c                        	popq	%r12
    2d10: 41 5e                        	popq	%r14
    2d12: 41 5f                        	popq	%r15
    2d14: 5d                           	popq	%rbp
    2d15: ff e0                        	jmpq	*%rax
    2d17: 66 0f 1f 84 00 00 00 00 00   	nopw	(%rax,%rax)

0000000000002d20 <hipLaunchKernel>:
    2d20: 55                           	pushq	%rbp
    2d21: 41 57                        	pushq	%r15
    2d23: 41 56                        	pushq	%r14
    2d25: 41 55                        	pushq	%r13
    2d27: 41 54                        	pushq	%r12
    2d29: 53                           	pushq	%rbx
    2d2a: 48 83 ec 48                  	subq	$72, %rsp
    2d2e: 4d 89 ce                     	movq	%r9, %r14
    2d31: 44 89 44 24 0c               	movl	%r8d, 12(%rsp)
    2d36: 49 89 cd                     	movq	%rcx, %r13
    2d39: 41 89 d7                     	movl	%edx, %r15d
    2d3c: 49 89 f4                     	movq	%rsi, %r12
    2d3f: 48 89 fa                     	movq	%rdi, %rdx
    2d42: 48 8b ac 24 88 00 00 00      	movq	136(%rsp), %rbp
    2d4a: 48 89 f3                     	movq	%rsi, %rbx
    2d4d: 48 c1 eb 20                  	shrq	$32, %rbx
    2d51: 48 89 4c 24 18               	movq	%rcx, 24(%rsp)
    2d56: 49 c1 ed 20                  	shrq	$32, %r13
    2d5a: 48 8b 05 97 1a 00 00         	movq	6807(%rip), %rax  # 47f8 <puts+0x47f8>
    2d61: 48 83 38 00                  	cmpq	$0, (%rax)
    2d65: 48 89 7c 24 28               	movq	%rdi, 40(%rsp)
    2d6a: 75 22                        	jne	0x2d8e <hipLaunchKernel+0x6e>
    2d6c: 48 8d 35 4f e5 ff ff         	leaq	-6833(%rip), %rsi  # 12c2 <puts+0x12c2>
    2d73: 48 c7 c7 ff ff ff ff         	movq	$-1, %rdi
    2d7a: e8 91 06 00 00               	callq	0x3410 <dlsym@plt>
    2d7f: 48 8b 54 24 28               	movq	40(%rsp), %rdx
    2d84: 48 8b 0d 6d 1a 00 00         	movq	6765(%rip), %rcx  # 47f8 <puts+0x47f8>
    2d8b: 48 89 01                     	movq	%rax, (%rcx)
    2d8e: 48 8d 3d 9b e4 ff ff         	leaq	-7013(%rip), %rdi  # 1230 <puts+0x1230>
    2d95: 48 89 d6                     	movq	%rdx, %rsi
    2d98: 48 89 ea                     	movq	%rbp, %rdx
    2d9b: 31 c0                        	xorl	%eax, %eax
    2d9d: e8 5e 06 00 00               	callq	0x3400 <printf@plt>
    2da2: 48 8d 2d 41 e7 ff ff         	leaq	-6335(%rip), %rbp  # 14ea <puts+0x14ea>
    2da9: 48 89 ef                     	movq	%rbp, %rdi
    2dac: 4c 89 64 24 38               	movq	%r12, 56(%rsp)
    2db1: 44 89 e6                     	movl	%r12d, %esi
    2db4: 48 89 5c 24 10               	movq	%rbx, 16(%rsp)
    2db9: 89 da                        	movl	%ebx, %edx
    2dbb: 44 89 7c 24 08               	movl	%r15d, 8(%rsp)
    2dc0: 44 89 f9                     	movl	%r15d, %ecx
    2dc3: 31 c0                        	xorl	%eax, %eax
    2dc5: e8 36 06 00 00               	callq	0x3400 <printf@plt>
    2dca: 48 89 ef                     	movq	%rbp, %rdi
    2dcd: 48 8b 6c 24 18               	movq	24(%rsp), %rbp
    2dd2: 89 ee                        	movl	%ebp, %esi
    2dd4: 44 89 ea                     	movl	%r13d, %edx
    2dd7: 8b 5c 24 0c                  	movl	12(%rsp), %ebx
    2ddb: 89 d9                        	movl	%ebx, %ecx
    2ddd: 31 c0                        	xorl	%eax, %eax
    2ddf: e8 1c 06 00 00               	callq	0x3400 <printf@plt>
    2de4: 49 8b 46 28                  	movq	40(%r14), %rax
    2de8: 4c 89 74 24 40               	movq	%r14, 64(%rsp)
    2ded: 49 8b 4e 30                  	movq	48(%r14), %rcx
    2df1: 48 8b 30                     	movq	(%rax), %rsi
    2df4: 8b 11                        	movl	(%rcx), %edx
    2df6: 48 8d 3d 83 e6 ff ff         	leaq	-6525(%rip), %rdi  # 1480 <puts+0x1480>
    2dfd: 31 c0                        	xorl	%eax, %eax
    2dff: e8 fc 05 00 00               	callq	0x3400 <printf@plt>
    2e04: 4c 8b 25 f5 19 00 00         	movq	6645(%rip), %r12  # 4800 <puts+0x4800>
    2e0b: 49 8b 04 24                  	movq	(%r12), %rax
    2e0f: 48 89 44 24 20               	movq	%rax, 32(%rsp)
    2e14: 4d 8b 74 24 08               	movq	8(%r12), %r14
    2e19: bf 38 00 00 00               	movl	$56, %edi
    2e1e: e8 3d 06 00 00               	callq	0x3460 <malloc@plt>
    2e23: 49 89 c7                     	movq	%rax, %r15
    2e26: 4d 3b 74 24 10               	cmpq	16(%r12), %r14
    2e2b: 74 17                        	je	0x2e44 <hipLaunchKernel+0x124>
    2e2d: 4d 89 3e                     	movq	%r15, (%r14)
    2e30: 49 83 44 24 08 08            	addq	$8, 8(%r12)
    2e36: 8b 74 24 08                  	movl	8(%rsp), %esi
    2e3a: 48 8b 54 24 10               	movq	16(%rsp), %rdx
    2e3f: e9 d2 00 00 00               	jmp	0x2f16 <hipLaunchKernel+0x1f6>
    2e44: 4c 2b 74 24 20               	subq	32(%rsp), %r14
    2e49: 48 b8 f8 ff ff ff ff ff ff 7f	movabsq	$9223372036854775800, %rax
    2e53: 49 39 c6                     	cmpq	%rax, %r14
    2e56: 0f 84 7a 02 00 00            	je	0x30d6 <hipLaunchKernel+0x3b6>
    2e5c: 4c 89 6c 24 30               	movq	%r13, 48(%rsp)
    2e61: 4c 89 f3                     	movq	%r14, %rbx
    2e64: 48 c1 fb 03                  	sarq	$3, %rbx
    2e68: 4d 85 f6                     	testq	%r14, %r14
    2e6b: b8 01 00 00 00               	movl	$1, %eax
    2e70: 48 0f 45 c3                  	cmovneq	%rbx, %rax
    2e74: 48 8d 2c 18                  	leaq	(%rax,%rbx), %rbp
    2e78: 48 89 e9                     	movq	%rbp, %rcx
    2e7b: 48 c1 e9 3c                  	shrq	$60, %rcx
    2e7f: 48 b9 ff ff ff ff ff ff ff 0f	movabsq	$1152921504606846975, %rcx
    2e89: 48 0f 45 e9                  	cmovneq	%rcx, %rbp
    2e8d: 48 01 d8                     	addq	%rbx, %rax
    2e90: 48 0f 42 e9                  	cmovbq	%rcx, %rbp
    2e94: 48 85 ed                     	testq	%rbp, %rbp
    2e97: 74 12                        	je	0x2eab <hipLaunchKernel+0x18b>
    2e99: 48 8d 3c ed 00 00 00 00      	leaq	(,%rbp,8), %rdi
    2ea1: e8 ca 05 00 00               	callq	0x3470 <_Znwm@plt>
    2ea6: 49 89 c4                     	movq	%rax, %r12
    2ea9: eb 03                        	jmp	0x2eae <hipLaunchKernel+0x18e>
    2eab: 45 31 e4                     	xorl	%r12d, %r12d
    2eae: 48 8b 54 24 10               	movq	16(%rsp), %rdx
    2eb3: 4d 8d 2c dc                  	leaq	(%r12,%rbx,8), %r13
    2eb7: 4d 89 3c dc                  	movq	%r15, (%r12,%rbx,8)
    2ebb: 4d 85 f6                     	testq	%r14, %r14
    2ebe: 48 8b 5c 24 20               	movq	32(%rsp), %rbx
    2ec3: 7e 13                        	jle	0x2ed8 <hipLaunchKernel+0x1b8>
    2ec5: 4c 89 e7                     	movq	%r12, %rdi
    2ec8: 48 89 de                     	movq	%rbx, %rsi
    2ecb: 4c 89 f2                     	movq	%r14, %rdx
    2ece: e8 ad 05 00 00               	callq	0x3480 <memmove@plt>
    2ed3: 48 8b 54 24 10               	movq	16(%rsp), %rdx
    2ed8: 49 83 c5 08                  	addq	$8, %r13
    2edc: 48 85 db                     	testq	%rbx, %rbx
    2edf: 74 0d                        	je	0x2eee <hipLaunchKernel+0x1ce>
    2ee1: 48 89 df                     	movq	%rbx, %rdi
    2ee4: e8 07 05 00 00               	callq	0x33f0 <_ZdlPv@plt>
    2ee9: 48 8b 54 24 10               	movq	16(%rsp), %rdx
    2eee: 48 8b 0d 0b 19 00 00         	movq	6411(%rip), %rcx  # 4800 <puts+0x4800>
    2ef5: 4c 89 21                     	movq	%r12, (%rcx)
    2ef8: 4c 89 69 08                  	movq	%r13, 8(%rcx)
    2efc: 49 8d 04 ec                  	leaq	(%r12,%rbp,8), %rax
    2f00: 48 89 41 10                  	movq	%rax, 16(%rcx)
    2f04: 8b 5c 24 0c                  	movl	12(%rsp), %ebx
    2f08: 8b 74 24 08                  	movl	8(%rsp), %esi
    2f0c: 4c 8b 6c 24 30               	movq	48(%rsp), %r13
    2f11: 48 8b 6c 24 18               	movq	24(%rsp), %rbp
    2f16: 89 e8                        	movl	%ebp, %eax
    2f18: 0f af c3                     	imull	%ebx, %eax
    2f1b: 41 0f af c5                  	imull	%r13d, %eax
    2f1f: 83 c0 3f                     	addl	$63, %eax
    2f22: c1 e8 06                     	shrl	$6, %eax
    2f25: 4c 8b 64 24 38               	movq	56(%rsp), %r12
    2f2a: 44 89 e1                     	movl	%r12d, %ecx
    2f2d: 0f af ce                     	imull	%esi, %ecx
    2f30: 0f af ca                     	imull	%edx, %ecx
    2f33: 0f af c8                     	imull	%eax, %ecx
    2f36: c1 e1 03                     	shll	$3, %ecx
    2f39: 8d 2c 49                     	leal	(%rcx,%rcx,2), %ebp
    2f3c: 41 89 ed                     	movl	%ebp, %r13d
    2f3f: 41 c1 ed 02                  	shrl	$2, %r13d
    2f43: 48 89 ef                     	movq	%rbp, %rdi
    2f46: e8 15 05 00 00               	callq	0x3460 <malloc@plt>
    2f4b: 49 89 47 08                  	movq	%rax, 8(%r15)
    2f4f: 4c 89 ff                     	movq	%r15, %rdi
    2f52: 48 89 ee                     	movq	%rbp, %rsi
    2f55: e8 36 05 00 00               	callq	0x3490 <hipMalloc@plt>
    2f5a: 49 8b 37                     	movq	(%r15), %rsi
    2f5d: 49 89 77 20                  	movq	%rsi, 32(%r15)
    2f61: 41 89 6f 10                  	movl	%ebp, 16(%r15)
    2f65: 48 8b 5c 24 40               	movq	64(%rsp), %rbx
    2f6a: 48 8b 43 28                  	movq	40(%rbx), %rax
    2f6e: 48 8b 10                     	movq	(%rax), %rdx
    2f71: 4c 8d 35 f2 e2 ff ff         	leaq	-7438(%rip), %r14  # 126a <puts+0x126a>
    2f78: 4c 89 f7                     	movq	%r14, %rdi
    2f7b: 31 c0                        	xorl	%eax, %eax
    2f7d: e8 7e 04 00 00               	callq	0x3400 <printf@plt>
    2f82: 49 8b 37                     	movq	(%r15), %rsi
    2f85: 48 8b 43 28                  	movq	40(%rbx), %rax
    2f89: 48 8b 10                     	movq	(%rax), %rdx
    2f8c: 4c 89 f7                     	movq	%r14, %rdi
    2f8f: 31 c0                        	xorl	%eax, %eax
    2f91: e8 6a 04 00 00               	callq	0x3400 <printf@plt>
    2f96: 41 89 6f 14                  	movl	%ebp, 20(%r15)
    2f9a: 41 c7 47 18 01 00 00 00      	movl	$1, 24(%r15)
    2fa2: 41 c7 47 28 00 00 00 00      	movl	$0, 40(%r15)
    2faa: 45 89 6f 2c                  	movl	%r13d, 44(%r15)
    2fae: 41 c7 47 30 01 00 00 00      	movl	$1, 48(%r15)
    2fb6: 49 8b 7f 08                  	movq	8(%r15), %rdi
    2fba: 31 f6                        	xorl	%esi, %esi
    2fbc: 48 89 ea                     	movq	%rbp, %rdx
    2fbf: e8 dc 04 00 00               	callq	0x34a0 <memset@plt>
    2fc4: 49 8b 3f                     	movq	(%r15), %rdi
    2fc7: 49 8b 77 08                  	movq	8(%r15), %rsi
    2fcb: 48 89 ea                     	movq	%rbp, %rdx
    2fce: b9 01 00 00 00               	movl	$1, %ecx
    2fd3: e8 78 04 00 00               	callq	0x3450 <hipMemcpy@plt>
    2fd8: bf 30 00 00 00               	movl	$48, %edi
    2fdd: e8 7e 04 00 00               	callq	0x3460 <malloc@plt>
    2fe2: 48 89 c5                     	movq	%rax, %rbp
    2fe5: 0f 10 03                     	movups	(%rbx), %xmm0
    2fe8: 0f 10 4b 10                  	movups	16(%rbx), %xmm1
    2fec: 0f 10 53 20                  	movups	32(%rbx), %xmm2
    2ff0: 0f 11 50 20                  	movups	%xmm2, 32(%rax)
    2ff4: 0f 11 00                     	movups	%xmm0, (%rax)
    2ff7: 0f 11 48 10                  	movups	%xmm1, 16(%rax)
    2ffb: 4c 89 78 28                  	movq	%r15, 40(%rax)
    2fff: 48 8b 43 28                  	movq	40(%rbx), %rax
    3003: 48 8b 38                     	movq	(%rax), %rdi
    3006: ba 60 00 00 00               	movl	$96, %edx
    300b: be cd ab 00 00               	movl	$43981, %esi
    3010: e8 9b 04 00 00               	callq	0x34b0 <hipMemset@plt>
    3015: 85 c0                        	testl	%eax, %eax
    3017: 75 7f                        	jne	0x3098 <hipLaunchKernel+0x378>
    3019: 48 8b 7c 24 28               	movq	40(%rsp), %rdi
    301e: 4c 89 e6                     	movq	%r12, %rsi
    3021: 8b 54 24 08                  	movl	8(%rsp), %edx
    3025: 48 8b 4c 24 18               	movq	24(%rsp), %rcx
    302a: 44 8b 44 24 0c               	movl	12(%rsp), %r8d
    302f: 49 89 e9                     	movq	%rbp, %r9
    3032: ff b4 24 88 00 00 00         	pushq	136(%rsp)
    3039: ff b4 24 88 00 00 00         	pushq	136(%rsp)
    3040: 48 8b 05 b1 17 00 00         	movq	6065(%rip), %rax  # 47f8 <puts+0x47f8>
    3047: ff 10                        	callq	*(%rax)
    3049: 48 83 c4 10                  	addq	$16, %rsp
    304d: 49 8b 7f 08                  	movq	8(%r15), %rdi
    3051: 48 8b 43 28                  	movq	40(%rbx), %rax
    3055: 48 8b 30                     	movq	(%rax), %rsi
    3058: 41 8b 57 14                  	movl	20(%r15), %edx
    305c: b9 02 00 00 00               	movl	$2, %ecx
    3061: e8 ea 03 00 00               	callq	0x3450 <hipMemcpy@plt>
    3066: 85 c0                        	testl	%eax, %eax
    3068: 75 4d                        	jne	0x30b7 <hipLaunchKernel+0x397>
    306a: 49 8b 7f 20                  	movq	32(%r15), %rdi
    306e: 41 8b 77 28                  	movl	40(%r15), %esi
    3072: 41 8b 57 2c                  	movl	44(%r15), %edx
    3076: 41 8b 4f 30                  	movl	48(%r15), %ecx
    307a: e8 a1 03 00 00               	callq	0x3420 <_Z12print_resultPjjjj@plt>
    307f: 41 c7 47 18 02 00 00 00      	movl	$2, 24(%r15)
    3087: 31 c0                        	xorl	%eax, %eax
    3089: 48 83 c4 48                  	addq	$72, %rsp
    308d: 5b                           	popq	%rbx
    308e: 41 5c                        	popq	%r12
    3090: 41 5d                        	popq	%r13
    3092: 41 5e                        	popq	%r14
    3094: 41 5f                        	popq	%r15
    3096: 5d                           	popq	%rbp
    3097: c3                           	retq
    3098: 48 8d 3d f9 e3 ff ff         	leaq	-7175(%rip), %rdi  # 1498 <puts+0x1498>
    309f: 48 8d 35 de e4 ff ff         	leaq	-6946(%rip), %rsi  # 1584 <puts+0x1584>
    30a6: 48 8d 0d 8d e2 ff ff         	leaq	-7539(%rip), %rcx  # 133a <puts+0x133a>
    30ad: ba dd 00 00 00               	movl	$221, %edx
    30b2: e8 09 04 00 00               	callq	0x34c0 <__assert_fail@plt>
    30b7: 48 8d 3d 60 e4 ff ff         	leaq	-7072(%rip), %rdi  # 151e <puts+0x151e>
    30be: 48 8d 35 bf e4 ff ff         	leaq	-6977(%rip), %rsi  # 1584 <puts+0x1584>
    30c5: 48 8d 0d 6e e2 ff ff         	leaq	-7570(%rip), %rcx  # 133a <puts+0x133a>
    30cc: ba ec 00 00 00               	movl	$236, %edx
    30d1: e8 ea 03 00 00               	callq	0x34c0 <__assert_fail@plt>
    30d6: 48 8d 3d e1 e2 ff ff         	leaq	-7455(%rip), %rdi  # 13be <puts+0x13be>
    30dd: e8 ee 03 00 00               	callq	0x34d0 <_ZSt20__throw_length_errorPKc@plt>
    30e2: 66 2e 0f 1f 84 00 00 00 00 00	nopw	%cs:(%rax,%rax)
    30ec: 0f 1f 40 00                  	nopl	(%rax)

00000000000030f0 <hipMalloc>:
    30f0: 55                           	pushq	%rbp
    30f1: 41 56                        	pushq	%r14
    30f3: 53                           	pushq	%rbx
    30f4: 49 89 f6                     	movq	%rsi, %r14
    30f7: 48 89 fb                     	movq	%rdi, %rbx
    30fa: 48 8b 2d 07 17 00 00         	movq	5895(%rip), %rbp  # 4808 <puts+0x4808>
    3101: 48 8b 4d 00                  	movq	(%rbp), %rcx
    3105: 48 85 c9                     	testq	%rcx, %rcx
    3108: 75 1a                        	jne	0x3124 <hipMalloc+0x34>
    310a: 48 8d 35 03 e4 ff ff         	leaq	-7165(%rip), %rsi  # 1514 <puts+0x1514>
    3111: 48 c7 c7 ff ff ff ff         	movq	$-1, %rdi
    3118: e8 f3 02 00 00               	callq	0x3410 <dlsym@plt>
    311d: 48 89 c1                     	movq	%rax, %rcx
    3120: 48 89 45 00                  	movq	%rax, (%rbp)
    3124: 48 89 df                     	movq	%rbx, %rdi
    3127: 4c 89 f6                     	movq	%r14, %rsi
    312a: ff d1                        	callq	*%rcx
    312c: 89 c5                        	movl	%eax, %ebp
    312e: 48 8b 33                     	movq	(%rbx), %rsi
    3131: 48 8d 3d 55 e2 ff ff         	leaq	-7595(%rip), %rdi  # 138d <puts+0x138d>
    3138: 4c 89 f2                     	movq	%r14, %rdx
    313b: 31 c0                        	xorl	%eax, %eax
    313d: e8 be 02 00 00               	callq	0x3400 <printf@plt>
    3142: 89 e8                        	movl	%ebp, %eax
    3144: 5b                           	popq	%rbx
    3145: 41 5e                        	popq	%r14
    3147: 5d                           	popq	%rbp
    3148: c3                           	retq
    3149: 0f 1f 80 00 00 00 00         	nopl	(%rax)

0000000000003150 <hipMemset>:
    3150: 55                           	pushq	%rbp
    3151: 41 57                        	pushq	%r15
    3153: 41 56                        	pushq	%r14
    3155: 53                           	pushq	%rbx
    3156: 50                           	pushq	%rax
    3157: 49 89 d6                     	movq	%rdx, %r14
    315a: 89 f5                        	movl	%esi, %ebp
    315c: 48 89 fb                     	movq	%rdi, %rbx
    315f: 4c 8b 3d aa 16 00 00         	movq	5802(%rip), %r15  # 4810 <puts+0x4810>
    3166: 49 83 3f 00                  	cmpq	$0, (%r15)
    316a: 75 16                        	jne	0x3182 <hipMemset+0x32>
    316c: 48 8d 35 b1 e1 ff ff         	leaq	-7759(%rip), %rsi  # 1324 <puts+0x1324>
    3173: 48 c7 c7 ff ff ff ff         	movq	$-1, %rdi
    317a: e8 91 02 00 00               	callq	0x3410 <dlsym@plt>
    317f: 49 89 07                     	movq	%rax, (%r15)
    3182: 48 8d 3d 95 e2 ff ff         	leaq	-7531(%rip), %rdi  # 141e <puts+0x141e>
    3189: 48 89 de                     	movq	%rbx, %rsi
    318c: 89 ea                        	movl	%ebp, %edx
    318e: 4c 89 f1                     	movq	%r14, %rcx
    3191: 31 c0                        	xorl	%eax, %eax
    3193: e8 68 02 00 00               	callq	0x3400 <printf@plt>
    3198: 49 8b 07                     	movq	(%r15), %rax
    319b: 48 89 df                     	movq	%rbx, %rdi
    319e: 89 ee                        	movl	%ebp, %esi
    31a0: 4c 89 f2                     	movq	%r14, %rdx
    31a3: 48 83 c4 08                  	addq	$8, %rsp
    31a7: 5b                           	popq	%rbx
    31a8: 41 5e                        	popq	%r14
    31aa: 41 5f                        	popq	%r15
    31ac: 5d                           	popq	%rbp
    31ad: ff e0                        	jmpq	*%rax
    31af: 90                           	nop

00000000000031b0 <hipSetupArgument>:
    31b0: 50                           	pushq	%rax
    31b1: 48 8d 3d 1b e3 ff ff         	leaq	-7397(%rip), %rdi  # 14d3 <puts+0x14d3>
    31b8: e8 73 02 00 00               	callq	0x3430 <puts@plt>
    31bd: bf ff ff ff ff               	movl	$4294967295, %edi
    31c2: e8 19 03 00 00               	callq	0x34e0 <exit@plt>
    31c7: 66 0f 1f 84 00 00 00 00 00   	nopw	(%rax,%rax)

00000000000031d0 <_Z23ihipLaunchKernelCommandRPvP18ihipModuleSymbol_tjjjjjjjS_PS_S3_P11ihipEvent_tS5_jjjjmmj>:
    31d0: 50                           	pushq	%rax
    31d1: bf ff ff ff ff               	movl	$4294967295, %edi
    31d6: e8 05 03 00 00               	callq	0x34e0 <exit@plt>
    31db: 0f 1f 44 00 00               	nopl	(%rax,%rax)

00000000000031e0 <_ZL5setupv>:
    31e0: 41 56                        	pushq	%r14
    31e2: 53                           	pushq	%rbx
    31e3: 50                           	pushq	%rax
    31e4: 48 8b 05 0d 16 00 00         	movq	5645(%rip), %rax  # 47f8 <puts+0x47f8>
    31eb: 48 c7 00 00 00 00 00         	movq	$0, (%rax)
    31f2: 48 8b 05 ef 15 00 00         	movq	5615(%rip), %rax  # 47e8 <puts+0x47e8>
    31f9: 48 c7 00 00 00 00 00         	movq	$0, (%rax)
    3200: 48 8b 05 01 16 00 00         	movq	5633(%rip), %rax  # 4808 <puts+0x4808>
    3207: 48 c7 00 00 00 00 00         	movq	$0, (%rax)
    320e: 48 8b 05 db 15 00 00         	movq	5595(%rip), %rax  # 47f0 <puts+0x47f0>
    3215: 48 c7 00 00 00 00 00         	movq	$0, (%rax)
    321c: 48 8b 05 ed 15 00 00         	movq	5613(%rip), %rax  # 4810 <puts+0x4810>
    3223: 48 c7 00 00 00 00 00         	movq	$0, (%rax)
    322a: 48 8b 1d af 15 00 00         	movq	5551(%rip), %rbx  # 47e0 <puts+0x47e0>
    3231: 48 83 3b 00                  	cmpq	$0, (%rbx)
    3235: 75 16                        	jne	0x324d <_ZL5setupv+0x6d>
    3237: 48 8d 35 5d e3 ff ff         	leaq	-7331(%rip), %rsi  # 159b <puts+0x159b>
    323e: 48 c7 c7 ff ff ff ff         	movq	$-1, %rdi
    3245: e8 c6 01 00 00               	callq	0x3410 <dlsym@plt>
    324a: 48 89 03                     	movq	%rax, (%rbx)
    324d: 48 8d 3d 38 e0 ff ff         	leaq	-8136(%rip), %rdi  # 128c <puts+0x128c>
    3254: 48 8d 35 76 e2 ff ff         	leaq	-7562(%rip), %rsi  # 14d1 <puts+0x14d1>
    325b: e8 90 02 00 00               	callq	0x34f0 <fopen@plt>
    3260: 48 89 c3                     	movq	%rax, %rbx
    3263: 48 89 c7                     	movq	%rax, %rdi
    3266: 31 f6                        	xorl	%esi, %esi
    3268: ba 02 00 00 00               	movl	$2, %edx
    326d: e8 8e 02 00 00               	callq	0x3500 <fseek@plt>
    3272: 48 89 df                     	movq	%rbx, %rdi
    3275: e8 96 02 00 00               	callq	0x3510 <ftell@plt>
    327a: 49 89 c6                     	movq	%rax, %r14
    327d: 48 89 df                     	movq	%rbx, %rdi
    3280: 31 f6                        	xorl	%esi, %esi
    3282: 31 d2                        	xorl	%edx, %edx
    3284: e8 77 02 00 00               	callq	0x3500 <fseek@plt>
    3289: bf 00 10 00 00               	movl	$4096, %edi
    328e: 4c 89 f6                     	movq	%r14, %rsi
    3291: e8 8a 02 00 00               	callq	0x3520 <aligned_alloc@plt>
    3296: 48 8b 0d 3b 15 00 00         	movq	5435(%rip), %rcx  # 47d8 <puts+0x47d8>
    329d: 48 89 01                     	movq	%rax, (%rcx)
    32a0: be 01 00 00 00               	movl	$1, %esi
    32a5: 48 89 c7                     	movq	%rax, %rdi
    32a8: 4c 89 f2                     	movq	%r14, %rdx
    32ab: 48 89 d9                     	movq	%rbx, %rcx
    32ae: e8 7d 02 00 00               	callq	0x3530 <fread@plt>
    32b3: 48 89 df                     	movq	%rbx, %rdi
    32b6: e8 85 02 00 00               	callq	0x3540 <fclose@plt>
    32bb: 48 8d 3d f2 df ff ff         	leaq	-8206(%rip), %rdi  # 12b4 <puts+0x12b4>
    32c2: 48 83 c4 08                  	addq	$8, %rsp
    32c6: 5b                           	popq	%rbx
    32c7: 41 5e                        	popq	%r14
    32c9: e9 62 01 00 00               	jmp	0x3430 <puts@plt>
    32ce: 66 90                        	nop

00000000000032d0 <_ZL4finiv>:
    32d0: 41 57                        	pushq	%r15
    32d2: 41 56                        	pushq	%r14
    32d4: 41 54                        	pushq	%r12
    32d6: 53                           	pushq	%rbx
    32d7: 50                           	pushq	%rax
    32d8: 4c 8b 3d 21 15 00 00         	movq	5409(%rip), %r15  # 4800 <puts+0x4800>
    32df: 4d 8b 27                     	movq	(%r15), %r12
    32e2: 49 8b 47 08                  	movq	8(%r15), %rax
    32e6: 49 39 c4                     	cmpq	%rax, %r12
    32e9: 74 66                        	je	0x3351 <_ZL4finiv+0x81>
    32eb: 4d 8d 74 24 08               	leaq	8(%r12), %r14
    32f0: eb 1b                        	jmp	0x330d <_ZL4finiv+0x3d>
    32f2: 66 2e 0f 1f 84 00 00 00 00 00	nopw	%cs:(%rax,%rax)
    32fc: 0f 1f 40 00                  	nopl	(%rax)
    3300: 48 83 c0 f8                  	addq	$-8, %rax
    3304: 49 89 47 08                  	movq	%rax, 8(%r15)
    3308: 49 39 c4                     	cmpq	%rax, %r12
    330b: 74 44                        	je	0x3351 <_ZL4finiv+0x81>
    330d: 49 8b 1c 24                  	movq	(%r12), %rbx
    3311: 83 7b 18 01                  	cmpl	$1, 24(%rbx)
    3315: 75 1c                        	jne	0x3333 <_ZL4finiv+0x63>
    3317: 48 8b 3b                     	movq	(%rbx), %rdi
    331a: e8 31 02 00 00               	callq	0x3550 <hipFree@plt>
    331f: 48 8b 7b 08                  	movq	8(%rbx), %rdi
    3323: e8 38 02 00 00               	callq	0x3560 <free@plt>
    3328: c7 43 18 00 00 00 00         	movl	$0, 24(%rbx)
    332f: 49 8b 47 08                  	movq	8(%r15), %rax
    3333: 49 39 c6                     	cmpq	%rax, %r14
    3336: 74 c8                        	je	0x3300 <_ZL4finiv+0x30>
    3338: 48 89 c2                     	movq	%rax, %rdx
    333b: 4c 29 f2                     	subq	%r14, %rdx
    333e: 74 c0                        	je	0x3300 <_ZL4finiv+0x30>
    3340: 4c 89 e7                     	movq	%r12, %rdi
    3343: 4c 89 f6                     	movq	%r14, %rsi
    3346: e8 35 01 00 00               	callq	0x3480 <memmove@plt>
    334b: 49 8b 47 08                  	movq	8(%r15), %rax
    334f: eb af                        	jmp	0x3300 <_ZL4finiv+0x30>
    3351: 48 83 c4 08                  	addq	$8, %rsp
    3355: 5b                           	popq	%rbx
    3356: 41 5c                        	popq	%r12
    3358: 41 5e                        	popq	%r14
    335a: 41 5f                        	popq	%r15
    335c: c3                           	retq
    335d: cc                           	int3
    335e: cc                           	int3
    335f: cc                           	int3

0000000000003360 <_ZNSt6vectorIP21kernel_launch_trackerSaIS1_EED2Ev>:
    3360: 48 8b 3f                     	movq	(%rdi), %rdi
    3363: 48 85 ff                     	testq	%rdi, %rdi
    3366: 74 05                        	je	0x336d <_ZNSt6vectorIP21kernel_launch_trackerSaIS1_EED2Ev+0xd>
    3368: e9 83 00 00 00               	jmp	0x33f0 <_ZdlPv@plt>
    336d: c3                           	retq
    336e: cc                           	int3
    336f: cc                           	int3

0000000000003370 <_GLOBAL__sub_I_preload2.cpp>:
    3370: 0f 57 c0                     	xorps	%xmm0, %xmm0
    3373: 48 8b 35 86 14 00 00         	movq	5254(%rip), %rsi  # 4800 <puts+0x4800>
    337a: 0f 11 06                     	movups	%xmm0, (%rsi)
    337d: 48 c7 46 10 00 00 00 00      	movq	$0, 16(%rsi)
    3385: 48 8b 3d 44 14 00 00         	movq	5188(%rip), %rdi  # 47d0 <puts+0x47d0>
    338c: 48 8d 15 ed 11 00 00         	leaq	4589(%rip), %rdx  # 4580 <__dso_handle>
    3393: e9 d8 01 00 00               	jmp	0x3570 <__cxa_atexit@plt>

Disassembly of section .init:

0000000000003398 <_init>:
    3398: f3 0f 1e fa                  	endbr64
    339c: 48 83 ec 08                  	subq	$8, %rsp
    33a0: 48 8b 05 09 14 00 00         	movq	5129(%rip), %rax  # 47b0 <puts+0x47b0>
    33a7: 48 85 c0                     	testq	%rax, %rax
    33aa: 74 02                        	je	0x33ae <_init+0x16>
    33ac: ff d0                        	callq	*%rax
    33ae: 48 83 c4 08                  	addq	$8, %rsp
    33b2: c3                           	retq

Disassembly of section .fini:

00000000000033b4 <_fini>:
    33b4: f3 0f 1e fa                  	endbr64
    33b8: 48 83 ec 08                  	subq	$8, %rsp
    33bc: 48 83 c4 08                  	addq	$8, %rsp
    33c0: c3                           	retq

Disassembly of section .plt:

00000000000033d0 <.plt>:
    33d0: ff 35 4a 24 00 00            	pushq	9290(%rip)  # 5820 <_GLOBAL_OFFSET_TABLE_+0x8>
    33d6: ff 25 4c 24 00 00            	jmpq	*9292(%rip)  # 5828 <_GLOBAL_OFFSET_TABLE_+0x10>
    33dc: 0f 1f 40 00                  	nopl	(%rax)

00000000000033e0 <__cxa_finalize@plt>:
    33e0: ff 25 4a 24 00 00            	jmpq	*9290(%rip)  # 5830 <_GLOBAL_OFFSET_TABLE_+0x18>
    33e6: 68 00 00 00 00               	pushq	$0
    33eb: e9 e0 ff ff ff               	jmp	0x33d0 <.plt>

00000000000033f0 <_ZdlPv@plt>:
    33f0: ff 25 42 24 00 00            	jmpq	*9282(%rip)  # 5838 <_GLOBAL_OFFSET_TABLE_+0x20>
    33f6: 68 01 00 00 00               	pushq	$1
    33fb: e9 d0 ff ff ff               	jmp	0x33d0 <.plt>

0000000000003400 <printf@plt>:
    3400: ff 25 3a 24 00 00            	jmpq	*9274(%rip)  # 5840 <_GLOBAL_OFFSET_TABLE_+0x28>
    3406: 68 02 00 00 00               	pushq	$2
    340b: e9 c0 ff ff ff               	jmp	0x33d0 <.plt>

0000000000003410 <dlsym@plt>:
    3410: ff 25 32 24 00 00            	jmpq	*9266(%rip)  # 5848 <_GLOBAL_OFFSET_TABLE_+0x30>
    3416: 68 03 00 00 00               	pushq	$3
    341b: e9 b0 ff ff ff               	jmp	0x33d0 <.plt>

0000000000003420 <_Z12print_resultPjjjj@plt>:
    3420: ff 25 2a 24 00 00            	jmpq	*9258(%rip)  # 5850 <_GLOBAL_OFFSET_TABLE_+0x38>
    3426: 68 04 00 00 00               	pushq	$4
    342b: e9 a0 ff ff ff               	jmp	0x33d0 <.plt>

0000000000003430 <puts@plt>:
    3430: ff 25 22 24 00 00            	jmpq	*9250(%rip)  # 5858 <_GLOBAL_OFFSET_TABLE_+0x40>
    3436: 68 05 00 00 00               	pushq	$5
    343b: e9 90 ff ff ff               	jmp	0x33d0 <.plt>

0000000000003440 <hipDeviceSynchronize@plt>:
    3440: ff 25 1a 24 00 00            	jmpq	*9242(%rip)  # 5860 <_GLOBAL_OFFSET_TABLE_+0x48>
    3446: 68 06 00 00 00               	pushq	$6
    344b: e9 80 ff ff ff               	jmp	0x33d0 <.plt>

0000000000003450 <hipMemcpy@plt>:
    3450: ff 25 12 24 00 00            	jmpq	*9234(%rip)  # 5868 <_GLOBAL_OFFSET_TABLE_+0x50>
    3456: 68 07 00 00 00               	pushq	$7
    345b: e9 70 ff ff ff               	jmp	0x33d0 <.plt>

0000000000003460 <malloc@plt>:
    3460: ff 25 0a 24 00 00            	jmpq	*9226(%rip)  # 5870 <_GLOBAL_OFFSET_TABLE_+0x58>
    3466: 68 08 00 00 00               	pushq	$8
    346b: e9 60 ff ff ff               	jmp	0x33d0 <.plt>

0000000000003470 <_Znwm@plt>:
    3470: ff 25 02 24 00 00            	jmpq	*9218(%rip)  # 5878 <_GLOBAL_OFFSET_TABLE_+0x60>
    3476: 68 09 00 00 00               	pushq	$9
    347b: e9 50 ff ff ff               	jmp	0x33d0 <.plt>

0000000000003480 <memmove@plt>:
    3480: ff 25 fa 23 00 00            	jmpq	*9210(%rip)  # 5880 <_GLOBAL_OFFSET_TABLE_+0x68>
    3486: 68 0a 00 00 00               	pushq	$10
    348b: e9 40 ff ff ff               	jmp	0x33d0 <.plt>

0000000000003490 <hipMalloc@plt>:
    3490: ff 25 f2 23 00 00            	jmpq	*9202(%rip)  # 5888 <_GLOBAL_OFFSET_TABLE_+0x70>
    3496: 68 0b 00 00 00               	pushq	$11
    349b: e9 30 ff ff ff               	jmp	0x33d0 <.plt>

00000000000034a0 <memset@plt>:
    34a0: ff 25 ea 23 00 00            	jmpq	*9194(%rip)  # 5890 <_GLOBAL_OFFSET_TABLE_+0x78>
    34a6: 68 0c 00 00 00               	pushq	$12
    34ab: e9 20 ff ff ff               	jmp	0x33d0 <.plt>

00000000000034b0 <hipMemset@plt>:
    34b0: ff 25 e2 23 00 00            	jmpq	*9186(%rip)  # 5898 <_GLOBAL_OFFSET_TABLE_+0x80>
    34b6: 68 0d 00 00 00               	pushq	$13
    34bb: e9 10 ff ff ff               	jmp	0x33d0 <.plt>

00000000000034c0 <__assert_fail@plt>:
    34c0: ff 25 da 23 00 00            	jmpq	*9178(%rip)  # 58a0 <_GLOBAL_OFFSET_TABLE_+0x88>
    34c6: 68 0e 00 00 00               	pushq	$14
    34cb: e9 00 ff ff ff               	jmp	0x33d0 <.plt>

00000000000034d0 <_ZSt20__throw_length_errorPKc@plt>:
    34d0: ff 25 d2 23 00 00            	jmpq	*9170(%rip)  # 58a8 <_GLOBAL_OFFSET_TABLE_+0x90>
    34d6: 68 0f 00 00 00               	pushq	$15
    34db: e9 f0 fe ff ff               	jmp	0x33d0 <.plt>

00000000000034e0 <exit@plt>:
    34e0: ff 25 ca 23 00 00            	jmpq	*9162(%rip)  # 58b0 <_GLOBAL_OFFSET_TABLE_+0x98>
    34e6: 68 10 00 00 00               	pushq	$16
    34eb: e9 e0 fe ff ff               	jmp	0x33d0 <.plt>

00000000000034f0 <fopen@plt>:
    34f0: ff 25 c2 23 00 00            	jmpq	*9154(%rip)  # 58b8 <_GLOBAL_OFFSET_TABLE_+0xa0>
    34f6: 68 11 00 00 00               	pushq	$17
    34fb: e9 d0 fe ff ff               	jmp	0x33d0 <.plt>

0000000000003500 <fseek@plt>:
    3500: ff 25 ba 23 00 00            	jmpq	*9146(%rip)  # 58c0 <_GLOBAL_OFFSET_TABLE_+0xa8>
    3506: 68 12 00 00 00               	pushq	$18
    350b: e9 c0 fe ff ff               	jmp	0x33d0 <.plt>

0000000000003510 <ftell@plt>:
    3510: ff 25 b2 23 00 00            	jmpq	*9138(%rip)  # 58c8 <_GLOBAL_OFFSET_TABLE_+0xb0>
    3516: 68 13 00 00 00               	pushq	$19
    351b: e9 b0 fe ff ff               	jmp	0x33d0 <.plt>

0000000000003520 <aligned_alloc@plt>:
    3520: ff 25 aa 23 00 00            	jmpq	*9130(%rip)  # 58d0 <_GLOBAL_OFFSET_TABLE_+0xb8>
    3526: 68 14 00 00 00               	pushq	$20
    352b: e9 a0 fe ff ff               	jmp	0x33d0 <.plt>

0000000000003530 <fread@plt>:
    3530: ff 25 a2 23 00 00            	jmpq	*9122(%rip)  # 58d8 <_GLOBAL_OFFSET_TABLE_+0xc0>
    3536: 68 15 00 00 00               	pushq	$21
    353b: e9 90 fe ff ff               	jmp	0x33d0 <.plt>

0000000000003540 <fclose@plt>:
    3540: ff 25 9a 23 00 00            	jmpq	*9114(%rip)  # 58e0 <_GLOBAL_OFFSET_TABLE_+0xc8>
    3546: 68 16 00 00 00               	pushq	$22
    354b: e9 80 fe ff ff               	jmp	0x33d0 <.plt>

0000000000003550 <hipFree@plt>:
    3550: ff 25 92 23 00 00            	jmpq	*9106(%rip)  # 58e8 <_GLOBAL_OFFSET_TABLE_+0xd0>
    3556: 68 17 00 00 00               	pushq	$23
    355b: e9 70 fe ff ff               	jmp	0x33d0 <.plt>

0000000000003560 <free@plt>:
    3560: ff 25 8a 23 00 00            	jmpq	*9098(%rip)  # 58f0 <_GLOBAL_OFFSET_TABLE_+0xd8>
    3566: 68 18 00 00 00               	pushq	$24
    356b: e9 60 fe ff ff               	jmp	0x33d0 <.plt>

0000000000003570 <__cxa_atexit@plt>:
    3570: ff 25 82 23 00 00            	jmpq	*9090(%rip)  # 58f8 <_GLOBAL_OFFSET_TABLE_+0xe0>
    3576: 68 19 00 00 00               	pushq	$25
    357b: e9 50 fe ff ff               	jmp	0x33d0 <.plt>
