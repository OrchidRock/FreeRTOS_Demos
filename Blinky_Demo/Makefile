#/*
# * FreeRTOS V202112.00
# * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
# *
# * Permission is hereby granted, free of charge, to any person obtaining a copy of
# * this software and associated documentation files (the "Software"), to deal in
# * the Software without restriction, including without limitation the rights to
# * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# * the Software, and to permit persons to whom the Software is furnished to do so,
# * subject to the following conditions:
# *
# * The above copyright notice and this permission notice shall be included in all
# * copies or substantial portions of the Software.
# *
# * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
# *
# * https://www.FreeRTOS.org
# * https://aws.amazon.com/freertos
# *
# * 1 tab == 4 spaces!
# */

CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
SIZE = arm-none-eabi-size
OBJCOPY = arm-none-eabi-objcopy
MCU = TM4C123GH6PM
BIN = Blinky_Demo.bin

BUILD_DIR := build

DRIVERLIB_PATH := ../driverlib

DEBUG = #-DDEBUG #-g 
OPT = -Os
WARNINGS = -Wall -Wunused -Wmissing-prototypes -Wmissing-declarations -Wstrict-prototypes -Wbad-function-cast -Wcast-align -Wsign-compare -Wshadow -Wpointer-arith
INCLUDE = -I. -I../ -I$(DRIVERLIB_PATH)
CFLAGS = -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp $(OPT) $(DEBUG) -ffunction-sections -fdata-sections -MD -std=c99 $(WARNINGS) -pedantic $(INCLUDE) 
CFLAGS += -DPART_$(MCU) -Dgcc -DTARGET_IS_BLIZZARD_RA1

LDFLAGS = -T $(MCU).ld --entry ResetISR --gc-sections 
LDFLAGS += -Map $(BUILD_DIR)/$(BIN)_gcc.Map 

LIBC = ${shell ${CC} -mthumb -march=armv6t2 -print-file-name=libc.a}

# driverlib source
DRIVERLIB_FILES :=  \
$(DRIVERLIB_PATH)/sysctl.c \
$(DRIVERLIB_PATH)/gpio.c \
$(DRIVERLIB_PATH)/uart.c \
$(DRIVERLIB_PATH)/interrupt.c \
$(DRIVERLIB_PATH)/systick.c

DEMO_SOURCE_FILES := \
main.c \
startup_gcc.c \

VPATH := $(DRIVERLIB_PATH)

# driverlib objs
DRIVERLIB_OBJS := $(patsubst %.c,%.o,$(notdir $(DRIVERLIB_FILES)))

# demo objs
DEMO_OBJS := $(DEMO_SOURCE_FILES:%.c=%.o)

OBJS := \
$(addprefix $(BUILD_DIR)/driverlib/, $(DRIVERLIB_OBJS)) \
$(addprefix $(BUILD_DIR)/, $(DEMO_OBJS))

$(info $(OBJS))

.PHONY: clean

#
# Rules for all object files.
#

$(BUILD_DIR)/$(BIN) : $(BUILD_DIR)/a.out
	$(OBJCOPY) -O binary $< $@
	@echo ""
	@echo "-------------------------------------------------"
	$(SIZE) $(BUILD_DIR)/a.out
	@echo "-------------------------------------------------"


$(BUILD_DIR)/a.out : $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBC)

${BUILD_DIR}/driverlib/%.o : %.c Makefile
	-mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

${BUILD_DIR}/freertos/%.o : %.c Makefile
	-mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

${BUILD_DIR}/%.o : %.c Makefile
	-mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	-rm -rf $(BUILD_DIR)
