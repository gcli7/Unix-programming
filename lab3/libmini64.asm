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

    global sigreturn:function
sigreturn:
    mov     rax, 15
    syscall
    ret
