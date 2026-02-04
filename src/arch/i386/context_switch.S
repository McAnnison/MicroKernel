.global ctx_switch
.type ctx_switch, @function

// void ctx_switch(uint32_t **old_sp, uint32_t *new_sp);
// Saves full CPU context, stores ESP into *old_sp, loads new_sp into ESP, restores context, then returns.
ctx_switch:
    pushfl
    pushal
    mov 40(%esp), %eax
    mov 44(%esp), %edx
    mov %esp, (%eax)
    mov %edx, %esp
    popal
    popfl
    ret

.section .note.GNU-stack,"",@progbits
