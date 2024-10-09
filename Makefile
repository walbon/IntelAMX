COMPILER = gcc
COMPILER_FLAGS = -O0 -g3 -static -mamx-tile -mamx-bf16 -mamx-int8
FILE = teste_amx.c
BIN  = teste
OBJDUMP = teste.obj
SDE = /opt/Intel/sde

all: build sde

build:
	$(COMPILER) $(COMPILER_FLAGS) -o $(BIN) $(FILE) 
	objdump -S $(BIN) > $(OBJDUMP)

qemu:
	qemu-x86_64-static -cpu SapphireRapids $(BIN)

sde:
	$(SDE)/sde -spr -- ./$(BIN)

sde-gdb:
	$(SDE)/sde -debug -spr -- ./$(BIN) & \
	echo  "Execute : gdb $(BIN) -ex \"target remote  localhost:<PORT>\""

