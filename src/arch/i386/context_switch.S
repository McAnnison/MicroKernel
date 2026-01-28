.global ctx_switch
.type ctx_switch, @function

// void ctx_switch(uint32_t **old_sp, uint32_t *new_sp);
// Saves current ESP into *old_sp, loads new_sp into ESP, then returns.
ctx_switch:
    mov 4(%esp), %eax
    mov 8(%esp), %edx
    mov %esp, (%eax)
    mov %edx, %esp
    ret

.section .note.GNU-stack,"",@progbits
