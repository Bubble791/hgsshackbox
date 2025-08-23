.nds
.thumb

.loadtable "charmap.txt"
.include "macro/hg_en.s"

.create "build/new_overlay.bin", 0x02229EE0

.area 0x5300
.importobj "build/hackBox.o"

.include "macro/thumb_help.s"

debugsyscall:
swi 0xFC
bx lr

gText_titleName:
    .string "Hack Box$"

.org 0x02229EE0 + 0x5300 - 1
.byte 0
.endarea
.close