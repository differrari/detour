ARCH       ?= aarch64-none-elf
CC         := $(ARCH)-gcc
CXX        := $(ARCH)-g++
LD         := $(ARCH)-ld
AR         := $(ARCH)-ar

OS_PATH ?= ../os

EXEC_NAME ?= detour.a

STDINC ?= $(OS_PATH)/shared/
STDLIB ?= $(OS_PATH)/shared/libshared.a
CFLAGS ?= -ffreestanding -nostdlib -std=c99 -I$(STDINC) -O0
FS_PATH ?= $(OS_PATH)/fs/redos/user/
FS_EXEC_PATH ?= $(FS_PATH)/$(EXEC_NAME)
C_SOURCE ?= $(shell find . -name '*.c')
OBJ ?= $(C_SOURCE:%.c=%.o)

.PHONY: dump

%.o : %.c
	$(CC) $(CFLAGS) -c -c $< -o $@

$(EXEC_NAME): $(OBJ)
	$(AR) rcs $@ $(OBJ)

all: $(EXEC_NAME)
	
run: all
	cp $(EXEC_NAME) $(FS_EXEC_PATH)
	(cd $(OS_PATH); ./createfs; ./run_virt)

clean: 	
	rm $(OBJ)
	rm $(EXEC_NAME)

dump: all
	$(ARCH)-objdump -S $(EXEC_NAME) > dump
