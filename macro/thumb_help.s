__gnu_thumb1_case_uqi:
    push    {r1}
    mov    r1, lr
    lsr    r1, r1, #1
    lsl    r1, r1, #1
    ldrb    r1, [r1, r0]
    lsl    r1, r1, #1
    add    lr, r1
    pop    {r1}
    bx    lr

__aeabi_uidiv:
__aeabi_uidivmod:
    push {lr}
    blx _u32_div_f
    pop {pc}

__gnu_thumb1_case_uhi:
    push    {r0, r1}
    mov     r1, lr
    lsr    r1, r1, #1
    lsl    r0, r0, #1
    lsl    r1, r1, #1
    ldrh    r1, [r1, r0]
    lsl    r1, r1, #1
    add     lr, r1
    pop     {r0, r1}
    bx      lr