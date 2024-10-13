
CACHE_LINE_SIZE = 64  #  Cache line size 64bytes
TILE_LOADS = 1_000_000
L1_CACHE_SIZE= 48*1024
ADDRESS_LIMIT = 2 * 1024 * 1024 # 2MB
MATRIX_SIZE =  1024 # 1 KB  =  16 Lin * 64 bytes
BATCH_SIZE = 1_000

_file = "include_tileloadds.h"
content = ""
offset=0x00

with open(_file, "w") as file:
    WROTE=0
    content+=f"_tile_loadd (0, address+{hex(offset)}, 1);"
    for i in range(TILE_LOADS):
        WROTE+=1
        offset = (i * L1_CACHE_SIZE) % ADDRESS_LIMIT
        if offset + MATRIX_SIZE > ADDRESS_LIMIT:
            offset = offset % ADDRESS_LIMIT
        content+="""\n__asm__ __volatile__( """
        content+=(" \"tileloadd %0, %%tmm0\\n\"")
        content+=f""":: \"m\"(*(address+{hex(offset)})) );"""
        if  WROTE % BATCH_SIZE == 0:
            file.writelines(content)
            content = []
    file.writelines(content)

print(f"""
Build with following parameters:
    TILE_LOADS={TILE_LOADS}
    ADDRESS_LIMIT={ADDRESS_LIMIT}
    MATRIX_SIZE={MATRIX_SIZE}
    L1_CACHE_SIZE={L1_CACHE_SIZE}
    file output={_file}
""")
