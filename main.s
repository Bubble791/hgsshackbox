.nds
.thumb

.include "macro/hg_en.s"

.create "build/new_overlay.bin", 0x02229EE0

.area 0x5300
.importobj "build/hackBox.o"
.endarea

.include "macro/thumb_help.s"

debugsyscall:
swi 0xFC
bx lr

.org 0x02229EE0 + 0x5300 - 1
.byte 0
.close