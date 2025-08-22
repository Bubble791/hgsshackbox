.nds
.thumb

.open "arm9.bin", 0x02000000

.org 0x020848E2
ldr r3, NamingScreen_HandleCharacterInput_HOOK
bx r3
.pool

.close

.open "", 0x023E0000
NamingScreen_HandleCharacterInput_HOOK:
    push {r0-r5}
    mov r0, r4
    mov r1, r5
    bl NamingScreen_HandleCharacterInput
    pop {r0-r5}
    ldr r3, =0xD002
    cmp r5, r3
    bgt ret_0x020848F4
    sub r0, r3, #0x1
    cmp r5, r0
    ldr r0, =0x020848EC
    bx r0

.importobj "build/naming_screen.o"
.close