%macro gensys 2
    global  sys_%2:function
sys_%2:
    push    r10
    mov     r10, rcx
    mov     rax, %1
    syscall
    pop     r10
    ret
%endmacro

; RDI, RSI, RDX, RCX, R8, R9

extern errno

    section .data

    section .text

    gensys   1, write
    ; Extended code
    gensys  13, sigaction
    gensys  14, sigprocmask
    ; End of extended code
    gensys  34, pause
    gensys  35, nanosleep
    ; Extended code
    gensys  37, alarm
    gensys  60, exit
    gensys 127, sigpending
    ; End of extended code


    global sleep:function
sleep:
    sub     rsp, 32             ; allocate timespec * 2
    mov     [rsp], rdi          ; req.tv_sec
    mov     QWORD [rsp+8], 0    ; req.tv_nsec
    mov     rdi, rsp            ; rdi = req @ rsp
    lea     rsi, [rsp+16]       ; rsi = rem @ rsp+16
    call    sys_nanosleep
    cmp     rax, 0
    jge     sleep_quit          ; no error :)
sleep_error:
    neg     rax
    cmp     rax, 4              ; rax == EINTR?
    jne     sleep_failed
sleep_interrupted:
    lea     rsi, [rsp+16]
    mov     rax, [rsi]          ; return rem.tv_sec
    jmp     sleep_quit
sleep_failed:
    mov     rax, 0              ; return 0 on error
sleep_quit:
    add     rsp, 32
    ret


    global sys_sigreturn:function
sys_sigreturn:
    mov     rax, 15
    syscall
    ret


    global setjmp:function
setjmp:
    mov     QWORD [rdi], rbx
    mov     QWORD [rdi+8], rsp
    mov     QWORD [rdi+16], rbp
    mov     QWORD [rdi+24], r12
    mov     QWORD [rdi+32], r13
    mov     QWORD [rdi+40], r14
    mov     QWORD [rdi+48], r15

    mov rax, QWORD [rsp]
    mov QWORD [rdi+56], rax

    mov     rsi, 0
    lea     rdx, [rdi+64]
    call    sys_sigprocmask
    mov     rax, 0
    ret


    global longjmp:function
longjmp:
    mov     rbx, QWORD [rdi]
    mov     rsp, QWORD [rdi+8]
    mov     rbp, QWORD [rdi+16]
    mov     r12, QWORD [rdi+24]
    mov     r13, QWORD [rdi+32]
    mov     r14, QWORD [rdi+40]
    mov     r15, QWORD [rdi+48]

    pop rax
    mov rax, QWORD [rdi+56]
    push rax

    push    rsi
    mov     rdi, 2
    lea     rsi, [rdi+64]
    mov     rdx, 0
    call    sys_sigprocmask
    pop     rax
    ret
