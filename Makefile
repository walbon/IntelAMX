#!/usr/bin/env gmake

COMPILER = gcc
COMPILER_FLAGS = -O0 -g3 -static -mamx-tile -mamx-bf16 -mamx-int8
NAMES := tileload_misses tileload_nomisses  
SDE = /opt/Intel/sde
REPEAT = 1

all: fill  build sde

fill:
	$(foreach name,  $(NAMES), \
		echo "- Creating Include files : include_$(name).h ...";\
		python ./include_$(name).py && \
		printf "DONE \n" || printf "ERROR\n";\
	)

build:
	$(foreach name,  $(NAMES), \
		echo "- Build : $(name) ...";\
		$(COMPILER) $(COMPILER_FLAGS) -o $(name) teste_$(name).c \
	    && objdump -S $(name) > $(name).obj && \
		printf "DONE \n" || printf "ERROR\n";\
	)

sde:
	$(foreach name,  $(NAMES), \
		echo "- SDE : $(name) ...";\
		$(SDE)/sde -spr -ptr-check -- perf stat -e cache-references,cache-misses,cycles,instructions,L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores -r $(REPEAT) -o $(name).json --json ./$(name) && \
		printf "DONE \n" || printf "ERROR\n";\
	)
	
sde-gdb:
	$(SDE)/sde -debug -spr -- ./$(TARGET) &

clean:
	rm -rf *obj *json include_*.h $(NAMES)

