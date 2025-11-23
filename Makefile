ARCH       ?= aarch64-none-elf
CC         := $(ARCH)-gcc
CXX        := $(ARCH)-g++
LD         := $(ARCH)-ld
AR         := $(ARCH)-ar

OS_PATH ?= ../os

EXEC_NAME ?= detour.a

STDINC ?= $(OS_PATH)/shared/
STDLIB ?= $(OS_PATH)/shared/libshared.a
CFLAGS ?= -ffreestanding -nostdlib -std=c99 -I$(STDINC) -I. -O0
C_SOURCE ?= $(shell find . -name '*.c')
OBJ ?= $(C_SOURCE:%.c=%.o)

.PHONY: dump

%.o : %.c
	$(CC) $(CFLAGS) -c -c $< -o $@

$(EXEC_NAME): $(OBJ)
	$(AR) rcs $@ $(OBJ)

all: $(EXEC_NAME)

clean: 	
	rm $(OBJ)
	rm $(EXEC_NAME)

dump: all
	$(ARCH)-objdump -S $(EXEC_NAME) > dump
