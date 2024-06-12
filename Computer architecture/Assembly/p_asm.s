.intel_syntax noprefix

.global potprogram_asm

potprogram_asm:
    push ebp
    mov ebp, esp

    xor eax, eax
    mov ebx, [ebp + 8]
l1:
    sub ebx, 0x1
    jle ret_label
    add eax, ebx
    jmp l1

ret_label:
    pop ebp
    ret
