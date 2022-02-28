# parameter 0 (%rcx) - save ctx ptr
# parameter 1 (%rdx) - load ctx ptr
# parameter 2 (%r8)  - return value
# parameter 3 (%r9)  - arg passed to context code 

swap_context:
save_context:
lea back(%rip), %rax
push %rax
movq %rbx, 8*0(%rcx)
movq %rsp, 8*1(%rcx)
movq %rbp, 8*2(%rcx)
movq %r12, 8*3(%rcx)
movq %r13, 8*4(%rcx)
movq %r14, 8*5(%rcx)
movq %r15, 8*6(%rcx)
movq %rdi, 8*7(%rcx)
movq %rsi, 8*8(%rcx)
stmxcsr 8*9+4*0(%rcx)
fstcw   8*9+4*1(%rcx)
movdqa %xmm6, 8*9+4*2+16*0(%rcx)   
movdqa %xmm7, 8*9+4*2+16*1(%rcx)   
movdqa %xmm8, 8*9+4*2+16*2(%rcx)   
movdqa %xmm9, 8*9+4*2+16*3(%rcx)   
movdqa %xmm10, 8*9+4*2+16*4(%rcx)   
movdqa %xmm11, 8*9+4*2+16*5(%rcx)   
movdqa %xmm12, 8*9+4*2+16*6(%rcx)   
movdqa %xmm13, 8*9+4*2+16*7(%rcx)   
movdqa %xmm14, 8*9+4*2+16*8(%rcx)   
movdqa %xmm15, 8*9+4*2+16*9(%rcx)
load_context:
movq 8*0(%rdx), %rbx
movq 8*1(%rdx), %rsp 
movq 8*2(%rdx), %rbp 
movq 8*3(%rdx), %r12 
movq 8*4(%rdx), %r13 
movq 8*5(%rdx), %r14 
movq 8*6(%rdx), %r15 
movq 8*7(%rdx), %rdi 
movq 8*8(%rdx), %rsi 
ldmxcsr 8*9+4*0(%rdx)
fldcw   8*9+4*1(%rdx)
movdqa 8*9+4*2+16*0(%rdx), %xmm6   
movdqa 8*9+4*2+16*1(%rdx), %xmm7   
movdqa 8*9+4*2+16*2(%rdx), %xmm8   
movdqa 8*9+4*2+16*3(%rdx), %xmm9   
movdqa  8*9+4*2+16*4(%rdx), %xmm10   
movdqa  8*9+4*2+16*5(%rdx), %xmm11   
movdqa  8*9+4*2+16*6(%rdx), %xmm12   
movdqa  8*9+4*2+16*7(%rdx), %xmm13   
movdqa  8*9+4*2+16*8(%rdx), %xmm14   
movdqa  8*9+4*2+16*9(%rdx), %xmm15
mov %rdx, %rcx
mov %r9, %rdx
mov %r8, %rax
back:
ret