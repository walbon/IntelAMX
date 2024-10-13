COMPILER = gcc
COMPILER_FLAGS = -O0 -g3 -static -mamx-tile -mamx-bf16 -mamx-int8
FILE = teste_amx.c
BIN  = teste
OBJDUMP = teste.obj
SDE = /opt/Intel/sde

all: fill build sde
	@printf "Number of tileloads "
	@grep -c  tileloadd include_tileloadds.h

fill:
	@python ./include_fill.py

build:
	$(COMPILER) $(COMPILER_FLAGS) -o $(BIN) $(FILE) 
	objdump -S $(BIN) > $(OBJDUMP)

qemu:
	qemu-x86_64-static -cpu SapphireRapids $(BIN)

sde:
	$(SDE)/sde -spr -ptr-check -- ./$(BIN)

sde-gdb:
	$(SDE)/sde -debug -spr -- ./$(BIN) & \
	echo  "Execute : gdb $(BIN) -ex \"target remote  localhost:<PORT>\""

