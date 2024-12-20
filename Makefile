# Makefile to generate vstart tests
#
# Copyright (C) 2024 SiFive Limited
# Contributor Max Chou <max.chou@sifive.com>

QEMU=qemu-riscv64
CC=riscv64-unknown-linux-gnu-gcc
CFLAGS=-march=rv64gcv -O0 -static

.PHONY: all
all: test_vstart_whole

run: test_vstart_whole
	${QEMU} -cpu max,vlen=128 test_vstart_whole

test_vstart_whole: whole_reg_vstart.c memcpy_vec.S
	$(CC) $(CFLAGS) $^ -o $@

.PHONE: clean
clean:
	$(RM) test_vstart_whole
