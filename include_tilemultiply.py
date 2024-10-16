
TILE_MULTs = 1_000_000
BATCH_SIZE = 1_000

_file = "include_tilemultiply.h"
content = ""

with open(_file, "w") as file:
    WROTE=0
    for i in range(TILE_MULTs//3):
        WROTE+=3
        content+="_tile_dpbssd (1, 2, 3);\n"
        content+="_tile_dpbssd (3, 1, 2);\n"
        content+="_tile_dpbssd (2, 3, 1);\n"
        if  WROTE % BATCH_SIZE == 0:
            file.writelines(content)
            content = []
    file.writelines(content)

print(f"""
Build with following parameters:
    TILE_MULTs = {WROTE}
    file output={_file}
""")
