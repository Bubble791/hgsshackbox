import ndspy.rom
import ndspy.code
import ndspy.narc
import sys

rom_path = sys.argv[1]
output_path = sys.argv[2]
rom = ndspy.rom.NintendoDSRom.fromFile(rom_path)

def ChangeArm9Word(offset, bytes):
    offset -= 0x02000000
    arm9_code = ndspy.code.MainCodeFile(
        data=rom.arm9,
        ramAddress=rom.arm9RamAddress,
        codeSettingsPointerAddress=rom.arm9EntryAddress
    )
    arm9_code.sections[0].data[offset : offset + 4] = bytes.to_bytes(4, 'little')
    rom.arm9 = arm9_code.save(compress=False)

def ReplaceOverlay(index, newOvy):
    # 替换overlay
    with open(newOvy, 'rb') as f:
        new_data = f.read()

    rom.files[index] = new_data

# 修改arm9中朋友手册的程序地址
# 020FA2A4
ReplaceOverlay(43, "build/new_overlay.bin")

## 修改arm9
symTable = {}
with open("output.sym", "r", encoding="utf-8") as sym:
    for text in sym.readlines():
        text = text.strip()
        if "," in text:
            offset, name = text.split(" ")
            name = name.split(",")[0]
            symTable[name] = int(offset, 16)

ChangeArm9Word(0x020FA2A4, symTable["HackBoxTool_Init"] + 1)
ChangeArm9Word(0x020FA2A8, symTable["HackBoxTool_Main"] + 1)
ChangeArm9Word(0x020FA2AC, symTable["HackBoxTool_Exit"] + 1)

# 修改narc
narcData = rom.files[214]
narc = ndspy.narc.NARC(narcData)
with open('graphic/title.bin', 'rb') as f:
    ncgData = f.read()
    narc.files[14] = ncgData
with open('graphic/button.bin', 'rb') as f:
    ncgData = f.read()
    narc.files[11] = ncgData
rom.files[214] = narc.save()

rom.saveToFile(output_path)