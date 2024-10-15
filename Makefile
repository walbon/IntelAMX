COMPILER = gcc
COMPILER_FLAGS = -O0 -g3 -static -mamx-tile -mamx-bf16 -mamx-int8
FILE = teste_tileload_misses.c
BIN  = teste_tileload_misses
OBJDUMP = teste_tileload_misses.obj
SDE = /opt/Intel/sde
REPEAT = 1

all: fill build sde
	@printf "Number of tileloads "
	@grep -c  tileloadd include_tileload_misses.h

fill:
	@python ./include_load_misses.py

build:
	$(COMPILER) $(COMPILER_FLAGS) -o $(BIN) $(FILE) 
	objdump -S $(BIN) > $(OBJDUMP)

qemu:
	qemu-x86_64-static -cpu SapphireRapids $(BIN)

sde:
	$(SDE)/sde -spr -ptr-check -- perf stat -e L1-dcache-load-misses -r $(REPEAT) -o $(BIN).json --json ./$(BIN)

sde-gdb:
	$(SDE)/sde -debug -spr -- ./$(BIN) & \
	echo  "Execute : gdb $(BIN) -ex \"target remote  localhost:<PORT>\""
clean:
	rm -rf *obj *json  $(BIN) include_*.h

