import re

dirs = {}
with open("HGSS_EU.txt", "r", encoding="utf-8") as hgss:
    for text in hgss.readlines():
        line = text.strip()
        pattern = r'^(\w+)\s+(\w+)\s+(\S+)\s+(\S.*?)\s+\(([^)]+)\)'
        match = re.match(pattern, line)
        if match:
            address, size, section, func_name, obj_file = match.groups()

            if address not in dirs:
                dirs[address] = {}

            if "$" in func_name:
                if func_name == "$t":
                    dirs[address]["type"] = "definethumblabel"
                elif func_name == "$a":
                    dirs[address]["type"] = "definearmlabel"
                else:
                    dirs[address]["type"] = "definedatalabel"
            else:
                dirs[func_name.lower()] = {"offset" : address, "name" : func_name}
                dirs[address]["name"] = func_name.lower()

with open("hand.txt", "r", encoding="utf-8") as needLoad:
    for text in needLoad.readlines():
        text = text.split("Undefined external symbol")[1].split(" ")[1]

        address = dirs[text]["offset"]
        type = dirs[address]["type"]
        fullname = dirs[text]["name"]
        print(f".{type} {fullname}, 0x{address}")