all : flash

TARGET:=rx
TARGET_MCU?=CH32V003

CH32FUN:=/Users/hiro/ch32fun/ch32fun
MINICHLINK:=/Users/hiro/ch32fun/minichlink

PREFIX:=/Users/hiro/tools/xpack-riscv-none-elf-gcc-15.2.0-1/bin/riscv-none-elf

FLASH_COMMAND:=$(MINICHLINK)/minichlink -c 0x1209b803 -w $(TARGET).bin flash -b

include $(CH32FUN)/ch32fun.mk

flash : cv_flash
clean : cv_clean
